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

    float GetX() const;
    float GetY() const;
    float GetRadius() const;
    float GetEnergy() const;

    void AddNeuron( const unsigned int x, const unsigned int y, const float motor_x, const float motor_y );
    void AddSynapse( const unsigned int self_id, const Ogre::String type, const float power, const bool inverted, const unsigned int neuron_id, const float length, const float degree );

    void AddEnergy( float energy );
    void Move( const float x, const float y );

private:
    Entity();

private:
    float m_Think;
    float m_X;
    float m_Y;
    std::vector< Neuron* > m_Network;

    float m_Radius;
    float m_Energy;
};



#endif // ENTITY_H
