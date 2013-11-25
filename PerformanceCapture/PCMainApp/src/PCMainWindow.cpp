#include "PcErrChk.h"
#include "PcMainWindow.h"
#include "PcFrameViewer.h"
#include "PcParameterHandler.h"
#include "PcAppMenu.h"

#include <QTimer>
#include <VimbaCPP/Include/VimbaSystem.h>

using namespace AVT;
using namespace pcm;

PcMainWindow::PcMainWindow ( QWidget *iParent )
    :   QMainWindow ( iParent )
{
    // VimbaSystem setup.
    VmbErrorType err;
    VmbAPI::VimbaSystem& vmbs = VmbAPI::VimbaSystem::GetInstance ();
    err = vmbs.Startup ();
    ERR_CHK ( err, VmbErrorSuccess, "Error initializing VimbaSystem." );

    PcParameterHandler& params = PcParameterHandler::Instance ();

    // Member initialization.
    m_frameViewer = new PcFrameViewer ( this );
    m_leftMenu = new PcAppMenu ( this );

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
    addDockWidget ( Qt::LeftDockWidgetArea, m_leftMenu );
}

PcMainWindow::~PcMainWindow()
{
    // Shuts down the operating VimbaSystem.
    VmbAPI::VimbaSystem& vmbs = VmbAPI::VimbaSystem::GetInstance ();
    vmbs.Shutdown ();
}
