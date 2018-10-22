#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <OgreSingleton.h>

#include "../core/Event.h"
#include "Entity.h"



class EntityManager : public Ogre::Singleton< EntityManager >
{
public:
    EntityManager();
    virtual ~EntityManager();

    void Input( const Event& event );
    void Update();
    void Draw();

    void InitCommands();
    void RunGeneration( Ogre::String& file_name );
    struct Generation
    {
        int id;
        float top_fitness;
        size_t top_id;
        std::vector< Ontogenesis::Gene > base_genome;
        std::vector< Entity* > species;

        Ogre::String file_name;
    };
    void LoadGeneration( const Generation& generation );
    void DumpGeneration( Generation& generation ) const;

    const int GetGenerationByEntity( Entity* entity, size_t& species_id ) const;

    const float FeelFood( const float x, const float y ) const;
    const float FeelEnemy( const float x, const float y ) const;
    const bool CheckMove( Entity* entity, const float move_x, const float move_y ) const;

private:
    std::vector< Entity* > m_Entity;

    struct Food
    {
        float power;
        float x;
        float y;
    };
    std::vector< Food > m_Food;

    float m_NextFoodTime;
    float m_SpawnTime;

    float m_X;
    float m_Y;
    float m_Width;
    float m_Height;

    Generation m_Generation;
    Generation m_GenerationPrev;
};



#endif // ENTITY_MANAGER_H
