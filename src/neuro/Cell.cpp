#include "Cell.h"

#include <OgreMath.h>

#include "../core/DebugDraw.h"
#include "Entity.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



Cell::Cell( Entity* entity, const float x, const float y ):
    m_Entity( entity ),
    m_X( x ),
    m_Y( y ),
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
    for( size_t i = 0; i < m_Synapses.size(); ++i )
    {
        float value = 0.0f;

        switch( m_Synapses[ i ].type )
        {
            case SYN_FOOD:
            {
                value = m_Entity->GetSensorFood( m_X, m_Y );
                if( value > 0 )
                {
                    m_Synapses[ i ].activated = true;
                }
            }
            break;
            case SYN_NEURON:
            {
                Cell* cell = m_Entity->GetCell( m_Synapses[ i ].cell_id );
if( cell == NULL )
{
    LOG_ERROR( "[ERROR] Cell::Update" );
    return;
}
                if( cell->m_Fired == true )
                {
                    value = 1.0f;
                    m_Synapses[ i ].activated = true;
                }
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



void
Cell::UpdateFire()
{
    if( m_Fired == true )
    {
        m_Fired = false;
    }
    else
    {
        if( m_Value >= m_Threshold )
        {
            m_Value = 0.0f;
            m_Fired = true;

            for( size_t i = 0; i < m_Activators.size(); ++i )
            {
                switch( m_Activators[ i ].type )
                {
                    case ACT_MOVE:
                    {
                        m_Entity->SetMove( m_Activators[ i ].power, 0.5f );
                    }
                    break;
                    case ACT_ROTATE:
                    {
                        m_Entity->SetRotate( m_Activators[ i ].power, 0.5f );
                    }
                    break;
                }
            }
        }
    }
}



void
Cell::Draw( const float ui_x, const float ui_y, const float x, const float y, const Ogre::Quaternion& rotation )
{
    float scale = 1.0f;

    // pos of cell related to entity pos
    Ogre::Vector3 pos( m_X, m_Y, 0.0f );
    pos = rotation * pos;

    // draw neuron
    if( m_Fired == true )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
    }
    else if( m_Activators.size() > 0 )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 1 ) );
    }
    else
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    }
    float radius = 2.0f;
    DEBUG_DRAW.Disc( ui_x + m_X * scale, ui_y + m_Y * scale, radius );
    //DEBUG_DRAW.Disc( x + pos.x, y + pos.y, radius );

    // draw all synapses
    for( size_t i = 0; i < m_Synapses.size(); ++i )
    {
        if( m_Synapses[ i ].type == SYN_NEURON )
        {
            Cell* cell = m_Entity->GetCell( m_Synapses[ i ].cell_id );
            if( m_Synapses[ i ].activated == true )
            {
                DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
                m_Synapses[ i ].activated = false;
            }
            else
            {
                DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
            }
            DEBUG_DRAW.Line( ui_x + m_X * scale, ui_y + m_Y * scale, ui_x + cell->m_X * scale, ui_y + cell->m_Y * scale );
            //Ogre::Vector3 pos2( cell->m_X, cell->m_Y, 0.0f );
            //pos2 = rotation * pos2;
            //DEBUG_DRAW.Line( x + pos.x, y + pos.y, x + pos2.x, y + pos2.y );
        }
        else if( m_Synapses[ i ].type == SYN_FOOD )
        {
            if( m_Synapses[ i ].activated == true )
            {
                DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
                m_Synapses[ i ].activated = false;
            }
            else
            {
                DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
            }
            radius = 5.0f;
            DEBUG_DRAW.Circle( ui_x + m_X * scale, ui_y + m_Y * scale, radius );
            DEBUG_DRAW.Circle( x + pos.x, y + pos.y, radius );
        }
    }
}



void
Cell::AddSynapse( const float power, const bool inverted, const int cell_id )
{
    Synapse synapse;
    synapse.type = SYN_NEURON;
    synapse.power = power;
    synapse.inverted = inverted;
    synapse.cell_id = cell_id;
    synapse.activated = false;
    m_Synapses.push_back( synapse );
}



void
Cell::AddFoodSynapse( const float power, const bool inverted )
{
    Synapse synapse;
    synapse.type = SYN_FOOD;
    synapse.power = power;
    synapse.inverted = inverted;
    synapse.activated = false;
    m_Synapses.push_back( synapse );
}



void
Cell::AddMoveActivator( const float power )
{
    Activator activator;
    activator.type = ACT_MOVE;
    activator.power = power;
    m_Activators.push_back( activator );
}



void
Cell::AddRotateActivator( const float power )
{
    Activator activator;
    activator.type = ACT_ROTATE;
    activator.power = power;
    m_Activators.push_back( activator );
}
