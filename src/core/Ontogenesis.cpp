#include "Ontogenesis.h"

#include "DebugDraw.h"
#include "Logger.h"



int protein_sensor_food = 1;
int protein_activator_forward = 2;
int protein_activator_left = 3;
int protein_activator_right = 4;



Ontogenesis::Ontogenesis()
{
    Generation generation;
    generation.top_fitness = 0.0f;

    Gene gene;
    Condition cond;
    Expression expr;

    // Split: C_NAME( "stem" ) -> E_NAME( "neuron" ) E_SPLIT( "neuron" )
    gene.name = "Split";
    cond.type = Ontogenesis::C_NAME;
    cond.value = Cell::STEM;
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_NAME;
    expr.value = Cell::NEURON;
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_SPLIT;
    expr.value = Cell::NEURON;
    gene.expr.push_back( expr );
    generation.base_genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    // ConnectLR: C_NAME( "neuron" ) -> E_DENDRITE( "sensor_food" ) E_AXON( "activator_left" ) E_AXON( "activator_right" )
    gene.name = "ConnectLR";
    cond.type = Ontogenesis::C_NAME;
    cond.value = Cell::NEURON;
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_DENDRITE;
    expr.value = protein_sensor_food;
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.value = protein_activator_left;
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.value = protein_activator_right;
    gene.expr.push_back( expr );
    generation.base_genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    // ConnectForward: C_NAME( "neuron" ) -> E_DENDRITE( "sensor_food" ) E_AXON( "activator_forward" ) E_MIGRATE( "activator_forward" )
    gene.name = "ConnectForward";
    cond.type = Ontogenesis::C_NAME;
    cond.value = Cell::NEURON;
    gene.cond.push_back( cond );
    expr.type = Ontogenesis::E_DENDRITE;
    expr.value = protein_sensor_food;
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_AXON;
    expr.value = protein_activator_forward;
    gene.expr.push_back( expr );
    expr.type = Ontogenesis::E_MIGRATE;
    expr.value = protein_activator_forward;
    gene.expr.push_back( expr );
    generation.base_genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    m_Generations.push_back( generation );
}



Ontogenesis::~Ontogenesis()
{
}




void
Ontogenesis::Draw( const unsigned int x, const unsigned int y )
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    size_t cur_gen = m_Generations.size() - 1;
    DEBUG_DRAW.Text( x, y, "Generation " + IntToString( cur_gen ) );
    DEBUG_DRAW.Text( x + 200, y, "Top fitness: " + IntToString( m_Generations[ cur_gen ].top_fitness ) );
    DEBUG_DRAW.Text( x + 400, y, "Number of species: " + IntToString( m_Generations[ cur_gen ].species.size() ) );
}



void
Ontogenesis::LoadNetwork( Entity* entity )
{
    Logger* export_script = new Logger( "onthogenesis.txt" );



    size_t cur_gen = m_Generations.size() - 1;
    if( m_Generations[ cur_gen ].species.size() >= 5 )
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

    export_script->Log( "Step: 0\n" );
    export_script->Log( "    add stem sensor_food sensor_food activator_forward activator_left activator_right\n" );



    bool changes = true;
    int cycles = 0;
    while( changes == true && cycles < 10 )
    {
        ++cycles;
        changes = false;

        export_script->Log( "Step: " + IntToString( cycles ) + "\n" );

        std::vector< std::vector< unsigned int > > species_genes;

        for( size_t i = 0; i < species.network.size(); ++i )
        {
            std::vector< unsigned int > expr_genes;

            export_script->Log( "    cell_" + IntToString( i ) + ": " + IntToString( species.network[ i ]->GetType() ) + "\n" );

            for( size_t gene_id = 0; gene_id < species.genome.size(); ++gene_id )
            {
                Gene gene = species.genome[ gene_id ];
                bool exec = true;

                export_script->Log( "        gene " + gene.name + "\n" );

                for( size_t cond_id = 0; cond_id < gene.cond.size(); ++cond_id )
                {
                    Condition cond = gene.cond[ cond_id ];

                    switch( cond.type )
                    {
                        case Ontogenesis::C_NAME:
                        {
                            if( species.network[ i ]->GetType() != cond.value )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_NNAME:
                        {
                            if( species.network[ i ]->GetType() == cond.value )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_PROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchProtein( species.network, cond.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            if( power == false )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case Ontogenesis::C_NPROTEIN:
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
                    export_script->Log( "            activated\n" );
                    expr_genes.push_back( gene_id );
                }
            }

            species_genes.push_back( expr_genes );
        }

        export_script->Log( "\nexpress\n" );

        size_t net_size = species.network.size();
        for( size_t i = 0; i < net_size; ++i )
        {
            export_script->Log( "    cell_" + IntToString( i ) + ": " + IntToString( species.network[ i ]->GetType() ) + "\n" );

            for( size_t ge_id = 0; ge_id < species_genes[ i ].size(); ++ge_id )
            {
                size_t gene_id = species_genes[ i ][ ge_id ];

                for( size_t expr_id = 0; expr_id < species.genome[ gene_id ].expr.size(); ++expr_id )
                {
                    Expression expr = species.genome[ gene_id ].expr[ expr_id ];

                    switch( expr.type )
                    {
                        case Ontogenesis::E_NAME:
                        {
                            if( species.network[ i ]->GetType() != expr.value )
                            {
                                species.network[ i ]->SetType( ( Cell::CellType )expr.value );
                                changes = true;
                                export_script->Log( "        E_NAME(\"" + IntToString( expr.value ) + "\") expressed.\n" );
                            }
                            else
                            {
                                export_script->Log( "        E_NAME(\"" + IntToString( expr.value ) + "\") depressed.\n" );
                            }
                        }
                        break;
                        case Ontogenesis::E_SPLIT:
                        {
                            int x = 0;
                            int y = 0;
                            bool place = FindPlaceForCell( species.network, species.network[ i ]->GetX(), species.network[ i ]->GetY(), 1, x, y );
                            if( place == true )
                            {
                                Cell* cell = new Cell( entity, ( Cell::CellType )expr.value, x, y );
                                species.network.push_back( cell );
                                changes = true;
                                export_script->Log( "        E_SPLIT(\"" + IntToString( expr.value ) + "\") expressed.\n" );
                            }
                            else
                            {
                                export_script->Log( "        E_SPLIT(\"" + IntToString( expr.value ) + "\") no place.\n" );
                            }
                        }
                        break;
                        case Ontogenesis::E_MIGRATE:
                        {
                            export_script->Log( "        E_MIGRATE(\"" + IntToString( expr.value ) + "\") expressed.\n" );
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
                        case Ontogenesis::E_PROTEIN:
                        {
                            if( species.network[ i ]->GetProtein() != expr.value )
                            {
                                species.network[ i ]->SetProtein( expr.value );
                                changes = true;
                                export_script->Log( "        E_PROTEIN(\"" + IntToString( expr.value ) + "\") expressed.\n" );
                            }
                            else
                            {
                                export_script->Log( "        E_PROTEIN(\"" + IntToString( expr.value ) + "\") depressed.\n" );
                            }
                        }
                        break;
                        case Ontogenesis::E_DENDRITE:
                        {
                            export_script->Log( "        E_DENDRITE(\"" + IntToString( expr.value ) + "\") expressed.\n" );
                            std::vector< PowerProtein > powers;
                            SearchProtein( species.network, expr.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                species.network[ i ]->AddSynapse( powers[ c ].power, false, species.network[ powers[ c ].cell_id ] );
                            }
                            powers.clear();
                        }
                        break;
                        case Ontogenesis::E_AXON:
                        {
                            export_script->Log( "        E_AXON(\"" + IntToString( expr.value ) + "\") expressed.\n" );
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
Ontogenesis::UpdateFitness( const float fitness, const size_t generation_id, const size_t species_id )
{
    m_Generations[ generation_id ].species[ species_id ].fitness = fitness;
    if( m_Generations[ generation_id ].top_fitness < fitness )
    {
        m_Generations[ generation_id ].top_fitness = fitness;
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

    bool mutation = false;
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
                gene.name = gene.name + "X";
                mutated.push_back( gene );
            }
        }
        else
        {
            mutation = true;
        }
    }



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


    return mutated;
}
