#include "Person.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



Person::Person( const float x, const float y ):
    m_Radius( 5.0f ),
    m_X( x ),
    m_Y( y ),
    m_Rotation( 0.0f ),
    m_Life( 200.0f )
{
    m_Rotation = ( float )( rand() % 360 );
}



Person::~Person()
{
}



void
Person::Update()
{
    float delta = Timer::getSingleton().GetGameTimeDelta();



    static float wait_rotation = 3.0f;
    wait_rotation -= delta;
    if( wait_rotation <= 0.0f )
    {
        m_Rotation = ( float )( rand() % 360 );
        wait_rotation = 5.0f;
    }



    m_Life -= delta * 5.0f;
}



void
Person::Draw()
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
    DEBUG_DRAW.Disc( m_X, m_Y, 2 );
    DEBUG_DRAW.Disc( m_X, m_Y, m_Radius );

    Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
    Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
    q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
    rotation *= 5.0f;
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.4f ) );
    DEBUG_DRAW.Line( m_X, m_Y, m_X + rotation.x, m_Y + rotation.y );
}



float
Person::GetRadius() const
{
    return m_Radius;
}



float
Person::GetX() const
{
    return m_X;
}



void
Person::SetX( const float x )
{
    m_X = x;
}



float
Person::GetY() const
{
    return m_Y;
}



void
Person::SetY( const float y )
{
    m_Y = y;
}



float
Person::GetRotation() const
{
    return m_Rotation;
}



void
Person::SetRotation( const float rotation )
{
    m_Rotation = rotation;
}



bool
Person::IsDead() const
{
    return ( m_Life <= 0 );
}



float
Person::GetKnowledge( const int know_id ) const
{
    if( m_Knowledge.size() > know_id )
    {
        return m_Knowledge[ know_id ];
    }
    return 0.0f;
}



void
Person::ChangeKnowledge( const int person_id, const int know_id, const float opinion )
{

}
