#include "PcFrameObserver.h"
#include "PcSystem.h"

using namespace pcc;

void PcFrameObserver::FrameReceived ( VmbAPI::FramePtr const pFrame )
{
    PcSystem& cs = PcSystem::GetInstance ();
    
    VmbAPI::VmbFrameStatusType status;
    pFrame->GetReceiveStatus ( status );
    if ( status == VmbAPI::VmbFrameStatusComplete ) {
        cs.SetFrame ( m_pCamera, pFrame );
    }

    m_pCamera->QueueFrame ( pFrame );
}

PcFrameObserver::PcFrameObserver ( VmbAPI::CameraPtr const& iCamera )
    :   VmbAPI::IFrameObserver ( iCamera )
{}

PcFrameObserver::~PcFrameObserver ()
{}