#ifndef PCFRAMEOBSERVER_H
#define PCFRAMEOBSERVER_H

#include "PcExport.h"

#include <VimbaCPP/Include/IFrameObserver.h>
using namespace AVT;

namespace pcc
{
    class PcFrameObserver :
        public VmbAPI::IFrameObserver
    {
    public:
        PCCORE_EXPORT void FrameReceived ( VmbAPI::FramePtr const pFrame );

        PCCORE_EXPORT explicit PcFrameObserver ( VmbAPI::CameraPtr const& iCamera );
        PCCORE_EXPORT ~PcFrameObserver ( void );
    };
}

#endif // PCFRAMEOBSERVER_H