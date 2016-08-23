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
    bool SearchProtein( const Ogre::String name, const float x, const float y, std::vector< PowerProtein >& powers );

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
        E_MOVEAWAY,
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
            x( 0.0f ),
            y( 0.0f ),
            protein( "" ),
            protein_radius( 0.0f )
        {
        }

        Ogre::String name;
        float x;
        float y;
        Ogre::String protein;
        float protein_radius;
        std::vector< Synapse > synapses;

        std::vector< unsigned int > expr_genes;
    };

private:
    std::vector< Gene > m_Genome;
    std::vector< Cell > m_Network;
};



#endif // ONTOGENESIS_H
