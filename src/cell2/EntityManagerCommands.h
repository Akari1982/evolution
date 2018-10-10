#include <OgreStringConverter.h>

#include "EntityManager.h"
#include "../core/ConfigCmdManager.h"
#include "../core/Console.h"



void
CmdAddCell( const Ogre::StringVector& params )
{
    if( params.size() != 3 )
    {
        Console::getSingleton().AddTextToOutput( "Usage: /debug_add_cell <x> <y>" );
        return;
    }

    int x = Ogre::StringConverter::parseInt( params[ 1 ] );
    int y = Ogre::StringConverter::parseInt( params[ 2 ] );

    EntityManager::getSingleton().DebugOnthogenesisAddCell( x, y );
}



void
CmdAddProtein( const Ogre::StringVector& params )
{
    if( params.size() != 5 )
    {
        Console::getSingleton().AddTextToOutput( "Usage: /debug_add_cell <id> <power> <x> <y>" );
        return;
    }

    int id = Ogre::StringConverter::parseInt( params[ 1 ] );
    float power = Ogre::StringConverter::parseReal( params[ 2 ] );
    int x = Ogre::StringConverter::parseInt( params[ 3 ] );
    int y = Ogre::StringConverter::parseInt( params[ 4 ] );

    EntityManager::getSingleton().DebugOnthogenesisAddProtein( id, power, x, y );
}



void
EntityManager::InitCommands()
{
    ConfigCmdManager::getSingleton().AddCommand( "debug_add_cell", "Add cell.", "", CmdAddCell, NULL );
    ConfigCmdManager::getSingleton().AddCommand( "debug_add_protein", "Add protein.", "", CmdAddProtein, NULL );
}
