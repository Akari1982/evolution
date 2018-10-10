#include "Ontogenesis.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"



const int MAX_PROTEIN = 7;
const int MAX_CELL_PER_SPLIT = 4;
const int MAX_CELL = 10;
const int SPECIES_PER_GENERATION = 50;
const int GENES_PER_GENOME = 7;
const int COND_PER_GENE = 3;
const int EXPR_PER_GENE = 5;
const int CHANGES_PER_MUTATION = 3;



Ontogenesis::Ontogenesis( const Ogre::String& file_prefix ):
    m_FilePrefix( file_prefix ),
    m_GeneUniqueId( 0 )
{
}



Ontogenesis::~Ontogenesis()
{
}




void
Ontogenesis::Draw( const unsigned int x, const unsigned int y )
{
    float scale = 8.0f;

    int cur_gen = m_Generations.size() - 1;
    if( cur_gen >= 0 )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        DEBUG_DRAW.Text( x, y, "Generation " + IntToString( m_Generations[ cur_gen ].id ) );
        DEBUG_DRAW.Text( x + 200, y, "Top fitness: " + IntToString( ( int )m_Generations[ cur_gen ].top_fitness ) + " (" + IntToString( m_Generations[ cur_gen ].top_id ) + ")" );
        DEBUG_DRAW.Text( x + 400, y, "Number of species: " + IntToString( m_Generations[ cur_gen ].species.size() ) );

        for( size_t s = 0; s < m_Generations[ cur_gen ].species.size(); ++s )
        {
            std::vector< Protein > proteins = m_Generations[ cur_gen ].species[ s ].proteins;
            for( size_t i = 0; i < proteins.size(); ++i )
            {
                for( size_t j = 0; j < proteins[ i ].data.size(); ++j )
                {
                    float x1 = 50 + s * 120 + x + proteins[ i ].data[ j ].x * scale;
                    float y1 = 120 + y + proteins[ i ].data[ j ].y * scale;

                    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 2 * proteins[ i ].data[ j ].power ) );
                    DEBUG_DRAW.Quad( x1 - scale / 2.0f, y1 - scale / 2.0f, x1 + scale / 2.0f, y1 - scale / 2.0f, x1 + scale / 2.0f, y1 + scale / 2.0f, x1 - scale / 2.0f, y1 + scale / 2.0f );
                }
            }
        }
    }
}



void
Ontogenesis::LoadNetwork( Entity* entity )
{
    int cur_gen = m_Generations.size() - 1;
    if( ( cur_gen < 0 ) || ( m_Generations[ cur_gen ].species.size() >= SPECIES_PER_GENERATION ) )
    {
        Generation generation;
        generation.top_fitness = 0.0f;
        generation.top_id = 0;
        generation.id = 0;

        if( ( cur_gen >= 0 ) && ( m_Generations[ cur_gen ].species.size() >= SPECIES_PER_GENERATION ) )
        {
            // save old generation, this save all species from this generation
            DumpGeneration( m_Generations[ cur_gen ] );

            generation.base_genome = m_Generations[ cur_gen ].species[ m_Generations[ cur_gen ].top_id ].genome;
            generation.id = m_Generations[ cur_gen ].id + 1;
        }

        ++cur_gen;
        generation.file_name = m_FilePrefix + "_" + IntToString( generation.id ) + ".xml";
        m_Generations.push_back( generation );

        // save new generation
        DumpGeneration( generation );
    }



    Species species;
    species.fitness = -1;
    species.genome = Mutate( m_Generations[ cur_gen ].base_genome );



    // Default cells
    int protein_stem = 0;
    int protein_sensor_energy = 6;
    int protein_sensor_food = 1;
    int protein_sensor_enemy = 2;
    int protein_activator_forward = 3;
    int protein_activator_left = 4;
    int protein_activator_right = 5;

    Cell* cell = new Cell( entity, Cell::NEURON_COMMON, 0, 0 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_ENERGY, 0, 3 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD, 5, -5 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD, 5, 5 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_ENEMY, 8, 0 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_FORWARD, -5, 0 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_LEFT, -3, -3 );
    species.network.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_RIGHT, -3, 3 );
    species.network.push_back( cell );

    // default protein
    ProteinData data;
    Protein protein;

    protein.id = protein_sensor_energy;
    data.power = 8.0f;
    data.x = 0;
    data.y = 3;
    protein.data.push_back( data );
    species.proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_sensor_food;
    data.power = 8.0f;
    data.x = 5;
    data.y = -5;
    protein.data.push_back( data );
    species.proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_sensor_food;
    data.power = 8.0f;
    data.x = 5;
    data.y = 5;
    protein.data.push_back( data );
    species.proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_sensor_enemy;
    data.power = 8.0f;
    data.x = 8;
    data.y = 0;
    protein.data.push_back( data );
    species.proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_activator_forward;
    data.power = 6.0f;
    data.x = -5;
    data.y = 0;
    protein.data.push_back( data );
    species.proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_activator_left;
    data.power = 5.0f;
    data.x = -3;
    data.y = -3;
    protein.data.push_back( data );
    species.proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_activator_right;
    data.power = 5.0f;
    data.x = -3;
    data.y = 3;
    protein.data.push_back( data );
    species.proteins.push_back( protein );
    protein.data.clear();



    int cycles = 0;
    while( cycles < 5 )
    {
        ++cycles;

        for( size_t i = 0; i < species.proteins.size(); ++i )
        {
            for( size_t j = 0; j < species.proteins[ i ].data.size(); ++j )
            {
                float power = species.proteins[ i ].data[ j ].power;
                power /= 1.2f;
                if( power < 0.05f )
                {
                    power = 0.0f;
                }
                species.proteins[ i ].data[ j ].power = power;
            }
        }
        for( size_t i = 0; i < species.proteins.size(); ++i )
        {
            size_t size = species.proteins[ i ].data.size();
            for( size_t j = 0; j < size; ++j )
            {
                float power = species.proteins[ i ].data[ j ].power;

                if( power > 0.2f )
                {
                    int x = species.proteins[ i ].data[ j ].x;
                    int y = species.proteins[ i ].data[ j ].y;
                    float add_power = power / 8.0f;

                    AddProtein( species.proteins, species.proteins[ i ].id, add_power, x, y - 1 );
                    AddProtein( species.proteins, species.proteins[ i ].id, add_power, x, y + 1 );
                    AddProtein( species.proteins, species.proteins[ i ].id, add_power, x - 1, y );
                    AddProtein( species.proteins, species.proteins[ i ].id, add_power, x + 1, y );

                    species.proteins[ i ].data[ j ].power -= add_power * 4.0f;
                }
            }
        }
        for( size_t i = 0; i < species.proteins.size(); ++i )
        {
            for( std::vector< ProteinData >::iterator it = species.proteins[ i ].data.begin(); it != species.proteins[ i ].data.end(); )
            {
                if( ( *it ).power < 0.05f )
                {
                    it = species.proteins[ i ].data.erase( it );
                }
                else
                {
                    ++it;
                }
            }
        }
        for( std::vector< Protein >::iterator it = species.proteins.begin(); it != species.proteins.end(); )
        {
            if( ( *it ).data.size() == 0 )
            {
                it = species.proteins.erase( it );
            }
            else
            {
                ++it;
            }
        }

        for( size_t i = 0; i < species.network.size() && i < MAX_CELL; ++i )
        {
            // express genes only for neurons
            if( species.network[ i ]->GetType() != Cell::NEURON )
            {
                continue;
            }

            for( size_t gene_id = 0; gene_id < species.genome.size(); ++gene_id )
            {
                Gene gene = species.genome[ gene_id ];
                bool exec = true;

                for( size_t cond_id = 0; cond_id < gene.cond.size(); ++cond_id )
                {
                    Condition cond = gene.cond[ cond_id ];
 
                    switch( cond.type )
                    {
                        case C_PROTEIN:
                        {
                            float power = GetProteinPower( species.proteins, ( int )cond.value1, species.network[ i ]->GetX(), species.network[ i ]->GetY() );
                            if( ( cond.value2 != -1 ) && ( power < cond.value2 ) )
                            {
                                exec = false;
                            }
                            else if( ( cond.value3 != -1 ) && ( power > cond.value3 ) )
                            {
                                exec = false;
                            }
                        }
                        break;
                    }
                }

                if( exec == true )
                {
                    for( size_t expr_id = 0; expr_id < gene.expr.size(); ++expr_id )
                    {
                        Expression expr = gene.expr[ expr_id ];
                        switch( expr.type )
                        {
                            case E_SPLIT:
                            {
                                int x = 0;
                                int y = 0;
                                bool place = FindPlaceForCell( species.network, species.network[ i ]->GetX(), species.network[ i ]->GetY(), 1, x, y );
                                if( place == true )
                                {
                                    Cell* cell = new Cell( entity, species.network[ i ]->GetName(), x, y );
                                    species.network.push_back( cell );
                                }
                            }
                            break;
                            case E_SPAWN:
                            {
                                int x = ( int )expr.value2;
                                int y = ( int )expr.value3;
                                bool occupied = IsCell( species.network, x, y );
                                if( occupied == false )
                                {
                                    Cell* cell = new Cell( entity, ( Cell::CellName )( int )expr.value1, x, y );
                                    species.network.push_back( cell );
                                }
                            }
                            break;
                            case E_PROTEIN:
                            {
                                AddProtein( species.proteins, ( int )expr.value1, expr.value2, species.network[ i ]->GetX(), species.network[ i ]->GetY() );
                            }
                            break;
                            case E_DENDRITE:
                            case E_DENDRITE_I:
                            case E_AXON:
                            case E_AXON_I:
                            {
/*
                                std::vector< PowerProtein > powers;
                                GetProteinPower( species.proteins, ( int )expr.value1, species.network[ i ]->GetX(), species.network[ i ]->GetY(), powers );
                                for( size_t c = 0; c < powers.size(); ++c )
                                {
                                    bool inverted = ( expr.type == E_DENDRITE_I ) || ( expr.type == E_AXON_I );
                                    int type = species.network[ powers[ c ].cell_id ]->GetType();
                                    if( ( expr.type == E_DENDRITE ) || ( expr.type == E_DENDRITE_I ) )
                                    {
                                        if( type == Cell::NEURON || type == Cell::SENSOR )
                                        {
                                            species.network[ i ]->AddSynapse( powers[ c ].power, inverted, species.network[ powers[ c ].cell_id ] );
                                        }
                                    }
                                    else
                                    {
                                        if( type == Cell::ACTIVATOR )
                                        {
                                            species.network[ powers[ c ].cell_id ]->AddSynapse( powers[ c ].power, inverted, species.network[ i ] );
                                        }
                                    }
                                }
                                powers.clear();
*/
                            }
                            break;
                        }
                    }
                }
            }
        }
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

    m_Generations[ generation_id ].species[ species_id ].fitness = fitness;

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

            // resave new and old generation
            DumpGeneration( m_Generations[ generation_id ] );
            DumpGeneration( m_Generations[ generation_id + 1 ] );
        }
    }
}



void
Ontogenesis::LoadGeneration( const Generation& generation )
{
    m_Generations.push_back( generation );
}



void
Ontogenesis::AddProtein( std::vector< Protein >& proteins, const int id, const float power, const int x, const int y )
{
    for( size_t i = 0; i < proteins.size(); ++i )
    {
        if( proteins[ i ].id == id )
        {
            for( size_t j = 0; j < proteins[ i ].data.size(); ++j )
            {
                if( proteins[ i ].data[ j ].x == x && proteins[ i ].data[ j ].y == y )
                {
                    proteins[ i ].data[ j ].power += power;
                    return;
                }
            }

            ProteinData data;
            data.power = power;
            data.x = x;
            data.y = y;
            proteins[ i ].data.push_back( data );
            return;
        }
    }

    ProteinData data;
    Protein protein;
    protein.id = id;
    data.power = power;
    data.x = x;
    data.y = y;
    protein.data.push_back( data );
    proteins.push_back( protein );
}



float
Ontogenesis::GetProteinPower( std::vector< Protein >& proteins, const int protein, const int x, const int y )
{
    float power = 0.0f;

    for( size_t i = 0; i < proteins.size(); ++i )
    {
        if( proteins[ i ].id == protein )
        {
            for( size_t j = 0; j < proteins[ i ].data.size(); ++j )
            {
                if( proteins[ i ].data[ j ].x == x && proteins[ i ].data[ j ].y == y )
                {
                    power = proteins[ i ].data[ j ].power;
                }
            }
        }
    }
    return power;
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
    int changes = 0;



    // insert new genes if genome is empty or if 20% chance
    // add one random condition and one random expression
    if( ( genome.size() == 0 ) || ( ( genome.size() < GENES_PER_GENOME ) && ( ( rand() % 5 ) == 1 ) ) )
    {
        Gene gene;
        gene.id = m_GeneUniqueId;
        gene.conserv = 0.0f;
        Condition cond = GenerateRandomCondition();
        gene.cond.push_back( cond );
        Expression expr = GenerateRandomExpression();
        gene.expr.push_back( expr );
        mutated.push_back( gene );
        ++m_GeneUniqueId;
        ++changes;
    }



    // copy existed genome to mutated and do some mutation
    for( size_t i = 0; i < genome.size(); ++i )
    {
        Gene gene = genome[ i ];

        // chance to be deleted 0-5% (reduced if this is conservative gene)
        // don't delete if this is only gene in genome
        // just don't copy it to new genome
        if( ( changes >= CHANGES_PER_MUTATION ) || ( genome.size() == 1 ) || ( ( rand() % ( int )( 20.0f * ( gene.conserv + 1.0f ) ) ) != 1 ) )
        {
            //gene.conserv += ( 100.0f - gene.conserv ) / 4.0f;
            //gene.conserv = ( gene.conserv > 99.0f ) ? 99.0f : gene.conserv;
            mutated.push_back( gene );

            // chance to be duplicated 20% (not affected by conservativeness not count as chagnes)
            if( ( genome.size() < GENES_PER_GENOME ) && ( ( rand() % 5 ) == 1 ) )
            {
                Gene dup_gene = gene;
                dup_gene.id = m_GeneUniqueId;
                dup_gene.conserv = 0.0f;
                mutated.push_back( dup_gene );
                ++m_GeneUniqueId;
            }
        }
        else
        {
            ++changes;
        }
    }




    for( size_t i = 0; i < mutated.size(); ++i )
    {
        // mutate only if conserv roll pass
        if( ( rand() % 100 ) < 100.0f - mutated[ i ].conserv )
        {
            // remove random condition if there are more than one condition
            // chance 10%
            if( ( changes < CHANGES_PER_MUTATION ) && ( mutated[ i ].cond.size() > 1 ) && ( ( rand() % 100 ) < 10 ) )
            {
                mutated[ i ].cond.erase( mutated[ i ].cond.begin() + ( rand() % mutated[ i ].cond.size() ) );
                ++changes;
                //mutated[ i ].conserv /= 2.0f;
            }
            for( size_t cond_id = 0; cond_id < mutated[ i ].cond.size(); ++cond_id )
            {
                // conditions can interchangebly mutate as they want
                // chance of mutation 0-30%
                if( ( changes < CHANGES_PER_MUTATION ) && ( ( rand() % 100 ) < 30 ) )
                {
                    mutated[ i ].cond[ cond_id ].type = ( ConditionType )( rand() % C_TOTAL );
                    ++changes;
                    //mutated[ i ].conserv /= 2.0f;
                }
                if( ( changes < CHANGES_PER_MUTATION ) && ( ( rand() % 100 ) < 30 ) )
                {
                    GenerateRandomConditionValue( mutated[ i ].cond[ cond_id ] );
                    ++changes;
                    //mutated[ i ].conserv /= 2.0f;
                }
            }
            // add random condition with chance 10%
            if( ( changes < CHANGES_PER_MUTATION ) && ( ( rand() % 100 ) < 10 ) )
            {
                mutated[ i ].cond.push_back( GenerateRandomCondition() );
                ++changes;
                //mutated[ i ].conserv /= 2.0f;
            }



            // remove random expression if there are more than one expression
            // chance 10%
            if( ( changes < CHANGES_PER_MUTATION ) && ( mutated[ i ].expr.size() > 1 ) && ( ( rand() % 100 ) < 5 ) )
            {
                mutated[ i ].expr.erase( mutated[ i ].expr.begin() + ( rand() % mutated[ i ].expr.size() ) );
                ++changes;
                //mutated[ i ].conserv /= 2.0f;
            }
            for( size_t expr_id = 0; expr_id < mutated[ i ].expr.size(); ++expr_id )
            {
                // only protein expression can mutate
                // don't mutate SPLIT because this is the only expression
                // uses cell types instead of protein
                // chance of mutation 0-30%
                ExpressionType type = mutated[ i ].expr[ expr_id ].type;
                if( ( changes < CHANGES_PER_MUTATION ) && ( ( rand() % 100 ) < 30 ) )
                {
                    if( type == E_PROTEIN || type == E_DENDRITE || type == E_DENDRITE_I || type == E_AXON || type == E_AXON_I )
                    {
                        ExpressionType new_type;
                        switch( rand() % 7 )
                        {
                            case 0: new_type = E_PROTEIN; break;
                            case 1: new_type = E_DENDRITE; break;
                            case 2: new_type = E_DENDRITE_I; break;
                            case 3: new_type = E_AXON; break;
                            case 4: new_type = E_AXON_I; break;
                        }

                        if( type != new_type )
                        {
                            mutated[ i ].expr[ expr_id ].type = new_type;
                            ++changes;
                            //mutated[ i ].conserv /= 2.0f;
                        }
                    }
                }
                if( ( changes < CHANGES_PER_MUTATION ) && ( ( rand() % 100 ) < 30 ) )
                {
                    GenerateRandomExpressionValue( mutated[ i ].expr[ expr_id ] );
                    ++changes;
                    //mutated[ i ].conserv /= 2.0f;
                }
            }
            // add random expression with chance 10%
            if( ( changes < CHANGES_PER_MUTATION ) && ( ( rand() % 100 ) < 10 ) )
            {
                mutated[ i ].expr.push_back( GenerateRandomExpression() );
                ++changes;
                //mutated[ i ].conserv /= 2.0f;
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
        case C_PROTEIN:
        {
            cond.value1 = ( rand() % ( MAX_PROTEIN + 1 ) ) - 1;
            cond.value2 = ( rand() % 100 ) / 100.0f;
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
            expr.value1 = rand() % 1;
        }
        break;
        case E_SPAWN:
        {
            expr.value1 = ( float )Cell::NEURON_COMMON;
            expr.value2 = -5 + ( rand() % 10 );
            expr.value3 = -5 + ( rand() % 10 );
        }
        break;
        case E_PROTEIN:
        case E_DENDRITE:
        case E_DENDRITE_I:
        case E_AXON:
        case E_AXON_I:
        {
            expr.value1 = ( float )( ( rand() % ( MAX_PROTEIN + 1 ) ) - 1 );
        }
        break;
    }
}



Ogre::String
Ontogenesis::ConditionTypeToString( const ConditionType type )
{
    switch( type )
    {
        case C_PROTEIN: return "PROTEIN";
    }
    return "UNKNOWN";
}



Ontogenesis::ConditionType
Ontogenesis::ConditionStringToType( const Ogre::String& type )
{
    if( type == "PROTEIN" )
    {
        return C_PROTEIN;
    }
    return C_TOTAL;
}



Ogre::String
Ontogenesis::ExpressionTypeToString( const ExpressionType type )
{
    switch( type )
    {
        case E_SPLIT: return "SPLIT";
        case E_SPAWN: return "SPAWN";
        case E_PROTEIN: return "PROTEIN";
        case E_DENDRITE: return "DENDRITE";
        case E_DENDRITE_I: return "DENDRITE_I";
        case E_AXON: return "AXON";
        case E_AXON_I: return "AXON_I";
    }
    return "UNKNOWN";
}



Ontogenesis::ExpressionType
Ontogenesis::ExpressionStringToType( const Ogre::String& type )
{
    if( type == "SPLIT" )
    {
        return E_SPLIT;
    }
    else if( type == "SPAWN" )
    {
        return E_SPAWN;
    }
    else if( type == "PROTEIN" )
    {
        return E_PROTEIN;
    }
    else if( type == "DENDRITE" )
    {
        return E_DENDRITE;
    }
    else if( type == "DENDRITE_I" )
    {
        return E_DENDRITE_I;
    }
    else if( type == "AXON" )
    {
        return E_AXON;
    }
    else if( type == "AXON_I" )
    {
        return E_AXON_I;
    }
    return E_TOTAL;
}



void
Ontogenesis::DumpGenome( Logger* file, std::vector< Ontogenesis::Gene >& genome )
{
    for( size_t i = 0; i < genome.size(); ++i )
    {
        file->Log( "        <gene id=\"" + IntToString( genome[ i ].id ) + "\" conserv=\"" + FloatToString( genome[ i ].conserv ) + "\">\n" );

        for( size_t cond_id = 0; cond_id < genome[ i ].cond.size(); ++cond_id )
        {
            Condition cond = genome[ i ].cond[ cond_id ];
            file->Log( "            <condition type=\"" + ConditionTypeToString( cond.type ) + "\" " );
            switch( cond.type )
            {
                case C_PROTEIN:
                {
                    file->Log( "value1=\"" + FloatToString( cond.value1 ) + "\" value2=\"" + FloatToString( cond.value2 ) + "\" />\n" );
                }
                break;
            }
        }

        for( size_t expr_id = 0; expr_id < genome[ i ].expr.size(); ++expr_id )
        {
            Expression expr = genome[ i ].expr[ expr_id ];
            file->Log( "            <expression type=\"" + ExpressionTypeToString( expr.type ) + "\" " );
            switch( expr.type )
            {
                case E_SPLIT:
                case E_PROTEIN:
                case E_DENDRITE:
                case E_DENDRITE_I:
                case E_AXON:
                case E_AXON_I:
                {
                    file->Log( "value1=\"" + FloatToString( expr.value1 ) + "\" />\n" );
                }
                break;
                case E_SPAWN:
                {
                    file->Log( "value1=\"" + FloatToString( expr.value1 ) + "\" value2=\"" + FloatToString( expr.value2 ) + "\" value3=\"" + FloatToString( expr.value3 ) + "\" />\n" );
                }
                break;
            }
        }

        file->Log( "        </gene>\n" );
    }
}



void
Ontogenesis::DumpGeneration( Generation& generation )
{
    Logger* dump = new Logger( generation.file_name );
    dump->Log( "<generation id=\"" + IntToString( generation.id ) + "\">\n" );
    dump->Log( "    <base_genome>\n" );
    DumpGenome( dump, generation.base_genome );
    dump->Log( "    </base_genome>\n\n" );

    for( size_t i = 0; i < generation.species.size(); ++i )
    {
        dump->Log( "    <specie id=\"" + IntToString( i ) + "\" fitness=\"" + FloatToString( generation.species[ i ].fitness ) + "\">\n" );
        DumpGenome( dump, generation.species[ i ].genome );
        dump->Log( "    </specie>\n" );
    }
    dump->Log( "</generation>\n" );
}
