#ifndef CELL_H
#define CELL_H


#include <OgreString.h>
#include <vector>



class Cell;
class Entity;



class Cell
{
public:
    enum SynapseType
    {
        SYN_NEURON,
        SYN_FOOD,
    };
    enum ActivatorType
    {
        ACT_MOVE,
        ACT_ROTATE,
    };

    Cell( Entity* entity, const float x, const float y );
    virtual ~Cell();
    void Copy( Cell* cell );

    void Update();
    void UpdateFire();
    void Draw( const float ui_x, const float ui_y, const float x, const float y, const Ogre::Quaternion& rotation );

    void AddSynapse( const float power, const bool inverted, Cell* cell );
    void AddFoodSynapse( const float power, const bool inverted );

    void AddMoveActivator( const float move );
    void AddRotateActivator( const float rotate );

private:
    Cell();

protected:
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
        Cell* cell;
        bool activated;
    };
    std::vector< Synapse > m_Synapses;

    struct Activator
    {
        ActivatorType type;
        float move;
        float rotate;
    };
    std::vector< Activator > m_Activators;
};



#endif // CELL_H
