#include "PCCoreCalibrationHelper.h"

#include "PCCoreCommon.h"
#include "PCCoreCamera.h"
#include "PCCoreSystem.h"

#include <opencv2/highgui/highgui.hpp>

// ----------------------------------------------------------------------
// Chessboard
// ----------------------------------------------------------------------
// Public
Chessboard::Chessboard (
    unsigned int const&     iRows,
    unsigned int const&     iCols,
    float const&            iSquareSize
)   :   m_size (iCols, iRows)
    ,   m_squareSize (iSquareSize)
    ,   m_points ()
{
    for ( int i = 0; i < m_size.height; i++ ) {
        for ( int j = 0; j < m_size.width; j++ ) {
            m_points.push_back ( cv::Point3f ( i * m_squareSize, j * m_squareSize, 0.0f ) );
        }   
    }
}
std::vector< std::vector< cv::Point3f > > Chessboard::CreateInputArray (
    unsigned int        iFrameCount
) {
    std::vector< std::vector< cv::Point3f > > srcArray;

    while ( iFrameCount-- ) {
        srcArray.push_back ( GetPoints () );
    }

    return srcArray;
}

// ----------------------------------------------------------------------
// PCCoreCalibrationHelper
// ----------------------------------------------------------------------
// Private static
Chessboard PCCoreCalibrationHelper::sm_chessboard ( 7u, 10u, 10u );
PCCoreCalibrationHelper* PCCoreCalibrationHelper::sm_pInstance;

// Public static
PCCoreCalibrationHelper& PCCoreCalibrationHelper::GetInstance ()
{
    if ( !sm_pInstance ) {
        sm_pInstance = new PCCoreCalibrationHelper ();
    }
    return (*sm_pInstance);
}
void PCCoreCalibrationHelper::DestroyInstance ()
{
    PCC_OBJ_FREE ( sm_pInstance );
}

// Public
PCCoreCalibrationHelper::~PCCoreCalibrationHelper ()
{}
std::vector< std::vector< cv::Point3f > > PCCoreCalibrationHelper::GetChessboardPoints ()
{
    return sm_chessboard.CreateInputArray ( m_frameCount );
}

// Private
PCCoreCalibrationHelper::PCCoreCalibrationHelper ()
    :   m_frameDelay ( 1000 )
    ,   m_frameCount ( 15 )
{}


// ----------------------------------------------------------------------
// PCCoreCameraCalibration
// ----------------------------------------------------------------------
// Public
PCCoreCameraCalibration::PCCoreCameraCalibration ( PCCoreCamera* iParent )
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
PCCoreCameraCalibration::~PCCoreCameraCalibration ()
{
    boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
    boost::upgrade_to_unique_lock<boost::shared_mutex> lock ( upgradedLock );

    if ( m_calibThread ) {
        m_calibThread->interrupt ();
    }
    PCC_OBJ_FREE ( m_calibThread );
}
void PCCoreCameraCalibration::StartCalibration ()
{
    boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
    boost::upgrade_to_unique_lock<boost::shared_mutex> lock ( upgradedLock );

    DoStartCalibration ();
}
void PCCoreCameraCalibration::AbortCalibration ()
{
    if ( m_calibState == CALIBRATING || m_calibState == ACQUIRING ) {
        DoAbortCalibration ();
    }
}
void PCCoreCameraCalibration::Process ()
{
    PCCoreSystem& cs = PCCoreSystem::GetInstance ();
    PCCoreCalibrationHelper& calib = PCCoreCalibrationHelper::GetInstance ();

    cv::Size const& size = calib.GetChessboardSize ();
    m_count = 0;
    do {
        boost::this_thread::sleep_for ( boost::chrono::milliseconds ( calib.FrameDelay () ) );
        //boost::this_thread::interruption_point ();

        cv::Mat frame;
        {
            boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
            boost::upgrade_to_unique_lock<boost::shared_mutex> lock ( upgradedLock );

            if ( !m_frameQueue.empty () ) {
                frame = m_frameQueue.front ();
                m_frameQueue.pop ();
            } else {
                continue;
            }
        }

        std::vector<cv::Point2f> corners;
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
        boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
        boost::upgrade_to_unique_lock<boost::shared_mutex> lock ( upgradedLock );

        SetCalibrationState ( CALIBRATED );
        m_listeners.clear ();

        std::cout   << m_camera->CameraMatrix ()    << std::endl 
                    << m_camera->DistCoeffs ()      << std::endl;
    }
}
void PCCoreCameraCalibration::PushFrame ( PCCoreFramePtr const& iFrame )
{
    boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
    boost::upgrade_to_unique_lock<boost::shared_mutex> lock ( upgradedLock );

    m_frameQueue.push ( iFrame->GetImagePoints ().clone () );
}
CalibrationState PCCoreCameraCalibration::GetCalibrationState ()
{
    //boost::shared_lock<boost::shared_mutex> lock ( m_stateMutex );

    return m_calibState;
}
double PCCoreCameraCalibration::GetCalibrationProgress () const
{
    return ( (double)m_frameList.size () / (double)PCCoreCalibrationHelper::GetInstance ().FrameCount () );
}

// Private
void PCCoreCameraCalibration::DoStartCalibration ()
{
    if ( m_calibState == CALIBRATING || m_calibState == ACQUIRING ) {
        DoAbortCalibration ();
    }

    SetCalibrationState ( ACQUIRING );
    m_calibThread = new boost::thread ( boost::bind ( &PCCoreCameraCalibration::Process, this ) );
}
void PCCoreCameraCalibration::DoAbortCalibration ()
{
    SetCalibrationState ( UNKNOWN );

    PCC_OBJ_FREE ( m_calibThread );

    std::queue<cv::Mat> ().swap ( m_frameQueue );
    std::vector<cv::Mat> ().swap ( m_frameList );
    std::vector< std::vector<cv::Point2f> >().swap ( m_corners );
}
void PCCoreCameraCalibration::SetCalibrationState ( CalibrationState const& iNewState )
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
