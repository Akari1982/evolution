#include "Entity.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "EntityManager.h"
#include "../core/Timer.h"



Entity::Entity( const float x, const float y ):
    m_Radius( 2.0f + 3.0f ),
    m_X( x ),
    m_Y( y ),
    m_Rotation( 0.0f ),
    m_MovePower( 0.0f ),
    m_MoveTime( 0.0f ),
    m_RotatePower( 0.0f ),
    m_RotateTime( 0.0f ),

    m_Life( 100.0f ),
    m_Energy( 20.0f ),

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
        for( size_t i = 0; i < m_Network.size(); ++i )
        {
            m_Network[ i ]->UpdateFire();
        }

        m_Think = 0.1f;
    }

    m_Life -= delta;

    SetEnergy( m_Energy - delta );
}



void
Entity::Draw( const float x, const float y )
{
    // draw entity itself
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
    DEBUG_DRAW.Disc( m_X, m_Y, m_Radius );

    // draw network
    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        Ogre::Quaternion rotation( 0.0f, 0.0f, 0.0f, 1.0f );
        rotation.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
        m_Network[ i ]->Draw( x, y, m_X, m_Y, rotation );
    }

    // line connecting neural network and entity
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 0.1f ) );
    DEBUG_DRAW.Line( m_X, m_Y, x, y );

    // draw info about entity
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Text( x - 40, y - 90, "Life:" + IntToString( ( int )m_Life ) + "" );
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
Entity::GetMovePower() const
{
    return m_Move;
}



float
Entity::GetMoveTime() const
{
    return m_MoveTime;
}



void
Entity::SetMove( const float power, const float time )
{
    m_MovePower = power;
    m_MoveTime = time;
}



float
Entity::GetRotatePower() const
{
    return m_RotatePower;
}



float
Entity::GetRotateTime() const
{
    return m_RotateTime;
}



void
Entity::SetRotate( const float power, const float time )
{
    m_RotatePower = power;
    m_RotateTime = time;
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
    m_Radius = 2.0f + ( energy / 20.0f ) * 3.0f;
}



void
Entity::AddNetwork( std::vector< Cell* >& network )
{
    m_Network = network;
}



std::vector< Cell* >&
Entity::GetNetwork()
{
    return m_Network;
}



float
Entity::GetSensorFood( const float x, const float y ) const
{
    Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
    Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
    q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
    rotation.x = x;
    rotation.y = y;
    rotation.z = 0;
    rotation = q * rotation;
    return EntityManager::getSingleton().FeelFood( m_X + rotation.x, m_Y + rotation.y );
}
