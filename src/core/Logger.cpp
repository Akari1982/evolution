#include "Logger.h"

#include <OgreStringConverter.h>
#include <stdio.h>



Logger* LOGGER = NULL; // global and accessable from anywhere in our program



Ogre::String
HexToString( int value, unsigned short width, char fill )
{
    std::stringstream stream;
    stream.width( width );
    stream.fill( fill );
    stream.setf( std::ios::hex, std::ios::basefield );
    stream << value;
    return stream.str();
}



Ogre::String
BoolToString( bool value )
{
    return Ogre::StringConverter::toString( value );
}



Ogre::String
IntToString( int value )
{
    return Ogre::StringConverter::toString( value );
}



Ogre::String
FloatToString( float value )
{
    return Ogre::StringConverter::toString( value );
}



Logger::Logger( const Ogre::String& logFileName ):
    m_LogFile( logFileName )
{
    remove( m_LogFile.c_str() );
}



Logger::~Logger()
{
}



void
Logger::Log( const Ogre::String& text )
{
    FILE* file = fopen( m_LogFile.c_str(), "ab" );

    if( file != NULL )
    {
        fwrite( text.c_str(), sizeof( char ), text.size(), file );
        fclose( file );
    }
}



void
Logger::LogW( const Ogre::UTFString& text )
{
    FILE* file = fopen( m_LogFile.c_str(), "ab" );

    if( file != NULL )
    {
        fwrite( text.c_str(), sizeof( char ), text.size() * 2, file );
        fclose( file );
    }
}



void
Logger::Log( std::vector< unsigned char >& text )
{
    unsigned char* temp = new unsigned char[ text.size() ];
    for( int i = 0; i < text.size(); ++i )
    {
        temp[ i ] = text[ i ];
    }

    FILE* file = fopen( m_LogFile.c_str(), "ab" );

    if( file != NULL )
    {
        fwrite( temp, sizeof( char ), text.size(), file );
        fclose( file );
    }

    delete[] temp;
}
