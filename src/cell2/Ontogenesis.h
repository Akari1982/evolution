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
        E_SPLIT = 0,        // клетка делится случайно
        E_SPLIT_GRADIENT,   // клетка делится по градиенту протеина
        E_PROTEIN,      // создать белок

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

    void AddCell( const int x, const int y );
    void AddProtein( const int id, const float power, const int x, const int y );

private:
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
    float GetProteinPower( const int protein, const int x, const int y );
    void MoveCell( Cell* cell, const int dir_x, const int dir_y );
    void DirToMove( const int dir, int& x, int& y );
    Cell* GetCell( const int x, const int y );

    Ogre::String ConditionTypeToString( const ConditionType type );
    Ogre::String ExpressionTypeToString( const ExpressionType type );

public:
    std::vector< Cell* > m_Cells;

private:
    std::vector< Gene > m_Genome;
    std::vector< Protein > m_Proteins;
};



#endif // ONTOGENESIS_H
