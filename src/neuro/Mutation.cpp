#include "Mutation.h"



void
NetworkMutation( Entity* parent, Entity* child )
{
    std::vector< Cell* > parent_net = parent->GetNetwork();
    std::vector< Cell* > child_net;

    for( size_t i = 0; i < parent_net.size(); ++i )
    {
        child_net.push_back( new Cell( child, 0, 0 ) );
    }
    for( size_t i = 0; i < parent_net.size(); ++i )
    {
        child_net[ i ]->Copy( parent_net[ i ] );
    }

    child->AddNetwork( child_net );
}
