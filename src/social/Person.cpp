#include "Person.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "../core/Timer.h"

const float KNOW_TIME = 5.0f;



Person::Person( const float x, const float y ):
    m_Radius( 10.0f ),
    m_X( x ),
    m_Y( y ),
    m_Rotation( 0.0f ),
    m_Life( 200.0f ),
    m_KnowTime( KNOW_TIME )
{
    m_Rotation = ( float )( rand() % 360 );

    m_Knowledge.push_back( 0.7f ); m_Confidence.push_back( 0.5f );
    m_Knowledge.push_back( 0.9f ); m_Confidence.push_back( 0.5f );
    m_Knowledge.push_back( 0.5f ); m_Confidence.push_back( 0.5f );
}



Person::~Person()
{
}



void
Person::Update()
{
    float delta = Timer::getSingleton().GetGameTimeDelta();
    m_KnowTime -= delta;



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
Person::Draw( const float x, const float y )
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
    DEBUG_DRAW.Disc( m_X, m_Y, 2 );
    DEBUG_DRAW.Circle( m_X, m_Y, m_Radius );

    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 0, 0, 1 ) );
    DEBUG_DRAW.SetTextAlignment( DebugDraw::CENTER );
    DEBUG_DRAW.Text( m_X, m_Y - 8, IntToString( ( int )m_Life ) );
    DEBUG_DRAW.SetTextAlignment( DebugDraw::LEFT );

    Ogre::Vector3 rotation( 0.0f, 0.0f, 0.0f );
    Ogre::Quaternion q( 0.0f, 0.0f, 0.0f, 1.0f );
    q.FromAngleAxis( Ogre::Radian( Ogre::Degree( m_Rotation ) ), Ogre::Vector3::UNIT_Z );
    rotation *= 10.0f;
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.4f ) );
    DEBUG_DRAW.Line( m_X, m_Y, m_X + rotation.x, m_Y + rotation.y );

    // line connecting
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 0, 0.1f ) );
    DEBUG_DRAW.Line( m_X, m_Y, x, y );

    // knowledge data
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, m_Knowledge[ 0 ] ) );
    DEBUG_DRAW.Quad( x +  0, y, x + 10, y, x + 10, y + 10, x +  0, y + 10 );
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, m_Knowledge[ 1 ] ) );
    DEBUG_DRAW.Quad( x + 10, y, x + 20, y, x + 20, y + 10, x + 10, y + 10 );
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 0, 1, m_Knowledge[ 2 ] ) );
    DEBUG_DRAW.Quad( x + 20, y, x + 30, y, x + 30, y + 10, x + 20, y + 10 );
    //DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    //DEBUG_DRAW.Text( x +  5, y + 5, FloatToString( m_Confidence[ 0 ] ) );
    //DEBUG_DRAW.Text( x + 15, y + 5, FloatToString( m_Confidence[ 1 ] ) );
    //DEBUG_DRAW.Text( x + 25, y + 5, FloatToString( m_Confidence[ 2 ] ) );
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
    if( m_KnowTime > 0 )
    {
        return;
    }
    m_KnowTime = KNOW_TIME;
}



void
Person::ChangeKnowledge( const int know_id, const float value )
{
    if( m_KnowTime > 0 )
    {
        return;
    }
    m_KnowTime = KNOW_TIME;

    if( m_Knowledge.size() > know_id )
    {
        return;
    }

    float diff = value - m_Knowledge[ know_id ];
    float moddiff = ( diff < 0 ) ? - diff : diff;
    m_Confidence[ know_id ] += ( moddiff < 0.5f ) ? 0.01f : -0.01f;
    float unconf = 1 - m_Confidence[ know_id ];
    m_Knowledge[ know_id ] += unconf * diff;
}
