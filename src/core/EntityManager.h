#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <OgreSingleton.h>

#include "Event.h"
#include "Entity.h"
#include "Ontogenesis.h"



class EntityManager : public Ogre::Singleton< EntityManager >
{
public:
    EntityManager();
    virtual ~EntityManager();

    void Input( const Event& event );
    void Update();
    void Draw();

    float FeelFood( const float x, const float y, const float radius );
    bool CheckMove( Entity* entity, const float move_x, const float move_y );
    void UpdateFitness( const int type, const float fitness, const size_t generation_id, const size_t species_id );

private:
    Ontogenesis m_Ontogenesis0;
    Ontogenesis m_Ontogenesis1;
    std::vector< Entity* > m_Entity;
    int m_TypeNum0;
    int m_TypeNum1;

    struct Food
    {
        float nutrition;
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
};



#endif // ENTITY_MANAGER_H
