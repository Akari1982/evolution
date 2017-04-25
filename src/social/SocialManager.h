#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include <OgreSingleton.h>

#include "../core/Event.h"
#include "Person.h"



struct Area
{
    float x;
    float y;
    float radius;
    unsigned int know_id;
};



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
    std::vector< Area > m_Area;
    std::vector< Person* > m_Person;

    float m_X;
    float m_Y;
    float m_Width;
    float m_Height;
};



#endif // SOCIAL_MANAGER_H
