#include "SocialManager.h"
#include "SocialManagerCommands.h"

#include "../core/DebugDraw.h"
#include "../core/InputManager.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



template<>SocialManager *Ogre::Singleton< SocialManager >::msSingleton = NULL;



const size_t MAX_ENTITY = 100;
const float SPAWN_TIME = 5.0f;



SocialManager::SocialManager():
    m_X( 100.0f ),
    m_Y( 300.0f ),
    m_Width( 1080.0f ),
    m_Height( 300.0f ),
    m_SpawnTime( SPAWN_TIME )
{
    InitCommands();
}



SocialManager::~SocialManager()
{
    for( unsigned int i = 0; i < m_Person.size(); ++i )
    {
        delete m_Person[ i ];
    }
}



void
SocialManager::Input( const Event& event )
{
}



void
SocialManager::Update()
{
    float delta = Timer::getSingleton().GetGameTimeDelta();
    m_SpawnTime -= delta;

    for( size_t i = 0; i < m_Person.size(); ++i )
    {
        Person* person = m_Person[ i ];
        person->Update();
    }



    // remove dead entity
    for( std::vector< Person* >::iterator it = m_Person.begin(); it != m_Person.end(); )
    {
        if( ( *it )->IsDead() == true )
        {
            delete ( *it );
            it = m_Person.erase( it );
        }
        else
        {
            ++it;
        }
    }



    if( m_SpawnTime <= 0 && m_Person.size() < MAX_ENTITY )
    {
        Person* person = new Person( m_X + rand() % ( int )m_Width, m_Y + rand() % ( int )m_Height );
        m_Person.push_back( person );
        m_SpawnTime = SPAWN_TIME;
    }



    Draw();
}



void
SocialManager::Draw()
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Line( m_X, m_Y, m_X, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X + m_Width, m_Y, m_X + m_Width, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X, m_Y, m_X + m_Width, m_Y );
    DEBUG_DRAW.Line( m_X, m_Y + m_Height, m_X + m_Width, m_Y + m_Height );

    for( size_t i = 0; i < m_Person.size(); ++i )
    {
        m_Person[ i ]->Draw();
    }
}
