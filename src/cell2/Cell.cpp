#include "Cell.h"

#include <OgreMath.h>

#include "../core/DebugDraw.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



Cell::Cell():
    m_Division( 0 )
{
}



Cell::~Cell()
{
}



void
Cell::Draw()
{
}



int
Cell::GetX() const
{
    return m_X;
}



void
Cell::SetX( const int x )
{
    m_X = x;
}



int
Cell::GetY() const
{
    return m_Y;
}



void
Cell::SetY( const int y )
{
    m_Y = y;
}



int
Cell::GetDivision() const
{
    return m_Division;
}


void
Cell::SetDivision( const int div )
{
    m_Division = div;
}
