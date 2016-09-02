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

    Cell( Entity* entity, const CellName name, const int x, const int y );
    virtual ~Cell();
    void Update();
    void Draw( const unsigned int x, const unsigned int y );

    const CellName GetName() const;
    const CellType GetType() const;
    int GetX() const;
    void SetX( const int x );
    int GetY() const;
    void SetY( const int y );
    int GetOuterProtein() const;
    void SetOuterProtein( const int protein );
    int GetOuterProteinRadius() const;
    void SetOuterProteinRadius( const int protein_radius );
    int GetInnerProtein() const;
    void SetInnerProtein( const int protein );

    void AddSynapse( const float power, const bool inverted, Cell* cell );

    static Ogre::String CellTypeToString( const CellType type );

private:
    Cell();

protected:
    Entity* m_Entity;
    CellName m_Name;
    CellType m_Type;

    int m_X;
    int m_Y;

    int m_OuterProtein;
    int m_OuterProteinRadius;
    int m_InnerProtein;

    float m_Threshold;
    float m_Value;
    bool m_Fired;

    struct Synapse
    {
        float power;
        bool inverted;
        Cell* cell;
        float x;
        float y;
    };

    std::vector< Synapse > m_Synapses;
};



#endif // CELL_H
