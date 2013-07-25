#ifndef COREFRAMEOBSERVER_H
#define COREFRAMEOBSERVER_H

#include "PCCoreExport.h"

#include <VimbaCPP/Include/IFrameObserver.h>

using namespace AVT::VmbAPI;

class CoreFrameObserver :
    public IFrameObserver
{
public:
    PCCORE_EXPORT void FrameReceived ( const FramePtr pFrame );

    PCCORE_EXPORT CoreFrameObserver ( const unsigned int& iCameraId, CameraPtr iCamera );
    PCCORE_EXPORT ~CoreFrameObserver ( void );

private:
    unsigned int        m_camId;
};

#endif // COREFRAMEOBSERVER_H