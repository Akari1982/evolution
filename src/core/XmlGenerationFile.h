#ifndef XML_GENERATION_FILE_H
#define XML_GENERATION_FILE_H

#include "XmlFile.h"
#include "Entity.h"



class XmlGenerationFile : public XmlFile
{
public:
    explicit XmlGenerationFile( const Ogre::String& file );
    virtual ~XmlGenerationFile();

    void LoadGeneration( Ontogenesis* ontogenesis );

private:
    Ogre::String m_FileName;
};



#endif // XML_GENERATION_FILE_H