#include "PcCameraCalibration.h"

#include "PcCommon.h"

#include "PcCamera.h"
#include "PcCalibrationHelper.h"
#include "PcSystem.h"

// ----------------------------------------------------------------------
// PcCameraCalibration
// ----------------------------------------------------------------------
// Public
PcCameraCalibration::PcCameraCalibration ( PcCamera* iParent )
    :   m_mutex ()
    ,   m_stateMutex ()
    ,   m_calibState ( UNKNOWN )
    ,   m_calibThread ( (boost::thread*) 0x0 )
    ,   m_frameQueue ()
    ,   m_frameList ()
    ,   m_corners ()
    ,   m_camera ( iParent )
    ,   m_count ( 0u )
    ,   m_fuckupCount ( 0u )
{}
PcCameraCalibration::~PcCameraCalibration ()
{
    UpgradeLockType upgradedLock ( m_mutex );
    UniqueLockType lock ( upgradedLock );

    if ( m_calibThread ) {
        m_calibThread->interrupt ();
    }
    PCC_OBJ_FREE ( m_calibThread );
}
void PcCameraCalibration::StartCalibration ()
{
    UpgradeLockType upgradedLock ( m_mutex );
    UniqueLockType lock ( upgradedLock );

    DoStartCalibration ();
}
void PcCameraCalibration::AbortCalibration ()
{
    if ( m_calibState == CALIBRATING || m_calibState == ACQUIRING ) {
        DoAbortCalibration ();
    }
}
void PcCameraCalibration::Process ()
{
    PcSystem& cs = PcSystem::GetInstance ();
    PcCalibrationHelper& calib = PcCalibrationHelper::GetInstance ();

    cv::Size const& size = calib.GetChessboardSize ();
    m_count = 0;
    do {
        boost::this_thread::sleep_for ( boost::chrono::milliseconds ( calib.FrameDelay () ) );
        //boost::this_thread::interruption_point ();

        cv::Mat frame;
        {
            UpgradeLockType upgradedLock ( m_mutex );
            UniqueLockType lock ( upgradedLock );

            if ( !m_frameQueue.empty () ) {
                frame = m_frameQueue.front ();
                m_frameQueue.pop ();
            } else {
                continue;
            }
        }

        VEC(cv::Point2f) corners;
        if ( cv::findChessboardCorners ( frame, size, corners, cv::CALIB_CB_FAST_CHECK ) ) {
            m_frameList.push_back ( frame.clone () );
            m_corners.push_back ( corners );

            std::stringstream sFrame;
            sFrame << ".\\Calibration\\" << m_camera->GetID () <<  "\\";

            sFrame << m_count++ << ".png";
            cv::drawChessboardCorners ( frame, size, corners, true );
            cv::imwrite ( sFrame.str (), frame );

            if ( calib.FrameCount () == m_count ) {
                SetCalibrationState ( CALIBRATING );
            }
        }
    } while ( m_calibState == ACQUIRING );

    m_camera->DoCalibration ( calib.GetChessboardPoints (), m_corners, m_camera->GetFrameSize () );
    {
        UpgradeLockType upgradedLock ( m_mutex );
        UniqueLockType lock ( upgradedLock );

        SetCalibrationState ( CALIBRATED );
        m_listeners.clear ();

        std::cout   << m_camera->CameraMatrix ()    << std::endl 
                    << m_camera->DistCoeffs ()      << std::endl;
    }
}
void PcCameraCalibration::PushFrame ( PcFramePtr const& iFrame )
{
    UpgradeLockType upgradedLock ( m_mutex );
    UniqueLockType lock ( upgradedLock );

    m_frameQueue.push ( iFrame->GetImagePoints ().clone () );
}
CalibrationState PcCameraCalibration::GetCalibrationState ()
{
    //boost::shared_lock<boost::shared_mutex> lock ( m_stateMutex );

    return m_calibState;
}
double PcCameraCalibration::GetCalibrationProgress () const
{
    return ( (double)m_frameList.size () / (double)PcCalibrationHelper::GetInstance ().FrameCount () );
}

// Private
void PcCameraCalibration::DoStartCalibration ()
{
    if ( m_calibState == CALIBRATING || m_calibState == ACQUIRING ) {
        DoAbortCalibration ();
    }

    SetCalibrationState ( ACQUIRING );
    m_calibThread = new ThreadType ( boost::bind ( &PcCameraCalibration::Process, this ) );
}
void PcCameraCalibration::DoAbortCalibration ()
{
    SetCalibrationState ( UNKNOWN );

    PCC_OBJ_FREE ( m_calibThread );

    QUEUE(cv::Mat) ().swap ( m_frameQueue );
    VEC(cv::Mat)().swap ( m_frameList );
    VECOFVECS(cv::Point2f)().swap ( m_corners );
}
void PcCameraCalibration::SetCalibrationState ( CalibrationState const& iNewState )
{
    CalibrationState oldState;
    {
        //boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
        //boost::upgrade_to_unique_lock<boost::shared_mutex> lock ( upgradedLock );

        oldState = m_calibState;
        m_calibState = iNewState;
    }

    for ( auto listener = m_listeners.begin (); listener != m_listeners.end (); listener++ ) {
        listener->operator()( m_camera, oldState, iNewState );
    }
}
