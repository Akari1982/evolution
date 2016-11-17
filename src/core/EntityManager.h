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

    void InitCommands();
    void RunGeneration( const int type, Ogre::String& file_name );

    float FeelFood( const float x, const float y );
    bool CheckMove( Entity* entity, const float move_x, const float move_y );

private:
    Ontogenesis* m_Ontogenesis0;
    Ontogenesis* m_Ontogenesis1;
    std::vector< Entity* > m_Entity;
    int m_TypeNum0;
    int m_TypeNum1;

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
};



#endif // ENTITY_MANAGER_H
