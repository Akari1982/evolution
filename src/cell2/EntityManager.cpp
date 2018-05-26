#include "EntityManager.h"
#include "EntityManagerCommands.h"

#include "../core/DebugDraw.h"
#include "../core/InputManager.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



template<>EntityManager *Ogre::Singleton< EntityManager >::msSingleton = NULL;



EntityManager::EntityManager()
{
    InitCommands();

    m_Ontogenesis = new Ontogenesis();
}



EntityManager::~EntityManager()
{
    delete m_Ontogenesis;
}



void
EntityManager::Input( const Event& event )
{
    if( event.type == ET_PRESS )
    {
        if( event.event == "ontho_step" )
        {
            m_Ontogenesis->Step();
        }
    }
}



void
EntityManager::Update()
{
    Draw();
}



void
EntityManager::Draw()
{
    m_Ontogenesis->Draw();
}
