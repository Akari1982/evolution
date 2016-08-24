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
    m_RotationImpulse( 0.0f ),
    m_Energy( 100.0f ),
    m_Think( 0.1f )
{
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
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, m_Energy / 100.0f, 0, 1 ) );
    float radius = m_Radius * 0.75f;
    DEBUG_DRAW.Quad( m_X - radius, m_Y - radius, m_X + radius, m_Y - radius, m_X + radius, m_Y + radius, m_X - radius, m_Y + radius );

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
Entity::GetRotationImpulse() const
{
    return m_RotationImpulse;
}



void
Entity::SetRotationImpulse( const float rotation_impulse )
{
    m_RotationImpulse = rotation_impulse;
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
Entity::AddNeuron( const unsigned int x, const unsigned int y, const float motor_x, const float motor_y )
{
    Neuron* neuron = new Neuron( this, x, y, motor_x, motor_y );
    m_Network.push_back( neuron );
}



void
Entity::AddSynapse( const unsigned int self_id, const Ogre::String type, const float power, const bool inverted, const unsigned int neuron_id, const float length, const float degree )
{
    if( type == "neuron" )
    {
        m_Network[ self_id ]->AddNeuronSynapse( power, inverted, m_Network[ neuron_id ] );
    }
    else if( type == "food" )
    {
        m_Network[ self_id ]->AddFoodSynapse( power, inverted, length, degree );
    }
    else if( type == "energy" )
    {
        m_Network[ self_id ]->AddEnergySynapse( power, inverted );
    }
}
