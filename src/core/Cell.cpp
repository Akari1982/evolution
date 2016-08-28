#include "Cell.h"

#include <OgreMath.h>

#include "DebugDraw.h"
#include "Entity.h"
#include "Logger.h"
#include "Timer.h"



Cell::Cell( Entity* entity, const CellType type, const int x, const int y ):
    m_Entity( entity ),
    m_Type( type ),
    m_X( x ),
    m_Y( y ),
    m_Protein( -1 ),
    m_ProteinRadius( 0 ),
    m_Threshold( 1.0f ),
    m_Value( 0.0f ),
    m_Fired( false )
{
}



Cell::~Cell()
{
}



void
Cell::Update()
{
    if( m_Fired == true )
    {
        m_Fired = false;
    }
    else
    {
        for( size_t i = 0; i < m_Synapses.size(); ++i )
        {
            float value = 0;

            switch( m_Synapses[ i ].cell->m_Type )
            {
                case Cell::NEURON:
                {
                    if( m_Synapses[ i ].cell->m_Fired == true )
                    {
                        value = 1;
                    }
                }
                break;

                case Cell::SENSOR_FOOD_LEFT:
                {
                    value = m_Entity->GetSensorFoodLeft();
                }
                break;

                case Cell::SENSOR_FOOD_RIGHT:
                {
                    value = m_Entity->GetSensorFoodRight();
                }
                break;

                case Cell::SENSOR_ENERGY:
                {
                    value = m_Entity->GetSensorEnergy();
                }
                break;
            }

            if( m_Synapses[ i ].inverted == false )
            {
                m_Value += value * m_Synapses[ i ].power;
            }
            else
            {
                m_Value += ( 1 - value ) * m_Synapses[ i ].power;
            }
        }

        if( m_Value >= m_Threshold )
        {
            m_Value = 0.0f;
            m_Fired = true;

            if( m_Type == Cell::ACTIVATOR_FORWARD )
            {
                m_Entity->SetForwardImpulse( 2.0f );
            }
            else if( m_Type == Cell::ACTIVATOR_LEFT )
            {
                 m_Entity->SetLeftImpulse( 2.0f );
            }
            else if( m_Type == Cell::ACTIVATOR_RIGHT )
            {
                 m_Entity->SetRightImpulse( 2.0f );
            }
        }
    }
}



void
Cell::Draw( const unsigned int x, const unsigned int y )
{
    if( m_Fired == true )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
    }
    else if( m_Type == Cell::ACTIVATOR_FORWARD || m_Type == Cell::ACTIVATOR_LEFT || m_Type == Cell::ACTIVATOR_RIGHT )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 1 ) );
    }
    else if( m_Type == Cell::SENSOR_FOOD_LEFT || m_Type == Cell::SENSOR_FOOD_RIGHT || m_Type == Cell::SENSOR_ENERGY )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
    }
    else
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    }
    float radius = 2.0f;
    float scale = 8.0f;
    DEBUG_DRAW.Disc( x + m_X * scale, y + m_Y * scale, radius );

    //DEBUG_DRAW.SetColour( Ogre::ColourValue( 0.5f, 0.5f, 0.5f, 1 ) );
    //DEBUG_DRAW.Circle( x + m_X * scale, y + m_Y * scale, m_ProteinRadius * scale );

    for( size_t i = 0; i < m_Synapses.size(); ++i )
    {
        Cell* cell = m_Synapses[ i ].cell;
        if( cell->m_Fired == true )
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
        }
        else
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        }
        DEBUG_DRAW.Line( x + m_X * scale, y + m_Y * scale, x + cell->m_X * scale, y + cell->m_Y * scale );
    }
}



void
Cell::SetType( const Cell::CellType type )
{
    m_Type = type;
}



const Cell::CellType
Cell::GetType() const
{
    return m_Type;
}



int
Cell::GetX() const
{
    return m_X;
}



void
Cell::SetX( const int x )
{
    m_X = x;
}



int
Cell::GetY() const
{
    return m_Y;
}



void
Cell::SetY( const int y )
{
    m_Y = y;
}



int
Cell::GetProtein() const
{
    return m_Protein;
}



void
Cell::SetProtein( const int protein )
{
    m_Protein = protein;
}



int
Cell::GetProteinRadius() const
{
    return m_ProteinRadius;
}



void
Cell::SetProteinRadius( const int protein_radius )
{
    m_ProteinRadius = protein_radius;
}



void
Cell::AddSynapse( const float power, const bool inverted, Cell* cell )
{
    Synapse synapse;
    synapse.power = power;
    synapse.inverted = inverted;
    synapse.cell = cell;
    m_Synapses.push_back( synapse );
}
