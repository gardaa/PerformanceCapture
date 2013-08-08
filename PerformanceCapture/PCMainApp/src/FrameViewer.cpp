#include "PCCoreErrChk.h"
#include "FrameViewer.h"
#include "PCCoreSystem.h"
#include "PCCoreFrame.h"
#include "ParameterHandler.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include <fstream>
#include <iostream>

#include <QTimer>

FrameViewer::FrameViewer ( QWidget *iParent )
    :   QGLWidget ( iParent ),
        m_nCols ( 0 )
{
    ParameterHandler& params = ParameterHandler::Instance ();

    m_updateTimer = new QTimer ( this );
    m_updateTimer->setInterval ( params.GetRefreshTimeout() );
    m_updateTimer->start ();

    // Wiring.
    connect (
        m_updateTimer, SIGNAL ( timeout  () ),
                 this, SLOT   ( updateGL () )
    );
    
    PCCoreSystem& cs = PCCoreSystem::GetInstance ();
    unsigned int nFrames = cs.GetNumFrames ();

    //cs.StartCapture ();
    
    m_textures = new GLuint[10];
    glGenTextures ( nFrames, m_textures );

    setNumColumns ( params.GetViewportCols () );
}

FrameViewer::~FrameViewer ()
{
    PCCoreSystem& cs = PCCoreSystem::GetInstance ();
    cs.EndCapture ();
    
    PCCoreSystem::DestroyInstance ();

    if ( m_textures ) {
        delete m_textures;
        m_textures = (GLuint*)0x0;
    }
}

void FrameViewer::initializeGL ()
{
    glEnable (GL_TEXTURE_2D);
    glShadeModel (GL_SMOOTH);
    glClearColor ( 0.39f, 0.58f, 0.93f, 1.0f );
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
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    PCCoreSystem& cs = PCCoreSystem::GetInstance ();
    cs.UpdateCameras ();

    std::vector<std::string> cameras = cs.GetCameraList ();

    const int nc = cameras.size ();
    AdjustGrid ( nc );
    for ( unsigned int f = 0; f < nc; f++ ) {
        const int r = f / m_nCols;
        const int c = f % m_nCols;

        std::string const& status = cs.GetCameraStatus ( cameras[f] );
        PCCoreFrame const& frame = cs.GetFrameFromCamera ( cameras[f] );

        //std::cout << "Camera " << cameras[f] << ": " << status << std::endl;
        
        glBindTexture (GL_TEXTURE_2D, m_textures[f]);
        DrawFrame ( r, c, frame, status );
    }
    //DrawFrame ( 0, 0, frames[0] );
    //DrawFrame ( 0, 1, frames[1] );
}
void FrameViewer::setNumColumns ( int c )
{
    PCCoreSystem& cs = PCCoreSystem::GetInstance ();
    m_nCols = c;
    AdjustGrid ( cs.GetNumFrames () );
}

void FrameViewer::calibrateCameras ()
{
    PCCoreSystem& cs = PCCoreSystem::GetInstance ();

    cs.CalibrateCameras ();
}

void FrameViewer::DrawFrame ( int const& row, int const& col, PCCoreFrame const& frame, std::string const& cameraStatus )
{
    int vpw = m_width  / m_nCols;
    int vph = m_height / m_nRows;
    int vpx = col * vpw;
    int vpy = m_height - (row+1) * vph;
    
    unsigned int fWidth = frame.Width ();
    unsigned int fHeight = frame.Height ();
    unsigned int numChannels;
    unsigned char const* frameData;
    frame.GetData ( frameData, numChannels );

    GLint glPixelFormat;
    switch (numChannels) {
    case 1:
        glPixelFormat = GL_LUMINANCE;
        break;

    case 2:
        glPixelFormat = GL_RG;
        break;

    case 3:
        glPixelFormat = GL_RGB;
        break;

    default:
        glPixelFormat = GL_RGB;
        break;
    }
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, glPixelFormat, fWidth, fHeight, 0, glPixelFormat, GL_UNSIGNED_BYTE, frameData);

    glPushMatrix ();

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glViewport ( vpx, vpy, vpw, vph );
    glOrtho ( 0.0, (GLdouble)vpw, (GLdouble)vph, 0.0, -1.0, 1.0 );

    GLfloat imgRatio = (GLfloat)fWidth / (GLfloat)fHeight;
    GLfloat vwpRatio = (GLfloat)vpw    / (GLfloat)vph;

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
        glColor3f( 1.0f, 1.0f, 1.0f );
        glTexCoord2f ( 1.0f, 0.0f );
        glVertex2f ( right, top );
        
        glTexCoord2f ( 0.0f, 0.0f );
        glVertex2f ( left, top );
        
        glTexCoord2f ( 0.0f, 1.0f );
        glVertex2f ( left, bottom );
        
        glTexCoord2f ( 1.0f, 1.0f );
        glVertex2f ( right, bottom );

    glEnd ();
    
    glDisable ( GL_TEXTURE_2D );


    float sWid = 0.01f * rWid;
    float sHei = 0.01f * rWid;

    top += 2 * sHei;
    right -= 2 * sWid;
    
    left = right - 10.0f;
    bottom = top + 10.0f;
    
    glBegin (GL_QUADS);
        if ( cameraStatus.compare ( "Off" ) == 0 ) {
            glColor3f( 1.0f, 0.0f, 0.0f );
        } else if ( cameraStatus.compare ("Master") == 0 ) {
            glColor3f( 0.0f, 1.0f, 0.0f );
        } else if ( cameraStatus.compare ("Slave") == 0 ) {
            glColor3f( 0.0f, 0.0f, 1.0f );
        } else {
            glColor3f( 1.0f, 1.0f, 0.0f );
        }

        glVertex2f ( right, top );
        glVertex2f ( left, top );
        glVertex2f ( left, bottom );
        glVertex2f ( right, bottom );
    glEnd ();
    glEnable ( GL_TEXTURE_2D );

    glPopMatrix ();
}

void FrameViewer::AdjustGrid ( int const& nElems )
{
    m_nRows = (nElems / m_nCols);
    if ( nElems % m_nCols ) {
        m_nRows++;
    }
}