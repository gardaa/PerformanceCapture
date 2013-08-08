#include "PCCoreCalibrationHelper.h"

#include "PCCoreCommon.h"

#include <opencv2/highgui/highgui.hpp>

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
std::vector< std::vector< cv::Point3f > > CreateInputArray (
    Chessboard const&   iChessboard,
    unsigned int        iFrameCount
) {
    std::vector< std::vector< cv::Point3f > > srcArray;

    while ( iFrameCount-- ) {
        srcArray.push_back ( iChessboard.GetPoints () );
    }

    return srcArray;
}

Chessboard PCCoreCalibrationHelper::sm_chessboard ( 7u, 10u, 10u );
PCCoreCalibrationHelper* PCCoreCalibrationHelper::sm_pInstance = (PCCoreCalibrationHelper*)0x0;

PCCoreCalibrationHelper::PCCoreCalibrationHelper ()
    :   m_frameDelay ( 150 )
    ,   m_frameCount ( 15 )
    ,   m_calibratedCameras ( 0 )
    ,   m_state ( UNKNOWN )
    ,   m_cameras ()
    ,   m_frames ()
    ,   m_chessboardCorners ()
{
    Reset ();
}

PCCoreCalibrationHelper::~PCCoreCalibrationHelper ()
{}

PCCoreCalibrationHelper& PCCoreCalibrationHelper::GetInstance () {
    if ( sm_pInstance == (PCCoreCalibrationHelper*)0x0 ) {
        sm_pInstance = new PCCoreCalibrationHelper ();
    }
    return (*sm_pInstance);
}
void PCCoreCalibrationHelper::DestroyInstance () {
    PCC_OBJ_FREE ( sm_pInstance );
}

void PCCoreCalibrationHelper::StartCalibration ()
{
    // Writer access to global maps.
    boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock ( upgradedLock );

    Reset ();
    m_state = ACQUIRING;

    for ( auto camera = m_cameras.begin (); camera != m_cameras.end (); camera++ ) {
        boost::thread* thread = m_workers.create_thread ( boost::bind ( &PCCoreCalibrationHelper::ProcessCalibration, this, camera->first ) );
        
        //boost::thread* thread = new boost::thread ( boost::bind ( &PCCoreCalibrationHelper::ProcessCalibration, this, camera->first ) );
        //boost::thread::attributes a;
#ifdef WIN32
        SetThreadPriority ( thread->native_handle (), THREAD_PRIORITY_LOWEST );
#endif
        //thread->start_thread_noexcept ( a );

        //m_threads.insert ( std::make_pair ( camera->first, (boost::thread*)NULL ) );
        m_threads.insert ( std::make_pair ( camera->first, thread ) );
    }
}
void PCCoreCalibrationHelper::AbortCalibration ()
{
    // Writer access to global maps.
    boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock ( upgradedLock );

    Reset ();
    m_state = UNKNOWN;
    m_workers.join_all ();
}

void PCCoreCalibrationHelper::Reset ()
{
    m_calibratedCameras = 0;
    auto frames = m_frames.begin (); 
    auto corners = m_chessboardCorners.begin ();

    std::queue < PCCoreFrame > emptyQueue;
    for ( ; frames != m_frames.end (); frames++, corners++ ) {
        frames->second.swap ( emptyQueue );
        corners->second.clear ();
    }
}

bool PCCoreCalibrationHelper::RegisterCamera ( PCCoreCamera const& iCamera )
{
    // Writer access to global maps.
    boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock ( upgradedLock );
    
    std::string const& camId = iCamera.GetID ();
    if ( m_cameras.find ( camId ) == m_cameras.end () ) {
        m_cameras.insert ( std::make_pair ( camId, iCamera ) );

        std::queue< PCCoreFrame > frames;
        m_frames.insert ( std::make_pair ( camId, frames ) );

        std::vector< std::vector< cv::Point2f > > corners;
        m_chessboardCorners.insert ( std::make_pair ( camId, corners ) );

        boost::shared_ptr<boost::mutex> mutex ( new boost::mutex () );
        m_camMutexes.insert ( std::make_pair ( camId, mutex ) );
        
        std::cout << camId << " registered to calibration module." << std::endl;

        return true;
    }
    return false;
}

bool PCCoreCalibrationHelper::UnregisterCamera ( PCCoreCamera const& iCamera )
{
    // Writer access to global maps.
    boost::upgrade_lock<boost::shared_mutex> upgradedLock ( m_mutex );
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock ( upgradedLock );

    std::string const& camId = iCamera.GetID ();
    if ( m_cameras.find ( camId ) != m_cameras.end () ) {
        // Writer access to individual camera lists.
        boost::shared_ptr<boost::mutex> camMutex = m_camMutexes.at ( camId );
        boost::lock_guard<boost::mutex> lock ( *camMutex );
        
        m_cameras.erase ( camId );
        m_frames.erase ( camId );
        m_chessboardCorners.erase ( camId );

        m_camMutexes.erase ( camId );

        m_workers.remove_thread ( m_threads.at ( camId ) );
        m_threads.erase ( camId );

        std::cout << camId << " unregistered from calibration module." << std::endl;

        return true;
    }
    return false;
}

bool PCCoreCalibrationHelper::RegisterFrame ( std::string const& iCameraId, PCCoreFrame const& iFrame )
{
    // Reader lock to global maps.
    boost::shared_lock<boost::shared_mutex> sharedLock ( m_mutex );
    
    // Writer lock to frame queue.
    boost::shared_ptr<boost::mutex> camMutex = m_camMutexes.at ( iCameraId );
    boost::lock_guard<boost::mutex> lock ( *camMutex );

    if ( m_frames.find ( iCameraId ) != m_frames.end () ) {        
        m_frames.at ( iCameraId ).push ( iFrame );

        return true;
    }
    return false;
}

void PCCoreCalibrationHelper::ProcessCalibration (
    std::string     iCamId
) {
    boost::shared_ptr<boost::mutex> camMutex = m_camMutexes.at ( iCamId );
    do {
        // Reader lock to global maps.
        boost::shared_lock<boost::shared_mutex> mapLock ( m_mutex );

        if ( m_state != ACQUIRING ) {
            break;
        }
        
        // Writer lock to individual camera data.
        boost::unique_lock<boost::mutex> camLock ( *camMutex );

        std::queue< PCCoreFrame >& frameQueue = m_frames.at ( iCamId );
        if ( frameQueue.empty () ) {
            continue;
        }
        PCCoreFrame frame = frameQueue.front ();
        frameQueue.pop ();

        mapLock.unlock ();
        camLock.unlock ();
                
        mapLock.lock ();
        std::vector< std::vector < cv::Point2f > >& detectedCorners = m_chessboardCorners.at ( iCamId );
        if ( detectedCorners.size () == m_frameCount ) {
            m_calibratedCameras++;
            if ( m_cameras.size () == m_calibratedCameras ) {
                m_state = CALIBRATING;
                for ( auto camera = m_cameras.begin (); camera != m_cameras.end (); camera++ ) {
                    camera->second.Calibrate (
                        CreateInputArray ( sm_chessboard, m_frameCount ),
                        detectedCorners,
                        cv::Size ( frame.Width (), frame.Height () )
                    );
                }
                m_state = CALIBRATED;
            }
            mapLock.unlock ();
            break;
        }
        mapLock.unlock ();

        std::vector< cv::Point2f > corners;
        bool foundCorners = cv::findChessboardCorners ( frame.GetImagePoints(), sm_chessboard.GetSize (), corners );
        if ( foundCorners ) {
            mapLock.lock ();
            std::cout << iCamId << " Found one!" << std::endl;

            if ( m_cameras.find ( iCamId ) == m_cameras.end () ) {
                return;
            }
            
            //std::stringstream windowName;
            //windowName << iCamId << " - " << detectedCorners.size ();

            //cv::namedWindow ( windowName.str () );
            //cv::drawChessboardCorners ( frame.GetImagePoints (), cv::Size ( ), corners, foundCorners );
            //cv::imshow ( windowName.str (), frame.GetImagePoints () );

            std::vector< std::vector < cv::Point2f > >& detectedCorners = m_chessboardCorners.at ( iCamId );
            if ( detectedCorners.size () < m_frameCount ) {
                detectedCorners.push_back ( corners );
                std::cout << iCamId << ": " << detectedCorners.size () << " / " << m_frameCount << std::endl;
            }
            mapLock.unlock ();
        }
    } while ( 1 );

    std::cout << "Calibrated " << iCamId << std::endl;
    if ( m_state == CALIBRATED ) {
        Reset ();
    }
}