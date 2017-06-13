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

    std::vector< Cell* > network;

    node = node->FirstChild();
    while( node != NULL )
    {
        if( node->Type() == TiXmlNode::TINYXML_ELEMENT && node->ValueStr() == "neuron" )
        {
            Cell* cell = new Cell( entity, GetInt( node, "x" ), GetInt( node, "y" ) );
            network.push_back( cell );
        }
        node = node->NextSibling();
    }

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
                    Ogre::String type = GetString( node2, "type" );
                    if( type == "food" )
                    {
                        network[ self_id ]->AddFoodSynapse( GetFloat( node2, "power" ), GetBool( node2, "inverted", false ) );
                    }
                    else
                    {
                        network[ self_id ]->AddSynapse( GetFloat( node2, "power" ), GetBool( node2, "inverted", false ), GetInt( node2, "neuron_id" ) );
                    }
                }
                if( node2->Type() == TiXmlNode::TINYXML_ELEMENT && node2->ValueStr() == "activator" )
                {
                    Ogre::String type = GetString( node2, "type" );
                    if( type == "move" )
                    {
                        network[ self_id ]->AddMoveActivator( GetFloat( node2, "power" ) );
                    }
                    else if( type == "rotate" )
                    {
                        network[ self_id ]->AddRotateActivator( GetFloat( node2, "power" ) );
                    }
                }
                node2 = node2->NextSibling();
            }
            ++self_id;
        }
        node = node->NextSibling();
    }

    entity->SetNetwork( network );
}



void
XmlNetworkFile::SaveNetwork( const Ogre::String& filename, Entity* entity )
{
    std::vector< int > parents = entity->GetParents();
    std::vector< int > children = entity->GetChildren();
    int name = entity->GetName();

    Logger* dump = new Logger( filename );
    dump->Log( "<network>\n" );
    dump->Log( "    <entity name=\"" + IntToString( name ) + "\">\n" );
    dump->Log( "        <parents>" );
    for( size_t i = 0; i < parents.size(); ++i )
    {
        if( i > 0 )
        {
            dump->Log( " " );
        }
        dump->Log( IntToString( parents[ i ] ) );
    }
    dump->Log( "</parents>\n" );
    dump->Log( "        <children>" );
    for( size_t i = 0; i < children.size(); ++i )
    {
        if( i > 0 )
        {
            dump->Log( " " );
        }
        dump->Log( IntToString( children[ i ] ) );
    }
    dump->Log( "</children>\n" );
    dump->Log( "    </entity>\n\n" );

    std::vector< Cell* > net = entity->GetNetwork();
    for( size_t i = 0; i < net.size(); ++i )
    {
        Cell* cell = net[ i ];
        dump->Log( "    <neuron x=\"" + FloatToString( cell->m_X ) + "\" y=\"" + FloatToString( cell->m_Y ) + "\">\n" );

        for( size_t i = 0; i < cell->m_Synapses.size(); ++i )
        {
            dump->Log( "        <synapse power=\"" + FloatToString( cell->m_Synapses[ i ].power ) + "\"" );
            bool inverted = cell->m_Synapses[ i ].inverted;
            if( inverted == true )
            {
                dump->Log( " inverted=\"true\"" );
            }

            switch( cell->m_Synapses[ i ].type )
            {
                case Cell::SYN_FOOD:
                {
                    dump->Log( " type=\"food\" />\n" );
                }
                break;
                case Cell::SYN_NEURON:
                {
                    dump->Log( " neuron_id=\"" + IntToString( cell->m_Synapses[ i ].cell_id ) + "\" />\n" );
                }
                break;
            }
        }

        for( size_t i = 0; i < cell->m_Activators.size(); ++i )
        {
            dump->Log( "        <activator power=\"" + FloatToString( cell->m_Activators[ i ].power ) + "\"" );
            switch( cell->m_Activators[ i ].type )
            {
                case Cell::ACT_MOVE:
                {
                    dump->Log( " type=\"move\" />\n" );
                }
                break;
                case Cell::ACT_ROTATE:
                {
                    dump->Log( " type=\"rotate\" />\n" );
                }
                break;
            }
        }

        dump->Log( "    </neuron>\n" );
    }

    dump->Log( "</network>\n" );
    delete dump;
}
