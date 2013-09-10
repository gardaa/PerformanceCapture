#ifndef PCFRAMEVIEWER_H
#define PCFRAMEVIEWER_H

#include <QGLWidget>
#include <VimbaCPP/Include/VimbaCPP.h>
#include <opencv2/opencv.hpp>

#include "PcFrame.h"

class QTimer;

class PcFrameViewer : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit PcFrameViewer ( QWidget* iParent = 0 );
    ~PcFrameViewer ();
    
signals:

public slots:
    void initializeGL ();
    void resizeGL ( int w, int h );
    void paintGL ();
    void setNumColumns ( int c );
    void calibrateCameras ();
    
private:
    void DrawFrame ( int const& row, int const& col, pcc::PcFramePtr const& frame, std::string const& cameraStatus, float const& progress );
    void AdjustGrid ( const int& nElems );

private:
    int         m_nRows;
    int         m_nCols;
    int         m_width;
    int         m_height;
    
    GLuint*     m_textures;

    QTimer*     m_updateTimer;
};

#endif // PCFRAMEVIEWER_H
