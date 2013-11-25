#ifndef PCCAMERA_H
#define PCCAMERA_H

#include "PcExport.h"
#include "PcCommon.h"

#include "PcCameraCalibration.h"

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <VimbaCPP/Include/Camera.h>
using namespace AVT;

//#define BOOST_ALL_DYN_LINK
//#include <boost/thread.hpp>

namespace pcc
{
    /// \ingroup    PCCORE 
    ///
    /// \brief      Wraps around the AVT::VmbAPI::Camera class.
    ///
    /// This class provides easier access and initialisation of AVT::VmbAPI::Camera objects.
    /// The PcCamera class aims to provide easier access to camera calibration, PTP camera synchronisation,
    /// frame stream acquisition and GigE parameter regulation so that multiple cameras can be used on a single ethernet card.
    /// No instance of a PcCamera is guaranteed to have it's underlying VmpAPI::Camera object open, initialised and ready to
    /// operate until it's Setup method has been called. This means that:
    ///     - The underlying Camera object has been opened for manipulation
    ///     - The proper amount of bandwidth has been reserved for the camera to avoid packet collision in a scenario where
    ///       the input from multiple cameras comes through a single ethernet interface
    ///     - The pixel format of the frame acquisition streams is set to 8-bit depth monochromatic (grayscale)
    ///     - The hardware gain of the camera is set to "Auto", to decrease the set-up time of the physical cameras
    /// 
    /// The PcCamera class caches data such as:
    ///     - Frame dimensions
    ///     - Camera's unique id, used to refer to it across multiple contexts
    ///     - Camera's PTP synchronisation status
    /// 
    /// Each PcCamera object contains a reference to an instance of PcCameraCalibration, as well as two matrices describing the
    /// current intrinsic calibration parameters of the camera in real world.
    /// Accessing the underlying data is thread-safe, and only one thread at a time can have access to the data.
    ///
    /// \todo   Provide support for an alternative scenario with multiple cameras connected to multiple ethernet interfaces.
    class PcCamera
    {
    private:
        typedef boost::mutex                    MutexType;  ///< The type of mutex used to internally lock the access to the camera's data. 
        typedef boost::lock_guard<MutexType>    GuardType;  ///< The RAII lock guard to be used conjointely with the mutex.

    public:
        /// \brief Creates a PcCamera from a VmbAPI::Camera.
        ///
        /// Sets the internal camera pointer to the provided value, initialises the PcCameraCalibration instance
        /// for the camera and prepares internal members to receive data from the underlying camera.
        ///
        /// \param [in] iCamera     the camera instance represented by this object.
        PCCORE_EXPORT explicit PcCamera ( VmbAPI::CameraPtr const& iCamera );

        ///// \brief Default destructor.
        //PCCORE_EXPORT ~PcCamera ();

        /// \brief Sets the camera up for usage.
        ///
        /// Performs the initial set up on the camera, to prepare it for acquisition.
        /// This method performs the following actions:
        ///     - Open the camera with full access privileges
        ///     - Read the camera's GUID
        ///     - Read the camera's frame dimensions
        ///     - Set up the exposure and gain of the camera to a reasonable empiric default value
        ///     - Set the camera to work on Grayscale mode (8-bit depth pixels)
        ///     - Set the packet size to the usual ethernet MTU (1500 bytes)
        /// 
        /// \todo Make exposure and gain adjustable from the outside.
        PCCORE_EXPORT void Setup ();

        /// \brief Starts a frame aquisition stream.
        ///
        /// Calls the corresponding method on the underlying camera to start a continuous acquisition stream.
        /// Continuous acquisition streams rely on a frame observer (see PcFrameObserver) and a frame queue to
        /// work. Currently, continuous acquisition streams on the PCCore library work with a 10-frame queue.
        /// If the camera is synchronised with other cameras on the network (see Synchronise), this method
        /// also tells the camera to delay the beginning of the stream until all cameras can begin at the same time.
        PCCORE_EXPORT void StartAcquisition ();

        /// \brief Stops the frame acquisition stream.
        ///
        /// Calls the corresponding method on the VmbAPI::Camera to stop a running continuous acquisition stream.
        PCCORE_EXPORT void StopAcquisition ();

        /// \brief Adjusts the available bandwidth for the camera.
        ///
        /// Resets the available bandwidth the camera has to send frame data through the ethernet network.
        ///
        /// \param [in] iBandwidth  the new value for the available bandwidth, in bytes per second
        PCCORE_EXPORT void AdjustBandwidth ( unsigned int const& iBandwidth );

        /// \brief Starts the PTP synchronisation process.
        ///
        /// Launches the PTP synchronisation process. Instantiates and registers on the camera a PcPtpSyncAgent
        /// feature observer to monitor the PTP synchronisation progress.
        PCCORE_EXPORT void Synchronise ();

        /// \brief Starts the calibration process.
        /// 
        /// Launches the calibration process through the camera's PcCameraCalibration object.
        PCCORE_EXPORT void StartCalibration ();

        /// \brief Starts the calibration process.
        /// 
        /// Aborts the calibration process through the camera's PcCameraCalibration object.
        PCCORE_EXPORT void AbortCalibration ();

        /// \brief Tries to register a new frame to be used on the calibration process.
        ///
        /// Tries to register a frame to be used on calibration, on the PcCameraCalibration member instance.
        /// This method ensures the capture delay between two consecutive calibration frames.
        ///
        /// \param [in] iFrame      the frame to be queued for calibration
        PCCORE_EXPORT void TryPushFrame ( PcFramePtr const& iFrame );
        
        /// \brief Reads the current state of the calibration process.
        /// \return the current calibration state
        PCCORE_EXPORT pcc::CalibrationState GetCalibrationState ();

        /// \brief Calculates the current progress, in percentage, of the calibration process.
        ///
        /// \returns a double contained in the interval [0,1]
        PCCORE_EXPORT double GetCalibrationProgress () const;

        /// \brief Gets the list of detected chessboard corners in each calibration frame.
        ///
        /// See PcCameraCalibration::GetChessboardCorners for more information.
        ///
        /// \return a vector of vectors of transformed chessboard point's coordinates
        PCCORE_EXPORT VECOFVECS(cv::Point2f) const& GetChessboardCorners () const;

        /// \brief Actually launches the camera calibration process.
        ///
        /// Calls the corresponding OpenCV function to calculate the intrinsic calibration matrix and the distortion coefficient, from
        /// a set of chessboard descriptors and their detected projections on the calibration frames.
        /// \param [in] iChessboardPoints           a list of lists of points in 3D space describing the chessboards detected in each calibration frame
        /// \param [in] iDetectedChessboardPoints   a list of lists of the detected chessboard points in each calibration image
        /// \param [in] iImageSize                  the dimensions of the image where the chessboards were detected, in pixels
        /// \return the average reprojection error over all images. The reprojection error is the error between the projection of the chessboard points
        ///         using the calculated matrix and the detected points given as input.        
        double DoCalibration ( cv::InputArrayOfArrays iChessboardPoints, cv::InputArrayOfArrays iDetectedChessboardPoints, cv::Size iImageSize );

        /// \brief Gets the underlying camera's GUID (read-only).
        /// \return a constant string reference to the camera's GUID
        inline std::string const& GetID () const { return m_cameraId; };
    
        /// \brief Gets the intrinsic calibration matrix.
        /// \return a cv::Mat reference to the intrinsic calibration matrix
        inline cv::Mat& CameraMatrix () { return (m_cameraMatrix); };

        /// \brief Gets the intrinsic calibration matrix (read-only).
        /// \return a constant cv::Mat reference to the intrinsic calibration matrix
        inline cv::Mat const& CameraMatrix () const { return (m_cameraMatrix); };

        /// \brief Gets the camera's calculated distortion coefficients. 
        /// \return a cv::Mat reference to the array of distortion coefficients
        inline cv::Mat& DistCoeffs () { return (m_distCoeffs); };

        /// \brief Gets the camera's calculated distortion coefficients (read-only). 
        /// \return a constant reference to the array of distortion coefficients
        inline cv::Mat const& DistCoeffs () const { return (m_distCoeffs); };

        /// \brief Gets the current PTP syncrhonisation status (read only)
        /// \return a constant string reference to the PTP synchronisation status. The string contains:
        ///             - "Off" if synchronisation process has not been started.
        ///             - "Master" if synchronisation is achieved and the camera is the master camera.
        ///             - "Slave" if synchronisation is achieved and the camera is a slave camera.
        ///             - "Syncing" if synchronisation has not yet been achieved.
        ///             - "Error" if synchronisation was previously achieved and has been lost.
        inline std::string const& GetPtpStatus () const { return m_ptpStatus; }
        
        ///// \brief Sets the current PTP synchronisation status.
        ///// \param [in] iPtpStatus  the new PTP synchronisation status.
        //inline void SetPtpStatus ( std::string const& iPtpStatus ) { m_ptpStatus = iPtpStatus; }
    
        ///// \brief Sets the camera's sync flag to a given value.
        ///// \param [in] iSynced     the new sync flag
        //inline void SetSynced ( bool const& iSynced ) { m_isSynced = iSynced; }
        
        /// \brief Gets the camera's sync flag (read-only).
        /// \return true if PTP synchronisation is achieved, false otherwise
        inline bool volatile const& IsSynced () const { return m_isSynced; }

        /// \brief Gets the camera's acquiring state (read-only).
        /// \return true if frame acquisition stream is running, false otherwise
        inline bool const& IsAcquiring () { return m_isAcquiring; }
    
        /// \brief Gets the camera's frame dimensions (read-only).
        /// \return a cv::Size structure containing the fame's dimensions
        inline cv::Size const& GetFrameSize () { return m_frameSize; }

    private:
        //void Release ();
        //void DoCopy ( PcCamera const& iOther );

        /// \brief Private copy constructor
        ///
        /// Disables copy of PcCamera objects.
        PcCamera ( PcCamera const& iOther ) {}

        /// \brief Private assignment operator.
        ///
        /// Disables assignment of PcCamera objects.
        /// \return this object, unchanged
        PcCamera& operator= ( PcCamera const& iOther ) { return (*this); }

    private:
        /// \brief Tries to set a camera feature to a given value.
        /// \param [in]     iFeatureName        the name of the camera feature to be set
        /// \param [in] 	iFeatureValue       the value to set the feature to
        /// \param [in] 	iVerbose            tells the method whether or not to be verbose about the process. Defaults to true
        /// \return true upon success, false otherwise
        template<typename T>
        bool TrySetFeature (
            std::string const&                  iFeatureName,
            T const&                            iFeatureValue,
            bool const&                         iVerbose=true
        );
        /// \brief Tries to get the value of a camera feature.
        /// \param [in]     iFeatureName        the name of the camera feature to be read
        /// \param [out]    oFeatureValue       the value of the feature that was read from the camera
        /// \param [in]     iVerbose            tells the method whether or not to be verbose about the process. Defaults to true
        /// \return true upon success, false otherwise
        template<typename T>
        bool TryGetFeature (
            std::string const&                  iFeatureName,
            T&                                  oFeatureValue,
            bool const&                         iVerbose=true
        );
        /// \brief Tries to run a command feature on the camera.
        /// \param [in]     iFeatureName        the name of the command feature to be run
        /// \param [in]     iVerbose            tells the method whether or not to be verbose about the process. Defaults to true
        /// \return true upon success, false otherwise
        bool TryRunFeature (
            std::string const&                  iFeatureName,
            bool const&                         iVerbose=true
        );

        /// \brief Tries to register a feature observer to detect a change in a given feature's value.
        /// 
        /// Registers a VmbAPI::IFeatureObserverPtr to the underlying camera object, to be called upon
        /// a change in a given feature's name.
        /// 
        /// \param [in]     iFeatureName        the name of the feature to be monitored
        /// \param [in]     iObserver       	the feature observer to register
        /// \param [in]     iVerbose            tells the method whether or not to be verbose about the process. Defaults to true
        ///
        /// \return true upon success, false otherwise
        bool TryRegisterObserver (
            std::string const&                  iFeatureName,
            VmbAPI::IFeatureObserverPtr const&  iObserver,
            bool const&                         iVerbose=true
        );

    public:
        /// \brief  Registers a free function as a callback for the calibration process.
        ///
        /// Exposes the RegisterCallback method of the PcCameraCalibration instance of the camera.
        ///
        /// \param [in] iFunc   a pointer to a function that receives a PcCamera pointer and two CalibrationStates.
        inline void RegisterCallback ( void (*iFunc)(pcc::PcCamera*, pcc::CalibrationState, pcc::CalibrationState) )
        {
            m_calibration->RegisterCallback ( iFunc );
        }

        /// \brief  Registers a member function as a callback for the calibration process.
        ///
        /// Exposes the RegisterCallback method of the PcCameraCalibration instance of the camera.
        ///
        /// \param [in] iObj    a pointer to an instance of the class containing iFunc
        /// \param [in] iFunc   a pointer to a function that receives a PcCamera pointer and two CalibrationStates.
        template<typename T>
        inline void RegisterCallback ( T* iObj, void (T::*iFunc)(pcc::PcCamera*, pcc::CalibrationState, pcc::CalibrationState) )
        {
            m_calibration->RegisterCallback ( iObj, iFunc );
        }

    private:
        bool                                            m_isSetup;          ///< Indicated if the camera has been set up.
        bool                                            m_isAcquiring;      ///< Indicates whether or not the camera is aquiring.
        volatile bool                                   m_isSynced;         ///< PTP synchronisation status indication flag.

        VmbAPI::CameraPtr                               m_camera;           ///< The VmbAPI::CameraPtr to the underlying camera.
        std::string                                     m_cameraId;         ///< The camera's GUID.
        std::string                                     m_ptpStatus;        ///< The PTP synchronisation status.

        cv::Size                                        m_frameSize;        ///< The frame dimensions obtained from the camera.

        cv::Mat                                         m_cameraMatrix;     ///< The intrinsic camera matrix obtained from the calibration process.
        cv::Mat                                         m_distCoeffs;       ///< The distortion coefficients obtained from the calibration process.
        
        unsigned int                                    m_frameCount;       ///< How many frames have been acquired since the beginning of the calibration process.
        unsigned int                                    m_lastFrameCount;   ///< The value of the frame counter when the last valid calibration frame was added to the calibration frame queue.
        PcCameraCalibration*                            m_calibration;      ///< The calibrator instance for the camera.
    };
    
    typedef boost::shared_ptr<PcCamera> PcCameraPtr;    ///< Defines a reference-counted pointer type to be used as pointers to PcCamera objects.
}

#endif // PCCAMERA_H
