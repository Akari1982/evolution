#include "EntityManager.h"

#include "DebugDraw.h"
#include "InputManager.h"
#include "Logger.h"
#include "Timer.h"
#include "Ontogenesis.h"



template<>EntityManager *Ogre::Singleton< EntityManager >::msSingleton = NULL;



Ontogenesis genome;



EntityManager::EntityManager():
    m_X( 100.0f ),
    m_Y( 300.0f ),
    m_Width( 1080.0f ),
    m_Height( 300.0f ),
    m_NextFoodTime( 5.0f ),
    m_SpawnTime( 10.0f )
{
    Entity* entity = new Entity( m_X + m_Width / 2.0f - 50.0f, m_Y + m_Height / 2.0f - 50.0f );
    genome.LoadNetwork( entity );
    m_Entity.push_back( entity );
/*
    entity = new Entity( m_X + m_Width / 2.0f - 50.0f, m_Y + m_Height / 2.0f + 50.0f );
    genome.LoadNetwork( entity );
    m_Entity.push_back( entity );

    entity = new Entity( m_X + m_Width / 2.0f + 50.0f, m_Y + m_Height / 2.0f - 50.0f );
    genome.LoadNetwork( entity );
    m_Entity.push_back( entity );

    entity = new Entity( m_X + m_Width / 2.0f + 50.0f, m_Y + m_Height / 2.0f + 50.0f );
    genome.LoadNetwork( entity );
    m_Entity.push_back( entity );
*/
    LOG_TRIVIAL( "EntityManager created." );
}



EntityManager::~EntityManager()
{
    for( unsigned int i = 0; i < m_Entity.size(); ++i )
    {
        delete m_Entity[ i ];
    }

    LOG_TRIVIAL( "EntityManager destroyed." );
}



void
EntityManager::Input( const Event& event )
{
}



void
EntityManager::Update()
{
    genome.Update();

/*
    float delta = Timer::getSingleton().GetGameTimeDelta();
    m_NextFoodTime -= delta;
    m_SpawnTime -= delta;

    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        m_Entity[ i ]->Update();

        // check entity [ food collision
        for( std::vector< Food >::iterator it = m_Food.begin(); it != m_Food.end(); )
        {
            float x1 = ( *it ).x;
            float y1 = ( *it ).y;
            float x2 = m_Entity[ i ]->GetX();
            float y2 = m_Entity[ i ]->GetY();
            float distance = sqrt( ( x2 - x1 ) * ( x2 - x1 ) + ( y2 - y1 ) * ( y2 - y1 ) );
            if( distance <= m_Entity[ i ]->GetRadius() )
            {
                m_Entity[ i ]->AddEnergy( ( *it ).nutrition );
                it = m_Food.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }



    // remove dead entity
    for( std::vector< Entity* >::iterator it = m_Entity.begin(); it != m_Entity.end(); )
    {
        if( ( *it )->GetEnergy() <= 0 )
        {
            it = m_Entity.erase( it );
        }
        else
        {
            ++it;
        }
    }



    if( m_SpawnTime <= 0 && m_Entity.size() < 12 )
    {
        Entity* entity = new Entity( m_X + rand() % ( int )m_Width, m_Y + rand() % ( int )m_Height );
        Ontogenesis genome;
        genome.LoadNetwork( entity );
        m_Entity.push_back( entity );

        m_SpawnTime = 10.0f;
    }



    if( m_NextFoodTime <= 0 && m_Food.size() < 50 )
    {
        Food food;
        food.nutrition = 100;
        food.x = m_X + rand() % ( int )m_Width;
        food.y = m_Y + rand() % ( int )m_Height;
        m_Food.push_back( food );

        m_NextFoodTime = 5.0f;
    }



    Draw();
*/
}



void
EntityManager::Draw()
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Line( m_X, m_Y, m_X, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X + m_Width, m_Y, m_X + m_Width, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X, m_Y, m_X + m_Width, m_Y );
    DEBUG_DRAW.Line( m_X, m_Y + m_Height, m_X + m_Width, m_Y + m_Height );

    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        m_Entity[ i ]->Draw( 100 + i * 100, 100 );
    }

    for( size_t i = 0; i < m_Food.size(); ++i )
    {
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, m_Food[ i ].nutrition / 100.0f, 0, 1 ) );
        float radius = 1;
        float x = m_Food[ i ].x;
        float y = m_Food[ i ].y;
        DEBUG_DRAW.Quad( x - radius, y - radius, x + radius, y - radius, x + radius, y + radius, x - radius, y + radius );
    }
}



float
EntityManager::FeelFood( const float x, const float y, const float radius )
{
    float ret = 0.0f;

    for( size_t i = 0; i < m_Food.size(); ++i )
    {
        float x1 = m_Food[ i ].x;
        float y1 = m_Food[ i ].y;
        float distance = sqrt( ( x - x1 ) * ( x - x1 ) + ( y - y1 ) * ( y - y1 ) );
        if( distance <= radius )
        {
            ret += 1.0f - distance / radius;
        }
    }

    return ( ret > 1.0f ) ? 1.0f: ret;
}



bool
EntityManager::CheckMove( Entity* entity, const float move_x, const float move_y )
{
    float pos_x = entity->GetX() + move_x;
    float pos_y = entity->GetY() + move_y;
    float radius = entity->GetRadius();

    if( ( pos_x - radius < m_X ) || ( pos_x + radius > m_X + m_Width ) )
    {
        return false;
    }
    if( ( pos_y - radius < m_Y ) || ( pos_y + radius > m_Y + m_Height ) )
    {
        return false;
    }

    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        if( m_Entity[ i ] != entity )
        {
            float x2 = m_Entity[ i ]->GetX();
            float y2 = m_Entity[ i ]->GetY();
            float distance = sqrt( ( x2 - pos_x ) * ( x2 - pos_x ) + ( y2 - pos_y ) * ( y2 - pos_y ) );
            if( distance <= radius + m_Entity[ i ]->GetRadius() )
            {
                return false;
            }
        }
    }

    return true;
}
