#include "Person.h"

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



Person::Person( const float x, const float y ):
    m_TalkRadius( 0.0f ),
    m_HearRadius( 10.0f ),
    m_X( x ),
    m_Y( y ),
    m_Life( 200.0f )
{
}



Person::~Person()
{
}



void
Person::Update()
{
    float delta = Timer::getSingleton().GetGameTimeDelta();
    m_Life -= delta * 5.0;
}



void
Person::Draw()
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
    DEBUG_DRAW.Disc( m_X, m_Y, 2 );
    DEBUG_DRAW.Circle( m_X, m_Y, m_TalkRadius );
    DEBUG_DRAW.Circle( m_X, m_Y, m_HearRadius );
}



float
Person::GetTalkRadius() const
{
    return m_TalkRadius;
}



float
Person::GetHearRadius() const
{
    return m_HearRadius;
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



bool
Person::IsDead() const
{
    return ( m_Life <= 0 );
}
