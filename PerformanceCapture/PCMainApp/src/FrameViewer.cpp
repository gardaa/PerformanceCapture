#include "FrameViewer.h"
#include "CoreFrameBuffer.h"
#include "ParameterHandler.h"
#include "ErrChk.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include <fstream>
#include <iostream>

FrameViewer::FrameViewer ( QWidget *iParent )
    :   QGLWidget ( iParent ),
        m_nCols ( ParameterHandler::GetViewportCols () )
{
    m_width     =   1280;
    m_height    =    720;
    
    CoreFrameBuffer& fb = CoreFrameBuffer::GetInstance ();
    
    unsigned int nFrames = fb.GetAllFrames ().size ();
    m_nRows = (nFrames / m_nCols);
    if ( nFrames % m_nCols ) {
        m_nRows++;
    }

    fb.StartCapture ();
    
    m_textures = new GLuint[nFrames];
    glGenTextures(nFrames, m_textures);
}

FrameViewer::~FrameViewer ()
{
    CoreFrameBuffer& reader = CoreFrameBuffer::GetInstance ();
    reader.EndCapture ();
    
    CoreFrameBuffer::DestroyInstance ();

    if ( m_textures ) {
        delete m_textures;
        m_textures = (GLuint*)0x0;
    }
}

void FrameViewer::initializeGL ()
{
    glEnable (GL_TEXTURE_2D);
    glShadeModel (GL_SMOOTH);
    glClearColor ( 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f );
    glClearDepth (1.0f);
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
void FrameViewer::resizeGL ( int w, int h )
{
    m_width = w;
    m_height = h;
}
void FrameViewer::paintGL ()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CoreFrameBuffer& reader = CoreFrameBuffer::GetInstance ();
    const FramePtrVector& frames = reader.GetAllFrames ();
    
    const int nf = frames.size ();
    for ( unsigned int f = 0; f < nf; f++ ) {
        const int r = f / m_nCols;
        const int c = f % m_nCols;

        glBindTexture (GL_TEXTURE_2D, m_textures[f]);
        DrawFrame ( r, c, frames[f] );
    }
    //DrawFrame ( 0, 0, frames[0] );
    //DrawFrame ( 0, 1, frames[1] );
}
void FrameViewer::setNumColumns ( int c )
{
    m_nCols = c;
}

void FrameViewer::DrawFrame ( const int &row, const int &col, const FramePtr &frame )
{
    if ( SP_ISNULL (frame) ) {
        std::cout << "Null frame ptr" << std::endl;
        return;
    }
    int vpw = m_width  / m_nCols;
    int vph = m_height / m_nRows;
    int vpx = col * vpw;
    int vpy = m_height - (row+1) * vph;
    
    VmbErrorType err;
    VmbUint32_t width, height;
    err = frame->GetHeight (height);
    ERR_CHK ( err, VmbErrorSuccess, "Error reading frame height." );
    err = frame->GetWidth (width);
    ERR_CHK ( err, VmbErrorSuccess, "Error reading frame width." );

    //std::cout << "Drawing " << width << "x" << height << std::endl;

    VmbPixelFormatType pixelFormat;
    err = frame->GetPixelFormat (pixelFormat);
    ERR_CHK ( err, VmbErrorSuccess, "Error reading frame pixel format data." );
    
    GLint glPixelFormat;
    switch (pixelFormat) {
    case VmbPixelFormatBayerGR8:
        glPixelFormat = GL_RGB;
        break;

    case VmbPixelFormatMono8:
        glPixelFormat = GL_LUMINANCE;
        break;

    case VmbPixelFormatRgb8:
        glPixelFormat = GL_RGB;
        break;

    default:
        glPixelFormat = GL_RGB;
        break;
    }
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    //GLubyte* tex_data = new GLubyte[3*width*height];
    //GLfloat sz = (width*height);
    //for ( unsigned int i = 0; i < height; i++ ) {
    //    for ( unsigned int j = 0; j < width; j++ ) {
    //        GLuint val = 255u*(i+j)/(height+width);
    //        
    //        tex_data[ 3 * (width*i + j) + 0 ] = (GLubyte)val;
    //        tex_data[ 3 * (width*i + j) + 1 ] = (GLubyte)val;
    //        tex_data[ 3 * (width*i + j) + 2 ] = (GLubyte)val;
    //    }
    //}
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

    VmbUchar_t* tex_data = (VmbUchar_t*)0x0;
    err = frame->GetImage( tex_data );
    
    ERR_CHK ( err, VmbErrorSuccess, "Error reading frame texture data." );
    glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, tex_data);

    glPushMatrix ();

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glViewport ( vpx, vpy, vpw, vph );
    glOrtho ( 0.0, (GLdouble)vpw, (GLdouble)vph, 0.0, -1.0, 1.0 );

    GLfloat imgRatio = (GLfloat)width / (GLfloat)height;
    GLfloat vwpRatio = (GLfloat)vpw   / (GLfloat)vph;

    GLfloat mX = 0.0f, mY = 0.0f;
    GLfloat rHei = vpw, rWid = vph;
    if ( vwpRatio > imgRatio ) {
        rWid    = vph  *       imgRatio;    rHei =  vph;
        mX      = 0.5f * ( vpw - rWid );    mY   = 0.0f;
    } else if ( vwpRatio < imgRatio ) {
        rHei    = vpw  /       imgRatio;    rWid =  vpw;
        mY      = 0.5f * ( vph - rHei );    mX   = 0.0f;
    } else {
        rWid    =  vpw;     rHei    =  vph;
        mY      = 0.0f;     mX      = 0.0f;
    }

    GLfloat  top = mY, bottom =  top + rHei;
    GLfloat left = mX,  right = left + rWid;

    glBegin ( GL_QUADS );
        glTexCoord2f ( 1.0f, 0.0f );
        glVertex2f ( right, top );
        
        glTexCoord2f ( 0.0f, 0.0f );
        glVertex2f ( left, top );
        
        glTexCoord2f ( 0.0f, 1.0f );
        glVertex2f ( left, bottom );
        
        glTexCoord2f ( 1.0f, 1.0f );
        glVertex2f ( right, bottom );
    glEnd ();
    
    //glDisable ( GL_TEXTURE_2D );
    //glBegin (GL_LINES);
    //    glVertex2i (       0,       0 );
    //    glVertex2i ( vpw,       0 );
    //    
    //    glVertex2i ( vpw,       0 );
    //    glVertex2i ( vpw, vph - 1 );
    //    
    //    glVertex2i ( vpw, vph - 1 );
    //    glVertex2i (       1, vph - 1 );
    //    
    //    glVertex2i (       1, vph - 1 );
    //    glVertex2i (       1,       0 );
    //glEnd ();
    //glEnable ( GL_TEXTURE_2D );

    glPopMatrix ();
}
