#ifndef ENTITY_H
#define ENTITY_H

#include "Cell.h"



class Entity
{
public:
    Entity( const float x, const float y );
    virtual ~Entity();

    void Update();
    void Draw( const float x, const float y );

    float GetRadius() const;
    float GetX() const;
    void SetX( const float x );
    float GetY() const;
    void SetY( const float y );
    float GetRotation() const;
    void SetRotation( const float rotation );

    float GetMovePower() const;
    float GetMoveTime() const;
    void SetMove( const float power, const float time );
    float GetRotatePower() const;
    float GetRotateTime() const;
    void SetRotate( const float power, const float time );

    bool IsDead() const;
    float GetEnergy() const;
    void SetEnergy( const float energy );

    void AddNetwork( std::vector< Cell* >& network );
    std::vector< Cell* >& GetNetwork();

    float GetSensorFood( const float x, const float y ) const;

private:
    Entity();

private:
    float m_Radius;
    float m_X;
    float m_Y;
    float m_Rotation;

    float m_MovePower;
    float m_MoveTime
    float m_RotatePower;
    float m_RotateTime;

    float m_Life;
    float m_Energy;

    float m_Think;
    std::vector< Cell* > m_Network;
};



#endif // ENTITY_H
