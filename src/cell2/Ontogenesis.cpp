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
    cond.value3 = 1.0f;
    gene.cond.push_back( cond );
    cond.type = ConditionType::C_DIVISION;
    cond.value1 = 0;
    cond.value2 = 2;
    gene.cond.push_back( cond );
    Expression expr;
    expr.type = ExpressionType::E_SPLIT;
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
    Protein protein;
    protein.id = 0;
    protein.power = 10.0f;
    protein.x = 20;
    protein.y = 20;
    m_Proteins.push_back( protein );

    protein.id = 1;
    protein.power = 5.0f;
    protein.x = 10;
    protein.y = 20;
    m_Proteins.push_back( protein );

    protein.id = 2;
    protein.power = 5.0f;
    protein.x = 30;
    protein.y = 20;
    m_Proteins.push_back( protein );
}



Ontogenesis::~Ontogenesis()
{
}




void
Ontogenesis::Draw()
{
    float scale = 20.0f;

    DEBUG_DRAW.SetTextAlignment( DebugDraw::CENTER );
    for( size_t i = 0; i < m_Cells.size(); ++i )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 0.5 ) );
        DEBUG_DRAW.Disc( m_Cells[ i ]->GetX() * scale, m_Cells[ i ]->GetY() * scale, 10 );
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        DEBUG_DRAW.Text( ( m_Cells[ i ]->GetX() ) * scale, ( m_Cells[ i ]->GetY() - 0.5f ) * scale, Ogre::StringConverter::toString( i ) );
    }

    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        if( m_Proteins[ i ].id == 1 )
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 0, 1, 1 ) );
        }
        else if( m_Proteins[ i ].id == 2 )
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
        }
        else
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
        }
        //LOG_ERROR( "x=" + Ogre::StringConverter::toString( m_Proteins[ i ].x ) + " y=" + Ogre::StringConverter::toString( m_Proteins[ i ].y ) + " p:" + Ogre::StringConverter::toString( (float)m_Proteins[ i ].power ) );
        DEBUG_DRAW.Circle( m_Proteins[ i ].x * scale, m_Proteins[ i ].y * scale, m_Proteins[ i ].power * scale );
        //DEBUG_DRAW.Text( m_Cells[ i ]->GetX() * scale, m_Cells[ i ]->GetY() * scale, Ogre::StringConverter::toString( i ) );
    }

    DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    for( size_t i = 0; i < m_Genome.size(); ++i )
    {
        DEBUG_DRAW.Text( 10, 10 + i * 20.0f, "Gene" + Ogre::StringConverter::toString( m_Genome[ i ].id ) + ": " );
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
                        float power = SearchProtein( ( int )cond.value1, m_Cells[ i ]->GetX(), m_Cells[ i ]->GetY() );
                        if( ( power < cond.value2 ) || ( power > cond.value3 ) )
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
                            int found = -1;
                            for( size_t p_id = 0; p_id < m_Proteins.size(); ++p_id )
                            {
                                if( m_Proteins[ p_id ].id == ( int )expr.value1 )
                                {
                                    if( ( m_Proteins[ p_id ].x == m_Cells[ i ]->GetX() ) && ( m_Proteins[ p_id ].y == m_Cells[ i ]->GetY() ) )
                                    {
                                        found = p_id;
                                    }
                                }
                            }
                            if( found != -1 )
                            {
                                //m_Proteins[ found ].power += expr.value2;
                            }
                            else
                            {
                                Protein protein;
                                protein.id = ( int )expr.value1;
                                protein.power = expr.value2;
                                protein.x =  m_Cells[ i ]->GetX();
                                protein.y =  m_Cells[ i ]->GetY();
                                m_Proteins.push_back( protein );
                            }
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
Ontogenesis::SearchProtein( const int protein, const int x, const int y )
{
    float power = 0.0f;

    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        if( m_Proteins[ i ].id == protein )
        {
            int x2 = m_Proteins[ i ].x;
            int y2 = m_Proteins[ i ].y;
            float distance = (float)sqrt( ( x2 - x ) * ( x2 - x ) + ( y2 - y ) * ( y2 - y ) );
            if( distance < m_Proteins[ i ].power )
            {
                power += ( m_Proteins[ i ].power - distance ) / m_Proteins[ i ].power;
            }
        }
    }
    //LOG_ERROR( "x=" + Ogre::StringConverter::toString( x ) + " y=" + Ogre::StringConverter::toString( y ) + " p:" + Ogre::StringConverter::toString( power ) );
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
        case E_SPAWN: return "SPAWN";
        case E_MIGRATE: return "MIGRATE";
        case E_PROTEIN: return "PROTEIN";
        case E_DENDRITE: return "DENDRITE";
        case E_DENDRITE_I: return "DENDRITE_I";
        case E_AXON: return "AXON";
        case E_AXON_I: return "AXON_I";
    }
    return "UNKNOWN";
}
