#include "EntityManager.h"

#include "DebugDraw.h"
#include "InputManager.h"
#include "Logger.h"
#include "Timer.h"



template<>EntityManager *Ogre::Singleton< EntityManager >::msSingleton = NULL;



EntityManager::EntityManager():
    m_X( 100.0f ),
    m_Y( 300.0f ),
    m_Width( 1080.0f ),
    m_Height( 300.0f ),
    m_NextFoodTime( 1.0f ),
    m_SpawnTime( 10.0f )
{
    Entity* entity = new Entity( m_X + m_Width / 2.0f - 50.0f, m_Y + m_Height / 2.0f - 50.0f );
    m_Ontogenesis.LoadNetwork( entity );
    m_Entity.push_back( entity );

    entity = new Entity( m_X + m_Width / 2.0f - 50.0f, m_Y + m_Height / 2.0f + 50.0f );
    m_Ontogenesis.LoadNetwork( entity );
    m_Entity.push_back( entity );

    entity = new Entity( m_X + m_Width / 2.0f + 50.0f, m_Y + m_Height / 2.0f - 50.0f );
    m_Ontogenesis.LoadNetwork( entity );
    m_Entity.push_back( entity );

    entity = new Entity( m_X + m_Width / 2.0f + 50.0f, m_Y + m_Height / 2.0f + 50.0f );
    m_Ontogenesis.LoadNetwork( entity );
    m_Entity.push_back( entity );
}



EntityManager::~EntityManager()
{
    for( unsigned int i = 0; i < m_Entity.size(); ++i )
    {
        delete m_Entity[ i ];
    }
}



void
EntityManager::Input( const Event& event )
{
}



void
EntityManager::Update()
{
    float delta = Timer::getSingleton().GetGameTimeDelta();
    m_NextFoodTime -= delta;
    m_SpawnTime -= delta;

    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        Entity* entity = m_Entity[ i ];
        entity->Update();



        // perform movement
        float start_x = entity->GetX();
        float start_y = entity->GetY();
        float rotation = entity->GetRotation();
        float radius = entity->GetRadius();
        float forward_impulse = entity->GetForwardImpulse();
        float left_impulse = entity->GetLeftImpulse();
        float right_impulse = entity->GetRightImpulse();
        if( left_impulse > 0.0f )
        {
            rotation -= 5.0f * delta;
            if( rotation < 0.0f )
            {
                rotation = ceil( -rotation / 360.0f ) * 360.0 - rotation;
            }
            entity->SetRotation( rotation );
            left_impulse -= delta;
            left_impulse = ( left_impulse < 0.0f ) ? 0.0f : left_impulse;
            entity->SetLeftImpulse( left_impulse );
        }
        if( right_impulse > 0.0f )
        {
            rotation += 5.0f * delta;
            if( rotation > 360.0f )
            {
                rotation -= ceil( rotation / 360.0f ) * 360.0;
            }
            entity->SetRotation( rotation );
            right_impulse -= delta;
            right_impulse = ( right_impulse < 0.0f ) ? 0.0f : right_impulse;
            entity->SetRightImpulse( right_impulse );
        }
        if( forward_impulse > 0.0f )
        {
            float pos_x = start_x + Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * 20.0f;
            float pos_y = start_y + Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * 20.0f;
            if( ( pos_x - radius > m_X ) && ( pos_x + radius < m_X + m_Width ) && ( pos_y - radius > m_Y ) && ( pos_y + radius < m_Y + m_Height ) )
            {
                entity->SetX( pos_x );
                entity->SetY( pos_y );
            }
            forward_impulse -= delta;
            forward_impulse = ( forward_impulse < 0.0f ) ? 0.0f : forward_impulse;
            entity->SetForwardImpulse( forward_impulse );
        }



        // check entity [ food collision
        for( std::vector< Food >::iterator it = m_Food.begin(); it != m_Food.end(); )
        {
            float x1 = ( *it ).x;
            float y1 = ( *it ).y;
            float x2 = entity->GetX();
            float y2 = entity->GetY();
            float distance = sqrt( ( x2 - x1 ) * ( x2 - x1 ) + ( y2 - y1 ) * ( y2 - y1 ) );
            if( distance <= entity->GetRadius() )
            {
                entity->SetEnergy( entity->GetEnergy() + ( *it ).nutrition );
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
        m_Ontogenesis.LoadNetwork( entity );
        m_Entity.push_back( entity );
        m_SpawnTime = 10.0f;
    }



    if( m_NextFoodTime <= 0 && m_Food.size() < 100 )
    {
        Food food;
        food.nutrition = 100;
        food.x = m_X + rand() % ( int )m_Width;
        food.y = m_Y + rand() % ( int )m_Height;
        m_Food.push_back( food );

        m_NextFoodTime = 1.0f;
    }



    Draw();
    m_Ontogenesis.Draw();
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



void
EntityManager::UpdateFitness( const float fitness, const size_t generation_id, const size_t species_id )
{
    m_Ontogenesis.UpdateFitness( fitness, generation_id, species_id );
}
