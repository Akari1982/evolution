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
    m_OuterProtein( -1 ),
    m_OuterProteinRadius( 0 ),
    m_InnerProtein( -1 ),
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
                case NEURON:
                {
                    if( m_Synapses[ i ].cell->m_Fired == true )
                    {
                        value = 1;
                    }
                }
                break;

                case SENSOR_FOOD_LEFT:
                {
                    value = m_Entity->GetSensorFoodLeft();
                }
                break;

                case SENSOR_FOOD_RIGHT:
                {
                    value = m_Entity->GetSensorFoodRight();
                }
                break;

                case SENSOR_ENERGY:
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

            if( m_Type == ACTIVATOR_FORWARD )
            {
                m_Entity->SetForwardImpulse( 2.0f );
            }
            else if( m_Type == ACTIVATOR_LEFT )
            {
                 m_Entity->SetLeftImpulse( 2.0f );
            }
            else if( m_Type == ACTIVATOR_RIGHT )
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
    else if( m_Type == ACTIVATOR_FORWARD || m_Type == ACTIVATOR_LEFT || m_Type == ACTIVATOR_RIGHT )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 1 ) );
    }
    else if( m_Type == SENSOR_FOOD_LEFT || m_Type == SENSOR_FOOD_RIGHT || m_Type == SENSOR_ENERGY )
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
Cell::SetType( const CellType type )
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
Cell::GetOuterProtein() const
{
    return m_OuterProtein;
}



void
Cell::SetOuterProtein( const int protein )
{
    m_OuterProtein = protein;
}



int
Cell::GetOuterProteinRadius() const
{
    return m_OuterProteinRadius;
}



void
Cell::SetOuterProteinRadius( const int protein_radius )
{
    m_OuterProteinRadius = protein_radius;
}



int
Cell::GetInnerProtein() const
{
    return m_InnerProtein;
}



void
Cell::SetInnerProtein( const int protein )
{
    m_InnerProtein = protein;
}



void
Cell::AddSynapse( const float power, const bool inverted, Cell* cell )
{
    Synapse synapse;

    bool found = false;
    for( size_t i = 0; i < m_Synapses.size(); ++i )
    {
        if( ( m_Synapses[ i ].cell == cell ) && ( m_Synapses[ i ].inverted == inverted ) )
        {
            synapse = m_Synapses[ i ];
            found = true;
        }
    }

    if( found == true )
    {
        synapse.power += power;
    }
    else
    {
        synapse.power = power;
        synapse.inverted = inverted;
        synapse.cell = cell;
        m_Synapses.push_back( synapse );
    }
}



Ogre::String
Cell::CellTypeToString( const CellType type )
{
    switch( type )
    {
        case NEURON: return "NEURON";
        case SENSOR_FOOD_LEFT: return "SENSOR_FOOD_LEFT";
        case SENSOR_FOOD_RIGHT: return "SENSOR_FOOD_RIGHT";
        case SENSOR_ENERGY: return "SENSOR_ENERGY";
        case ACTIVATOR_FORWARD: return "ACTIVATOR_FORWARD";
        case ACTIVATOR_LEFT: return "ACTIVATOR_LEFT";
        case ACTIVATOR_RIGHT: return "ACTIVATOR_RIGHT";
    }
    return "UNKNOWN";
}
