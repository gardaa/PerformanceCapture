#include "PCCoreFrameObserver.h"
#include "PCCoreSystem.h"

#include "PCCoreCalibrationHelper.h"

void PCCoreFrameObserver::FrameReceived ( FramePtr const pFrame )
{
    PCCoreSystem& cs = PCCoreSystem::GetInstance ();
    
    VmbFrameStatusType status;
    pFrame->GetReceiveStatus ( status );
    if ( status == VmbFrameStatusComplete ) {
        cs.SetFrame ( m_pCamera, pFrame );
    }

    m_pCamera->QueueFrame ( pFrame );
}

PCCoreFrameObserver::PCCoreFrameObserver ( CameraPtr const& iCamera )
    :   IFrameObserver ( iCamera )
{}

PCCoreFrameObserver::~PCCoreFrameObserver ()
{}