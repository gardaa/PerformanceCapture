#ifndef PCSYSTEM_H
#define PCSYSTEM_H

#include "PcCommon.h"
#include "PcExport.h"
#include "PcFrame.h"
#include "PcCamera.h"
#include "PcStereoCameraPair.h"

#include <unordered_map>
#include <map>
#include <queue>

#include <VimbaCPP/Include/VimbaCPP.h>
#include <opencv2/opencv.hpp>

using namespace AVT;

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>

/// The main namespace of the PerformanceCapture core library.
namespace pcc
{
    /// \ingroup PCCORE
    /// \brief Coordinates the PCCore module's main funcionality. Singleton.
    ///
    /// Presents a public interface to interact with the PCCore library.
    ///
    /// The PcSystem singleton monitors the underlying VimbaSystem for any newly plugged or unplugged
    /// classes. It also provides an interface to:
    ///     - Start/Stop image acquisition (all plugged cameras at once).
    ///     - Start camera calibration.
    ///     - Start PTP camera synchronisation.
    ///     - Get a list containing the IDs of the currently plugged cameras.
    ///     - Access a given camera's frame stream.
    ///     - Access a given camera's synchronisation and calibration status.
    ///
    /// The PcSystem class concentrates all information flow from managed cameras.
    ///
    /// Before the first PcSystem is created (that is, before the first call to PcSystem::GetInstance),
    /// the underlying VmbAPI should be initialised via a call to VimbaSystem::Startup.
    /// See the examples section for minimal examples of how to use the PcSystem class and interact with the PCCore library. 
    ///
    /// \todo   Review camera synchronisation process to find out why all of the cameras are assigned with the MASTER PTP state.
    class PcSystem
        :   public VmbAPI::ICameraListObserver
    {
    private:
        typedef boost::mutex                    MutexType;  ///< The mutex used to lock features inside the PcSystem.
        typedef boost::lock_guard<MutexType>    GuardType;  ///< The RAII lock used to lock PcSystem internal features, together with MutexType.

    private:
        /// \brief Default constructor. 
        ///
        /// Initialises all lists and queues to an empty state. Allocates memory for the
        /// mutex variable.
        PcSystem ();

        /// \brief Private copy constructor.
        ///
        /// Disables copies of the PcSystem singleton.
        /// \param [in] iOther   the instance to copy
        explicit PcSystem ( PcSystem const& iOther ) {}

        /// \brief Private copy constructor.
        ///
        /// Disables copies of the PcSystem singleton.
        /// \param [in] iOther   the instance to copy
        /// \return this object, unchanged.
        PcSystem& operator= ( PcSystem const& iOther ) { return (*this); }

    public:
        /// \brief Destructor.
        ///
        /// Deallocates the memory allocated for the internal mutex.
        ///
        /// Aborts any calibration process that is currently running.
        ~PcSystem ();

    public:
        /// \brief Gets the singleton instance.
        ///
        /// If there is no currently allocated PcSystem instance, creates one and registers it as
        /// a CameraListObserver on the underlying VimbaSystem to watch for newly plugged/unplugged cameras.
        ///
        /// \return a reference to the Singleton instance.
        PCCORE_EXPORT static PcSystem& GetInstance ();

        /// \brief Destroy the current singleton instance.
        /// 
        /// Unregisters the current CameraListObserver instance from the underlying VimbaSystem instance and
        /// deletes the current PcSystem object instance.
        PCCORE_EXPORT static void DestroyInstance ();

        /// \brief Updates the current camera list, based on two auxiliary input queues.
        ///
        /// Uses queues that are filled by the CameraListChanged method to add and remove cameras
        /// that are plugged and unplugged between two calls to UpdateCameras.
        PCCORE_EXPORT void UpdateCameras ();

        /// \brief Starts frame stream capturing for every registered camera.
        ///
        /// Iterates over the list of registered cameras and calls the PcCamera::StartCapture method.
        PCCORE_EXPORT void StartCapture ();

        /// \brief Ends frame stream capturing for every registered camera.
        ///
        /// Iterates over the list of registered cameras and calls the PcCamera::EndCapture method.
        PCCORE_EXPORT void EndCapture ();
        
        /// \brief Gets the number of currently registered frame queues.
        ///
        /// Returns the number of currently registered frame queues (equal to the number of
        /// of available cameras, since each camera has only one frame queue).
        ///
        /// \return the number of currently registered frame queues.
        PCCORE_EXPORT unsigned int GetNumFrames ();

        /// \brief Gets a list containing the GUID of every available camera.
        ///
        /// Iterates over every camera registered on the main camera map and puts their GUID inside a
        /// vector, in the same order they are returned by the map.
        ///
        /// \return a vector containing all the GUID of the cameras, as strings
        PCCORE_EXPORT VEC(std::string) GetCameraList ();

        /// \brief Reads the last registered frame for a given camera.
        /// \param [in] iCameraId    the GUID of the camera whose frame is being requested 
        /// \return the frame that is currently registered on the frame-camera map for a given camera.
        PCCORE_EXPORT PcFramePtr const GetFrameFromCamera ( std::string const& iCameraId );

        /// \brief Reads the PTP synchronisation status for a given camera.
        /// \param [in] iCameraId   the GUID of the camera whose status is being queried
        /// \return a string containing the status of the PTP synchronisation (see PcCamera::GetPtpStatus for further information)
        PCCORE_EXPORT std::string GetCameraStatus ( std::string const& iCameraId );
        
        /// \brief Gets the calibration progress for a given camera.
        ///
        /// Internally calls the PcCamera::GetCalibrationProgress method and returns its value.
        ///
        /// \param [in] iCameraId   the GUID of the camera whose calibration process is being queried
        /// \return the calibration progress for the camera
        PCCORE_EXPORT double GetCameraCalibrationProgress ( std::string const& iCameraId );
        
        /// \brief Callback to notify the PcSystem of changes in the list of plugged cameras.
        ///
        /// Whenever this method is called, if iUpdateReason indicates a camera has been PLUGGED,
        /// registers that camera's ID and VmbAPI::Camera object on the ADD queue.
        /// If iUpdateReason indicates a camera has been UNPLUGGED, registers that camera's ID for deletion, on the REMOVE queue.
        /// These queues will be further processed by the UpdateCameras method.
        ///
        /// \param [in] iCamera         a pointer to the VmbAPI::Camera object that triggered the call.
        /// \param [in] iUpdateReason   the reason why the call was triggered (a camera was plugged, a camera was unplugged)
        PCCORE_EXPORT void CameraListChanged ( VmbAPI::CameraPtr iCamera, VmbAPI::UpdateTriggerType iUpdateReason );
    
        /// \brief Gets the maximum available bandwidth for each individual camera.
        ///
        /// On a scenario where all cameras' inputs come through a single ethernet interface,
        /// limits the available bandwidth for each camera to a fraction of 1/N of the total available bandwidth,
        /// to avoid packet loss, where N is the number of currently available cameras.
        ///
        /// \return an integer value describing the per-camera available bandwidth, in bytes
        PCCORE_EXPORT int GetMaxPerCameraBandwidth ();

        /// \brief Sets up the PcSystem instance for usage. Should be called before the first interaction with the PcSystem singleton.
        ///
        /// Reads all available cameras from the underlying VimbaSystem and registers them on the ADD queue to be listed as availale
        /// by the PcSystem.
        PCCORE_EXPORT void Setup ();

        /// \brief Launches the PTP synchronisation process on all available cameras.
        /// 
        /// Iterates over the list of all available cameras and calls the PcCamera::Synchronise method.
        PCCORE_EXPORT void SynchroniseCameras ();

        /// \brief Launches the calibration process on all available cameras.
        ///
        /// Iterates over the list of all available cameras and calls the PcCamera::StartCalibration method.
        PCCORE_EXPORT void CalibrateCameras ();

        /// \brief Sets the current frame for a given camera.
        ///
        /// Sets the current frame for a given camera. Reads frame's dimensions and raw data and calls
        /// PcFrame::Reset for the corresponding camera, passing the new frame as input data.
        /// Also, if that camera is in the ACQUIRING phase of a calibration, pushes the newly read frame into its
        /// calibration frame list, through the PcCamera::TryPushFrame method.
        ///
        /// This method is called by a camera's PcFrameObserver's thread whenever a new frame is read from it.
        /// 
        /// \param [in] iCamera     a pointer to the underlying VmbAPI::Camera that triggered the call
        /// \param [in] iFrame      a pointer to the VmbAPI::Frame that triggered the call
        void SetFrame ( VmbAPI::CameraPtr const& iCamera, VmbAPI::FramePtr const& iFrame );

    private:
        /// \brief Effectively adds a camera to the list of active cameras.
        ///
        /// If the camera isn't already in the list, allocates memory to a new PcCamera, adds it to the 
        /// camera list. It then performs the camera's setup, adjusts the available bandwidth of all cameras
        /// and starts the camera's frame stream.
        ///
        /// \param [in] iCameraId   the camera's GUID
        /// \param [in] iCamera     a pointer to the camera from which to create the PcCamera
        void RegisterCamera ( std::string const& iCameraId, VmbAPI::CameraPtr const& iCamera );
        
        /// \brief Effectively removes a camera from the list of active cameras.
        ///
        /// If the camera is on the active list, stops its frame stream and adjusts the available bandwidth
        /// of the remaining cameras.
        /// 
        /// \param [in] iCameraId   the GUID of the camera to remove from the list
        void UnregisterCamera ( std::string const& iCameraId );

    private:
        static VmbAPI::ICameraListObserverPtr           sm_pInstance;       ///< The singleton instance of the PcSystem, stored as a reference-counted pointer to a CameraListObserver.

        MutexType*                                      m_mutex;            ///< The internal mutex for the PcSystem.
        QUEUE(std::string)                              m_removeQueue;      ///< The queue of camera GUIDs to remove from the active list.
        QUEUE(VmbAPI::CameraPtr)                        m_addQueue;         ///< The queue of camera pointers to add to the active list.

        STRMAP(PcCameraPtr)                             m_activeCameras;    ///< The list of active cameras, represented as a string-indexed ordered map.
        STRMAP(PcFramePtr)                              m_frames;           ///< The list of frames, indexed by its camera's GUID.

        VEC(PcStereoCameraPairPtr)                      m_stereo;           ///< The list of stereo pairs currently active in the system.
    };
}

#endif // PCSYSTEM_H
