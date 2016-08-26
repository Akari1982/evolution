#ifndef CELL_H
#define CELL_H


#include <OgreString.h>
#include <vector>



class Cell;
class Entity;



class Cell
{
public:
    Cell( Entity* entity, const Ogre::String& type, const int x, const int y );
    virtual ~Cell();
    void Update();
    void Draw( const unsigned int x, const unsigned int y );

    void SetTypeName( const Ogre::String& type );
    const Ogre::String& GetTypeName() const;
    float GetX() const;
    void SetX( const float x );
    float GetY() const;
    void SetY( const float y );
    const Ogre::String& GetProtein() const;
    void SetProtein( const Ogre::String& protein );
    int GetProteinRadius() const;
    void SetProteinRadius( const int protein_radius );

    void AddSynapse( const float power, const bool inverted, Cell* cell );

private:
    Cell();

protected:
    enum CellType
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
    CellType m_Type;
    Ogre::String m_TypeName;

    int m_X;
    int m_Y;

    Ogre::String m_Protein;
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
