#ifndef ONTOGENESIS_H
#define ONTOGENESIS_H

#include <vector>
#include <OgreString.h>

#include "Cell.h"
#include "Entity.h"
#include "../core/Logger.h"



class Ontogenesis
{
public:
    enum ConditionType
    {
        C_PROTEIN = 0,
        C_TOTAL
    };

    enum ExpressionType
    {
        // реальные клетки могут делится на 2 одинаковые и на 2 неодинаковые части.
        // протеины заключенные в клетке могут поделится полностью или
        // каждый отдельный протеин может сконцентрироватся только в одной дочерней клетке
        // клетка делится только на 2 части
        // [bool = 1] - внутренний протеин наследуется
        E_SPLIT = 0,
        E_SPAWN,
        E_PROTEIN,
        E_DENDRITE,
        E_DENDRITE_I,
        E_AXON,
        E_AXON_I,

        E_TOTAL
   };

    struct Condition
    {
        ConditionType type;
        float value1;
        float value2;
        float value3;
    };

    struct Expression
    {
        ExpressionType type;
        float value1;
        float value2;
        float value3;
    };

    struct Gene
    {
        int id;
        float conserv;
        std::vector< Condition > cond;
        std::vector< Expression > expr;
    };

    struct ProteinData
    {
        float power;
        int x;
        int y;
    };

    struct Protein
    {
        int id;
        std::vector< ProteinData > data;
    };

    struct Species
    {
        float fitness;
        std::vector< Gene > genome;
        std::vector< Protein > proteins;
        std::vector< Cell* > network;
    };

    struct Generation
    {
        int id;
        float top_fitness;
        size_t top_id;
        std::vector< Gene > base_genome;
        std::vector< Species > species;

        Ogre::String file_name;
    };

public:
    Ontogenesis( const Ogre::String& file_prefix );
    virtual ~Ontogenesis();

    void Draw( const unsigned int x, const unsigned int y );

    void LoadNetwork( Entity* entity );
    void EntityDeath( Entity* entity );

    ConditionType ConditionStringToType( const Ogre::String& type );
    ExpressionType ExpressionStringToType( const Ogre::String& type );
    void LoadGeneration( const Generation& generation );

private:
    Ontogenesis();

private:
    struct PowerProtein
    {
        float power;
        size_t cell_id;
    };
    void AddProtein( std::vector< Protein >& proteins, const int id, const float power, const int x, const int y );
    const float GetProteinPower( std::vector< Protein >& proteins, const int protein, const int x, const int y );
    const int FindCellByProteinGradient( std::vector< Cell* >& network, std::vector< Protein >& proteins, const int protein, const int x, const int y );
    const float FindProteinGradient( std::vector< ProteinData >& data, const int x, const int y, const float power, int& ret_x, int& ret_y );
    const bool FindPlaceForCell( std::vector< Cell* >& network, const int x, const int y, const int radius, int &new_x, int &new_y );
    const int GetCell( std::vector< Cell* >& network, const int x, const int y );
    std::vector< Gene > Mutate( std::vector< Gene >& genome );
    Condition GenerateRandomCondition();
    void GenerateRandomConditionValue( Condition& cond );
    Expression GenerateRandomExpression();
    void GenerateRandomExpressionValue( Expression& expr );

    Ogre::String ConditionTypeToString( const ConditionType type );
    Ogre::String ExpressionTypeToString( const ExpressionType type );
    void DumpGenome( Logger* file, std::vector< Gene >& genome );
    void DumpGeneration( Generation& generation );

private:
    Ogre::String m_FilePrefix;

    unsigned int m_GeneUniqueId;
    std::vector< Generation > m_Generations;
};



#endif // ONTOGENESIS_H
