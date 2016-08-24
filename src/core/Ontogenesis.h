#ifndef ONTOGENESIS_H
#define ONTOGENESIS_H

#include <vector>
#include <OgreString.h>

#include "Entity.h"



class Ontogenesis
{
public:
    Ontogenesis();
    virtual ~Ontogenesis();

    void LoadNetwork( Entity* entity );
    void Update();

private:
    struct PowerProtein
    {
        float power;
        size_t cell_id;
    };
    bool SearchProtein( const Ogre::String name, const int x, const int y, std::vector< PowerProtein >& powers );
    bool FindPlaceForCell( const int x, const int y, const int radius, int &new_x, int &new_y );
    bool IsCell( const int x, const int y );

public:
    enum ConditionType
    {
        C_NAME = 0,
        C_NNAME,
        C_PROTEIN,
        C_NPROTEIN
    };

    enum ExpressionType
    {
        E_NAME = 0,
        E_SPLIT,
        E_MIGRATE,
        E_PROTEIN,
        E_DENDRITE,
        E_AXON
    };

    struct Condition
    {
        ConditionType type;
        Ogre::String name;
    };

    struct Expression
    {
        ExpressionType type;
        Ogre::String name;
    };

    struct Gene
    {
        Ogre::String name;

        std::vector< Condition > cond;
        std::vector< Expression > expr;
    };

    struct Synapse
    {
        float power;
        size_t cell_id;
    };

    struct Cell
    {
        Cell():
            name( "" ),
            x( 0 ),
            y( 0 ),
            protein( "" ),
            protein_radius( 0 )
        {
        }

        Ogre::String name;
        int x;
        int y;
        Ogre::String protein;
        int protein_radius;
        std::vector< Synapse > synapses;

        std::vector< unsigned int > expr_genes;
    };

private:
    std::vector< Gene > m_Genome;
    std::vector< Cell > m_Network;
};



#endif // ONTOGENESIS_H
