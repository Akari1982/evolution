#ifndef XML_GENERATION_FILE_H
#define XML_GENERATION_FILE_H

#include "../core/XmlFile.h"



class XmlGenerationFile : public XmlFile
{
public:
    explicit XmlGenerationFile( const Ogre::String& file );
    virtual ~XmlGenerationFile();

    void LoadGeneration();

private:
    Ogre::String m_FileName;
};



#endif // XML_GENERATION_FILE_H
