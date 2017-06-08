#include "Cell.h"

#include <OgreMath.h>

#include "../core/DebugDraw.h"
#include "Entity.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



Cell::Cell( Entity* entity, const CellName name, const float x, const float y ):
    m_Entity( entity ),
    m_Name( name ),
    m_X( x ),
    m_Y( y ),
    m_Threshold( 1.0f ),
    m_Value( 0.0f ),
    m_Fired( false )
{
    switch( name )
    {
        case NEURON_COMMON:     m_Type = NEURON;    break;
        case SENSOR_FOOD:       m_Type = SENSOR;    break;
        case SENSOR_ENERGY:     m_Type = SENSOR;    break;
        case SENSOR_ENEMY:      m_Type = SENSOR;    break;
        case ACTIVATOR_FORWARD: m_Type = ACTIVATOR; break;
        case ACTIVATOR_LEFT:    m_Type = ACTIVATOR; break;
        case ACTIVATOR_RIGHT:   m_Type = ACTIVATOR; break;
    }
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

            switch( m_Synapses[ i ].cell->m_Name )
            {
                case NEURON_COMMON:
                {
                    if( m_Synapses[ i ].cell->m_Fired == true )
                    {
                        value = 1;
                    }
                }
                break;

                case SENSOR_ENERGY:
                {
                    value = m_Entity->GetSensorEnergy();
                }
                break;

                case SENSOR_FOOD:
                {
                    value = m_Entity->GetSensorFood( m_X, m_Y );
                }
                break;

                case SENSOR_ENEMY:
                {
                    value = m_Entity->GetSensorEnemy( m_X, m_Y );
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

            if( m_Name == ACTIVATOR_FORWARD )
            {
                m_Entity->SetForwardImpulse( 1.0f );
            }
            else if( m_Name == ACTIVATOR_LEFT )
            {
                 m_Entity->SetLeftImpulse( 1.0f );
            }
            else if( m_Name == ACTIVATOR_RIGHT )
            {
                 m_Entity->SetRightImpulse( 1.0f );
            }
        }
    }
}



void
Cell::Draw( const float x, const float y )
{
    if( m_Fired == true )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
    }
    else if( m_Type == ACTIVATOR )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 1 ) );
    }
    else if( m_Type == SENSOR )
    {
        float value = 0.0f;
        switch( m_Name )
        {
            case SENSOR_ENERGY:
            {
                value = m_Entity->GetSensorEnergy();
            }
            break;

            case SENSOR_FOOD:
            {
                value = m_Entity->GetSensorFood( m_X, m_Y );
            }
            break;

            case SENSOR_ENEMY:
            {
                value = m_Entity->GetSensorEnemy( m_X, m_Y );
            }
            break;
        }

        if( value > 0.0f)
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
        }
        else
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
        }
    }
    else
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    }
    float radius = 2.0f;
    float scale = 8.0f;
    DEBUG_DRAW.Disc( x + m_X * scale, y + m_Y * scale, radius );

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



const Cell::CellName
Cell::GetName() const
{
    return m_Name;
}



const Cell::CellType
Cell::GetType() const
{
    return m_Type;
}



float
Cell::GetX() const
{
    return m_X;
}



void
Cell::SetX( const float x )
{
    m_X = x;
}



float
Cell::GetY() const
{
    return m_Y;
}



void
Cell::SetY( const float y )
{
    m_Y = y;
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
        case SENSOR_FOOD: return "SENSOR_FOOD";
        case SENSOR_ENERGY: return "SENSOR_ENERGY";
        case SENSOR_ENEMY: return "SENSOR_ENEMY";
        case ACTIVATOR_FORWARD: return "ACTIVATOR_FORWARD";
        case ACTIVATOR_LEFT: return "ACTIVATOR_LEFT";
        case ACTIVATOR_RIGHT: return "ACTIVATOR_RIGHT";
    }
    return "UNKNOWN";
}
