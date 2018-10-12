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



Ontogenesis::Ontogenesis()
{
    // Default cells
    int protein_stem = 0;
    int protein_sensor_energy = 6;
    int protein_sensor_food = 1;
    int protein_sensor_enemy = 2;
    int protein_activator_forward = 3;
    int protein_activator_left = 4;
    int protein_activator_right = 5;

    Cell* cell = new Cell( entity, Cell::NEURON_COMMON, 0, 0 );
    m_Cells.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_ENERGY, 0, 3 );
    m_Cells.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD, 5, -5 );
    m_Cells.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_FOOD, 5, 5 );
    m_Cells.push_back( cell );

    cell = new Cell( entity, Cell::SENSOR_ENEMY, 8, 0 );
    m_Cells.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_FORWARD, -5, 0 );
    m_Cells.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_LEFT, -3, -3 );
    m_Cells.push_back( cell );

    cell = new Cell( entity, Cell::ACTIVATOR_RIGHT, -3, 3 );
    m_Cells.push_back( cell );

    // default protein
    ProteinData data;
    Protein protein;

    protein.id = protein_sensor_energy;
    data.power = 8.0f;
    data.x = 0;
    data.y = 3;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_sensor_food;
    data.power = 8.0f;
    data.x = 5;
    data.y = -5;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_sensor_food;
    data.power = 8.0f;
    data.x = 5;
    data.y = 5;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_sensor_enemy;
    data.power = 8.0f;
    data.x = 8;
    data.y = 0;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_activator_forward;
    data.power = 6.0f;
    data.x = -5;
    data.y = 0;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_activator_left;
    data.power = 5.0f;
    data.x = -3;
    data.y = -3;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();

    protein.id = protein_activator_right;
    data.power = 5.0f;
    data.x = -3;
    data.y = 3;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();
}



Ontogenesis::~Ontogenesis()
{
}



void
Ontogenesis::DevelopmentStep()
{
    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        for( size_t j = 0; j < m_Proteins[ i ].data.size(); ++j )
        {
            float power = m_Proteins[ i ].data[ j ].power;
            power /= 1.2f;
            if( power < 0.05f )
            {
                power = 0.0f;
            }
            m_Proteins[ i ].data[ j ].power = power;
        }
    }
    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        size_t size = m_Proteins[ i ].data.size();
        for( size_t j = 0; j < size; ++j )
        {
            float power = m_Proteins[ i ].data[ j ].power;

            if( power > 0.2f )
            {
                int x = m_Proteins[ i ].data[ j ].x;
                int y = m_Proteins[ i ].data[ j ].y;
                float add_power = power / 8.0f;

                AddProtein( m_Proteins[ i ].id, add_power, x, y - 1 );
                AddProtein( m_Proteins[ i ].id, add_power, x, y + 1 );
                AddProtein( m_Proteins[ i ].id, add_power, x - 1, y );
                AddProtein( m_Proteins[ i ].id, add_power, x + 1, y );

                m_Proteins[ i ].data[ j ].power -= add_power * 4.0f;
            }
        }
    }
    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        for( std::vector< ProteinData >::iterator it = m_Proteins[ i ].data.begin(); it != m_Proteins[ i ].data.end(); )
        {
            if( ( *it ).power < 0.05f )
            {
                it = m_Proteins[ i ].data.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }
    for( std::vector< Protein >::iterator it = m_Proteins.begin(); it != m_Proteins.end(); )
    {
        if( ( *it ).data.size() == 0 )
        {
            it = m_Proteins.erase( it );
        }
        else
        {
            ++it;
        }
    }

    size_t size = m_Cells.size();
    for( size_t i = 0; i < size; ++i )
    {
        // express genes only for neurons
        if( m_Cells[ i ]->GetType() != Cell::NEURON )
        {
            continue;
        }

        for( size_t gene_id = 0; gene_id < m_Genome.size(); ++gene_id )
        {
            Gene gene = m_Genome[ gene_id ];
            bool exec = true;

            for( size_t cond_id = 0; cond_id < gene.cond.size(); ++cond_id )
            {
                Condition cond = gene.cond[ cond_id ];

                switch( cond.type )
                {
                    case C_PROTEIN:
                    {
                        float power = GetProteinPower( ( int )cond.value1, m_Cells[ i ]->GetX(), m_Cells[ i ]->GetY() );
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
                            bool place = FindPlaceForCell( m_Cells[ i ]->GetX(), m_Cells[ i ]->GetY(), 1, x, y );
                            if( place == true )
                            {
                                Cell* cell = new Cell( entity, m_Cells[ i ]->GetName(), x, y );
                                m_Cells.push_back( cell );
                            }
                        }
                        break;
                        case E_SPAWN:
                        {
                            int x = ( int )expr.value2;
                            int y = ( int )expr.value3;
                            int occupied = GetCell( x, y );
                            if( occupied == -1 )
                            {
                                Cell* cell = new Cell( entity, ( Cell::CellName )( int )expr.value1, x, y );
                                m_Cells.push_back( cell );
                            }
                        }
                        break;
                        case E_PROTEIN:
                        {
                            AddProtein( ( int )expr.value1, expr.value2, m_Cells[ i ]->GetX(), m_Cells[ i ]->GetY() );
                        }
                        break;
                        case E_DENDRITE:
                        case E_DENDRITE_I:
                        case E_AXON:
                        case E_AXON_I:
                        {
                            int cell_id = FindCellByProteinGradient( ( int )expr.value1, m_Cells[ i ]->GetX(), m_Cells[ i ]->GetY() );
                            if( cell_id != -1 )
                            {
                                bool inverted = ( expr.type == E_DENDRITE_I ) || ( expr.type == E_AXON_I );
                                int type = m_Cells[ cell_id ]->GetType();
                                if( ( expr.type == E_DENDRITE ) || ( expr.type == E_DENDRITE_I ) )
                                {
                                    if( type == Cell::NEURON || type == Cell::SENSOR )
                                    {
                                        m_Cells[ i ]->AddSynapse( 1, inverted, m_Cells[ cell_id ] );
                                    }
                                }
                                else
                                {
                                    if( type == Cell::ACTIVATOR )
                                    {
                                        m_Cells[ cell_id ]->AddSynapse( 1, inverted, m_Cells[ i ] );
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}



void
Ontogenesis::AddProtein( const int id, const float power, const int x, const int y )
{
    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        if( m_Proteins[ i ].id == id )
        {
            for( size_t j = 0; j < m_Proteins[ i ].data.size(); ++j )
            {
                if( m_Proteins[ i ].data[ j ].x == x && m_Proteins[ i ].data[ j ].y == y )
                {
                    m_Proteins[ i ].data[ j ].power += power;
                    return;
                }
            }

            ProteinData data;
            data.power = power;
            data.x = x;
            data.y = y;
            m_Proteins[ i ].data.push_back( data );
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
    m_Proteins.push_back( protein );
}



const float
Ontogenesis::GetProteinPower( const int protein, const int x, const int y )
{
    float power = 0.0f;

    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        if( m_Proteins[ i ].id == protein )
        {
            for( size_t j = 0; j < m_Proteins[ i ].data.size(); ++j )
            {
                if( m_Proteins[ i ].data[ j ].x == x && m_Proteins[ i ].data[ j ].y == y )
                {
                    power = m_Proteins[ i ].data[ j ].power;
                }
            }
        }
    }
    return power;
}



const int
Ontogenesis::FindCellByProteinGradient( const int protein, const int x, const int y )
{
    float power = -1;
    int ret_x;
    int ret_y;

    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        if( m_Proteins[ i ].id == protein )
        {
            power = FindProteinGradient( m_Proteins[ i ].data, x, y, 0, ret_x, ret_y );
            break;
        }
    }

    if( power > -1 )
    {
        return GetCell( m_Cells, ret_x, ret_y );
    }

    return -1;
}



const float
Ontogenesis::FindProteinGradient( std::vector< ProteinData >& data, const int x, const int y, const float power, int& ret_x, int& ret_y )
{
    float power_new = 0.0f;
    for( size_t i = 0; i < data.size(); ++i )
    {
        if( data[ i ].x == x && data[ i ].y == y )
        {
            power_new = data[ i ].power;
        }
    }

    if( power_new > power )
    {
        int new_ret_x;
        int new_ret_y;
        float power1;
        float power_greatest = power_new;

        power1 = FindProteinGradient( data, x + 1, y, power_new, new_ret_x, new_ret_x );
        if( power1 > power_greatest )
        {
            ret_x = new_ret_x;
            ret_y = new_ret_y;
            power_greatest = power1;
        }
        power1 = FindProteinGradient( data, x - 1, y, power_new, new_ret_x, new_ret_x );
        if( power1 > power_greatest )
        {
            ret_x = new_ret_x;
            ret_y = new_ret_y;
            power_greatest = power1;
        }
        power1 = FindProteinGradient( data, x, y + 1, power_new, new_ret_x, new_ret_x );
        if( power1 > power_greatest )
        {
            ret_x = new_ret_x;
            ret_y = new_ret_y;
            power_greatest = power1;
        }
        power1 = FindProteinGradient( data, x, y - 1, power_new, new_ret_x, new_ret_x );
        if( power1 > power_greatest )
        {
            ret_x = new_ret_x;
            ret_y = new_ret_y;
            power_greatest = power1;
        }

        if( power_greatest == power_new )
        {
            ret_x = x;
            ret_y = y;
        }

        return power_greatest;
    }
    else
    {
        return -1;
    }
}



const bool
Ontogenesis::FindPlaceForCell( const int x, const int y, const int radius, int &new_x, int &new_y )
{
    for( int r = 1; r <= radius; ++r )
    {
        new_y = y - r;
        for( int i = -r + 1; i <= r; ++i )
        {
            new_x = x + i;
            if( GetCell( new_x, new_y ) == -1 )
            {
                return true;
            }
        }
        for( int i = -r + 1; i <= r; ++i )
        {
            new_y = y + i;
            if( GetCell( new_x, new_y ) == -1 )
            {
                return true;
            }
        }
        for( int i = r - 1; i <= -r; --i )
        {
            new_x = x + i;
            if( GetCell( new_x, new_y ) == -1 )
            {
                return true;
            }
        }
        for( int i = r - 1; i <= -r; --i )
        {
            new_y = y + i;
            if( GetCell( new_x, new_y ) == -1 )
            {
                return true;
            }
        }
    }

    return false;
}



const int
Ontogenesis::GetCell( const int x, const int y )
{
    for( size_t i = 0; i < m_Cells.size(); ++i )
    {
        if( m_Cells[ i ]->GetX() == x && m_Cells[ i ]->GetY() == y )
        {
            return i;
        }
    }

    return -1;
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
Ontogenesis::SetGenome( std::vector< Gene >& genome )
{
    m_Genome = Mutate( genome );
}



std::vector< Gene >
Ontogenesis::GetGenome() const
{
    return m_Genome;
}



void
Ontogenesis::DumpGenome( Logger* file, std::vector< Ontogenesis::Gene >& genome ) const
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
