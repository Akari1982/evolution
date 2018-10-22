#include "Entity.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "EntityManager.h"
#include "../core/Timer.h"



Entity::Entity( const float x, const float y ):
    m_X( x ),
    m_Y( y ),
    m_Rotation( 0.0f ),
    m_ForwardImpulse( 0.0f ),
    m_LeftImpulse( 0.0f ),
    m_RightImpulse( 0.0f ),
    m_Life( 100.0f ),
    m_Fitness( 0.0f ),
    m_Think( 0.1f )
{
    m_Rotation = ( float )( rand() % 360 );
}



Entity::~Entity()
{
}



void
Entity::Update()
{
    float delta = Timer::getSingleton().GetGameTimeDelta();
    m_Think -= delta;

    if( m_Think <= 0 )
    {
        for( size_t i = 0; i < m_Cells.size(); ++i )
        {
            m_Cells[ i ]->Update( this );
        }

        DevelopmentStep();
        m_Think = 0.1f;
    }

    m_Life -= delta;
    SetEnergy( m_Energy - delta );
}



void
Entity::Draw( const float x, const float y )
{
    // draw entity itself
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
    DEBUG_DRAW.Disc( m_X, m_Y, GetRadius() );

    // draw energy
    // DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 0, 0, 1 ) );
    // DEBUG_DRAW.SetTextAlignment( DebugDraw::CENTER );
    // DEBUG_DRAW.Text( m_X, m_Y - 8, IntToString( ( int )m_Energy ) );
    // DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );

    float scale = 8.0f;
    for( size_t i = 0; i < m_Proteins.size(); ++i )
    {
        for( size_t j = 0; j < m_Proteins[ i ].data.size(); ++j )
        {
            float x1 = x + m_Proteins[ i ].data[ j ].x * scale;
            float y1 = y + m_Proteins[ i ].data[ j ].y * scale;

            DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 2 * m_Proteins[ i ].data[ j ].power ) );
            DEBUG_DRAW.Quad( x1 - scale / 2.0f, y1 - scale / 2.0f, x1 + scale / 2.0f, y1 - scale / 2.0f, x1 + scale / 2.0f, y1 + scale / 2.0f, x1 - scale / 2.0f, y1 + scale / 2.0f );
        }
    }

    // draw network
    for( size_t i = 0; i < m_Cells.size(); ++i )
    {
        m_Cells[ i ]->Draw( this, x, y );

        Cell::CellName name = m_Cells[ i ]->GetName();
        if( name == Cell::SENSOR_FOOD || name == Cell::SENSOR_ENEMY )
        {
            Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
            Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
            q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
            rotation.x = m_Cells[ i ]->GetX();
            rotation.y = m_Cells[ i ]->GetY();
            rotation.z = 0;
            rotation = q * rotation;
            rotation *= 5.0f;
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.4f ) );
            DEBUG_DRAW.Line( m_X, m_Y, m_X + rotation.x, m_Y + rotation.y );
        }
    }

    // line connecting neural network and entity
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 0.1f ) );
    DEBUG_DRAW.Line( m_X, m_Y, x, y );

    // draw info about entity
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Text( x - 40, y - 70, "fit:" + IntToString( ( int )m_Fitness ) + " (" + IntToString( ( int )m_Life ) + "/" + IntToString( ( int )m_Energy ) + ")" );
}



const unsigned int
Entity::GetRadius() const
{
    return m_Cells.size();
}



const float
Entity::GetX() const
{
    return m_X;
}



void
Entity::SetX( const float x )
{
    m_X = x;
}



const float
Entity::GetY() const
{
    return m_Y;
}



void
Entity::SetY( const float y )
{
    m_Y = y;
}



const float
Entity::GetRotation() const
{
    return m_Rotation;
}



void
Entity::SetRotation( const float rotation )
{
    m_Rotation = rotation;
}



const float
Entity::GetForwardImpulse() const
{
    return m_ForwardImpulse;
}



void
Entity::SetForwardImpulse( const float forward_impulse )
{
    m_ForwardImpulse = forward_impulse;
}



const float
Entity::GetLeftImpulse() const
{
    return m_LeftImpulse;
}



void
Entity::SetLeftImpulse( const float left_impulse )
{
    m_LeftImpulse = left_impulse;
}



const float
Entity::GetRightImpulse() const
{
    return m_RightImpulse;
}



void
Entity::SetRightImpulse( const float right_impulse )
{
    m_RightImpulse = right_impulse;
}



const bool
Entity::IsDead() const
{
    return ( m_Life <= 0 );
}



const float
Entity::GetEnergy() const
{
    return m_Energy;
}




void
Entity::SetEnergy( const float energy )
{
    m_Energy = energy;
}



const float
Entity::GetFitness() const
{
    return m_Fitness;
}




void
Entity::SetFitness( const float fitness )
{
    m_Fitness = fitness;
}



const float
Entity::GetSensorEnergy() const
{
    return m_Energy / 100.0f;
}



const float
Entity::GetSensorFood( const float x, const float y ) const
{
    Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
    Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
    q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
    rotation.x = x;
    rotation.y = y;
    rotation.z = 0;
    rotation = q * rotation;
    rotation *= 5.0f;
    return EntityManager::getSingleton().FeelFood( m_X + rotation.x, m_Y + rotation.y );
}



const float
Entity::GetSensorEnemy( const float x, const float y ) const
{
    Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
    Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
    q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
    rotation.x = x;
    rotation.y = y;
    rotation.z = 0;
    rotation = q * rotation;
    rotation *= 5.0f;
    return EntityManager::getSingleton().FeelEnemy( m_X + rotation.x, m_Y + rotation.y );
}
