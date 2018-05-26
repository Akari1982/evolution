#ifndef ONTOGENESIS_H
#define ONTOGENESIS_H

#include <vector>
#include <OgreString.h>

#include "../core/Logger.h"

#include "Cell.h"



class Ontogenesis
{
public:
    // условия активации гена
    enum ConditionType
    {
        C_PROTEIN = 0,      // есть белок в среде
        C_DIVISION,         // количество делений клетки
        C_TOTAL
    };

    // что экспрессирует ген
    enum ExpressionType
    {
        // реальные клетки могут делится на 2 одинаковые и на 2 неодинаковые части.
        // протеины заключенные в клетке могут поделится полностью или
        // каждый отдельный протеин может сконцентрироватся только в одной дочерней клетке
        // клетка делится только на 2 части
        // [bool = 1] - внутренний протеин наследуется
        E_SPLIT = 0,    // клетка делится
        E_PROTEIN,      // создать белок

        E_SPAWN,        // клетка порождает другую клетку не меняя себя
        E_MIGRATE,      // клетка ползет
        E_DENDRITE,     // породить дендрит
        E_DENDRITE_I,   // породить инвертированный дендрит
        E_AXON,         // породить аксон
        E_AXON_I,       // породить инвертированный аксон
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

public:
    Ontogenesis();
    virtual ~Ontogenesis();

    void Draw();

    void Step();

private:
    struct Protein
    {
        int id;
        float power;
        int x;
        int y;
    };
    float SearchProtein( const int protein, const int x, const int y );
    void MoveCell( Cell* cell, const int dir_x, const int dir_y );
    void DirToMove( const int dir, int& x, int& y );
    Cell* GetCell( const int x, const int y );

    Ogre::String ConditionTypeToString( const ConditionType type );
    Ogre::String ExpressionTypeToString( const ExpressionType type );

private:
    std::vector< Gene > m_Genome;
    std::vector< Cell* > m_Cells;
    std::vector< Protein > m_Proteins;
};



#endif // ONTOGENESIS_H
