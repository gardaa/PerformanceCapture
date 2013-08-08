#include "PCCoreErrChk.h"
#include "PCMainWindow.h"
#include "FrameViewer.h"
#include "ParameterHandler.h"
#include "PCMainAppMenu.h"

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

    ParameterHandler& params = ParameterHandler::Instance ();

    // Member initialization.
    m_frameViewer = new FrameViewer ( this );
    m_leftMenu = new PCMainAppMenu ( this );

    // Wiring
    connect (
              &params, SIGNAL ( numColsChanged ( int ) ),
        m_frameViewer, SLOT   ( setNumColumns  ( int ) )
    );
    connect (
           m_leftMenu, SIGNAL ( calibrationClicked () ),
        m_frameViewer, SLOT   ( calibrateCameras   () )
    );

    // Widget setup.
    setCentralWidget ( m_frameViewer );
    addDockWidget ( Qt::DockWidgetArea::LeftDockWidgetArea, m_leftMenu );
}

PCMainWindow::~PCMainWindow()
{
    // Shuts down the operating VimbaSystem.
    VimbaSystem& vmbs = VimbaSystem::GetInstance ();
    vmbs.Shutdown ();
}
