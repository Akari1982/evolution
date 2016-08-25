#ifndef ENTITY_H
#define ENTITY_H

#include "Event.h"
#include "Neuron.h"



class Entity
{
public:
    Entity( const float x, const float y );
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

    void AddNeuron( const Ogre::String& type, const unsigned int x, const unsigned int y );
    void AddSynapse( const unsigned int self_id, const float power, const bool inverted, const unsigned int neuron_id );

    float GetSensorFood() const;
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

    float m_Think;
    std::vector< Neuron* > m_Network;
};



#endif // ENTITY_H
