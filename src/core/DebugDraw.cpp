#include "DebugDraw.h"

#include <Overlay/OgreFontManager.h>
#include <OgreHardwareBufferManager.h>
#include <OgreMaterialManager.h>

#include "CameraManager.h"
#include "Logger.h"



template<>DebugDraw *Ogre::Singleton< DebugDraw >::msSingleton = NULL;



DebugDraw::DebugDraw():
    m_Colour( 1, 1, 1, 1 ),
    m_ScreenSpace( true ),
    m_Z( 0 ),
    m_FadeStartSquare( 999999 ),
    m_FadeEndSquare( 999999 ),
    m_FontHeight( 16 )
{
    m_SceneManager = Ogre::Root::getSingleton().getSceneManager( "Scene" );
    m_RenderSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();

    CreateLineVertexBuffer();
    CreateLine3dVertexBuffer();
    CreateTriangle3dVertexBuffer();
    CreateQuadVertexBuffer();
    CreateCircleVertexBuffer();
    CreateDiscVertexBuffer();
    CreateTextVertexBuffer();

    m_Material = Ogre::MaterialManager::getSingleton().create( "DebugDraw", "General" );
    Ogre::Pass* pass = m_Material->getTechnique( 0 )->getPass( 0 );
    pass->setVertexColourTracking( Ogre::TVC_AMBIENT );
    pass->setCullingMode( Ogre::CULL_NONE );
    pass->setDepthCheckEnabled( true );
    pass->setDepthWriteEnabled( true );
    pass->setLightingEnabled( false );
    pass->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );

    m_Material3d = Ogre::MaterialManager::getSingleton().create( "DebugDraw3d", "General" );
    pass = m_Material3d->getTechnique( 0 )->getPass( 0 );
    pass->setVertexColourTracking( Ogre::TVC_AMBIENT );
    pass->setCullingMode( Ogre::CULL_NONE );
    pass->setDepthCheckEnabled( true );
    pass->setDepthWriteEnabled( true );
    pass->setLightingEnabled( false );
    pass->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );

    m_Font = Ogre::FontManager::getSingleton().getByName( "CourierNew" );
    if( m_Font.isNull() )
    {
        LOG_ERROR( "Could not find font \"CourierNew\" for debug draw." );
    }
    else
    {
        m_Font->load();
        pass = m_Font->getMaterial()->getTechnique( 0 )->getPass( 0 );
        pass->setVertexColourTracking( Ogre::TVC_AMBIENT );
        pass->setCullingMode( Ogre::CULL_NONE );
        pass->setDepthCheckEnabled( true );
        pass->setDepthWriteEnabled( true );
        pass->setLightingEnabled( false );
        pass->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );

        pass->setAlphaRejectFunction( Ogre::CMPF_GREATER );
        pass->setAlphaRejectValue( 0 );
        Ogre::TextureUnitState* tex = pass->getTextureUnitState( 0 );
        tex->setNumMipmaps( -1 );
        tex->setTextureFiltering( Ogre::TFO_NONE );
    }

    m_SceneManager->addRenderQueueListener( this );
}



DebugDraw::~DebugDraw()
{
    m_SceneManager->removeRenderQueueListener( this );

    DestroyLineVertexBuffer();
    DestroyLine3dVertexBuffer();
    DestroyTriangle3dVertexBuffer();
    DestroyQuadVertexBuffer();
    DestroyCircleVertexBuffer();
    DestroyDiscVertexBuffer();
    DestroyTextVertexBuffer();
}



void
DebugDraw::SetColour( const Ogre::ColourValue& colour )
{
    m_Colour = colour;
}



void
DebugDraw::SetScreenSpace( const bool screen_space )
{
    m_ScreenSpace = screen_space;
}



void
DebugDraw::SetZ( const float z )
{
    m_Z = z;
}



void
DebugDraw::SetFadeDistance( const float fade_s, const float fade_e )
{
    m_FadeStartSquare = fade_s * fade_s;
    m_FadeEndSquare = fade_e * fade_e;
}



void
DebugDraw::SetTextAlignment( TextAlignment alignment )
{
    m_TextAlignment = alignment;
}



void
DebugDraw::Line( const float x1, const float y1, const float x2, const float y2 )
{
    Ogre::RenderTarget* window  = Ogre::Root::getSingleton().getRenderTarget( "QGearsWindow" );
    if( window->isActive() == false )
    {
        return;
    }

    if( m_LineRenderOp.vertexData->vertexCount + 2 > m_LineMaxVertexCount )
    {
        LOG_ERROR( "Max number of lines reached. Can't create more than " + Ogre::StringConverter::toString( m_LineMaxVertexCount / 2 ) + " lines." );
        return;
    }

    float width = window->getViewport( 0 )->getActualWidth();
    float height = window->getViewport( 0 )->getActualHeight();

    float new_x1 = ( m_ScreenSpace == true ) ? ( ( int ) x1 / width ) * 2 - 1 : x1;
    float new_y1 = ( m_ScreenSpace == true ) ? -( ( ( int ) y1 / height ) * 2 - 1 ) : y1;
    float new_x2 = ( m_ScreenSpace == true ) ? ( ( int ) x2 / width ) * 2 - 1 : x2;
    float new_y2 = ( m_ScreenSpace == true ) ? -( ( ( int ) y2 / height ) * 2 - 1 ) : y2;

    float* writeIterator = ( float* ) m_LineVertexBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
    writeIterator += m_LineRenderOp.vertexData->vertexCount * 7;

    *writeIterator++ = new_x1; *writeIterator++ = new_y1; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x2; *writeIterator++ = new_y2; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;

    m_LineRenderOp.vertexData->vertexCount += 2;
    m_LineVertexBuffer->unlock();
}



void
DebugDraw::Line3d( const Ogre::Vector3& point1, const Ogre::Vector3& point2 )
{
    Ogre::RenderTarget* window  = Ogre::Root::getSingleton().getRenderTarget( "QGearsWindow" );
    if( ( Ogre::RenderWindow* )window->isActive() == false )
    {
        return;
    }

    if( m_Line3dRenderOp.vertexData->vertexCount + 2 > m_Line3dMaxVertexCount )
    {
        LOG_ERROR( "Max number of 3d lines reached. Can't create more than " + Ogre::StringConverter::toString( m_Line3dMaxVertexCount / 2 ) + " 3d lines." );
        return;
    }

    float* writeIterator = ( float* ) m_Line3dVertexBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
    writeIterator += m_Line3dRenderOp.vertexData->vertexCount * 7;

    *writeIterator++ = point1.x; *writeIterator++ = point1.y; *writeIterator++ = point1.z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = point2.x; *writeIterator++ = point2.y; *writeIterator++ = point2.z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;

    m_Line3dRenderOp.vertexData->vertexCount += 2;
    m_Line3dVertexBuffer->unlock();
}



void
DebugDraw::Triangle3d( const Ogre::Vector3& point1, const Ogre::Vector3& point2, const Ogre::Vector3& point3 )
{
    Ogre::RenderTarget* window  = Ogre::Root::getSingleton().getRenderTarget( "QGearsWindow" );
    if( ( Ogre::RenderWindow* )window->isActive() == false )
    {
        return;
    }

    if( m_Triangle3dRenderOp.vertexData->vertexCount + 3 > m_Triangle3dMaxVertexCount )
    {
        LOG_ERROR( "Max number of 3d triangles reached. Can't create more than " + Ogre::StringConverter::toString( m_Triangle3dMaxVertexCount / 3 ) + " 3d triangles." );
        return;
    }

    float* writeIterator = ( float* ) m_Triangle3dVertexBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
    writeIterator += m_Triangle3dRenderOp.vertexData->vertexCount * 7;

    *writeIterator++ = point1.x; *writeIterator++ = point1.y; *writeIterator++ = point1.z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = point2.x; *writeIterator++ = point2.y; *writeIterator++ = point2.z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = point3.x; *writeIterator++ = point3.y; *writeIterator++ = point3.z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;

    m_Triangle3dRenderOp.vertexData->vertexCount += 3;
    m_Triangle3dVertexBuffer->unlock();
}



void
DebugDraw::Quad( const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4 )
{
    Ogre::RenderTarget* window  = Ogre::Root::getSingleton().getRenderTarget( "QGearsWindow" );
    if( ( Ogre::RenderWindow* )window->isActive() == false )
    {
        return;
    }

    if( m_QuadRenderOp.vertexData->vertexCount + 6 > m_QuadMaxVertexCount )
    {
        LOG_ERROR( "Max number of quads reached. Can't create more than " + Ogre::StringConverter::toString( m_QuadMaxVertexCount / 6 ) + " quads." );
        return;
    }

    float width = window->getViewport( 0 )->getActualWidth();
    float height = window->getViewport( 0 )->getActualHeight();

    float new_x1 = ( m_ScreenSpace == true ) ? ( ( int ) x1 / width ) * 2 - 1 : x1;
    float new_y1 = ( m_ScreenSpace == true ) ? -( ( ( int ) y1 / height ) * 2 - 1 ) : y1;
    float new_x2 = ( m_ScreenSpace == true ) ? ( ( int ) x2 / width ) * 2 - 1 : x2;
    float new_y2 = ( m_ScreenSpace == true ) ? -( ( y2 / height ) * 2 - 1 ) : y2;
    float new_x3 = ( m_ScreenSpace == true ) ? ( ( int ) x3 / width ) * 2 - 1 : x3;
    float new_y3 = ( m_ScreenSpace == true ) ? -( ( ( int ) y3 / height ) * 2 - 1 ) : y3;
    float new_x4 = ( m_ScreenSpace == true ) ? ( ( int ) x4 / width ) * 2 - 1 : x4;
    float new_y4 = ( m_ScreenSpace == true ) ? -( ( ( int ) y4 / height ) * 2 - 1 ) : y4;

    float* writeIterator = ( float* ) m_QuadVertexBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
    writeIterator += m_QuadRenderOp.vertexData->vertexCount * 7;

    *writeIterator++ = new_x1; *writeIterator++ = new_y1; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x2; *writeIterator++ = new_y2; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x3; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y1; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x3; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x4; *writeIterator++ = new_y4; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;

    m_QuadRenderOp.vertexData->vertexCount += 6;
    m_QuadVertexBuffer->unlock();
}



void
DebugDraw::Circle( const float x, const float y, const float radius )
{
    Ogre::RenderTarget* window  = Ogre::Root::getSingleton().getRenderTarget( "QGearsWindow" );
    if( ( Ogre::RenderWindow* )window->isActive() == false )
    {
        return;
    }

    if( m_CircleRenderOp.vertexData->vertexCount + 16 > m_CircleMaxVertexCount )
    {
        LOG_ERROR( "Max number of circles reached. Can't create more than " + Ogre::StringConverter::toString( m_CircleMaxVertexCount / 16 ) + " circles." );
        return;
    }

    float width = window->getViewport( 0 )->getActualWidth();
    float height = window->getViewport( 0 )->getActualHeight();

    float radius23 = radius * ( 2.2f / 3.0f );

    float x1 = x;
    float y1 = y - radius;
    float x2 = x + radius23;
    float y2 = y - radius23;
    float x3 = x + radius;
    float y3 = y;
    float x4 = x + radius23;
    float y4 = y + radius23;
    float x5 = x;
    float y5 = y + radius;
    float x6 = x - radius23;
    float y6 = y + radius23;
    float x7 = x - radius;
    float y7 = y;
    float x8 = x - radius23;
    float y8 = y - radius23;

    float new_x1 = ( m_ScreenSpace == true ) ? ( ( int ) x1 / width ) * 2 - 1 : x1;
    float new_y1 = ( m_ScreenSpace == true ) ? -( ( ( int ) y1 / height ) * 2 - 1 ) : y1;
    float new_x2 = ( m_ScreenSpace == true ) ? ( ( int ) x2 / width ) * 2 - 1 : x2;
    float new_y2 = ( m_ScreenSpace == true ) ? -( ( ( int ) y2 / height ) * 2 - 1 ) : y2;
    float new_x3 = ( m_ScreenSpace == true ) ? ( ( int ) x3 / width ) * 2 - 1 : x3;
    float new_y3 = ( m_ScreenSpace == true ) ? -( ( ( int ) y3 / height ) * 2 - 1 ) : y3;
    float new_x4 = ( m_ScreenSpace == true ) ? ( ( int ) x4 / width ) * 2 - 1 : x4;
    float new_y4 = ( m_ScreenSpace == true ) ? -( ( ( int ) y4 / height ) * 2 - 1 ) : y4;
    float new_x5 = ( m_ScreenSpace == true ) ? ( ( int ) x5 / width ) * 2 - 1 : x5;
    float new_y5 = ( m_ScreenSpace == true ) ? -( ( ( int ) y5 / height ) * 2 - 1 ) : y5;
    float new_x6 = ( m_ScreenSpace == true ) ? ( ( int ) x6 / width ) * 2 - 1 : x6;
    float new_y6 = ( m_ScreenSpace == true ) ? -( ( ( int ) y6 / height ) * 2 - 1 ) : y6;
    float new_x7 = ( m_ScreenSpace == true ) ? ( ( int ) x7 / width ) * 2 - 1 : x7;
    float new_y7 = ( m_ScreenSpace == true ) ? -( ( ( int ) y7 / height ) * 2 - 1 ) : y7;
    float new_x8 = ( m_ScreenSpace == true ) ? ( ( int ) x8 / width ) * 2 - 1 : x8;
    float new_y8 = ( m_ScreenSpace == true ) ? -( ( ( int ) y8 / height ) * 2 - 1 ) : y8;

    float* writeIterator = ( float* ) m_CircleVertexBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
    writeIterator += m_CircleRenderOp.vertexData->vertexCount * 7;

    *writeIterator++ = new_x1; *writeIterator++ = new_y1; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x2; *writeIterator++ = new_y2; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x2; *writeIterator++ = new_y2; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x3; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x3; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x4; *writeIterator++ = new_y4; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x4; *writeIterator++ = new_y4; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x5; *writeIterator++ = new_y5; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x5; *writeIterator++ = new_y5; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x6; *writeIterator++ = new_y6; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x6; *writeIterator++ = new_y6; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x7; *writeIterator++ = new_y7; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x7; *writeIterator++ = new_y7; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x8; *writeIterator++ = new_y8; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x8; *writeIterator++ = new_y8; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y1; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;

    m_CircleRenderOp.vertexData->vertexCount += 16;
    m_CircleVertexBuffer->unlock();
}



void
DebugDraw::Disc( const float x, const float y, const float radius )
{
    Ogre::RenderTarget* window  = Ogre::Root::getSingleton().getRenderTarget( "QGearsWindow" );
    if( ( Ogre::RenderWindow* )window->isActive() == false )
    {
        return;
    }

    if( m_DiscRenderOp.vertexData->vertexCount + 24 > m_DiscMaxVertexCount )
    {
        LOG_ERROR( "Max number of discs reached. Can't create more than " + Ogre::StringConverter::toString( m_DiscMaxVertexCount / 24 ) + " discs." );
        return;
    }

    float width = window->getViewport( 0 )->getActualWidth();
    float height = window->getViewport( 0 )->getActualHeight();

    float radius23 = radius * ( 2.2f / 3.0f );

    float x1 = x;
    float y1 = y - radius;
    float x2 = x + radius23;
    float y2 = y - radius23;
    float x3 = x + radius;
    float y3 = y;
    float x4 = x + radius23;
    float y4 = y + radius23;
    float x5 = x;
    float y5 = y + radius;
    float x6 = x - radius23;
    float y6 = y + radius23;
    float x7 = x - radius;
    float y7 = y;
    float x8 = x - radius23;
    float y8 = y - radius23;

    float new_x1 = ( m_ScreenSpace == true ) ? ( ( int ) x1 / width ) * 2 - 1 : x1;
    float new_y1 = ( m_ScreenSpace == true ) ? -( ( ( int ) y1 / height ) * 2 - 1 ) : y1;
    float new_x2 = ( m_ScreenSpace == true ) ? ( ( int ) x2 / width ) * 2 - 1 : x2;
    float new_y2 = ( m_ScreenSpace == true ) ? -( ( ( int ) y2 / height ) * 2 - 1 ) : y2;
    float new_x3 = ( m_ScreenSpace == true ) ? ( ( int ) x3 / width ) * 2 - 1 : x3;
    float new_y3 = ( m_ScreenSpace == true ) ? -( ( ( int ) y3 / height ) * 2 - 1 ) : y3;
    float new_x4 = ( m_ScreenSpace == true ) ? ( ( int ) x4 / width ) * 2 - 1 : x4;
    float new_y4 = ( m_ScreenSpace == true ) ? -( ( ( int ) y4 / height ) * 2 - 1 ) : y4;
    float new_x5 = ( m_ScreenSpace == true ) ? ( ( int ) x5 / width ) * 2 - 1 : x5;
    float new_y5 = ( m_ScreenSpace == true ) ? -( ( ( int ) y5 / height ) * 2 - 1 ) : y5;
    float new_x6 = ( m_ScreenSpace == true ) ? ( ( int ) x6 / width ) * 2 - 1 : x6;
    float new_y6 = ( m_ScreenSpace == true ) ? -( ( ( int ) y6 / height ) * 2 - 1 ) : y6;
    float new_x7 = ( m_ScreenSpace == true ) ? ( ( int ) x7 / width ) * 2 - 1 : x7;
    float new_y7 = ( m_ScreenSpace == true ) ? -( ( ( int ) y7 / height ) * 2 - 1 ) : y7;
    float new_x8 = ( m_ScreenSpace == true ) ? ( ( int ) x8 / width ) * 2 - 1 : x8;
    float new_y8 = ( m_ScreenSpace == true ) ? -( ( ( int ) y8 / height ) * 2 - 1 ) : y8;

    float* writeIterator = ( float* ) m_DiscVertexBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
    writeIterator += m_DiscRenderOp.vertexData->vertexCount * 7;

    *writeIterator++ = new_x1; *writeIterator++ = new_y1; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x2; *writeIterator++ = new_y2; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x2; *writeIterator++ = new_y2; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x3; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x3; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x4; *writeIterator++ = new_y4; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x4; *writeIterator++ = new_y4; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x5; *writeIterator++ = new_y5; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x5; *writeIterator++ = new_y5; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x6; *writeIterator++ = new_y6; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x6; *writeIterator++ = new_y6; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x7; *writeIterator++ = new_y7; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x7; *writeIterator++ = new_y7; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x8; *writeIterator++ = new_y8; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x8; *writeIterator++ = new_y8; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y1; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;
    *writeIterator++ = new_x1; *writeIterator++ = new_y3; *writeIterator++ = m_Z; *writeIterator++ = m_Colour.r; *writeIterator++ = m_Colour.g; *writeIterator++ = m_Colour.b; *writeIterator++ = m_Colour.a;

    m_DiscRenderOp.vertexData->vertexCount += 24;
    m_DiscVertexBuffer->unlock();
}



void
DebugDraw::Text( const float x, const float y, const Ogre::String& text )
{
    Ogre::RenderTarget* window = Ogre::Root::getSingleton().getRenderTarget( "QGearsWindow" );
    if( ( Ogre::RenderWindow* )window->isActive() == false )
    {
        return;
    }

    if( m_TextRenderOp.vertexData->vertexCount + text.size() * 6 > m_TextMaxVertexCount )
    {
        LOG_ERROR( "Max number of text reached. Can't add text \"" + text + "\". Max number of letters is " + Ogre::StringConverter::toString( m_TextMaxVertexCount / 6 ) + "." );
        return;
    }

    float* writeIterator = ( float* ) m_TextVertexBuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
    writeIterator += m_TextRenderOp.vertexData->vertexCount * 9;

    float width = window->getViewport( 0 )->getActualWidth();
    float height = window->getViewport( 0 )->getActualHeight();

    float length = 0;
    if( m_TextAlignment != LEFT )
    {
        for( size_t i = 0; i < text.size(); ++i )
        {
            length += ( ( m_Font->getGlyphAspectRatio( text[ i ] ) * m_FontHeight ) / width ) * 2;
        }

        if( m_TextAlignment == CENTER )
        {
            length /= 2;
        }
    }

    float current_x = ( m_ScreenSpace == true ) ? ( ( int ) x / width ) * 2 - 1 : x;
    current_x -= length;
    float current_y =  ( m_ScreenSpace == true ) ? -( ( ( int ) y / height ) * 2 - 1 ) : y;
    float char_height = -( m_FontHeight / height ) * 2;

    for( size_t i = 0; i < text.size(); ++i )
    {
        float char_width = ( ( m_Font->getGlyphAspectRatio( text[ i ] ) * m_FontHeight ) / width ) * 2;

        float new_x1 = current_x;
        float new_y1 = current_y;

        float new_x2 = current_x + char_width;
        float new_y2 = current_y;

        float new_x3 = current_x + char_width;
        float new_y3 = current_y + char_height;

        float new_x4 = current_x;
        float new_y4 = current_y + char_height;

        current_x += char_width;

        const Ogre::Font::UVRect& uv = m_Font->getGlyphTexCoords( text[ i ] );

        *writeIterator++ = new_x1;
        *writeIterator++ = new_y1;
        *writeIterator++ = m_Z;
        *writeIterator++ = m_Colour.r;
        *writeIterator++ = m_Colour.g;
        *writeIterator++ = m_Colour.b;
        *writeIterator++ = m_Colour.a;
        *writeIterator++ = uv.left;
        *writeIterator++ = uv.top;

        *writeIterator++ = new_x2;
        *writeIterator++ = new_y2;
        *writeIterator++ = m_Z;
        *writeIterator++ = m_Colour.r;
        *writeIterator++ = m_Colour.g;
        *writeIterator++ = m_Colour.b;
        *writeIterator++ = m_Colour.a;
        *writeIterator++ = uv.right;
        *writeIterator++ = uv.top;

        *writeIterator++ = new_x3;
        *writeIterator++ = new_y3;
        *writeIterator++ = m_Z;
        *writeIterator++ = m_Colour.r;
        *writeIterator++ = m_Colour.g;
        *writeIterator++ = m_Colour.b;
        *writeIterator++ = m_Colour.a;
        *writeIterator++ = uv.right;
        *writeIterator++ = uv.bottom;

        *writeIterator++ = new_x1;
        *writeIterator++ = new_y1;
        *writeIterator++ = m_Z;
        *writeIterator++ = m_Colour.r;
        *writeIterator++ = m_Colour.g;
        *writeIterator++ = m_Colour.b;
        *writeIterator++ = m_Colour.a;
        *writeIterator++ = uv.left;
        *writeIterator++ = uv.top;

        *writeIterator++ = new_x3;
        *writeIterator++ = new_y3;
        *writeIterator++ = m_Z;
        *writeIterator++ = m_Colour.r;
        *writeIterator++ = m_Colour.g;
        *writeIterator++ = m_Colour.b;
        *writeIterator++ = m_Colour.a;
        *writeIterator++ = uv.right;
        *writeIterator++ = uv.bottom;

        *writeIterator++ = new_x4;
        *writeIterator++ = new_y4;
        *writeIterator++ = m_Z;
        *writeIterator++ = m_Colour.r;
        *writeIterator++ = m_Colour.g;
        *writeIterator++ = m_Colour.b;
        *writeIterator++ = m_Colour.a;
        *writeIterator++ = uv.left;
        *writeIterator++ = uv.bottom;

        m_TextRenderOp.vertexData->vertexCount += 6;
    }

    m_TextVertexBuffer->unlock();
}



void
DebugDraw::Text( const Ogre::Vector3& point, const float x, const float y, const Ogre::String& text )
{
    Ogre::Vector3 point2d = CameraManager::getSingleton().ProjectPointToScreen( point );

    if( point2d.z <= 0 )
    {
        float dist_sq = point.squaredDistance( CameraManager::getSingleton().GetCurrentCamera()->getPosition() );

        if( dist_sq < m_FadeEndSquare )
        {
            float a = ( dist_sq > m_FadeStartSquare ) ? ( 1.0f - ( dist_sq - m_FadeStartSquare ) / ( m_FadeEndSquare - m_FadeStartSquare ) ) : 1.0f;
            Ogre::ColourValue colour = m_Colour;
            colour.a = a;
            SetColour( colour );
            Text( point2d.x + x, point2d.y + y, text );
        }
    }
}



void
DebugDraw::renderQueueEnded( Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation )
{
    if( queueGroupId == Ogre::RENDER_QUEUE_OVERLAY )
    {
        m_RenderSystem->clearFrameBuffer( Ogre::FBT_DEPTH );
        m_RenderSystem->_setWorldMatrix( Ogre::Matrix4::IDENTITY );
        m_RenderSystem->_setViewMatrix( Ogre::Matrix4::IDENTITY );
        Ogre::Matrix4 mat;
        m_RenderSystem->_convertProjectionMatrix( Ogre::Matrix4::IDENTITY, mat );
        m_RenderSystem->_setProjectionMatrix( mat );

        if( m_LineRenderOp.vertexData->vertexCount != 0 )
        {
            m_SceneManager->_setPass( m_Material->getTechnique( 0 )->getPass( 0 ), true, false );
            m_RenderSystem->_render( m_LineRenderOp );
            m_LineRenderOp.vertexData->vertexCount = 0;
        }

        if( m_QuadRenderOp.vertexData->vertexCount != 0 )
        {
            m_SceneManager->_setPass( m_Material->getTechnique( 0 )->getPass( 0 ), true, false );
            m_RenderSystem->_render( m_QuadRenderOp );
            m_QuadRenderOp.vertexData->vertexCount = 0;
        }

        if( m_CircleRenderOp.vertexData->vertexCount != 0 )
        {
            m_SceneManager->_setPass( m_Material->getTechnique( 0 )->getPass( 0 ), true, false );
            m_RenderSystem->_render( m_CircleRenderOp );
            m_CircleRenderOp.vertexData->vertexCount = 0;
        }

        if( m_DiscRenderOp.vertexData->vertexCount != 0 )
        {
            m_SceneManager->_setPass( m_Material->getTechnique( 0 )->getPass( 0 ), true, false );
            m_RenderSystem->_render( m_DiscRenderOp );
            m_DiscRenderOp.vertexData->vertexCount = 0;
        }

        if( m_TextRenderOp.vertexData->vertexCount != 0 )
        {
            m_SceneManager->_setPass( m_Font->getMaterial()->getTechnique( 0 )->getPass( 0 ), true, false );
            m_RenderSystem->_render( m_TextRenderOp );
            m_TextRenderOp.vertexData->vertexCount = 0;
        }
    }
    else if( queueGroupId == Ogre::RENDER_QUEUE_MAIN )
    {
        m_RenderSystem->_setWorldMatrix( Ogre::Matrix4::IDENTITY );
        m_RenderSystem->_setViewMatrix( CameraManager::getSingleton().GetCurrentCamera()->getViewMatrix( true ) );
        m_RenderSystem->_setProjectionMatrix( CameraManager::getSingleton().GetCurrentCamera()->getProjectionMatrixRS() );

        if( m_Line3dRenderOp.vertexData->vertexCount != 0 )
        {
            m_SceneManager->_setPass( m_Material3d->getTechnique( 0 )->getPass( 0 ), true, false );
            m_RenderSystem->_render( m_Line3dRenderOp );
            m_Line3dRenderOp.vertexData->vertexCount = 0;
        }

        if( m_Triangle3dRenderOp.vertexData->vertexCount != 0 )
        {
            m_SceneManager->_setPass( m_Material3d->getTechnique( 0 )->getPass( 0 ), true, false );
            m_RenderSystem->_render( m_Triangle3dRenderOp );
            m_Triangle3dRenderOp.vertexData->vertexCount = 0;
        }
    }
}



void
DebugDraw::CreateLineVertexBuffer()
{
    m_LineMaxVertexCount = 8192 * 2;
    m_LineRenderOp.vertexData = new Ogre::VertexData;
    m_LineRenderOp.vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vDecl = m_LineRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    vDecl->addElement( 0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE );

    m_LineVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer( vDecl->getVertexSize( 0 ), m_LineMaxVertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

    m_LineRenderOp.vertexData->vertexBufferBinding->setBinding( 0, m_LineVertexBuffer );
    m_LineRenderOp.operationType = Ogre::RenderOperation::OT_LINE_LIST;
    m_LineRenderOp.useIndexes = false;
}



void
DebugDraw::DestroyLineVertexBuffer()
{
    delete m_LineRenderOp.vertexData;
    m_LineRenderOp.vertexData = 0;
    m_LineVertexBuffer.setNull();
    m_LineMaxVertexCount = 0;
}



void
DebugDraw::CreateLine3dVertexBuffer()
{
    m_Line3dMaxVertexCount = 16384 * 2;
    m_Line3dRenderOp.vertexData = new Ogre::VertexData;
    m_Line3dRenderOp.vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vDecl = m_Line3dRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    vDecl->addElement( 0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE );

    m_Line3dVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer( vDecl->getVertexSize( 0 ), m_Line3dMaxVertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

    m_Line3dRenderOp.vertexData->vertexBufferBinding->setBinding( 0, m_Line3dVertexBuffer );
    m_Line3dRenderOp.operationType = Ogre::RenderOperation::OT_LINE_LIST;
    m_Line3dRenderOp.useIndexes = false;
}


void
DebugDraw::DestroyLine3dVertexBuffer()
{
    delete m_Line3dRenderOp.vertexData;
    m_Line3dRenderOp.vertexData = 0;
    m_Line3dVertexBuffer.setNull();
    m_Line3dMaxVertexCount = 0;
}



void
DebugDraw::CreateTriangle3dVertexBuffer()
{
    m_Triangle3dMaxVertexCount = 128 * 3;
    m_Triangle3dRenderOp.vertexData = new Ogre::VertexData;
    m_Triangle3dRenderOp.vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vDecl = m_Triangle3dRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    vDecl->addElement( 0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE );

    m_Triangle3dVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer( vDecl->getVertexSize( 0 ), m_Triangle3dMaxVertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

    m_Triangle3dRenderOp.vertexData->vertexBufferBinding->setBinding( 0, m_Triangle3dVertexBuffer );
    m_Triangle3dRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    m_Triangle3dRenderOp.useIndexes = false;
}


void
DebugDraw::DestroyTriangle3dVertexBuffer()
{
    delete m_Triangle3dRenderOp.vertexData;
    m_Triangle3dRenderOp.vertexData = 0;
    m_Triangle3dVertexBuffer.setNull();
    m_Triangle3dMaxVertexCount = 0;
}



void
DebugDraw::CreateQuadVertexBuffer()
{
    m_QuadMaxVertexCount = 16384 * 6;
    m_QuadRenderOp.vertexData = new Ogre::VertexData;
    m_QuadRenderOp.vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vDecl = m_QuadRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    vDecl->addElement( 0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE );

    m_QuadVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer( vDecl->getVertexSize( 0 ), m_QuadMaxVertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

    m_QuadRenderOp.vertexData->vertexBufferBinding->setBinding( 0, m_QuadVertexBuffer );
    m_QuadRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    m_QuadRenderOp.useIndexes = false;
}



void
DebugDraw::DestroyQuadVertexBuffer()
{
    delete m_QuadRenderOp.vertexData;
    m_QuadRenderOp.vertexData = 0;
    m_QuadVertexBuffer.setNull();
    m_QuadMaxVertexCount = 0;
}



void
DebugDraw::CreateCircleVertexBuffer()
{
    m_CircleMaxVertexCount = 16384 * 16;
    m_CircleRenderOp.vertexData = new Ogre::VertexData;
    m_CircleRenderOp.vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vDecl = m_CircleRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    vDecl->addElement( 0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE );

    m_CircleVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer( vDecl->getVertexSize( 0 ), m_CircleMaxVertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

    m_CircleRenderOp.vertexData->vertexBufferBinding->setBinding( 0, m_CircleVertexBuffer );
    m_CircleRenderOp.operationType = Ogre::RenderOperation::OT_LINE_LIST;
    m_CircleRenderOp.useIndexes = false;
}



void
DebugDraw::DestroyCircleVertexBuffer()
{
    delete m_CircleRenderOp.vertexData;
    m_CircleRenderOp.vertexData = 0;
    m_CircleVertexBuffer.setNull();
    m_CircleMaxVertexCount = 0;
}



void
DebugDraw::CreateDiscVertexBuffer()
{
    m_DiscMaxVertexCount = 16384 * 24;
    m_DiscRenderOp.vertexData = new Ogre::VertexData;
    m_DiscRenderOp.vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vDecl = m_DiscRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    vDecl->addElement( 0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE );

    m_DiscVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer( vDecl->getVertexSize( 0 ), m_DiscMaxVertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

    m_DiscRenderOp.vertexData->vertexBufferBinding->setBinding( 0, m_DiscVertexBuffer );
    m_DiscRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    m_DiscRenderOp.useIndexes = false;
}



void
DebugDraw::DestroyDiscVertexBuffer()
{
    delete m_DiscRenderOp.vertexData;
    m_DiscRenderOp.vertexData = 0;
    m_DiscVertexBuffer.setNull();
    m_DiscMaxVertexCount = 0;
}



void
DebugDraw::CreateTextVertexBuffer()
{
    m_TextMaxVertexCount = 16384 * 6;
    m_TextRenderOp.vertexData = new Ogre::VertexData;
    m_TextRenderOp.vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vDecl = m_TextRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    vDecl->addElement( 0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT4 );
    vDecl->addElement( 0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES );

    m_TextVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer( vDecl->getVertexSize( 0 ), m_TextMaxVertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

    m_TextRenderOp.vertexData->vertexBufferBinding->setBinding( 0, m_TextVertexBuffer );
    m_TextRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    m_TextRenderOp.useIndexes = false;
}



void
DebugDraw::DestroyTextVertexBuffer()
{
    delete m_TextRenderOp.vertexData;
    m_TextRenderOp.vertexData = 0;
    m_TextVertexBuffer.setNull();
    m_TextMaxVertexCount = 0;
}
