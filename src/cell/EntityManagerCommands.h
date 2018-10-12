#include <OgreStringConverter.h>

#include "EntityManager.h"
#include "../core/ConfigCmdManager.h"
#include "../core/Console.h"



void
CommandRunGeneration( const Ogre::StringVector& params )
{
    if( params.size() < 2 )
    {
        Console::getSingleton().AddTextToOutput( "Usage: /run_generation <file to load>" );
        return;
    }

    EntityManager::getSingleton().RunGeneration( Ogre::String( params[ 1 ] ) );
}



void
EntityManager::InitCommands()
{
    ConfigCmdManager::getSingleton().AddCommand( "run_generation", "Reset type and load data from generation file.", "", CommandRunGeneration, NULL );
}
