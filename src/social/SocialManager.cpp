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
    m_Height( 300.0f )
{
    InitCommands();

    Area area;
    area.x = 100.0f;
    area.y = 100.0f;
    area.radius = 130.0f;
    area.know_id = 0;
    m_Area.push_back( area );
    area.x = 300.0f;
    area.y = 200.0f;
    area.radius = 130.0f;
    area.know_id = 1;
    m_Area.push_back( area );
    area.x = 700.0f;
    area.y = 300.0f;
    area.radius = 100.0f;
    area.know_id = 2;
    m_Area.push_back( area );
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
    static float spawn_time = SPAWN_TIME;
    spawn_time -= delta;



    for( size_t i = 0; i < m_Person.size(); ++i )
    {
        Person* person = m_Person[ i ];
        person->Update();



        // perform movement
        float start_x = person->GetX();
        float start_y = person->GetY();
        float rotation = person->GetRotation();
        float radius = person->GetRadius();

        float pos_x = start_x + Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * 20.0f;
        float pos_y = start_y + Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * 20.0f;
        pos_x = ( pos_x < m_X ) ? m_Width + pos_x : pos_x;
        pos_x = ( pos_x > m_X + m_Width ) ? 2 * m_X + m_Width - pos_x : pos_x;
        pos_y = ( pos_y < m_Y ) ? m_Height + pos_y : pos_y;
        pos_y = ( pos_y > m_Y + m_Height ) ? 2 * m_Y + m_Height - pos_y : pos_y;
        person->SetX( pos_x );
        person->SetY( pos_y );

        // check entity / area collision
        for( unsigned int i = 0; i < m_Area.size(); ++i )
        {
            float x1 = m_X + m_Area[ i ].x;
            float y1 = m_Y + m_Area[ i ].y;
            float distance = sqrt( ( pos_x - x1 ) * ( pos_x - x1 ) + ( pos_y - y1 ) * ( pos_y - y1 ) );
            if( distance <= m_Area[ i ].radius )
            {
                person->ChangeKnowledge( m_Area[ i ].know_id, 1.0f );
            }
        }

        // check entity / entity collision
        for( size_t j = 0; j < m_Person.size(); ++j )
        {
            float x1 = m_Person[ j ]->GetX();
            float y1 = m_Person[ j ]->GetY();
            float radius1 = m_Person[ j ]->GetRadius();
            float distance = sqrt( ( pos_x - x1 ) * ( pos_x - x1 ) + ( pos_y - y1 ) * ( pos_y - y1 ) );
            if( distance <= radius + radius1 )
            {
                if( radius >= radius1 )
                {
                    //m_Person[ j ]->ChangeKnowledge( i, 1, person->GetKnowledge( 1 ) );
                }
            }
        }
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



    if( spawn_time <= 0 && m_Person.size() < MAX_ENTITY )
    {
        Person* person = new Person( m_X + rand() % ( int )m_Width, m_Y + rand() % ( int )m_Height );
        m_Person.push_back( person );
        spawn_time = SPAWN_TIME;
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
        m_Person[ i ]->Draw( 50 + i * 120, 120 );
    }

    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 0, 0, 0.3f ) );
    DEBUG_DRAW.Disc( m_X + m_Area[ 0 ].x, m_Y + m_Area[ 0 ].y, m_Area[ 0 ].radius );
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.3f ) );
    DEBUG_DRAW.Disc( m_X + m_Area[ 1 ].x, m_Y + m_Area[ 1 ].y, m_Area[ 1 ].radius );
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 0, 1, 0.3f ) );
    DEBUG_DRAW.Disc( m_X + m_Area[ 2 ].x, m_Y + m_Area[ 2 ].y, m_Area[ 2 ].radius );
}
