#include "Ontogenesis.h"

#include "DebugDraw.h"
#include "Logger.h"



const int MAX_PROTEIN = 5;



Ontogenesis::Ontogenesis( const Ogre::String& file_name ):
    m_Filename( file_name ),
    m_GeneUniqueId( 0 )
{
}



Ontogenesis::~Ontogenesis()
{
}




void
Ontogenesis::Draw( const unsigned int x, const unsigned int y )
{
    size_t cur_gen = m_Generations.size() - 1;
    if( cur_gen >= 0 )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        DEBUG_DRAW.Text( x, y, "Generation " + IntToString( cur_gen ) );
        DEBUG_DRAW.Text( x + 200, y, "Top fitness: " + IntToString( m_Generations[ cur_gen ].top_fitness ) );
        DEBUG_DRAW.Text( x + 400, y, "Number of species: " + IntToString( m_Generations[ cur_gen ].species.size() ) );
    }
}



void
Ontogenesis::LoadNetwork( Entity* entity )
{
    Logger* dump = new Logger( m_FileName );



    size_t cur_gen = m_Generations.size() - 1;
    if( cur_gen < 0 || m_Generations[ cur_gen ].species.size() >= 5 )
    {
        Generation generation;
        generation.top_fitness = 0.0f;
        for( size_t i = 0; i < m_Generations[ cur_gen ].species.size(); ++i )
        {
            if( m_Generations[ cur_gen ].species[ i ].fitness == m_Generations[ cur_gen ].top_fitness )
            {
                generation.base_genome = m_Generations[ cur_gen ].species[ i ].genome;
            }
        }
        ++cur_gen;
        m_Generations.push_back( generation );
    }
    Species species;
    species.fitness = 0.0f;
    species.genome = Mutate( m_Generations[ cur_gen ].base_genome );



    dump->Log( "Generation: " + IntToString( cur_gen ) + " " );
    DumpGenome( dump, species.genome );
    dump->Log( "\n" );



    // Default cells
    int protein_sensor_food = 1;
    int protein_activator_forward = 2;
    int protein_activator_left = 3;
    int protein_activator_right = 4;

    Cell* cell = new Cell( entity, Cell::STEM, 0, 0 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD_LEFT, -5, -3 );
    cell->SetProtein( protein_sensor_food );
    cell->SetProteinRadius( 8 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD_RIGHT, -5, 3 );
    cell->SetProtein( protein_sensor_food );
    cell->SetProteinRadius( 8 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_FORWARD, 5, 0 );
    cell->SetProtein( protein_activator_forward );
    cell->SetProteinRadius( 6 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_LEFT, 3, -3 );
    cell->SetProtein( protein_activator_left );
    cell->SetProteinRadius( 5 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_RIGHT, 3, 3 );
    cell->SetProtein( protein_activator_right );
    cell->SetProteinRadius( 5 );
    species.network.push_back( cell );



    bool changes = true;
    int cycles = 0;
    while( changes == true && cycles < 3 )
    {
        ++cycles;
        changes = false;

        dump->Log( "Step: " + IntToString( cycles ) + "\n" );

        std::vector< std::vector< unsigned int > > species_genes;

        for( size_t i = 0; i < species.network.size(); ++i )
        {
            std::vector< unsigned int > expr_genes;

            dump->Log( "    cell_" + IntToString( i ) + ": " + IntToString( species.network[ i ]->GetType() ) + "\n" );

            for( size_t gene_id = 0; gene_id < species.genome.size(); ++gene_id )
            {
                Gene gene = species.genome[ gene_id ];
                bool exec = true;

                for( size_t cond_id = 0; cond_id < gene.cond.size(); ++cond_id )
                {
                    Condition cond = gene.cond[ cond_id ];

                    switch( cond.type )
                    {
                        case C_NAME:
                        {
                            if( species.network[ i ]->GetType() != cond.value )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case C_NNAME:
                        {
                            if( species.network[ i ]->GetType() == cond.value )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case C_PROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchProtein( species.network, cond.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            if( power == false )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case C_NPROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchProtein( species.network, cond.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            if( power == true )
                            {
                                exec = false;
                            }
                        }
                        break;
                    }
                }

                if( exec == true )
                {
                    expr_genes.push_back( gene_id );
                }
            }

            species_genes.push_back( expr_genes );
        }

        size_t net_size = species.network.size();
        for( size_t i = 0; i < net_size; ++i )
        {
            for( size_t ge_id = 0; ge_id < species_genes[ i ].size(); ++ge_id )
            {
                size_t gene_id = species_genes[ i ][ ge_id ];

                for( size_t expr_id = 0; expr_id < species.genome[ gene_id ].expr.size(); ++expr_id )
                {
                    Expression expr = species.genome[ gene_id ].expr[ expr_id ];

                    switch( expr.type )
                    {
                        case E_NAME:
                        {
                            if( species.network[ i ]->GetType() != expr.value )
                            {
                                species.network[ i ]->SetType( ( Cell::CellType )expr.value );
                                changes = true;
                            }
                        }
                        break;
                        case E_SPLIT:
                        {
                            int x = 0;
                            int y = 0;
                            bool place = FindPlaceForCell( species.network, species.network[ i ]->GetX(), species.network[ i ]->GetY(), 1, x, y );
                            if( place == true )
                            {
                                Cell* cell = new Cell( entity, ( Cell::CellType )expr.value, x, y );
                                species.network.push_back( cell );
                                changes = true;
                            }
                        }
                        break;
                        case E_MIGRATE:
                        {
                            std::vector< PowerProtein > powers;
                            SearchProtein( species.network, expr.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                Cell* cell = species.network[ powers[ c ].cell_id ];
                                int x = 0;
                                int y = 0;
                                bool place = FindPlaceForCell( species.network, cell->GetX(), cell->GetY(), cell->GetProteinRadius(), x, y );
                                if( place == true )
                                {
                                    species.network[ i ]->SetX( x );
                                    species.network[ i ]->SetY( y );
                                }
                            }
                            powers.clear();
                        }
                        break;
                        case E_PROTEIN:
                        {
                            if( species.network[ i ]->GetProtein() != expr.value )
                            {
                                species.network[ i ]->SetProtein( expr.value );
                                changes = true;
                            }
                        }
                        break;
                        case E_DENDRITE:
                        {
                            std::vector< PowerProtein > powers;
                            SearchProtein( species.network, expr.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                species.network[ i ]->AddSynapse( powers[ c ].power, false, species.network[ powers[ c ].cell_id ] );
                            }
                            powers.clear();
                        }
                        break;
                        case E_AXON:
                        {
                            std::vector< PowerProtein > powers;
                            SearchProtein( species.network, expr.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                species.network[ powers[ c ].cell_id ]->AddSynapse( powers[ c ].power, false, species.network[ i ] );
                            }
                            powers.clear();
                        }
                        break;
                    }
                }
            }
        }

        species_genes.clear();
    }



    m_Generations[ cur_gen ].species.push_back( species );



    entity->AddNetwork( species.network, m_Generations.size() - 1, m_Generations[ cur_gen ].species.size() - 1 );
}



void
Ontogenesis::UpdateFitness( const float delta, const size_t generation_id, const size_t species_id )
{
    m_Generations[ generation_id ].species[ species_id ].fitness += delta;
    if( m_Generations[ generation_id ].top_fitness < m_Generations[ generation_id ].species[ species_id ].fitness )
    {
        m_Generations[ generation_id ].top_fitness = m_Generations[ generation_id ].species[ species_id ].fitness;
    }
}



bool
Ontogenesis::SearchProtein( std::vector< Cell* >& network, const int protein, const int x, const int y, std::vector< PowerProtein >& powers )
{
    for( size_t i = 0; i < network.size(); ++i )
    {
        if( network[ i ]->GetProtein() == protein )
        {
            int x2 = network[ i ]->GetX();
            int y2 = network[ i ]->GetY();
            float distance = sqrt( ( x2 - x ) * ( x2 - x ) + ( y2 - y ) * ( y2 - y ) );
            if( distance < network[ i ]->GetProteinRadius() )
            {
                PowerProtein pp;
                pp.power = 1.0;
                pp.cell_id = i;
                powers.push_back( pp );
            }
        }
    }

    if( powers.size() > 0 )
    {
        return true;
    }
    return false;
}



bool
Ontogenesis::FindPlaceForCell( std::vector< Cell* >& network, const int x, const int y, const int radius, int &new_x, int &new_y )
{
    for( int r = 1; r <= radius; ++r )
    {
        new_y = y - r;
        for( int i = -r + 1; i <= r; ++i )
        {
            new_x = x + i;
            if( IsCell( network, new_x, new_y ) == false )
            {
                return true;
            }
        }
        for( int i = -r + 1; i <= r; ++i )
        {
            new_y = y + i;
            if( IsCell( network, new_x, new_y ) == false )
            {
                return true;
            }
        }
        for( int i = r - 1; i <= -r; --i )
        {
            new_x = x + i;
            if( IsCell( network, new_x, new_y ) == false )
            {
                return true;
            }
        }
        for( int i = r - 1; i <= -r; --i )
        {
            new_y = y + i;
            if( IsCell( network, new_x, new_y ) == false )
            {
                return true;
            }
        }
    }

    return false;
}



bool
Ontogenesis::IsCell( std::vector< Cell* >& network, const int x, const int y )
{
    for( size_t i = 0; i < network.size(); ++i )
    {
        if( network[ i ]->GetX() == x && network[ i ]->GetY() == y )
        {
            return true;
        }
    }

    return false;
}



std::vector< Ontogenesis::Gene >
Ontogenesis::Mutate( std::vector< Ontogenesis::Gene >& genome )
{
    std::vector< Gene > mutated;

    // Genome mutation: Another important kind of mutation that is found in nature as
    // well is mutation on the genome level: deletion, insertion, and doubling of entire
    // genes. For example, successful genes (e.g., structure genes that build a layer of
    // neurons) can be doubled while other useless genes may vanish entirely. Gene
    // doubling has become a success story in evolution. Indeed, it was shown recently
    // [20] that initially simple morphological genes (sometimes even entire genomes)
    // often doubled during evolution, only to subsequently specialize

    // allow only 1 mutation per mutate function
    bool mutation = false;
/*
    for( size_t i = 0; i < genome.size(); ++i )
    {
        // chance to be deleted 1%
        if( ( mutation == true ) || ( (rand() % 100 ) != 1 ) )
        {
            mutated.push_back( genome[ i ] );

            // chance to be duplicated 5%
            if( ( mutation == false ) && ( ( rand() % 100 ) < 5 ) )
            {
                mutation = true;
                Gene gene = genome[ i ];
                gene.unique_id = m_GeneUniqueId;
                mutated.push_back( gene );
                ++m_GeneUniqueId;
            }
        }
        else
        {
            mutation = true;
        }
    }
*/
    // insert new genes only if genome is empty
    // add one random condition and one random expression
    if( genome.size() == 0 )
    {
        mutation = true;
        Gene gene;
        gene.unique_id = m_GeneUniqueId;
        gene.conserv = 0.0f;
        Condition cond;
        cond.type = ( ConditionType )( rand() % C_TOTAL );
        switch( cond.type )
        {
            case C_NAME:
            case C_NNAME:
            {
                cond.value = rand() % Cell::TOTAL;
            }
            break;
            case C_PROTEIN:
            case C_NPROTEIN:
            {
                cond.value = rand() % MAX_PROTEIN;;
            }
            break;
        }
        gene.cond.push_back( cond );
        Expression expr;
        expr.type = ( ExpressionType )( rand() % E_TOTAL );
        switch( expr.type )
        {
            // now only neuron allowed for expression
            // other cells are fixed
            case E_NAME:
            case E_SPLIT:
            {
                cond.value = Cell::NEURON;
            }
            break;
            case E_MIGRATE:
            case E_PROTEIN:
            case E_DENDRITE:
            case E_AXON:
            {
                cond.value = rand() % MAX_PROTEIN;;
            }
            break;
        }
        gene.expr.push_back( cond );
        mutated.push_back( gene );
        ++m_GeneUniqueId;
    }



/*
    // Point mutations: These are restricted to one gene. A randomly chosen condition
    // atom or expression command of a randomly chosen gene changes to its nearest
    // neighbor in genetic space. This means that if, for example, the condition atom
    // ADD[IP2] has been chosen, mutation changes either the substrate (e.g., to ADD[IP1] )
    // or the operation (e.g., to MUL[IP2] ) but never both.
    for( size_t i = 0; i < mutated.size(); ++i )
    {
        for( size_t cond_id = 0; cond_id < mutated[ i ].cond.size(); ++cond_id )
        {
            // chance to be mutated
            if( ( mutation == false ) && ( (rand() % 100 ) < 30 ) )
            {
                mutation = true;
                mutated[ i ].cond[ cond_id ].type = ( ConditionType )( rand() % 3 );
            }
        }

        for( size_t expr_id = 0; expr_id < mutated[ i ].expr.size(); ++expr_id )
        {
            // chance to be mutated
            if( ( mutation == false ) && ( (rand() % 100 ) < 30 ) )
            {
                mutation = true;
                mutated[ i ].expr[ expr_id ].type = ( ExpressionType )( rand() % 5 );
            }
        }
    }
*/

    return mutated;
}



Ogre::String
Ontogenesis::ConditionTypeToString( const ConditionType type )
{
    switch( type )
    {
        case C_NAME: return "NAME";
        case C_NNAME: return "NNAME";
        case C_PROTEIN: return "PROTEIN";
        case C_NPROTEIN: return "NPROTEIN";
    }
    return "UNKNOWN";
}



Ogre::String
Ontogenesis::ExpressionTypeToString( const ExpressionType type )
{
    switch( type )
    {
        case E_NAME: return "NAME";
        case E_SPLIT: return "SPLIT";
        case E_MIGRATE: return "MIGRATE";
        case E_PROTEIN: return "PROTEIN";
        case E_DENDRITE: return "DENDRITE";
        case E_AXON: return "AXON";
    }
    return "UNKNOWN";
}



void
Ontogenesis::DumpGenome( Logger* dump, std::vector< Gene >& genome );
{
    for( size_t i = 0; i < genome.size(); ++i )
    {
        for( size_t cond_id = 0; cond_id < genome[ i ].cond.size(); ++cond_id )
        {
            Condition cond = genome[ i ].cond[ cond_id ];
            export_script->Log( ConditionTypeToString( cond.type ) + "( " );
            switch( cond.type )
            {
                case C_NAME:
                case C_NNAME:
                {
                    export_script->Log( Cell::CellTypeToString( cond.value ) );
                }
                break;
                case C_PROTEIN:
                case C_NPROTEIN:
                {
                    export_script->Log( IntToString( cond.value ) );
                }
                break;
            }
            export_script->Log( " ) " );
        }

        export_script->Log( ": " );

        for( size_t expr_id = 0; expr_id < genome[ i ].expr.size(); ++expr_id )
        {
            Expression expr = genome[ i ].expr[ expr_id ];
            export_script->Log( ExpressionTypeToString( cond.type ) + "( " );
            switch( expr.type )
            {
                case E_NAME:
                case E_SPLIT:
                {
                    export_script->Log( Cell::CellTypeToString( expr.value ) );
                }
                break;
                case E_MIGRATE:
                case E_PROTEIN:
                case E_DENDRITE:
                case E_AXON:
                {
                    export_script->Log( IntToString( expr.value ) );
                }
                break;
            }
            export_script->Log( " ) " );
        }
    }
}
