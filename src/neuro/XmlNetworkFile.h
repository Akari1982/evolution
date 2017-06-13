#ifndef XML_NETWORK_FILE_H
#define XML_NETWORK_FILE_H

#include "../core/XmlFile.h"
#include "Entity.h"



class XmlNetworkFile : public XmlFile
{
public:
    explicit XmlNetworkFile( const Ogre::String& file );
    virtual ~XmlNetworkFile();

    void LoadNetwork( Entity* entity );
    static void SaveNetwork( const Ogre::String& filename, Entity* entity );
};



#endif // XML_NETWORK_FILE_H
