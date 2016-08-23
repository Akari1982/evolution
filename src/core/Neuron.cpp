#include "Neuron.h"

#include <OgreMath.h>

#include "DebugDraw.h"
#include "EntityManager.h"
#include "Logger.h"
#include "Timer.h"



Neuron::Neuron( Entity* entity, const int x, const int y, const float motor_x, const float motor_y ):
    m_Entity( entity ),
    m_X( x ),
    m_Y( y ),
    m_MotorX( motor_x ),
    m_MotorY( motor_y ),
    m_Threshold( 1.0f ),
    m_Value( 0.0f ),
    m_Fired( false )
{
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
        for( size_t i = 0; i < m_Synapses.size(); ++i )
        {
            float value = 0;

            switch( m_Synapses[ i ].type )
            {
                case Neuron::NEURON:
                {
                    if( m_Synapses[ i ].neuron->m_Fired == true )
                    {
                        value = 1;
                    }
                }
                break;

                case Neuron::FOOD:
                {
                    value = EntityManager::getSingleton().FeelFood( m_Entity->GetX() + m_Synapses[ i ].x, m_Entity->GetY() + m_Synapses[ i ].y, 30.0f );
                }
                break;

                case Neuron::ENERGY:
                {
                    value = m_Entity->GetEnergy() / 100.0f;
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

            m_Entity->Move( m_MotorX, m_MotorY );
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
        switch( m_Synapses[ i ].type )
        {
            case Neuron::NEURON:
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
            break;

            case Neuron::FOOD:
            {
                DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
                DEBUG_DRAW.Line( m_Entity->GetX(), m_Entity->GetY(), m_Entity->GetX() + m_Synapses[ i ].x, m_Entity->GetY() + m_Synapses[ i ].y );
            }
            break;

            case Neuron::ENERGY:
            {
            }
            break;
        }
    }
}



void
Neuron::AddNeuronSynapse( const float power, const bool inverted, Neuron* neuron )
{
    Synapse synapse;
    synapse.type = Neuron::NEURON;
    synapse.power = power;
    synapse.inverted = inverted;
    synapse.neuron = neuron;
    m_Synapses.push_back( synapse );
}



void
Neuron::AddFoodSynapse( const float power, const bool inverted, const float length, const float degree )
{
    Synapse synapse;
    synapse.type = Neuron::FOOD;
    synapse.power = power;
    synapse.inverted = inverted;
    synapse.x = length * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( degree - 90 ) ) );
    synapse.y = length * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( degree - 90 ) ) );
    m_Synapses.push_back( synapse );
}



void
Neuron::AddEnergySynapse( const float power, const bool inverted )
{
    Synapse synapse;
    synapse.type = Neuron::ENERGY;
    synapse.power = power;
    synapse.inverted = inverted;
    m_Synapses.push_back( synapse );
}
