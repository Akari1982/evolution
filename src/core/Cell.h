#ifndef CELL_H
#define CELL_H


#include <OgreString.h>
#include <vector>



class Cell;
class Entity;



class Cell
{
public:
    enum CellType
    {
        STEM = 0,
        NEURON,
        SENSOR_FOOD_LEFT,
        SENSOR_FOOD_RIGHT,
        SENSOR_ENERGY,
        ACTIVATOR_FORWARD,
        ACTIVATOR_LEFT,
        ACTIVATOR_RIGHT,
        TOTAL
    };

    Cell( Entity* entity, const CellType type, const int x, const int y );
    virtual ~Cell();
    void Update();
    void Draw( const unsigned int x, const unsigned int y );

    void SetType( const CellType type );
    const CellType GetType() const;
    int GetX() const;
    void SetX( const int x );
    int GetY() const;
    void SetY( const int y );
    int GetProtein() const;
    void SetProtein( const int protein );
    int GetProteinRadius() const;
    void SetProteinRadius( const int protein_radius );

    void AddSynapse( const float power, const bool inverted, Cell* cell );

    static Ogre::String CellTypeToString( const CellType type );

private:
    Cell();

protected:
    Entity* m_Entity;
    CellType m_Type;

    int m_X;
    int m_Y;

    int m_Protein;
    int m_ProteinRadius;

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
