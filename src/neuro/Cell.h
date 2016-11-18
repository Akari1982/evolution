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
        SENSOR_FOOD_LEFT,
        SENSOR_FOOD_RIGHT,
        SENSOR_ENERGY,
        SENSOR_ENEMY,
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
    int GetInnerProtein() const;
    void SetInnerProtein( const int protein, const float power );
    float GetInnerProteinPower() const;
    void SetInnerProteinPower( const float power );
    int GetOuterProtein() const;
    void SetOuterProtein( const int protein );
    int GetOuterProteinRadius() const;
    void SetOuterProteinRadius( const int protein_radius );

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

    int m_InnerProtein;
    int m_InnerProteinPower;
    int m_OuterProtein;
    int m_OuterProteinRadius;

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
