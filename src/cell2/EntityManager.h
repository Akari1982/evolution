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

    void DebugOnthogenesisAddCell( const int x, const int y );
    void DebugOnthogenesisAddProtein( const int id, const float radius, const int x, const int y );

    void RunGeneration( const int type, Ogre::String& file_name );

    float FeelFood( const float x, const float y );
    float FeelEnemy( const int type, const float x, const float y );
    bool CheckMove( Entity* entity, const float move_x, const float move_y );

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

};



#endif // ENTITY_MANAGER_H
