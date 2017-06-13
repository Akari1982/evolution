#ifndef ENTITY_H
#define ENTITY_H

#include "Cell.h"



class Entity
{
public:
    Entity( const int name, const float x, const float y );
    virtual ~Entity();

    void Update();
    void Draw( const float x, const float y );

    int GetName() const;
    void SetParents( std::vector< int >& parents );
    std::vector< int >& GetParents();
    void AddChild( const int child );
    std::vector< int >& GetChildren();

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

    void SetNetwork( std::vector< Cell* >& network );
    std::vector< Cell* >& GetNetwork();
    Cell* GetCell( const int cell_id );

    float GetSensorFood( const float x, const float y ) const;

private:
    Entity();

private:
    int m_Name;
    std::vector< int > m_Parents;
    std::vector< int > m_Children;

    float m_Radius;
    float m_X;
    float m_Y;
    float m_Rotation;

    float m_MovePower;
    float m_MoveTime;
    float m_RotatePower;
    float m_RotateTime;

    float m_Life;
    float m_Energy;

    float m_Think;
    std::vector< Cell* > m_Network;
};



#endif // ENTITY_H
