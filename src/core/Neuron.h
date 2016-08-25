#ifndef NEURON_H
#define NEURON_H


#include <OgreString.h>
#include <vector>



class Neuron;
class Entity;



class Neuron
{
friend Neuron;
public:
    Neuron( Entity* entity, const Ogre::String& type, const int x, const int y );
    virtual ~Neuron();
    void Update();
    void Draw( const unsigned int x, const unsigned int y );

    void AddSynapse( const float power, const bool inverted, Neuron* neuron );

private:
    Neuron();

protected:
    enum NeuronType
    {
        NEURON,
        SENSOR_FOOD_LEFT,
        SENSOR_FOOD_RIGHT,
        SENSOR_ENERGY,
        ACTIVATOR_FORWARD,
        ACTIVATOR_LEFT,
        ACTIVATOR_RIGHT
    };

    Entity* m_Entity;
    NeuronType m_Type;
    int m_X;
    int m_Y;

    float m_Threshold;
    float m_Value;
    bool m_Fired;

    struct Synapse
    {
        float power;
        bool inverted;
        Neuron* neuron;
        float x;
        float y;
    };

    std::vector< Synapse > m_Synapses;
};



#endif // NEURON_H
