#ifndef PCMAINWINDOW_H
#define PCMAINWINDOW_H

#include <QMainWindow>

class FrameViewer;
class PCMainAppMenu;
class QTimer;

class PCMainWindow
    :   public QMainWindow
{
    Q_OBJECT
    
public:
    PCMainWindow ( QWidget* iParent = 0 );
    ~PCMainWindow();

private:
    // Widgets
    FrameViewer*    m_frameViewer;
    PCMainAppMenu*  m_leftMenu;
};

#endif // PCMAINWINDOW_H
