#include "Entity.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "EntityManager.h"
#include "../core/Timer.h"



const float SENSOR_LENGTH = 40.0f;
const float SENSOR_LEFT_ROT = 45.0f;
const float SENSOR_RIGHT_ROT = -45.0f;



Entity::Entity( const int type, const float x, const float y ):
    m_Radius( 12.0f ),
    m_X( x ),
    m_Y( y ),
    m_Rotation( 0.0f ),
    m_ForwardImpulse( 0.0f ),
    m_LeftImpulse( 0.0f ),
    m_RightImpulse( 0.0f ),
    m_Life( 200.0f ),
    m_Energy( 20.0f ),
    m_Fitness( 0.0f ),
    m_Type( type ),
    m_Think( 0.1f )
{
    m_Rotation = ( float )( rand() % 360 );
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

    m_Life -= delta * 5.0;
    SetEnergy( m_Energy - delta * 5.0f );
}



void
Entity::Draw( const float x, const float y )
{
    // draw entity itself
    if( m_Type == 0 )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
    }
    else
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 1 ) );
    }
    DEBUG_DRAW.Disc( m_X, m_Y, m_Radius );

    // draw energy
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 0, 0, 1 ) );
    DEBUG_DRAW.SetTextAlignment( DebugDraw::CENTER );
    DEBUG_DRAW.Text( m_X, m_Y - 8, IntToString( ( int )m_Energy ) );
    DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );

    // draw sensors
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.4f ) );
    float pos_x = m_X + SENSOR_LENGTH * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_LEFT_ROT ) ) );
    float pos_y = m_Y + SENSOR_LENGTH * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_LEFT_ROT ) ) );
    DEBUG_DRAW.Line( m_X, m_Y, pos_x, pos_y );
    pos_x = m_X + SENSOR_LENGTH * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_RIGHT_ROT ) ) );
    pos_y = m_Y + SENSOR_LENGTH * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_RIGHT_ROT ) ) );
    DEBUG_DRAW.Line( m_X, m_Y, pos_x, pos_y );

    // draw network
    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        m_Network[ i ]->Draw( x, y );

        Cell::CellName name = m_Network[ i ]->GetName();
        if( name == Cell::SENSOR_ENEMY )
        {
            Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
            Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
            q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
            rotation.x = x;
            rotation.y = y;
            rotation.z = 0;
            rotation = q * rotation;
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.4f ) );
            DEBUG_DRAW.Line( m_X, m_Y, m_X + rotation.x, m_Y + rotation.y );
        }
    }

    // line connecting neural network and entity
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 0.1f ) );
    DEBUG_DRAW.Line( m_X, m_Y, x, y );

    // draw info about entity
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Text( x - 40, y - 70, "gen:" + IntToString( m_GenerationId ) + " (" + IntToString( m_SpeciesId ) + ")" );
    DEBUG_DRAW.Text( x - 40, y - 50, "fit:" + IntToString( ( int )m_Fitness ) + " (" + IntToString( ( int )m_Life ) + ")" );
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



bool
Entity::IsDead() const
{
    return ( m_Life <= 0 );
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
    m_Radius = 10.0f + energy / 10.0f;
}



float
Entity::GetFitness() const
{
    return m_Fitness;
}




void
Entity::SetFitness( const float fitness )
{
    m_Fitness = fitness;
}



size_t
Entity::GetGenerationId() const
{
    return m_GenerationId;
}



size_t
Entity::GetSpeciesId() const
{
    return m_SpeciesId;
}



int
Entity::GetType() const
{
    return m_Type;
}



void
Entity::AddNetwork( std::vector< Cell* >& network, const size_t generation_id, const size_t species_id )
{
    m_Network = network;
    m_GenerationId = generation_id;
    m_SpeciesId = species_id;
}



float
Entity::GetSensorFoodLeft() const
{
    float x = m_X + SENSOR_LENGTH * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_LEFT_ROT ) ) );
    float y = m_Y + SENSOR_LENGTH * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_LEFT_ROT ) ) );
    return EntityManager::getSingleton().FeelFood( x, y );
}



float
Entity::GetSensorFoodRight() const
{
    float x = m_X + SENSOR_LENGTH * Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_RIGHT_ROT ) ) );
    float y = m_Y + SENSOR_LENGTH * Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( m_Rotation + SENSOR_RIGHT_ROT ) ) );
    return EntityManager::getSingleton().FeelFood( x, y );
}



float
Entity::GetSensorEnergy() const
{
    return m_Energy / 100.0f;
}



float
Entity::GetSensorEnemy( const float x, const float y ) const
{
    Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
    Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
    q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
    rotation.x = x;
    rotation.y = y;
    rotation.z = 0;
    rotation = q * rotation;
    return EntityManager::getSingleton().FeelEnemy( m_Type, m_X + rotation.x, m_Y + rotation.y );
}
