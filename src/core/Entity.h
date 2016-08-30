#ifndef ENTITY_H
#define ENTITY_H

#include "Cell.h"



class Entity
{
public:
    Entity( const int type, const float x, const float y );
    virtual ~Entity();

    void Update();
    void Draw( const unsigned int x, const unsigned int y );

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

    float GetEnergy() const;
    void SetEnergy( const float energy );

    size_t GetGenerationId() const;
    size_t GetSpeciesId() const;
    int GetType() const;

    void AddNetwork( std::vector< Cell* >& network, const size_t generation_id, const size_t species_id );

    float GetSensorFoodLeft() const;
    float GetSensorFoodRight() const;
    float GetSensorEnergy() const;

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

    float m_Energy;
    size_t m_GenerationId;
    size_t m_SpeciesId;
    int m_Type;

    float m_Think;
    std::vector< Cell* > m_Network;
};



#endif // ENTITY_H
