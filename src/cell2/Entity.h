#ifndef ENTITY_H
#define ENTITY_H

#include "Ontogenesis.h"



class Entity : public Ontogenesis
{
public:
    Entity( const int type, const float x, const float y );
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

    size_t GetGenerationId() const;
    size_t GetSpeciesId() const;
    int GetType() const;

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

    size_t m_GenerationId;
    size_t m_SpeciesId;
    int m_Type;

    float m_Think;
};



#endif // ENTITY_H
