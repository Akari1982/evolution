#ifndef CELL_H
#define CELL_H


#include <OgreString.h>
#include <vector>



class Cell;
class Entity;



class Cell
{
public:
    enum CellName
    {
        NEURON_COMMON = 0,
        SENSOR_FOOD,
        SENSOR_ENERGY,
        ACTIVATOR_FORWARD,
        ACTIVATOR_LEFT,
        ACTIVATOR_RIGHT
    };
    enum CellType
    {
        NEURON = 0,
        SENSOR,
        ACTIVATOR
    };

    Cell( Entity* entity, const CellName name, const float x, const float y );
    virtual ~Cell();
    void Update();
    void Draw( const float x, const float y );

    const CellName GetName() const;
    const CellType GetType() const;
    float GetX() const;
    void SetX( const float x );
    float GetY() const;
    void SetY( const float y );

    void AddSynapse( const float power, const bool inverted, Cell* cell );

    static Ogre::String CellTypeToString( const CellType type );

private:
    Cell();

protected:
    Entity* m_Entity;
    CellName m_Name;
    CellType m_Type;

    float m_X;
    float m_Y;

    float m_Threshold;
    float m_Value;
    bool m_Fired;

    struct Synapse
    {
        float power;
        bool inverted;
        Cell* cell;
    };

    std::vector< Synapse > m_Synapses;
};



#endif // CELL_H
