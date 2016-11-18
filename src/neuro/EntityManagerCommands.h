#include <OgreStringConverter.h>

#include "EntityManager.h"
#include "../core/ConfigCmdManager.h"
#include "../core/Console.h"



void
CommandRunGeneration( const Ogre::StringVector& params )
{
    if( params.size() < 2 )
    {
        Console::getSingleton().AddTextToOutput( "Usage: /run_generation <type id> <file to load>" );
        return;
    }

    EntityManager::getSingleton().RunGeneration( Ogre::StringConverter::parseInt( params[ 1 ] ), Ogre::String( params[ 2 ] ) );
}



void
CommandRunGenerationCompletition( Ogre::StringVector& complete_params )
{
    complete_params.push_back( "0" );
    complete_params.push_back( "1" );
}



void
EntityManager::InitCommands()
{
    ConfigCmdManager::getSingleton().AddCommand( "run_generation", "Reset type and load data from generation file.", "", CommandRunGeneration, CommandRunGenerationCompletition );
}
