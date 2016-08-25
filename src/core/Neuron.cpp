#include "Neuron.h"

#include <OgreMath.h>

#include "DebugDraw.h"
#include "EntityManager.h"
#include "Logger.h"
#include "Timer.h"



Neuron::Neuron( Entity* entity, const Ogre::String& type, const int x, const int y ):
    m_Entity( entity ),
    m_X( x ),
    m_Y( y ),
    m_Threshold( 1.0f ),
    m_Value( 0.0f ),
    m_Fired( false )
{
    if( type == "activator_forward" )
    {
         m_Type = Neuron::ACTIVATOR_FORWARD;
    }
    else if( type == "activator_left" )
    {
         m_Type = Neuron::ACTIVATOR_LEFT;
    }
    else if( type == "activator_right" )
    {
         m_Type = Neuron::ACTIVATOR_RIGHT;
    }
    else if( type == "sensor_food" )
    {
         m_Type = Neuron::SENSOR_FOOD;
    }
    else if( type == "sensor_energy" )
    {
         m_Type = Neuron::SENSOR_ENERGY;
    }
    else
    {
         m_Type = Neuron::NEURON;
    }
}



Neuron::~Neuron()
{
}



void
Neuron::Update()
{
    if( m_Fired == true )
    {
        m_Fired = false;
    }
    else
    {
        if( m_Type == Neuron::NEURON )
        {
            for( size_t i = 0; i < m_Synapses.size(); ++i )
            {
                float value = 0;

                switch( m_Synapses[ i ].neuron->m_Type )
                {
                    case Neuron::NEURON:
                    {
                        if( m_Synapses[ i ].neuron->m_Fired == true )
                        {
                            value = 1;
                        }
                    }
                    break;

                    case Neuron::SENSOR_FOOD:
                    {
                        value = m_Entity->GetSensorFood();
                    }
                    break;

                    case Neuron::SENSOR_ENERGY:
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
        }

        if( m_Value >= m_Threshold )
        {
            m_Value = 0.0f;
            m_Fired = true;

            if( m_Type == Neuron::ACTIVATOR_FORWARD )
            {
                 m_Entity->SetForwardImpulse( 2.0f );
            }
            else if( m_Type == Neuron::ACTIVATOR_LEFT )
            {
                 m_Entity->SetLeftImpulse( 2.0f );
            }
            else if( m_Type == Neuron::ACTIVATOR_RIGHT )
            {
                 m_Entity->SetRightImpulse( 2.0f );
            }
        }
    }
}



void
Neuron::Draw( const unsigned int x, const unsigned int y )
{
    if( m_Fired == true )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
    }
    else
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    }
    float radius = 2.0f;
    DEBUG_DRAW.Quad( x + m_X - radius, y + m_Y - radius, x + m_X + radius, y + m_Y - radius, x + m_X + radius, y + m_Y + radius, x + m_X - radius, y + m_Y + radius );

    for( size_t i = 0; i < m_Synapses.size(); ++i )
    {
        Neuron* neuron = m_Synapses[ i ].neuron;
        if( neuron->m_Fired == true )
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
        }
        else
        {
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
        }
        DEBUG_DRAW.Line( x + m_X, y + m_Y, x + neuron->m_X, y + neuron->m_Y );
    }
}



void
Neuron::AddSynapse( const float power, const bool inverted, Neuron* neuron )
{
    Synapse synapse;
    synapse.power = power;
    synapse.inverted = inverted;
    synapse.neuron = neuron;
    m_Synapses.push_back( synapse );
}
