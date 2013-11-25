#ifndef PCFRAMEOBSERVER_H
#define PCFRAMEOBSERVER_H

#include "PcExport.h"

#include <VimbaCPP/Include/IFrameObserver.h>
using namespace AVT;

namespace pcc
{
    /// \ingroup PCCORE
    /// \brief Monitors the arrival of new frames from a specific camera.
    ///
    /// This class serves as a proxy between the VimbaSystem and the PcSystem of the PCCore library.
    /// Whenever a frame is read from the underlying API, this class is responsible to translate
    /// it into a PcFrame and register it into the PCCore main system manager, PcSystem.
    class PcFrameObserver :
        public VmbAPI::IFrameObserver
    {
    public:
        /// \brief Called upon receiving a frame.
        ///
        /// Whenever the VimbaSystem reports a new frame, this method is called.
        /// The received frame is then converted into a PcFrame, registered into the PcSystem
        /// and the original frame is put back on the camera's acquisition queue. This ensures continuous
        /// acquisition throughout time.
        /// \param [in] pFrame      a pointer to the received frame data
        PCCORE_EXPORT void FrameReceived ( VmbAPI::FramePtr const pFrame );

        /// \brief Constructor.
        /// 
        /// Does nothing but call the parent's constructor, reporting the camera being
        /// monitored.
        /// \param [in] iCamera     the camera to monitor for new frames
        PCCORE_EXPORT explicit PcFrameObserver ( VmbAPI::CameraPtr const& iCamera );
    };
}

#endif // PCFRAMEOBSERVER_H
