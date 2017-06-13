#include "Mutation.h"
#include "../core/Logger.h"



void
NetworkMutate( Entity* parent, Entity* child )
{
    int new_cell_mutate = 10;
    int delete_cell_mutate = 5;
    int new_synapse_mutate = 90;
    int delete_synapse_mutate = 10;
    int new_activator_mutate = 10;
    int delete_activator_mutate = 10;

    int power_mutate = 20;
    int power_random = 10;
    float power_step = 0.1f;
    int inverted_mutate = 10;

    int activator_mutate = 20;
    int activator_random = 10;
    float activator_step = 0.1f;

    std::vector< int > parents = parent->GetParents();
    parents.push_back( parent->GetName() );
    child->SetParents( parents );

    std::vector< Cell* > parent_net = parent->GetNetwork();
    std::vector< Cell* > child_net;

    // copy existed cells
    for( size_t i = 0; i < parent_net.size(); ++i )
    {
        child_net.push_back( new Cell( child, 0, 0 ) );
    }

    // copy and mutate existed synapses and activators
    for( size_t i = 0; i < child_net.size(); ++i )
    {
        Cell* c_child = child_net[ i ];

        Cell* c_parent = parent_net[ i ];
        c_child->m_X = c_parent->m_X;
        c_child->m_Y = c_parent->m_Y;

        for( size_t i = 0; i < c_parent->m_Synapses.size(); ++i )
        {
            float power = c_parent->m_Synapses[ i ].power;
            bool inverted = c_parent->m_Synapses[ i ].inverted;

            if( rand() % 100 < power_mutate )
            {
                if( rand() % 100 < power_random )
                {
                    power = ( float )( rand() % 1000 ) / 1000.0f;
                }
                else
                {
                    power += ( ( float )( rand() % 100 ) / 100.0f ) * power_step * 2 - power_step;
                }
            }

            if( rand() % 100 < inverted_mutate )
            {
                inverted = !inverted;
            }

            switch( c_parent->m_Synapses[ i ].type )
            {
                case Cell::SYN_FOOD:
                {
                    c_child->AddFoodSynapse( power, inverted );
                }
                break;
                case Cell::SYN_NEURON:
                {
                    c_child->AddSynapse( power, inverted, c_parent->m_Synapses[ i ].cell_id );
                }
                break;
            }
        }

        for( size_t i = 0; i < c_parent->m_Activators.size(); ++i )
        {
            float power = c_parent->m_Activators[ i ].power;
            if( rand() % 100 < activator_mutate )
            {
                if( rand() % 100 < activator_random )
                {
                    power = ( float )( rand() % 1000 ) / 1000.0f;
                }
                else
                {
                    power += ( ( float )( rand() % 100 ) / 100.0f ) * activator_step * 2 - activator_step;
                }
            }

            switch( c_parent->m_Activators[ i ].type )
            {
                case Cell::ACT_MOVE:
                {
                    c_child->AddMoveActivator( power );
                }
                break;
                case Cell::ACT_ROTATE:
                {
                    c_child->AddRotateActivator( power );
                }
                break;
            }
        }
    }

    // add new as mutation
    if( rand() % 100 < new_cell_mutate )
    {
        child_net.push_back( new Cell( child, ( ( float )( rand() % 600 ) / 10.0f ) - 30.0f, ( ( float )( rand() % 600 ) / 10.0f ) - 30.0f ) );
    }

    if( child_net.size() > 0 )
    {
        // delete existed cells
        if( rand() % 100 < delete_cell_mutate )
        {
            int delete_cell_id = rand() % child_net.size();
            child_net.erase( child_net.begin() + delete_cell_id );
            for( size_t i = 0; i < child_net.size(); ++i )
            {
                for( std::vector< Cell::Synapse >::iterator it = child_net[ i ]->m_Synapses.begin(); it != child_net[ i ]->m_Synapses.end(); )
                {
                    if( ( ( *it ).type == Cell::SYN_NEURON ) && ( ( *it ).cell_id == delete_cell_id ) )
                    {
                        it = child_net[ i ]->m_Synapses.erase( it );
                    }
                    else if( ( ( *it ).type == Cell::SYN_NEURON ) && ( ( *it ).cell_id > delete_cell_id ) )
                    {
                        ( *it ).cell_id -= 1;
                        ++it;
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }
    }

    if( child_net.size() > 0 )
    {
        // add new connect
        if( rand() % 100 < new_synapse_mutate )
        {
            int cell_id_to = rand() % child_net.size();
            float power = ( float )( rand() % 1000 ) / 1000.0f;
            bool inverted = ( ( rand() % 2 ) == 1 ) ? true : false;

            switch( rand() % Cell::SYN_TYPE_MAX )
            {
                case Cell::SYN_FOOD:
                {
                    child_net[ cell_id_to ]->AddFoodSynapse( power, inverted );
                }
                break;
                case Cell::SYN_NEURON:
                {
                    int cell_id_from = rand() % child_net.size();
                    if( cell_id_from != cell_id_to )
                    {
                        child_net[ cell_id_to ]->AddSynapse( power, inverted, cell_id_from );
                    }
                }
                break;
            }
        }

        // delete existed synapse
        if( rand() % 100 < delete_synapse_mutate )
        {
            int cell_id = rand() % child_net.size();
            Cell* cell = child_net[ cell_id ];
            if( cell->m_Synapses.size() > 0 )
            {
                int synapse_id = rand() % cell->m_Synapses.size();
                cell->m_Synapses.erase( cell->m_Synapses.begin() + synapse_id );
            }
        }

        // add new activator
        if( rand() % 100 < new_activator_mutate )
        {
            int cell_id_to = rand() % child_net.size();
            float power = ( float )( rand() % 1000 ) / 1000.0f;

            switch( rand() % Cell::ACT_TYPE_MAX )
            {
                case Cell::ACT_MOVE:
                {
                    child_net[ cell_id_to ]->AddMoveActivator( power );
                }
                break;
                case Cell::ACT_ROTATE:
                {
                    child_net[ cell_id_to ]->AddRotateActivator( power );
                }
                break;
            }
        }

        // delete existed synapse
        if( rand() % 100 < delete_activator_mutate )
        {
            int cell_id = rand() % child_net.size();
            Cell* cell = child_net[ cell_id ];
            if( cell->m_Activators.size() > 0 )
            {
                int activator_id = rand() % cell->m_Activators.size();
                cell->m_Activators.erase( cell->m_Activators.begin() + activator_id );
            }
        }
    }

    child->SetNetwork( child_net );
}
