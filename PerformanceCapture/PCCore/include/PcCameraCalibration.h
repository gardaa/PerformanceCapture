#ifndef PCCAMERACALIBRATION_H
#define PCCAMERACALIBRATION_H

#include "PcCommon.h"
#include "PcFrame.h"

#define BOOST_ALL_DYN_LINK
#include <boost/thread.hpp>
#include <boost/function.hpp>

#include <queue>
#include <vector>

namespace pcc
{
    class PcCamera;

    /// \brief Indicates the state of the calibration process.
    enum CalibrationState {
        UNKNOWN     =   0x00,   ///< Unknown state/Uninitialised
        ACQUIRING   =   0x01,   ///< Acquiring calibration frames
        CALIBRATING =   0x02,   ///< Calculating the intrinsic matrices
        CALIBRATED  =   0x03,   ///< Calibration process completed
    };

    /// \ingroup PCCORE
    ///
    /// \brief Performs the calibration process of a given camera.
    ///
    /// Correct camera calibration is an important part of any computer vision application.
    /// It ensures the correct matching between world-space coordinates and image-space coordinates (or projection),
    /// taking into account the position and angle a camera has been set-up, as well as it's internal parameters,
    /// such as focal distance, axis scaling (i.e. the resolution in pixels in both x and y directions in image space),
    /// and camera distortion.
    /// 
    /// This class provides the user with a simple interface to calculate these projection parameters.
    /// Once the calibration process is running, a predefined number of frames will be captured continuously, with a given interval between captures, 
    /// to the purpose of derivating the camera's internal and external parameters. During the calibration process, the user is required to place
    /// a chessboard of known dimensions in front of the cameras, changing it's position and orientation
    /// continuously in time.
    ///
    /// The calibration process is thread-safe and takes place on it's own independent thread.
    ///
    /// Globally, the process works like this:
    ///     - A frame queue is kept where the frame acquisition thread can register
    ///       candidate frames to the calibration
    ///     - The calibration thread periodically checks for a new frame on the queue and tries to find a chessboard
    ///       on it. If it succeeds, it keeps that frame for usage on the actual calibration step, marks the position
    ///       of the detected corners of the chessboard on the image and exports it to a file. If it fails, the frame is
    ///       discarded.
    ///     - Once a predetermined number of frames has been collected, the thread moves to the actual calibration phase,
    ///       during which it will calculate all of the intrinsic parameters for the camera. After this step, the camera
    ///       is considered as calibrated and the thread exits.
    class PcCameraCalibration {
    private:
        typedef boost::thread                                                           ThreadType;         ///< The thread class to be used to manage the calibration thread.
        typedef boost::shared_mutex                                                     MutexType;          ///< The mutex type to be used to lock shared resources
        typedef boost::upgrade_lock<MutexType>                                          UpgradeLockType;    ///< The RAII lock guard that allows to upgrade a shared mutex to unique mutex
        typedef boost::upgrade_to_unique_lock<MutexType>                                UniqueLockType;     ///< The RAII lock guard that actually upgrades a shared mutex to unique
        typedef boost::function3<void, PcCamera*, CalibrationState, CalibrationState>   CallbackFn;         ///< The functor type to represent state-change listener functions

    public:
        /// \brief Constructs a new PcCameraCalibration object.
        ///
        /// Since the existance of a PcCameraCalibration object is strictly tied to that of its parent PcCamera, we
        /// use a raw pointer here to avoid cyclic references of PcCameraPtr.
        /// \param [in] iParent     the PcCamera that owns this object
        PcCameraCalibration ( PcCamera* iParent );

        /// \brief The destructor method.
        ///
        /// Interrupts the calibration thread if there is one and deletes the memory used by the manager object of type ThreadType.
        ~PcCameraCalibration ();

        /// \brief Starts the calibration process.
        /// 
        /// Internally locks the shared resources mutex and calls PcCameraCalibration::DoStartCalibration.
        void StartCalibration ();

        /// \brief Aborts any currently running calibration process.
        /// 
        /// Internally locks the shared resources mutex and calls PcCameraCalibration::DoAbortCalibration.
        void AbortCalibration ();

        /// \brief Processes the calibration frame queue on a separate thread.
        ///
        /// Loops while the calibration process is in the acquisition phase. On each iteration, performs the following actions:
        ///     - Sleeps for a predefined amount of time to avoid using the processor while there aren't frames in the queue
        ///     - Pop the front of the frame queue (if it's not empty)
        ///     - Check if the frame contains a chessboard in it
        ///     - If it does, push the list of coordinates of the corresponding chessboard on the frame image, marks them on the frame and exports it to a png file
        /// 
        /// Once the calibration has the required amount of frames, it passes to the actual calibration phase.
        /// This phase calls the DoCalibration method on the parent PcCamera object, providing the detected corners information and the chessboard descriptors.
        /// Once the calibration method finishes it's job, the thread empties the callback list and outputs
        /// the calculated matrices on the screen. 
        void Process ();

        /// \brief Pushes a frame into the calibration queue.
        /// \param [in] iFrame      the frame to be put on the queue
        void PushFrame ( PcFramePtr const& iFrame );

        /// \brief Reads the current calibration state.
        /// \return a copy of the current calibration state
        CalibrationState GetCalibrationState ();

        /// \brief Gets the progress of the calibration process, in percentage.
        ///
        /// Calculates the ratio between the currently acquired frames and the required amount.
        /// \return a double in the interval [0,1] representing the progress of the calibration process
        double GetCalibrationProgress () const;

        /// \brief Gets the chessboard corners detected during the acquisition phase.
        ///
        /// \return a vector of vectors of points describing the detected chessboard corners
        VECOFVECS(cv::Point2f) const& GetChessboardCorners () const { return m_corners; }

        /// \brief  Registers a free function as a callback for the calibration process.
        ///
        /// Adds a callback function to be called whenever there is a calibration state change. 
        ///
        /// \param [in] iFunc   a pointer to a function that receives a PcCamera pointer and two CalibrationStates.
        inline void RegisterCallback ( void (*iFunc)(PcCamera*, CalibrationState, CalibrationState) )
        {
            m_listeners.push_back ( CallbackFn ( iFunc ) );
        }

        /// \brief  Registers a member function as a callback for the calibration process.
        ///
        /// Adds a member function as a callback to be called whenever there is a calibration state change.
        /// Binds an actual instance to the member function so that we don't actually have to know the container class
        /// of the member function.
        ///
        /// \param [in] iObj    a pointer to an instance of the class containing iFunc
        /// \param [in] iFunc   a pointer to a function that receives a PcCamera pointer and two CalibrationStates.
        template<typename T>
        inline void RegisterCallback ( T* iObj, void (T::*iFunc)(PcCamera*, CalibrationState, CalibrationState) )
        {
            m_listeners.push_back ( CallbackFn ( boost::bind ( iFunc, iObj, _1, _2, _3 ) ) );
        }

    private:
        /// \brief Private copy constructor.
        /// 
        /// Disables copy operations on PcCameraCalibration objects.
        PcCameraCalibration ( PcCameraCalibration const& iOther) {}

        /// \brief Private assignment operator.
        /// 
        /// Disables assignment operations on PcCameraCalibration objects.
        /// \return this object, unchanged
        PcCameraCalibration& operator= ( PcCameraCalibration const& iOther ) { return (*this); }

        /// \brief Performs the calibration startup steps.
        /// 
        /// Aborts any currently running calibration, sets the calibration state to ACQUIRING and launches a new
        /// thread to process the frame queue.
        void DoStartCalibration ();

        /// \brief Performs the calibration abort steps.
        ///
        /// Sets the calibration state to UNKNOWN, deletes the current processing thread, empties the frame queue,
        /// empties the list of frames containing a chessboard and empties the list of detected chessboard corners.
        void DoAbortCalibration ();

        /// \brief Changes the current calibration state.
        ///
        /// Sets the new state to iNewState and notifies all the registered listeners.
        ///
        /// \param [in] iNewState   the target calibration state to achieve
        void SetCalibrationState ( CalibrationState const& iNewState );

    private:
        MutexType                       m_mutex;        ///< The shared mutex used to lock the frame queue.
        MutexType                       m_stateMutex;   ///< The shared mutex used to lock the state variable.
        CalibrationState                m_calibState;   ///< The current calibration state.
        ThreadType*                     m_calibThread;  ///< The thread that processes the frame queue.
        QUEUE(cv::Mat)                  m_frameQueue;   ///< The queue of frames to be processed.
        VEC(cv::Mat)                    m_frameList;    ///< The list of frames where a chessboard has been found.
        VECOFVECS(cv::Point2f)          m_corners;      ///< The list of coordinates of the chessboard corners detected on each frame.

        PcCamera*                       m_camera;       ///< The parent PcCamera

        unsigned int                    m_count;        ///< The frame counter used to build the filenames of outputted frames.
        VEC(CallbackFn)                 m_listeners;    ///< The list of listeners to be notified of a state change.
    };
}

#endif // PCCAMERACALIBRATION_H
