#include "EntityManager.h"
#include "EntityManagerCommands.h"
#include "Mutation.h"
#include "XmlNetworkFile.h"

#include "../core/DebugDraw.h"
#include "../core/InputManager.h"
#include "../core/Logger.h"
#include "../core/Timer.h"



template<>EntityManager *Ogre::Singleton< EntityManager >::msSingleton = NULL;



EntityManager::EntityManager():
    m_EntityMaxName( 0 ),
    m_FileName( "data/network.xml" ),
    m_BestFitness( 0 ),
    m_Current( 0 ),
    m_X( 100.0f ),
    m_Y( 400.0f ),
    m_Width( 1080.0f ),
    m_Height( 300.0f )
{
    InitCommands();
}



EntityManager::~EntityManager()
{
    for( size_t i = 0; i < m_Entity.size(); ++i )
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

    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        m_Current = i;
        Entity* entity = m_Entity[ i ];
        if( entity->IsDead() == true )
        {
            continue;
        }
        entity->Update();

        // perform movement
        float rotation = entity->GetRotation();
        float move_time = entity->GetMoveTime();
        float rotate_time = entity->GetRotateTime();
        if( rotate_time > 0.0f )
        {
            float rotate_power = entity->GetRotatePower();
            rotation += 90.0f * delta * rotate_power;
            if( rotation < 0.0f )
            {
                rotation = ceil( -rotation / 360.0f ) * 360.0 - rotation;
            }
            entity->SetRotation( rotation );
            rotate_time -= delta;
            rotate_time = ( rotate_time < 0.0f ) ? 0.0f : rotate_time;
            entity->SetRotate( rotate_power, rotate_time );
        }
        if( move_time > 0.0f )
        {
            float start_x = entity->GetX();
            float start_y = entity->GetY();
            float move_power = entity->GetMovePower();
            float pos_x = start_x + Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * move_power * 20.0f;
            float pos_y = start_y + Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * move_power * 20.0f;
            pos_x = ( pos_x < m_X ) ? m_Width + pos_x : pos_x;
            pos_x = ( pos_x > m_X + m_Width ) ? 2 * m_X + m_Width - pos_x : pos_x;
            pos_y = ( pos_y < m_Y ) ? m_Height + pos_y : pos_y;
            pos_y = ( pos_y > m_Y + m_Height ) ? 2 * m_Y + m_Height - pos_y : pos_y;
            entity->SetX( pos_x );
            entity->SetY( pos_y );
            move_time -= delta;
            move_time = ( move_time < 0.0f ) ? 0.0f : move_time;
            entity->SetMove( move_power, move_time );
        }

        // check entity+food collision
        float radius = entity->GetRadius();
        float x = entity->GetX();
        float y = entity->GetY();
        for( std::vector< Food >::iterator it = m_FoodPack[ i ].food.begin(); it != m_FoodPack[ i ].food.end(); )
        {
            float x1 = ( *it ).x;
            float y1 = ( *it ).y;
            float distance = sqrt( ( x - x1 ) * ( x - x1 ) + ( y - y1 ) * ( y - y1 ) );
            if( distance <= radius )
            {
                float energy = entity->GetEnergy() + ( *it ).power;
                m_EntityFitness[ i ] += ( *it ).power;
                entity->SetEnergy( energy );
                it = m_FoodPack[ i ].food.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

    // check if all entity is dead
    bool dead = true;
    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        if( m_Entity[ i ]->IsDead() == false )
        {
            dead = false;
            break;
        }
    }
    if( dead == true )
    {
        // select best fitness
        int best_fitness = 0;
        size_t best_id = 0;
        for( size_t i = 0; i < m_EntityFitness.size(); ++i )
        {
            if( m_EntityFitness[ i ] > best_fitness )
            {
                best_fitness = m_EntityFitness[ i ];
                best_id = i;
            }
        }

        if( m_Entity.size() > 0 )
        {
            if( best_fitness >= ( m_BestFitness - 100 ) )
            {
                if( best_fitness >= m_BestFitness )
                {
                    m_BestFitness = best_fitness;
                }
                m_FileName = "data/network_" + IntToString( best_fitness, 4, '0' ) + "_" + IntToString( m_Entity[ best_id ]->GetName(), 6, '0' ) + ".xml";
                XmlNetworkFile::SaveNetwork( m_FileName, m_Entity[ best_id ] );
            }
        }

        for( size_t i = 0; i < m_Entity.size(); ++i )
        {
            delete m_Entity[ i ];
        }
        m_Entity.clear();
        m_EntityFitness.clear();
        m_FoodPack.clear();

        FoodPack food_pack;
        for( size_t j = 0; j < 20; ++j )
        {
            Food food;
            food.power = 40;
            food.x = m_X + rand() % ( int )m_Width;
            food.y = m_Y + rand() % ( int )m_Height;
            food_pack.food.push_back( food );
        }

        for( size_t i = 0; i < 30; ++i )
        {
            Entity* entity = new Entity( 0, 0, 0 );
            Entity* child = new Entity( m_EntityMaxName, m_X + m_Width / 2.0f, m_Y + m_Height / 2.0f );
            //child->SetRotation( ( float )( rand() % 360 ) );
            ++m_EntityMaxName;
            XmlNetworkFile* network = new XmlNetworkFile( m_FileName );
            network->LoadNetwork( entity );
            delete network;
            NetworkMutate( entity, child );
            delete entity;
            m_Entity.push_back( child );
            m_EntityFitness.push_back( 0 );
            m_FoodPack.push_back( food_pack );
        }
    }

    Draw();
}



void
EntityManager::Draw()
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Line( m_X, m_Y, m_X, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X + m_Width, m_Y, m_X + m_Width, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X, m_Y, m_X + m_Width, m_Y );
    DEBUG_DRAW.Line( m_X, m_Y + m_Height, m_X + m_Width, m_Y + m_Height );

    for( size_t i = 0; ( i < m_Entity.size() ) && ( i < 30 ); ++i )
    {
        if( m_Entity[ i ]->IsDead() == true )
        {
            continue;
        }
        m_Entity[ i ]->Draw( 50 + ( i % 10 ) * 120, 20 + ( i / 10 ) * 120 );
    }

    for( size_t i = 0; i < m_FoodPack.size(); ++i )
    {
        if( m_Entity[ i ]->IsDead() == true )
        {
            continue;
        }
        for( size_t j = 0; j < m_FoodPack[ i ].food.size(); ++j )
        {
            float radius = 1;
            float x = m_FoodPack[ i ].food[ j ].x;
            float y = m_FoodPack[ i ].food[ j ].y;
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.1f ) );
            DEBUG_DRAW.Circle( x, y, m_FoodPack[ i ].food[ j ].power );
            DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
            DEBUG_DRAW.Quad( x - radius, y - radius, x + radius, y - radius, x + radius, y + radius, x - radius, y + radius );
        }
    }
}



float
EntityManager::FeelFood( const float x, const float y )
{
    float ret = 0.0f;
    for( size_t i = 0; i < m_FoodPack[ m_Current ].food.size(); ++i )
    {
        float x1 = m_FoodPack[ m_Current ].food[ i ].x;
        float y1 = m_FoodPack[ m_Current ].food[ i ].y;
        float radius = 100;
        float distance = sqrt( ( x - x1 ) * ( x - x1 ) + ( y - y1 ) * ( y - y1 ) );
        if( distance < radius )
        {
            ret += 1.0f - distance / radius;
        }
    }
    return ( ret > 1.0f ) ? 1.0f: ret;
}
