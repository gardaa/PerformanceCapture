#ifndef PCPTPSYNCAGENT_H
#define PCPTPSYNCAGENT_H

#define BOOST_ALL_DYN_LINK
#include <boost/thread.hpp>

#include <VimbaCPP/Include/VimbaCPP.h>
using namespace AVT;

namespace pcc
{
    class PcCamera;

    /// \ingroup PCCORE
    /// \brief Monitors the state of the PTP synchronisation.
    ///
    /// This observer is registered on a camera to aid on the PTP synchronisation process.
    /// Whenever the PTP status changes, the FeatureChanged method is invoked to process the state change.
    /// When synchronisation is achieved, sets the sync flag on each camera to true.
    ///
    /// \todo Change class-wide mutex to instance-wide mutex.
    /// \todo Find out why synchronisation is not achieved (all cameras are assigned to "master" PtpStatus).
    /// \todo Fix method FeatureChanged not to use the PcCamera::TryGetMethod template function.
    class PcPtpSyncAgent
        :   public VmbAPI::IFeatureObserver {
    public:
        /// \brief Called upon PTP synchronisation state change.
        /// \param [in] pFeature    the pointer to the PTP synchronisation feature
        void FeatureChanged ( VmbAPI::FeaturePtr const& pFeature );

        /// \brief Constructor.
        ///
        /// Initialises the monitored camera.
        ///
        /// \param [in] iCamera     the camera to monitor
        PcPtpSyncAgent ( PcCamera* iCamera ) : m_camera ( iCamera ) {}

    private:
        static boost::mutex         sm_mutex;   ///< The mutex for shared features.

        PcCamera*                   m_camera;   ///< The camera to monitor.
    };
}

#endif // PCPTPSYNCAGENT_H
