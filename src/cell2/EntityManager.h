#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <OgreSingleton.h>

#include "../core/Event.h"
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

private:
    Ontogenesis* m_Ontogenesis;
};



#endif // ENTITY_MANAGER_H
