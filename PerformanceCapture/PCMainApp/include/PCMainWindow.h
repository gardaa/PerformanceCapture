#ifndef PCMAINWINDOW_H
#define PCMAINWINDOW_H

#include <QMainWindow>

class PcFrameViewer;
class PcAppMenu;
class QTimer;

class PcMainWindow
    :   public QMainWindow
{
    Q_OBJECT
    
public:
    PcMainWindow ( QWidget* iParent = 0 );
    ~PcMainWindow();

private:
    // Widgets
    PcFrameViewer*    m_frameViewer;
    PcAppMenu*  m_leftMenu;
};

#endif // PCMAINWINDOW_H
