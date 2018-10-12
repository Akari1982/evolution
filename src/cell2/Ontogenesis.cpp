#include "Ontogenesis.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"



const int MAX_CELL = 4000;



Ontogenesis::Ontogenesis()
{
    // test genome
    Gene gene;
    gene.id = 0;
    Condition cond;
    cond.type = ConditionType::C_PROTEIN;
    cond.value1 = 0;
    cond.value2 = 0.5f;
    cond.value3 = -1;
    gene.cond.push_back( cond );
    Expression expr;
    expr.type = ExpressionType::E_SPLIT;
    gene.expr.push_back( expr );
    m_Genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    gene.id = 1;
    cond.type = ConditionType::C_PROTEIN;
    cond.value1 = 0;
    cond.value2 = -1;
    cond.value3 = 1.0f;
    gene.cond.push_back( cond );
    expr.type = ExpressionType::E_PROTEIN;
    expr.value1 = 1;
    expr.value2 = 1.0f;
    gene.expr.push_back( expr );
    m_Genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    gene.id = 2;
    cond.type = ConditionType::C_PROTEIN;
    cond.value1 = 1;
    cond.value2 = -1;
    cond.value3 = 1.0f;
    gene.cond.push_back( cond );
    expr.type = ExpressionType::E_PROTEIN;
    expr.value1 = 0;
    expr.value2 = 1.0f;
    gene.expr.push_back( expr );
    m_Genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();

    // default cell
    Cell* cell = new Cell();
    cell->SetX( 20 );
    cell->SetY( 20 );
    m_Cells.push_back( cell );

    // default protein
    ProteinData data;
    Protein protein;

    protein.id = 0;
    data.power = 10.0f;
    data.x = 20;
    data.y = 20;
    protein.data.push_back( data );
    m_Proteins.push_back( protein );
    protein.data.clear();
}



Ontogenesis::~Ontogenesis()
{
    for( size_t i = 0; i < m_Cells.size(); ++i )
    {
        delete m_Cells[ i ];
    }
}




void
Ontogenesis::Draw()
{
    float scale = 20.0f;

    DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );
    DEBUG_DRAW.Text( 0, 10, "Cells:" );
    for( size_t i = 0; i < m_Cells.size(); ++i )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 0.5 ) );
        DEBUG_DRAW.Disc( m_Cells[ i ]->GetX() * scale, m_Cells[ i ]->GetY() * scale, 10 );
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        DEBUG_DRAW.SetTextAlignment( DebugDraw::CENTER );
        DEBUG_DRAW.Text( ( m_Cells[ i ]->GetX() ) * scale, ( m_Cells[ i ]->GetY() - 0.5f ) * scale, Ogre::StringConverter::toString( i ) );
        DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );
        DEBUG_DRAW.Text( 10, 30 + i * 20, Ogre::StringConverter::toString( i ) + ":" + Ogre::StringConverter::toString( m_Cells[ i ]->GetDivision() ) );
    }

    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        for( size_t j = 0; j < m_Proteins[ i ].data.size(); ++j )
        {
            float x = m_Proteins[ i ].data[ j ].x;
            float y = m_Proteins[ i ].data[ j ].y;

            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 2 * m_Proteins[ i ].data[ j ].power ) );
            DEBUG_DRAW.Quad( x * scale - scale / 2.0f, y * scale - scale / 2.0f, x * scale + scale / 2.0f, y * scale - scale / 2.0f, x * scale + scale / 2.0f, y * scale + scale / 2.0f, x * scale - scale / 2.0f, y * scale + scale / 2.0f );
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
            DEBUG_DRAW.Text( ( x - 0.5f ) * scale, ( y - 0.5f ) * scale, Ogre::StringConverter::toString( ( int )m_Proteins[ i ].data[ j ].power ) );
        }
    }

    DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    for( size_t i = 0; i < m_Genome.size(); ++i )
    {
        DEBUG_DRAW.Text( 810, 10 + i * 20.0f, "Gene" + Ogre::StringConverter::toString( m_Genome[ i ].id ) + ": " );
/*
        for( size_t cond_id = 0; cond_id < m_Genome[ i ].cond.size(); ++cond_id )
        {
            Condition cond = m_Genome[ i ].cond[ cond_id ];
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
                {
                    file->Log( "value1=\"" + FloatToString( expr.value1 ) + "\" />\n" );
                }
                break;
            }
        }
*/
    }
}



void
Ontogenesis::Step()
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

                    case C_DIVISION:
                    {
                        int div = m_Cells[ i ]->GetDivision();
                        if( ( div < cond.value1 ) || ( div > cond.value2 ) )
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
                        case E_PROTEIN:
                        {
                            AddProtein( ( int )expr.value1, expr.value2, m_Cells[ i ]->GetX(), m_Cells[ i ]->GetY() );
                        }
                        break;

                        case E_SPLIT:
                        {
                            if( m_Cells.size() < MAX_CELL )
                            {
                                int direction = std::rand() % 8;
                                int dir_x = 0;
                                int dir_y = 0;
                                DirToMove( direction, dir_x, dir_y );
                                Cell* dir_cell = GetCell( m_Cells[ i ]->GetX() + dir_x, m_Cells[ i ]->GetY() + dir_y );
                                if( dir_cell != NULL )
                                {
                                    MoveCell( dir_cell, dir_x, dir_y );
                                }

                                m_Cells[ i ]->SetDivision( m_Cells[ i ]->GetDivision() + 1 );

                                Cell* cell = new Cell();
                                cell->SetX( m_Cells[ i ]->GetX() + dir_x );
                                cell->SetY( m_Cells[ i ]->GetY() + dir_y );
                                cell->SetDivision( m_Cells[ i ]->GetDivision() );
                                m_Cells.push_back( cell );
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}



float
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



void
Ontogenesis::MoveCell( Cell* cell, const int dir_x, const int dir_y )
{
    int x = cell->GetX() + dir_x;
    int y = cell->GetY() + dir_y;

    Cell* dir_cell = GetCell( x, y );
    if( dir_cell != NULL )
    {
        MoveCell( dir_cell, dir_x, dir_y );
    }

    cell->SetX( x );
    cell->SetY( y );
}



void
Ontogenesis::AddCell( const int x, const int y )
{
    Cell* cell = new Cell();
    cell->SetX( x );
    cell->SetY( y );
    m_Cells.push_back( cell );
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



Cell*
Ontogenesis::GetCell( const int x, const int y )
{
    for( size_t i = 0; i < m_Cells.size(); ++i )
    {
        if( m_Cells[ i ]->GetX() == x && m_Cells[ i ]->GetY() == y )
        {
            return m_Cells[ i ];
        }
    }

    return NULL;
}



void
Ontogenesis::DirToMove( const int dir, int& x, int& y )
{
    switch( dir )
    {
        case 0: x = 0; y = -1; return;
        case 1: x = 1; y = -1; return;
        case 2: x = 1; y = 0; return;
        case 3: x = 1; y = 1; return;
        case 4: x = 0; y = 1; return;
        case 5: x = -1; y = 1; return;
        case 6: x = -1; y = 0; return;
        case 7: x = -1; y = -1; return;
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



Ogre::String
Ontogenesis::ExpressionTypeToString( const ExpressionType type )
{
    switch( type )
    {
        case E_SPLIT: return "SPLIT";
        case E_SPLIT_GRADIENT: return "SPLIT_GRADIENT";
        case E_MIGRATE: return "MIGRATE";
        case E_PROTEIN: return "PROTEIN";
        case E_DENDRITE: return "DENDRITE";
        case E_DENDRITE_I: return "DENDRITE_I";
        case E_AXON: return "AXON";
        case E_AXON_I: return "AXON_I";
    }
    return "UNKNOWN";
}
