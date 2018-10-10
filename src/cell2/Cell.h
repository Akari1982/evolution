#ifndef CELL_H
#define CELL_H


#include <OgreString.h>
#include <vector>



class Cell
{
public:
    Cell();
    virtual ~Cell();

    void Update();
    void Draw();

    int GetX() const;
    void SetX( const int x );
    int GetY() const;
    void SetY( const int y );

    int GetDivision() const;
    void SetDivision( const int div );

protected:
    int m_X;
    int m_Y;

    int m_Division;
};



#endif // CELL_H
