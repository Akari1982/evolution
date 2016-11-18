#include "XmlGenerationFile.h"

#include "../core/Logger.h"
#include "../core/Utilites.h"



XmlGenerationFile::XmlGenerationFile( const Ogre::String& file ):
    XmlFile( file )
{
}



XmlGenerationFile::~XmlGenerationFile()
{
}



void
XmlGenerationFile::LoadGeneration( Ontogenesis* ontogenesis )
{
    TiXmlNode* node = m_File.RootElement();

    if( node == NULL || node->ValueStr() != "generation" )
    {
        LOG_ERROR( m_File.ValueStr() + " is not a valid generation file! No <generation> in root." );
        return;
    }

    Ontogenesis::Generation generation;
    generation.id = GetInt( node, "id" );
    generation.top_fitness = 0.0f;
    generation.top_id = 0;
    generation.file_name = m_FileName;

    node = node->FirstChild();
    while( node != NULL )
    {
        if( node->Type() == TiXmlNode::TINYXML_ELEMENT && node->ValueStr() == "base_genome" )
        {
            TiXmlNode* node2 = node->FirstChild();
            while( node2 != NULL )
            {
                if( node2->Type() == TiXmlNode::TINYXML_ELEMENT && node2->ValueStr() == "gene" )
                {
                    Ontogenesis::Gene gene;
                    gene.id = GetInt( node2, "id" );
                    gene.conserv = GetFloat( node2, "conserv" );

                    TiXmlNode* node3 = node2->FirstChild();
                    while( node3 != NULL )
                    {
                        if( node3->Type() == TiXmlNode::TINYXML_ELEMENT && node3->ValueStr() == "condition" )
                        {
                            Ontogenesis::Condition cond;
                            cond.type = ontogenesis->ConditionStringToType( GetString( node3, "type" ) );
                            cond.value1 = GetFloat( node3, "value1", 0.0f );
                            cond.value2 = GetFloat( node3, "value2", 0.0f );
                            gene.cond.push_back( cond );
                        }
                        else if( node3->Type() == TiXmlNode::TINYXML_ELEMENT && node3->ValueStr() == "expression" )
                        {
                            Ontogenesis::Expression expr;
                            expr.type = ontogenesis->ExpressionStringToType( GetString( node3, "type" ) );
                            expr.value1 = GetFloat( node3, "value1", 0.0f );
                            expr.value2 = GetFloat( node3, "value2", 0.0f );
                            expr.value3 = GetFloat( node3, "value3", 0.0f );
                            gene.expr.push_back( expr );
                        }
                        node3 = node3->NextSibling();
                    }
                    generation.base_genome.push_back( gene );
                }
                node2 = node2->NextSibling();
            }
        }
        node = node->NextSibling();
    }

    ontogenesis->LoadGeneration( generation );
}
