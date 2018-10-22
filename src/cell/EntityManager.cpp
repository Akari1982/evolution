#include "EntityManager.h"
#include "EntityManagerCommands.h"

#include "../core/DebugDraw.h"
#include "../core/InputManager.h"
#include "../core/Logger.h"
#include "../core/Timer.h"
#include "XmlGenerationFile.h"



template<>EntityManager *Ogre::Singleton< EntityManager >::msSingleton = NULL;



const float SPAWN_TIME = 0;
const size_t MAX_ENTITY = 10;
const size_t SPECIES_PER_GENERATION = 50;
const size_t MAX_FOOD = 50;
const float FOOD_TIME = 0;



EntityManager::EntityManager():
    m_X( 100.0f ),
    m_Y( 400.0f ),
    m_Width( 1080.0f ),
    m_Height( 300.0f ),
    m_NextFoodTime( FOOD_TIME ),
    m_SpawnTime( SPAWN_TIME )
{
    InitCommands();

    m_Generation.top_fitness = 0.0f;
    m_Generation.top_id = 0;
    m_Generation.id = 0;
    m_Generation.file_name = "gen_" + IntToString( m_Generation.id ) + ".xml";

    // test genome
    Ontogenesis::Gene gene;
    gene.id = 0;
    Ontogenesis::Condition cond;
    cond.type = Ontogenesis::ConditionType::C_PROTEIN;
    cond.value1 = 0;
    cond.value2 = 0.5f;
    cond.value3 = -1;
    gene.cond.push_back( cond );
    Ontogenesis::Expression expr;
    expr.type = Ontogenesis::ExpressionType::E_AXON_I;
    expr.value1 = 3;
    expr.value2 = 1;
    gene.expr.push_back( expr );
    m_Generation.base_genome.push_back( gene );
    gene.cond.clear();
    gene.expr.clear();
}



EntityManager::~EntityManager()
{
    for( size_t i = 0; i < m_GenerationPrev.species.size(); ++i )
    {
        delete m_GenerationPrev.species[ i ];
    }

    for( size_t i = 0; i < m_Generation.species.size(); ++i )
    {
        delete m_Generation.species[ i ];
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
        float forward_impulse = entity->GetForwardImpulse();
        float left_impulse = entity->GetLeftImpulse();
        float right_impulse = entity->GetRightImpulse();
        if( left_impulse > 0.0f )
        {
            rotation -= 90.0f * delta;
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
            rotation += 90.0f * delta;
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
            float radius = entity->GetRadius();
            float pos_x = start_x + Ogre::Math::Cos( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * ( 100.0f / radius );
            float pos_y = start_y + Ogre::Math::Sin( Ogre::Radian( Ogre::Degree( rotation ) ) ) * delta * ( 100.0f / radius );
            pos_x = ( pos_x < m_X ) ? m_Width + pos_x : pos_x;
            pos_x = ( pos_x > m_X + m_Width ) ? 2 * m_X + m_Width - pos_x : pos_x;
            pos_y = ( pos_y < m_Y ) ? m_Height + pos_y : pos_y;
            pos_y = ( pos_y > m_Y + m_Height ) ? 2 * m_Y + m_Height - pos_y : pos_y;
            entity->SetX( pos_x );
            entity->SetY( pos_y );
            forward_impulse -= delta;
            forward_impulse = ( forward_impulse < 0.0f ) ? 0.0f : forward_impulse;
            entity->SetForwardImpulse( forward_impulse );

            entity->SetEnergy( entity->GetEnergy() - delta );

            // check entity / food collision
            float x = entity->GetX();
            float y = entity->GetY();
            for( std::vector< Food >::iterator it = m_Food.begin(); it != m_Food.end(); )
            {
                float x1 = ( *it ).x;
                float y1 = ( *it ).y;
                float distance = sqrt( ( x - x1 ) * ( x - x1 ) + ( y - y1 ) * ( y - y1 ) );
                if( distance <= radius )
                {
                    float energy = entity->GetEnergy() + ( *it ).power;
                    energy = ( energy > 100.0f) ? 100.0f : energy;
                    entity->SetEnergy( energy );
                    entity->SetFitness( entity->GetFitness() + ( *it ).power );
                    it = m_Food.erase( it );
                }
                else
                {
                    ++it;
                }
            }

            // check entity / entity collision
            for( size_t j = 0; j < m_Entity.size(); ++j )
            {
                if( m_Entity[ j ] != entity )
                {
                    float x1 = m_Entity[ j ]->GetX();
                    float y1 = m_Entity[ j ]->GetY();
                    float radius1 = m_Entity[ j ]->GetRadius();
                    float distance = sqrt( ( x - x1 ) * ( x - x1 ) + ( y - y1 ) * ( y - y1 ) );

                    if( distance <= radius + radius1 )
                    {
                        if( radius >= radius1 )
                        {
                            float energy = entity->GetEnergy() + m_Entity[ j ]->GetEnergy();
                            energy = ( energy > 100.0f) ? 100.0f : energy;
                            entity->SetEnergy( energy );
                            entity->SetFitness( entity->GetFitness() + m_Entity[ j ]->GetEnergy() );
                            m_Entity[ j ]->SetEnergy( 0 );
                        }
                    }
                }
            }
        }
    }

    // remove dead entity
    for( std::vector< Entity* >::iterator it = m_Entity.begin(); it != m_Entity.end(); )
    {
        if( ( ( *it )->GetEnergy() <= 0 ) || ( ( *it )->IsDead() == true ) )
        {
            float fitness = ( *it )->GetFitness();
            size_t species_id;
            int gen_id = GetGenerationByEntity( ( *it ), species_id );
            if( gen_id == 1 )
            {
                if( m_Generation.top_fitness < fitness )
                {
                    m_Generation.top_fitness = fitness;
                    m_Generation.top_id = species_id;
                }
            }
            else if( m_GenerationPrev.top_fitness < fitness )
            {
                m_GenerationPrev.top_fitness = fitness;
                m_GenerationPrev.top_id = species_id;
                m_Generation.base_genome = ( *it )->GetGenome();

                //DumpGeneration( m_Generation );
                //DumpGeneration( m_GenerationPrev );
            }

            it = m_Entity.erase( it );
        }
        else
        {
            ++it;
        }
    }

    if( m_SpawnTime <= 0 && m_Entity.size() < MAX_ENTITY )
    {
        Entity* entity = new Entity( m_X + rand() % ( int )m_Width, m_Y + rand() % ( int )m_Height );

        if( m_Generation.species.size() >= SPECIES_PER_GENERATION )
        {
            for( size_t i = 0; i < m_GenerationPrev.species.size(); ++i )
            {
                delete m_GenerationPrev.species[ i ];
            }
            m_GenerationPrev = m_Generation;

            m_Generation.top_fitness = 0.0f;
            m_Generation.top_id = 0;
            m_Generation.id = m_GenerationPrev.id + 1;
            m_Generation.species.clear();
            m_Generation.base_genome = m_GenerationPrev.species[ m_GenerationPrev.top_id ]->GetGenome();
            m_Generation.file_name = "gen_" + IntToString( m_Generation.id ) + ".xml";

            //DumpGeneration( m_Generation );
            //DumpGeneration( m_GenerationPrev );
        }

        entity->SetGenome( m_Generation.base_genome );
        m_Entity.push_back( entity );
        m_Generation.species.push_back( entity );
        m_SpawnTime = SPAWN_TIME;
    }

    if( m_NextFoodTime <= 0 && m_Food.size() < MAX_FOOD )
    {
        Food food;
        food.power = 5 + rand() % 20;
        food.x = m_X + rand() % ( int )m_Width;
        food.y = m_Y + rand() % ( int )m_Height;
        m_Food.push_back( food );
        m_NextFoodTime = FOOD_TIME;
    }

    Draw();
}



void
EntityManager::Draw()
{
    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Text( 10, 10, "Generation " + IntToString( m_Generation.id ) );
    DEBUG_DRAW.Text( 210, 10, "Top fitness: " + IntToString( ( int )m_Generation.top_fitness ) + " (" + IntToString( m_Generation.top_id ) + ")" );
    DEBUG_DRAW.Text( 410, 10, "Number of species: " + IntToString( m_Generation.species.size() ) );

    DEBUG_DRAW.SetColour( Ogre::ColourValue( 1, 1, 1, 1 ) );
    DEBUG_DRAW.Line( m_X, m_Y, m_X, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X + m_Width, m_Y, m_X + m_Width, m_Y + m_Height );
    DEBUG_DRAW.Line( m_X, m_Y, m_X + m_Width, m_Y );
    DEBUG_DRAW.Line( m_X, m_Y + m_Height, m_X + m_Width, m_Y + m_Height );

    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        m_Entity[ i ]->Draw( 50 + ( i % 10 ) * 120, 120 + ( i / 10 ) * 240 );
    }

    for( size_t i = 0; i < m_Food.size(); ++i )
    {
        float radius = 1;
        float x = m_Food[ i ].x;
        float y = m_Food[ i ].y;
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 0.1f ) );
        DEBUG_DRAW.Circle( x, y, m_Food[ i ].power );
        DEBUG_DRAW.SetColour( Ogre::ColourValue( 0, 1, 0, 1 ) );
        DEBUG_DRAW.Quad( x - radius, y - radius, x + radius, y - radius, x + radius, y + radius, x - radius, y + radius );
    }
}



void
EntityManager::RunGeneration( Ogre::String& file_name )
{
    for( size_t i = 0; i < m_GenerationPrev.species.size(); ++i )
    {
        delete m_GenerationPrev.species[ i ];
    }
    m_GenerationPrev.species.clear();

    for( size_t i = 0; i < m_Generation.species.size(); ++i )
    {
        delete m_Generation.species[ i ];
    }
    m_Generation.species.clear();

    m_Entity.clear();

    XmlGenerationFile file( file_name );
    file.LoadGeneration();
}



void
EntityManager::LoadGeneration( const Generation& generation )
{
    m_Generation = generation;
}



void
EntityManager::DumpGeneration( Generation& generation ) const
{
    Logger* dump = new Logger( generation.file_name );
    dump->Log( "<generation id=\"" + IntToString( generation.id ) + "\">\n" );
    dump->Log( "    <base_genome>\n" );
    Ontogenesis::DumpGenome( dump, generation.base_genome );
    dump->Log( "    </base_genome>\n\n" );

    for( size_t i = 0; i < generation.species.size(); ++i )
    {
        dump->Log( "    <specie id=\"" + IntToString( i ) + "\" fitness=\"" + FloatToString( generation.species[ i ]->GetFitness() ) + "\">\n" );
        Ontogenesis::DumpGenome( dump, generation.species[ i ]->GetGenome() );
        dump->Log( "    </specie>\n" );
    }
    dump->Log( "</generation>\n" );
}



const int
EntityManager::GetGenerationByEntity( Entity* entity, size_t& species_id ) const
{
    for( size_t i = 0; i < m_Generation.species.size(); ++i )
    {
        if( m_Generation.species[ i ] == entity )
        {
            species_id = i;
            return 1;
        }
    }
    for( size_t i = 0; i < m_GenerationPrev.species.size(); ++i )
    {
        if( m_GenerationPrev.species[ i ] == entity )
        {
            species_id = i;
            return 0;
        }
    }
    return -1;
}



const float
EntityManager::FeelFood( const float x, const float y ) const
{
    float ret = 0.0f;
    for( size_t i = 0; i < m_Food.size(); ++i )
    {
        float x1 = m_Food[ i ].x;
        float y1 = m_Food[ i ].y;
        float radius = m_Food[ i ].power;
        float distance = sqrt( ( x - x1 ) * ( x - x1 ) + ( y - y1 ) * ( y - y1 ) );
        if( distance < radius )
        {
            ret += 1.0f - distance / radius;
        }
    }
    return ( ret > 1.0f ) ? 1.0f: ret;
}



const float
EntityManager::FeelEnemy( const float x, const float y ) const
{
    float ret = 0.0f;
    for( size_t i = 0; i < m_Entity.size(); ++i )
    {
        float x1 = m_Entity[ i ]->GetX();
        float y1 = m_Entity[ i ]->GetY();
        float radius = m_Entity[ i ]->GetEnergy();
        float distance = sqrt( ( x - x1 ) * ( x - x1 ) + ( y - y1 ) * ( y - y1 ) );
        if( distance < radius )
        {
            ret += 1.0f - distance / radius;
        }
    }
    return ( ret > 1.0f ) ? 1.0f: ret;
}
