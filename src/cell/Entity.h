#ifndef ENTITY_H
#define ENTITY_H

#include "Onthogenesis.h"



class Entity : public Onthogenesis
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

    float GetForwardImpulse() const;
    void SetForwardImpulse( const float forward_impulse );
    float GetLeftImpulse() const;
    void SetLeftImpulse( const float left_impulse );
    float GetRightImpulse() const;
    void SetRightImpulse( const float right_impulse );

    bool IsDead() const;
    float GetEnergy() const;
    void SetEnergy( const float energy );
    float GetFitness() const;
    void SetFitness( const float fitness );

    float GetSensorEnergy() const;
    float GetSensorFood( const float x, const float y ) const;
    float GetSensorEnemy( const float x, const float y ) const;

private:
    Entity();

private:
    float m_Radius;
    float m_X;
    float m_Y;
    float m_Rotation;

    float m_ForwardImpulse;
    float m_LeftImpulse;
    float m_RightImpulse;

    float m_Life;
    float m_Energy;
    float m_Fitness;

    float m_Think;
};



#endif // ENTITY_H
