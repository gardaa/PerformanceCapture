#include "ErrChk.h"
#include "PCMainWindow.h"
#include "FrameViewer.h"
#include "ParameterHandler.h"

#include <QTimer>
#include <VimbaCPP/Include/VimbaSystem.h>

using namespace AVT::VmbAPI;

PCMainWindow::PCMainWindow ( QWidget *iParent )
    :   QMainWindow ( iParent )
{
    // VimbaSystem setup.
    VmbErrorType err;
    VimbaSystem& vmbs = VimbaSystem::GetInstance ();
    err = vmbs.Startup ();
    ERR_CHK ( err, VmbErrorSuccess, "Error initializing VimbaSystem." );

    // Member initialization.
    m_frameViewer = new FrameViewer ( this );
    m_updateTimer = new QTimer ( this );
    m_updateTimer->setInterval ( ParameterHandler::GetRefreshTimeout() );
    m_updateTimer->start ();

    // Wiring.
    connect (
        m_updateTimer, SIGNAL ( timeout  () ),
        m_frameViewer, SLOT   ( updateGL () )
    );

    // Widget setup.
    setCentralWidget ( m_frameViewer );
}

PCMainWindow::~PCMainWindow()
{
    // Shuts down the operating VimbaSystem.
    VimbaSystem& vmbs = VimbaSystem::GetInstance ();
    vmbs.Shutdown ();
}
