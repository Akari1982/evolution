#ifndef ONTOGENESIS_H
#define ONTOGENESIS_H

#include <vector>
#include <OgreString.h>

#include "Cell.h"
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

public:
    Ontogenesis();
    virtual ~Ontogenesis();

    void DevelopmentStep();

    void SetGenome( std::vector< Gene >& genome );
    std::vector< Gene > GetGenome() const;

    static Ogre::String ConditionTypeToString( const ConditionType type );
    static Ogre::String ExpressionTypeToString( const ExpressionType type );
    static ConditionType ConditionStringToType( const Ogre::String& type );
    static ExpressionType ExpressionStringToType( const Ogre::String& type );

    static void DumpGenome( Logger* file, std::vector< Gene >& genome );
private:
    struct PowerProtein
    {
        float power;
        size_t cell_id;
    };
    void AddProtein( const int id, const float power, const int x, const int y );
    const float GetProteinPower( const int protein, const int x, const int y );
    const int FindCellByProteinGradient( const int protein, const int x, const int y );
    const float FindProteinGradient( std::vector< ProteinData >& data, const int x, const int y, const float power, int& ret_x, int& ret_y );
    const bool FindPlaceForCell( const int x, const int y, const int radius, int &new_x, int &new_y );
    const int GetCell( const int x, const int y );
    std::vector< Gene > Mutate( std::vector< Gene >& genome );
    Condition GenerateRandomCondition();
    void GenerateRandomConditionValue( Condition& cond );
    Expression GenerateRandomExpression();
    void GenerateRandomExpressionValue( Expression& expr );

protected:
    std::vector< Cell* > m_Cells;
    std::vector< Gene > m_Genome;
    std::vector< Protein > m_Proteins;

    static int m_GeneUniqueId;

    float m_Energy;
};



#endif // ONTOGENESIS_H
