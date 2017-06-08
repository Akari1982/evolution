#include "XmlNetworkFile.h"

#include "../core/Logger.h"
#include "../core/Utilites.h"



XmlNetworkFile::XmlNetworkFile( const Ogre::String& file ):
    XmlFile( file )
{
}



XmlNetworkFile::~XmlNetworkFile()
{
}



void
XmlNetworkFile::LoadNetwork( Entity* entity )
{
    TiXmlNode* node = m_File.RootElement();

    if( node == NULL || node->ValueStr() != "network" )
    {
        LOG_ERROR( m_File.ValueStr() + " is not a valid fields map file! No <network> in root." );
        return;
    }

    node = node->FirstChild();
    while( node != NULL )
    {
        if( node->Type() == TiXmlNode::TINYXML_ELEMENT && node->ValueStr() == "neuron" )
        {
            Cell* cell = new Cell( entity, Cell::NEURON_COMMON, GetInt( node, "x" ), GetInt( node, "y" ) );
            species.network.push_back( cell );
        }
        node = node->NextSibling();
    }

    std::vector< Cell* > network;

    unsigned int self_id = 0;
    node = m_File.RootElement()->FirstChild();
    while( node != NULL )
    {
        if( node->Type() == TiXmlNode::TINYXML_ELEMENT && node->ValueStr() == "neuron" )
        {
            TiXmlNode* node2 = node->FirstChild();
            while( node2 != NULL )
            {
                if( node2->Type() == TiXmlNode::TINYXML_ELEMENT && node2->ValueStr() == "synapse" )
                {
                    network[ self_id ]->AddSynapse( GetFloat( node2, "power" ), GetBool( node2, "inverted", false ), network[ GetInt( node2, "neuron_id" ) ] );
                }
                node2 = node2->NextSibling();
            }
            ++self_id;
        }
        node = node->NextSibling();
    }
}
