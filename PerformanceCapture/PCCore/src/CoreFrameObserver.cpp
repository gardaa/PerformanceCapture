#include "CoreFrameObserver.h"
#include "CoreFrameBuffer.h"

void CoreFrameObserver::FrameReceived ( const FramePtr pFrame )
{
    CoreFrameBuffer& fb = CoreFrameBuffer::GetInstance ();
    
    VmbFrameStatusType status;
    pFrame->GetReceiveStatus ( status );
    if ( status == VmbFrameStatusComplete ) {
        fb.SetFrame ( m_camId, pFrame );
    }

    m_pCamera->QueueFrame ( pFrame );
}

CoreFrameObserver::CoreFrameObserver ( const unsigned int& iCameraId, CameraPtr iCamera )
    :   IFrameObserver ( iCamera ),
        m_camId ( iCameraId )
{
}

CoreFrameObserver::~CoreFrameObserver(void)
{
}
