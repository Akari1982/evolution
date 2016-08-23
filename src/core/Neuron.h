#ifndef NEURON_H
#define NEURON_H


#include <vector>



class Neuron;
class Entity;



class Neuron
{
friend Neuron;
public:
    Neuron( Entity* entity, const int x, const int y, const float motor_x, const float motor_y );
    virtual ~Neuron();
    void Update();
    void Draw( const unsigned int x, const unsigned int y );

    void AddNeuronSynapse( const float power, const bool inverted, Neuron* neuron );
    void AddFoodSynapse( const float power, const bool inverted, const float length, const float degree );
    void AddEnergySynapse( const float power, const bool inverted );

private:
    Neuron();

protected:
    Entity* m_Entity;
    int m_X;
    int m_Y;
    float m_MotorX;
    float m_MotorY;

    float m_Threshold;
    float m_Value;
    bool m_Fired;

    enum SynapseType
    {
        NEURON,
        FOOD,
        ENERGY
    };

    struct Synapse
    {
        SynapseType type;
        float power;
        bool inverted;
        Neuron* neuron;
        float x;
        float y;
    };

    std::vector< Synapse > m_Synapses;
};



#endif // NEURON_H
