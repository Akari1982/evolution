#include "Entity.h"

#include "DebugDraw.h"
#include "EntityManager.h"
#include "Timer.h"



Entity::Entity( const float x, const float y ):
    m_Radius( 15.0f ),
    m_X( x ),
    m_Y( y ),
    m_Rotation( 0.0f ),
    m_ForwardImpulse( 0.0f ),
    m_LeftImpulse( 0.0f ),
    m_RightImpulse( 0.0f ),
    m_Energy( 100.0f ),
    m_Think( 0.1f )
{
    m_Rotation = rand() % 360;
}



Entity::~Entity()
{
    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        delete m_Network[ i ];
    }
}



void
Entity::Update()
{
    float delta = Timer::getSingleton().GetGameTimeDelta();
    m_Think -= delta;

    if( m_Think <= 0 )
    {
        for( size_t i = 0; i < m_Network.size(); ++i )
        {
            m_Network[ i ]->Update();
        }

        m_Think = 0.1f;
    }
    m_Energy -= delta * 1.0f;
}



void
Entity::Draw( const unsigned int x, const unsigned int y )
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1 - m_Energy / 100.0f, m_Energy / 100.0f, 0, 1 ) );
    DEBUG_DRAW.Disc( m_X, m_Y, m_Radius );
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
    float pos_x = m_X + 50.0f * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation + 45 ) ) );
    float pos_y = m_Y + 50.0f * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation + 45 ) ) );
    DEBUG_DRAW.Circle( pos_x, pos_y, 40.0f );
    DEBUG_DRAW.Line( m_X, m_Y, pos_x, pos_y );
    pos_x = m_X + 50.0f * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation - 45 ) ) );
    pos_y = m_Y + 50.0f * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation - 45 ) ) );
    DEBUG_DRAW.Circle( pos_x, pos_y, 40.0f );
    DEBUG_DRAW.Line( m_X, m_Y, pos_x, pos_y );
    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        m_Network[ i ]->Draw( x, y );
    }
}



float
Entity::GetRadius() const
{
    return m_Radius;
}



float
Entity::GetX() const
{
    return m_X;
}



void
Entity::SetX( const float x )
{
    m_X = x;
}



float
Entity::GetY() const
{
    return m_Y;
}



void
Entity::SetY( const float y )
{
    m_Y = y;
}



float
Entity::GetRotation() const
{
    return m_Rotation;
}



void
Entity::SetRotation( const float rotation )
{
    m_Rotation = rotation;
}



float
Entity::GetForwardImpulse() const
{
    return m_ForwardImpulse;
}



void
Entity::SetForwardImpulse( const float forward_impulse )
{
    m_ForwardImpulse = forward_impulse;
}



float
Entity::GetLeftImpulse() const
{
    return m_LeftImpulse;
}



void
Entity::SetLeftImpulse( const float left_impulse )
{
    m_LeftImpulse = left_impulse;
}



float
Entity::GetRightImpulse() const
{
    return m_RightImpulse;
}



void
Entity::SetRightImpulse( const float right_impulse )
{
    m_RightImpulse = right_impulse;
}



float
Entity::GetEnergy() const
{
    return m_Energy;
}




void
Entity::SetEnergy( const float energy )
{
    m_Energy = energy;
}



void
Entity::AddNeuron( const Ogre::String& type, const unsigned int x, const unsigned int y )
{
    Neuron* neuron = new Neuron( this, type, x, y );
    m_Network.push_back( neuron );
}



void
Entity::AddSynapse( const unsigned int self_id, const float power, const bool inverted, const unsigned int neuron_id )
{
    m_Network[ self_id ]->AddSynapse( power, inverted, m_Network[ neuron_id ] );
}



float
Entity::GetSensorFoodLeft() const
{
    float x = m_X + 50.0f * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation + 45 ) ) );
    float y = m_Y + 50.0f * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation + 45 ) ) );
    return EntityManager::getSingleton().FeelFood( x, y, 40.0f );
}



float
Entity::GetSensorFoodRight() const
{
    float x = m_X + 50.0f * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation - 45 ) ) );
    float y = m_Y + 50.0f * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation - 45 ) ) );
    return EntityManager::getSingleton().FeelFood( x, y, 40.0f );
}



float
Entity::GetSensorEnergy() const
{
    return m_Energy / 100.0f;
}
