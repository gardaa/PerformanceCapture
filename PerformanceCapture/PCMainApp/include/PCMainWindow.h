#ifndef PCMAINWINDOW_H
#define PCMAINWINDOW_H

#include <QMainWindow>

class FrameViewer;
class QTimer;

class PCMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    PCMainWindow ( QWidget* iParent = 0 );
    ~PCMainWindow();

private:
    FrameViewer*    m_frameViewer;

    QTimer*         m_updateTimer;
};

#endif // PCMAINWINDOW_H
