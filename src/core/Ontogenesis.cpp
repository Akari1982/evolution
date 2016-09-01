#include "Ontogenesis.h"

#include "DebugDraw.h"
#include "Logger.h"



const int MAX_PROTEIN = 6;



Ontogenesis::Ontogenesis( const Ogre::String& file_prefix ):
    m_FilePrefix( file_prefix ),
    m_GeneUniqueId( 0 )
{
}



Ontogenesis::~Ontogenesis()
{
    for( unsigned int i = 0; i < m_Generations.size(); ++i )
    {
        delete m_Generations[ i ].file;
    }
}




void
Ontogenesis::Draw( const unsigned int x, const unsigned int y )
{
    int cur_gen = m_Generations.size() - 1;
    if( cur_gen >= 0 )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        DEBUG_DRAW.Text( x, y, "Generation " + IntToString( cur_gen ) );
        DEBUG_DRAW.Text( x + 200, y, "Top fitness: " + IntToString( m_Generations[ cur_gen ].top_fitness ) + " (" + IntToString( m_Generations[ cur_gen ].top_id ) + ")" );
        DEBUG_DRAW.Text( x + 400, y, "Number of species: " + IntToString( m_Generations[ cur_gen ].species.size() ) );
    }
}



void
Ontogenesis::LoadNetwork( Entity* entity )
{
    int cur_gen = m_Generations.size() - 1;
    bool need_new_gen = ( m_Generations[ cur_gen ].species.size() >= 20 );
    if( ( cur_gen < 0 ) || ( need_new_gen == true ) )
    {
        ++cur_gen;
        Generation generation;
        generation.top_fitness = 0.0f;
        generation.top_id = 0;
        if( need_new_gen == true )
        {
            generation.base_genome = m_Generations[ cur_gen ].species[ m_Generations[ cur_gen ].top_id ].genome;
        }
        generation.file = new Logger( m_FilePrefix + "_" + cur_gen + ".txt" );
        generation.file->Log( "Generation: " + IntToString( cur_gen ) + "\n" );
        generation.file->Log( "base_genome" );
        DumpGenome( generation.file, generation.base_genome );
        m_Generations.push_back( generation );
    }



    Species species;
    species.genome = Mutate( m_Generations[ cur_gen ].base_genome );



    // Default cells
    int protein_sensor_food_left = 0;
    int protein_sensor_food_right = 1;
    int protein_activator_forward = 2;
    int protein_activator_left = 3;
    int protein_activator_right = 4;

    Cell* cell = new Cell( entity, Cell::NEURON, 0, 0 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD_LEFT, -5, -3 );
    cell->SetOuterProtein( protein_sensor_food_left );
    cell->SetOuterProteinRadius( 8 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD_RIGHT, -5, 3 );
    cell->SetOuterProtein( protein_sensor_food_right );
    cell->SetOuterProteinRadius( 8 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_FORWARD, 5, 0 );
    cell->SetOuterProtein( protein_activator_forward );
    cell->SetOuterProteinRadius( 6 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_LEFT, 3, -3 );
    cell->SetOuterProtein( protein_activator_left );
    cell->SetOuterProteinRadius( 5 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_RIGHT, 3, 3 );
    cell->SetOuterProtein( protein_activator_right );
    cell->SetOuterProteinRadius( 5 );
    species.network.push_back( cell );



    int cycles = 0;
    while( cycles < 5 )
    {
        ++cycles;

        std::vector< std::vector< unsigned int > > species_genes;

        for( size_t i = 0; i < species.network.size(); ++i )
        {
            // express genes only for neurons
            if( species.network[ i ]->GetType() != Cell::NEURON )
            {
                continue;
            }
            std::vector< unsigned int > expr_genes;

            for( size_t gene_id = 0; gene_id < species.genome.size(); ++gene_id )
            {
                Gene gene = species.genome[ gene_id ];
                bool exec = true;

                for( size_t cond_id = 0; cond_id < gene.cond.size(); ++cond_id )
                {
                    Condition cond = gene.cond[ cond_id ];

                    switch( cond.type )
                    {
                        case C_O_PROTEIN:
                        case C_NO_PROTEIN:
                        {
                            std::vector< PowerProtein > powers;
                            bool power = SearchOuterProtein( species.network, cond.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            if( ( ( cond.type == C_O_PROTEIN ) && ( power == false ) ) || ( ( cond.type == C_NO_PROTEIN ) && ( power != false ) ) )
                            {
                                exec = false;
                            }
                        }
                        break;
                        case C_I_PROTEIN:
                        case C_NI_PROTEIN:
                        {
                            bool found = ( species.network[ i ]->GetInnerProtein() == cond.value );
                            if( ( ( cond.type == C_I_PROTEIN ) && ( found == false ) ) || ( ( cond.type == C_NI_PROTEIN ) && ( found == true ) ) )
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
            expr_genes.clear();
        }

        size_t net_size = species.network.size();
        size_t i_active = 0;
        for( size_t i = 0; i < net_size; ++i )
        {
            // express genes only for neurons
            if( species.network[ i ]->GetType() != Cell::NEURON )
            {
                continue;
            }

            for( size_t ge_id = 0; ge_id < species_genes[ i_active ].size(); ++ge_id )
            {
                size_t gene_id = species_genes[ i_active ][ ge_id ];

                for( size_t expr_id = 0; expr_id < species.genome[ gene_id ].expr.size(); ++expr_id )
                {
                    Expression expr = species.genome[ gene_id ].expr[ expr_id ];
                    switch( expr.type )
                    {
                        case E_SPLIT:
                        {
                            int x = 0;
                            int y = 0;
                            bool place = FindPlaceForCell( species.network, species.network[ i ]->GetX(), species.network[ i ]->GetY(), 1, x, y );
                            if( place == true )
                            {
                                Cell* cell = new Cell( entity, ( Cell::CellType )expr.value, x, y );
                                species.network.push_back( cell );
                            }
                        }
                        break;
                        case E_MIGRATE:
                        {
                            std::vector< PowerProtein > powers;
                            SearchOuterProtein( species.network, expr.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                Cell* cell = species.network[ powers[ c ].cell_id ];
                                int x = 0;
                                int y = 0;
                                bool place = FindPlaceForCell( species.network, cell->GetX(), cell->GetY(), cell->GetOuterProteinRadius(), x, y );
                                if( place == true )
                                {
                                    species.network[ i ]->SetX( x );
                                    species.network[ i ]->SetY( y );
                                }
                            }
                            powers.clear();
                        }
                        break;
                        case E_O_PROTEIN:
                        {
                            if( species.network[ i ]->GetOuterProtein() != expr.value )
                            {
                                species.network[ i ]->SetOuterProtein( expr.value );
                            }
                        }
                        break;
                        case E_I_PROTEIN:
                        {
                            if( species.network[ i ]->GetInnerProtein() != expr.value )
                            {
                                species.network[ i ]->SetInnerProtein( expr.value );
                            }
                        }
                        break;
                        case E_DENDRITE:
                        case E_DENDRITE_I:
                        case E_AXON:
                        case E_AXON_I:
                        {
                            std::vector< PowerProtein > powers;
                            SearchOuterProtein( species.network, expr.value, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                            for( size_t c = 0; c < powers.size(); ++c )
                            {
                                bool inverted = ( expr.type == E_DENDRITE_I ) || ( expr.type == E_AXON_I );
                                if( ( expr.type == E_DENDRITE ) || ( expr.type == E_DENDRITE_I ) )
                                {
                                    species.network[ i ]->AddSynapse( powers[ c ].power, inverted, species.network[ powers[ c ].cell_id ] );
                                }
                                else
                                {
                                    if( species.network[ powers[ c ].cell_id ]->GetType() == Cell::NEURON )
                                    {
                                        species.network[ powers[ c ].cell_id ]->AddSynapse( powers[ c ].power, inverted, species.network[ i ] );
                                    }
                                }
                            }
                            powers.clear();
                        }
                        break;
                    }
                }
            }

            ++i_active;
        }

        species_genes.clear();
    }



    m_Generations[ cur_gen ].species.push_back( species );



    entity->AddNetwork( species.network, m_Generations.size() - 1, m_Generations[ cur_gen ].species.size() - 1 );
}



void
Ontogenesis::EntityDeath( Entity* entity )
{
    size_t generation_id = entity->GetGenerationId();
    size_t species_id = entity->GetSpeciesId();
    float fitness = entity->GetFitness();

    generation.file->Log( "Species: " + IntToString( species_id ) + "\n" );
    generation.file->Log( "fitness: " + FloatToString( fitness ) );
    generation.file->Log( "genome" );
    DumpGenome( m_Generations[ generation_id ].file, m_Generations[ generation_id ].species[ species_id ].genome );

    if( m_Generations[ generation_id ].top_fitness <= fitness )
    {
        m_Generations[ generation_id ].top_fitness = fitness;
        m_Generations[ generation_id ].top_id = species_id;

        // if already new generation but some old entity still live and better than old one
        // then update genome for futher use (only for next generation)
        int cur_gen = m_Generations.size() - 1;
        if( cur_gen != generation_id )
        {
            m_Generations[ generation_id + 1 ].base_genome = m_Generations[ generation_id ].species[ species_id ].genome;
        }
    }
}



bool
Ontogenesis::SearchOuterProtein( std::vector< Cell* >& network, const int protein, const int x, const int y, std::vector< PowerProtein >& powers )
{
    for( size_t i = 0; i < network.size(); ++i )
    {
        if( network[ i ]->GetOuterProtein() == protein )
        {
            int x2 = network[ i ]->GetX();
            int y2 = network[ i ]->GetY();
            float distance = sqrt( ( x2 - x ) * ( x2 - x ) + ( y2 - y ) * ( y2 - y ) );
            if( distance < network[ i ]->GetOuterProteinRadius() )
            {
                PowerProtein pp;
                pp.power = ( network[ i ]->GetOuterProteinRadius() / distance ) * 0.5f;
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



    // insert new genes only if genome is empty
    // add one random condition and one random expression
    if( genome.size() == 0 )
    {
        Gene gene;
        gene.unique_id = m_GeneUniqueId;
        gene.conserv = 0.0f;
        Condition cond = GenerateRandomCondition();
        gene.cond.push_back( cond );
        Expression expr = GenerateRandomExpression();
        gene.expr.push_back( expr );
        mutated.push_back( gene );
        ++m_GeneUniqueId;
    }



    // copy existed genome to mutated and do some mutation
    for( size_t i = 0; i < genome.size(); ++i )
    {
        Gene gene = genome[ i ];

        // chance to be deleted 0-20% (reduced if this is conservative gene)
        // don't delete if this is only gene in genome
        // just don't copy it to new genome
        if( ( genome.size() == 1 ) || ( ( rand() % ( int )( 5.0f * ( gene.conserv + 1.0f ) ) ) != 1 ) )
        {
            gene.conserv += ( 100.0f - gene.conserv ) / 4.0f
            gene.conserv = ( gene.conserv > 99.0f ) ? 99.0f : gene.conserv;
            mutated.push_back( gene );

            // chance to be duplicated 5% (not affected by conservativeness)
            // this not counts as mutation
            if( ( rand() % 20 ) == 1 )
            {
                Gene dup_gene = gene;
                dup_gene.unique_id = m_GeneUniqueId;
                dup_gene.conserv = 0.0f;
                mutated.push_back( dup_gene );
                ++m_GeneUniqueId;
            }
        }
    }



    for( size_t i = 0; i < mutated.size(); ++i )
    {
        // mutate only if conserv roll pass
        if( ( rand() % 100 ) < 100.0f - mutated[ i ].conserv )
        {
            // remove random condition if there are more than one condition
            // chance 10%
            if( ( mutated[ i ].cond.size() > 1 ) && ( ( rand() % 100 ) < 10 ) )
            {
                mutated[ i ].cond.erase( mutated[ i ].cond.begin() + ( rand() % mutated[ i ].cond.size() ) );
            }
            for( size_t cond_id = 0; cond_id < mutated[ i ].cond.size(); ++cond_id )
            {
                // conditions can interchangebly mutate as they want
                // chance of mutation 0-30%
                if( ( rand() % 100 ) < 30 )
                {
                    mutated[ i ].cond[ cond_id ].type = ( ConditionType )( rand() % C_TOTAL );
                }
                if( ( rand() % 100 ) < 30 )
                {
                    GenerateRandomConditionValue( mutated[ i ].cond[ cond_id ] );
                }
            }
            // add random condition with chance 10%
            if( ( rand() % 100 ) < 10 )
            {
                mutated[ i ].cond.push_back( GenerateRandomCondition() );
            }



            // remove random expression if there are more than one expression
            // chance 10%
            if( ( mutated[ i ].expr.size() > 1 ) && ( ( rand() % 100 ) < 5 ) )
            {
                mutated[ i ].expr.erase( mutated[ i ].expr.begin() + ( rand() % mutated[ i ].expr.size() ) );
            }
            for( size_t expr_id = 0; expr_id < mutated[ i ].expr.size(); ++expr_id )
            {
                // only protein expression can mutate
                // don't mutate SPLIT because this is the only expression
                // uses cell types instead of protein
                // chance of mutation 0-30%
                ExpressionType type = mutated[ i ].expr[ expr_id ].type;
                if( ( rand() % 100 ) < 30 )
                {
                    if( type == E_MIGRATE || type == E_O_PROTEIN || type == E_I_PROTEIN || type == E_DENDRITE || type == E_DENDRITE_I || type == E_AXON || type == E_AXON_I )
                    {
                        ExpressionType new_type;
                        switch( rand() % 7 )
                        {
                            case 0: new_type = E_MIGRATE; break;
                            case 1: new_type = E_O_PROTEIN; break;
                            case 2: new_type = E_I_PROTEIN; break;
                            case 3: new_type = E_DENDRITE; break;
                            case 4: new_type = E_DENDRITE_I; break;
                            case 5: new_type = E_AXON; break;
                            case 6: new_type = E_AXON_I; break;
                        }

                        if( type != new_type )
                        {
                            mutated[ i ].expr[ expr_id ].type = new_type;
                        }
                    }
                }
                if( ( rand() % 100 ) < 30 )
                {
                    GenerateRandomExpressionValue( mutated[ i ].expr[ expr_id ] );
                }
            }
            // add random expression with chance 10%
            if( ( rand() % 100 ) < 10 )
            {
                mutated[ i ].expr.push_back( GenerateRandomExpression() );
            }
        }
    }



    return mutated;
}



Ontogenesis::Condition
Ontogenesis::GenerateRandomCondition()
{
    Condition cond;
    cond.type = ( ConditionType )( rand() % C_TOTAL );
    GenerateRandomConditionValue( cond );
    return cond;
}



void
Ontogenesis::GenerateRandomConditionValue( Condition& cond )
{
    switch( cond.type )
    {
        case C_O_PROTEIN:
        case C_NO_PROTEIN:
        case C_I_PROTEIN:
        case C_NI_PROTEIN:
        {
            cond.value = rand() % MAX_PROTEIN;
        }
        break;
    }
}



Ontogenesis::Expression
Ontogenesis::GenerateRandomExpression()
{
    Expression expr;
    expr.type = ( ExpressionType )( rand() % E_TOTAL );
    GenerateRandomExpressionValue( expr );
    return expr;
}



void
Ontogenesis::GenerateRandomExpressionValue( Expression& expr )
{
    switch( expr.type )
    {
        // now only neuron allowed for expression
        // other cells are fixed
        case E_SPLIT:
        {
            expr.value = Cell::NEURON;
        }
        break;
        case E_MIGRATE:
        case E_O_PROTEIN:
        case E_I_PROTEIN:
        case E_DENDRITE:
        case E_DENDRITE_I:
        case E_AXON:
        case E_AXON_I:
        {
            expr.value = rand() % MAX_PROTEIN;
        }
        break;
    }
}



Ogre::String
Ontogenesis::ConditionTypeToString( const ConditionType type )
{
    switch( type )
    {
        case C_O_PROTEIN: return "O_PROTEIN";
        case C_NO_PROTEIN: return "NO_PROTEIN";
        case C_I_PROTEIN: return "I_PROTEIN";
        case C_NI_PROTEIN: return "NI_PROTEIN";
    }
    return "UNKNOWN";
}



Ogre::String
Ontogenesis::ExpressionTypeToString( const ExpressionType type )
{
    switch( type )
    {
        case E_SPLIT: return "SPLIT";
        case E_MIGRATE: return "MIGRATE";
        case E_O_PROTEIN: return "O_PROTEIN";
        case E_I_PROTEIN: return "I_PROTEIN";
        case E_DENDRITE: return "DENDRITE";
        case E_DENDRITE_I: return "DENDRITE_I";
        case E_AXON: return "AXON";
        case E_AXON_I: return "AXON_I";
    }
    return "UNKNOWN";
}



void
Ontogenesis::DumpGenome( Logger* file, std::vector< Ontogenesis::Gene >& genome )
{
    for( size_t i = 0; i < genome.size(); ++i )
    {
        file->Log( "\n    " );

        for( size_t cond_id = 0; cond_id < genome[ i ].cond.size(); ++cond_id )
        {
            Condition cond = genome[ i ].cond[ cond_id ];
            file->Log( ConditionTypeToString( cond.type ) + "(" );
            switch( cond.type )
            {
                case C_O_PROTEIN:
                case C_NO_PROTEIN:
                case C_I_PROTEIN:
                case C_NI_PROTEIN:
                {
                    file->Log( IntToString( cond.value ) );
                }
                break;
            }
            file->Log( ") " );
        }

        file->Log( ": " );

        for( size_t expr_id = 0; expr_id < genome[ i ].expr.size(); ++expr_id )
        {
            Expression expr = genome[ i ].expr[ expr_id ];
            file->Log( ExpressionTypeToString( expr.type ) + "(" );
            switch( expr.type )
            {
                case E_SPLIT:
                {
                    file->Log( Cell::CellTypeToString( ( Cell::CellType )expr.value ) );
                }
                break;
                case E_MIGRATE:
                case E_O_PROTEIN:
                case E_I_PROTEIN:
                case E_DENDRITE:
                case E_DENDRITE_I:
                case E_AXON:
                case E_AXON_I:
                {
                    file->Log( IntToString( expr.value ) );
                }
                break;
            }
            file->Log( ") " );
        }
    }
}
