#ifndef FRAMEVIEWER_H
#define FRAMEVIEWER_H

#include <QGLWidget>
#include <VimbaCPP/Include/VimbaCPP.h>

using namespace AVT::VmbAPI;

class FrameViewer : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit FrameViewer ( QWidget *iParent = 0 );
    ~FrameViewer ();
    
signals:
    
public slots:
    void initializeGL ();
    void resizeGL ( int w, int h );
    void paintGL ();
    void setNumColumns ( int c );
    
private:
    void DrawFrame ( const int& row, const int& col, const FramePtr& frame );
    
private:
    int         m_nRows;
    int         m_nCols;
    int         m_width;
    int         m_height;
    
    GLuint*     m_textures;
};

#endif // FRAMEVIEWER_H
