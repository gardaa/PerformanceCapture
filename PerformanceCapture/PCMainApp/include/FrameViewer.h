#ifndef FRAMEVIEWER_H
#define FRAMEVIEWER_H

#include <QGLWidget>
#include <VimbaCPP/Include/VimbaCPP.h>
#include <opencv2/opencv.hpp>

using namespace AVT::VmbAPI;

class PCCoreFrame;
class QTimer;

class FrameViewer : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit FrameViewer ( QWidget* iParent = 0 );
    ~FrameViewer ();
    
signals:

public slots:
    void initializeGL ();
    void resizeGL ( int w, int h );
    void paintGL ();
    void setNumColumns ( int c );
    void calibrateCameras ();
    
private:
    void DrawFrame ( int const& row, int const& col, PCCoreFrame const& frame, std::string const& cameraStatus );
    void AdjustGrid ( const int& nElems );

private:
    int         m_nRows;
    int         m_nCols;
    int         m_width;
    int         m_height;
    
    GLuint*     m_textures;

    QTimer*     m_updateTimer;
};

#endif // FRAMEVIEWER_H
