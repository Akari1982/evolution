#include "Entity.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "EntityManager.h"
#include "../core/Timer.h"



Entity::Entity( const int type, const float x, const float y ):
    m_Radius( 2.0f + 3.0f ),
    m_X( x ),
    m_Y( y ),
    m_Rotation( 0.0f ),
    m_ForwardImpulse( 0.0f ),
    m_LeftImpulse( 0.0f ),
    m_RightImpulse( 0.0f ),
    m_Life( 100.0f ),
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

    m_Life -= delta;
    SetEnergy( m_Energy - delta );
}



void
Entity::Draw( const float x, const float y )
{
    // draw entity itself
    int col = ( m_Type % 6 ) + 1;
    DEBUG_DRAW.SetColour( Ogre::ColourValue( col & 1, ( col & 2 ) >> 1, ( col & 4 ) >> 2, 1 ) );
    DEBUG_DRAW.Disc( m_X, m_Y, m_Radius );

    // draw energy
    // DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 0, 0, 1 ) );
    // DEBUG_DRAW.SetTextAlignment( DebugDraw::CENTER );
    // DEBUG_DRAW.Text( m_X, m_Y - 8, IntToString( ( int )m_Energy ) );
    // DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );

    // draw network
    for( size_t i = 0; i < m_Network.size(); ++i )
    {
        m_Network[ i ]->Draw( x, y );

        Cell::CellName name = m_Network[ i ]->GetName();
        if( name == Cell::SENSOR_FOOD || name == Cell::SENSOR_ENEMY )
        {
            Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
            Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
            q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
            rotation.x = m_Network[ i ]->GetX();
            rotation.y = m_Network[ i ]->GetY();
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
    DEBUG_DRAW.Text( x - 40, y - 90, "id:" + IntToString( m_SpeciesId ) );
    DEBUG_DRAW.Text( x - 40, y - 70, "fit:" + IntToString( ( int )m_Fitness ) + " (" + IntToString( ( int )m_Life ) + ")" );
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
    m_Radius = 2.0f + ( energy / 20.0f ) * 3.0f;
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
Entity::GetSensorEnergy() const
{
    return m_Energy / 100.0f;
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
    rotation *= 5.0f;
    return EntityManager::getSingleton().FeelFood( m_X + rotation.x, m_Y + rotation.y );
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
    rotation *= 5.0f;
    return EntityManager::getSingleton().FeelEnemy( m_Type, m_X + rotation.x, m_Y + rotation.y );
}
