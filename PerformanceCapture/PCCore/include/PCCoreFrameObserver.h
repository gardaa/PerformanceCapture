#ifndef PCCOREFRAMEOBSERVER_H
#define PCCOREFRAMEOBSERVER_H

#include "PCCoreExport.h"

#include <VimbaCPP/Include/IFrameObserver.h>

using namespace AVT::VmbAPI;

class PCCoreFrameObserver :
    public IFrameObserver
{
public:
    PCCORE_EXPORT void FrameReceived ( FramePtr const pFrame );

    PCCORE_EXPORT explicit PCCoreFrameObserver ( CameraPtr const& iCamera );
    PCCORE_EXPORT ~PCCoreFrameObserver ( void );
};

#endif // PCCOREFRAMEOBSERVER_H