#ifndef ONTOGENESIS_H
#define ONTOGENESIS_H

#include <vector>
#include <OgreString.h>

#include "Cell.h"
#include "Entity.h"
#include "Logger.h"



class Ontogenesis
{
public:
    Ontogenesis( const Ogre::String& file_name );
    virtual ~Ontogenesis();

    void Draw( const unsigned int x, const unsigned int y );

    void LoadNetwork( Entity* entity );
    void UpdateFitness( const float delta, const size_t generation_id, const size_t species_id );

private:
    Ontogenesis();

public:
    enum ConditionType
    {
        C_NAME = 0,
        C_NNAME,
        C_PROTEIN,
        C_NPROTEIN,

        C_TOTAL
    };

    enum ExpressionType
    {
        E_NAME = 0,
        E_SPLIT,
        E_MIGRATE,
        E_PROTEIN,
        E_DENDRITE,
        E_AXON,

        E_TOTAL
   };

    struct Condition
    {
        ConditionType type;
        int value;
    };

    struct Expression
    {
        ExpressionType type;
        int value;
    };

    struct Gene
    {
        unsigned int unique_id;
        float conserv;
        std::vector< Condition > cond;
        std::vector< Expression > expr;
    };

    struct Species
    {
        float fitness;
        std::vector< Gene > genome;
        std::vector< Cell* > network;
    };

    struct Generation
    {
        float top_fitness;
        std::vector< Gene > base_genome;
        std::vector< Species > species;
    };

private:
    struct PowerProtein
    {
        float power;
        size_t cell_id;
    };
    bool SearchProtein( std::vector< Cell* >& network, const int protein, const int x, const int y, std::vector< PowerProtein >& powers );
    bool FindPlaceForCell( std::vector< Cell* >& network, const int x, const int y, const int radius, int &new_x, int &new_y );
    bool IsCell( std::vector< Cell* >& network, const int x, const int y );
    std::vector< Gene > Mutate( std::vector< Gene >& genome );

    Ogre::String ConditionTypeToString( const ConditionType type );
    Ogre::String ExpressionTypeToString( const ExpressionType type );
    void DumpGenome( Logger* dump, std::vector< Gene >& genome );

private:
    Ogre::String m_FileName;

    unsigned int m_GeneUniqueId;
    std::vector< Generation > m_Generations;
};



#endif // ONTOGENESIS_H
