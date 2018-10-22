#ifndef ENTITY_H
#define ENTITY_H

#include "Ontogenesis.h"



class Entity : public Ontogenesis
{
public:
    Entity( const float x, const float y );
    virtual ~Entity();

    void Update();
    void UpdateCells();
    void Draw( const float x, const float y );

    const unsigned int GetRadius() const;
    const float GetX() const;
    void SetX( const float x );
    const float GetY() const;
    void SetY( const float y );
    const float GetRotation() const;
    void SetRotation( const float rotation );

    const float GetForwardImpulse() const;
    void SetForwardImpulse( const float forward_impulse );
    const float GetLeftImpulse() const;
    void SetLeftImpulse( const float left_impulse );
    const float GetRightImpulse() const;
    void SetRightImpulse( const float right_impulse );

    const bool IsDead() const;
    const float GetEnergy() const;
    void SetEnergy( const float energy );
    const float GetFitness() const;
    void SetFitness( const float fitness );

    const float GetSensorEnergy() const;
    const float GetSensorFood( const float x, const float y ) const;
    const float GetSensorEnemy( const float x, const float y ) const;

private:
    Entity();

private:
    float m_X;
    float m_Y;
    float m_Rotation;

    float m_ForwardImpulse;
    float m_LeftImpulse;
    float m_RightImpulse;

    float m_Life;
    float m_Fitness;

    float m_Think;
};



#endif // ENTITY_H
