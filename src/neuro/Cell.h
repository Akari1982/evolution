#ifndef CELL_H
#define CELL_H


#include <OgreString.h>
#include <vector>



class Cell;
class Entity;



class Cell
{
friend class XmlNetworkFile;
friend void NetworkMutate( Entity* parent, Entity* child );

public:
    enum SynapseType
    {
        SYN_NEURON,
        SYN_FOOD,

        SYN_TYPE_MAX,
    };
    enum ActivatorType
    {
        ACT_MOVE,
        ACT_ROTATE,

        ACT_TYPE_MAX,
    };

    Cell( Entity* entity, const float x, const float y );
    virtual ~Cell();

    void Update();
    void UpdateFire();
    void Draw( const float ui_x, const float ui_y, const float x, const float y, const Ogre::Quaternion& rotation );

    void AddSynapse( const float power, const bool inverted, const int cell_id );
    void AddFoodSynapse( const float power, const bool inverted );

    void AddMoveActivator( const float power );
    void AddRotateActivator( const float power );

private:
    Cell();

private:
    Entity* m_Entity;

    float m_X;
    float m_Y;

    float m_Threshold;
    float m_Value;
    bool m_Fired;

    struct Synapse
    {
        SynapseType type;
        float power;
        bool inverted;
        int cell_id;
        bool activated;
    };
    std::vector< Synapse > m_Synapses;

    struct Activator
    {
        ActivatorType type;
        float power;
    };
    std::vector< Activator > m_Activators;
};



#endif // CELL_H
