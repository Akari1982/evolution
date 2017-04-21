#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include <OgreSingleton.h>

#include "../core/Event.h"
#include "Person.h"



class SocialManager : public Ogre::Singleton< SocialManager >
{
public:
    SocialManager();
    virtual ~SocialManager();

    void Input( const Event& event );
    void Update();
    void Draw();

    void InitCommands();

private:
    std::vector< Person* > m_Person;

    float m_SpawnTime;

    float m_X;
    float m_Y;
    float m_Width;
    float m_Height;
};



#endif // SOCIAL_MANAGER_H
