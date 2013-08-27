#include "PCCoreSystem.h"

#include "PCCoreFrame.h"
#include "PCCoreCamera.h"
#include "PCCoreFrameObserver.h"
#include "PCCoreErrChk.h"
#include "CalibrationHelper.h"

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>

#include <opencv2/gpu/gpu.hpp>
#include <opencv2/gpu/gpumat.hpp>

using namespace AVT::VmbAPI;

// Total available bandwidth in bytes
static unsigned int const MAX_BW = 124000000;

ICameraListObserverPtr PCCoreSystem::sm_pInstance ( (PCCoreSystem*)0x0 );
int PCCoreSystem::sm_nAvailableCameras = 0;

PCCoreSystem::PCCoreSystem()
    :   ICameraListObserver ()
    ,   m_activeCameras ()
    ,   m_mutex ( new boost::mutex () )
    ,   m_frames ()
    ,   m_stereo ()
{}

void PCCoreSystem::Setup ()
{
    VmbErrorType err;
    VimbaSystem& vmbs = VimbaSystem::GetInstance ();    

    CameraPtrVector cameras;
    err = vmbs.GetCameras ( cameras );
    ERR_CHK ( err, VmbErrorSuccess, "Error getting camera list." );

    //VmbInt32_t const maxCameraBandwidth = GetMaxPerCameraBandwidth ();
    for ( auto cam = cameras.begin (); cam != cameras.end (); cam++ ) {
        //std::string sCamId;
        //(*cam)->GetID ( sCamId );
        //RegisterCamera ( sCamId, *cam );
        m_addQueue.push ( *cam );

        //PCCoreCamera camera ( *cam, MAX_BW / cameras.size () );
        //PCCoreFrame frame;
        //std::string sCamId;
        //(*cam)->GetID ( sCamId );
        //RegisterCamera ( sCamId, *cam );
        //m_cameras.insert ( std::make_pair ( camera.GetID (), camera ) );
        //m_frames.insert ( std::make_pair ( camera.GetID (), frame ) );
        //CameraPtr& camera = (*cam);

        //VmbInt64_t payload;
        //FeaturePtr f;
        //err = camera->GetFeatureByName ( "PayloadSize", f );
        //err = f->GetValue ( payload );
        //std::cout << "Camera Payload " << payload << std::endl;


        //err = camera->Open ( VmbAccessModeFull );
        //
        //err = camera->GetFeatureByName ( "PixelFormat", f );
        //err = f->SetValue ( VmbPixelFormatMono8 );

        //err = camera->GetFeatureByName ( "StreamBytesPerSecond", f );
        //err = f->SetValue ( maxCameraBandwidth );

        //err = camera->GetFeatureByName ( "GVSPPacketSize", f );
        //err = f->SetValue ( 1500 );

        //err = camera->GetFeatureByName ( "Exposure", f );
        //err = f->SetValue ( 15000 );

        //err = camera->GetFeatureByName ( "PtpMode", f );
        //err = f->SetValue ( "Auto" );
        ////err = f->SetValue ( "Off" );
        ////if ( !i ) {
        ////    err = f->SetValue ( "Master" );
        ////} else {
        ////    err = f->SetValue ( "Slave" );
        ////}

        //err = camera->GetFeatureByName ( "GainAuto", f );
        //err = f->SetValue ( "Continuous" );

        //FramePtrVector frameList;
        //frameList.resize ( 3 );

        //IFrameObserverPtr observer ( new PCCoreFrameObserver ( camera ) );
        //for ( unsigned int j = 0; j < 3; j++ ) {
        //    FramePtr newFrame ( new Frame ( payload ) );
        //    
        //    err = newFrame->RegisterObserver ( observer );

        //    err = camera->AnnounceFrame ( newFrame );

        //    frameList[j] = newFrame;
        //}
    }
    std::cout << cameras.size () << " cameras found" << std::endl;
    std::cout << "Maximum per-camera bandwidth: " << GetMaxPerCameraBandwidth () << std::endl;
}

PCCoreSystem::~PCCoreSystem ()
{
    PCC_OBJ_FREE ( m_mutex );

    //pcc::CalibrationHelper& calib = pcc::CalibrationHelper::GetInstance ();
    //if ( calib.CurrentState () == pcc::CalibrationHelper::ACQUIRING || 
    //    calib.CurrentState () == pcc::CalibrationHelper::CALIBRATING ) {
    //    calib.AbortCalibration ();
    //}
    for ( auto camera = m_activeCameras.begin (); camera != m_activeCameras.end (); camera++ ) {
        camera->second->AbortCalibration ();
    }
}

PCCoreSystem& PCCoreSystem::GetInstance ()
{
    PCCoreSystem* instance = dynamic_cast<PCCoreSystem*>(sm_pInstance.get ());

    if ( instance == (PCCoreSystem*)0x0 ) {
        instance = new PCCoreSystem ();
        sm_pInstance.reset ( instance );
        
        VimbaSystem& vmbs = VimbaSystem::GetInstance ();
        vmbs.RegisterCameraListObserver ( sm_pInstance );
        instance->Setup ();
    }
    return (*instance);
}
void PCCoreSystem::DestroyInstance ()
{
    sm_pInstance.reset ( (PCCoreSystem*)0x0 );
}

void PCCoreSystem::StartCapture ()
{
    //VmbErrorType err;
    //for ( unsigned int i = 0; i < m_cameras.size (); i++ ) {
    //    FeaturePtr acquisitionStartCmd;
    //    FeaturePtr acquisitionMode;

    //    CameraPtr camera = m_cameras[i];
    //    err = camera->StartCapture ();
    //    ERR_CHK ( err, VmbErrorSuccess, "Error starting capture" );

    //    for ( FramePtrVector::iterator iter = m_camFrames[i].begin ();
    //        m_camFrames[i].end() != iter;
    //        ++iter )
    //        {
    //        err = camera-> QueueFrame( *iter );
    //        ERR_CHK ( err, VmbErrorSuccess, "Error queueing frame" );
    //    }

    //    //err = camera->GetFeatureByName( "AcquisitionMode", acquisitionMode );
    //    //err = acquisitionMode->SetValue ( "Continuous" );
    //    //ERR_CHK ( err, VmbErrorSuccess, "Error setting camera acquisition mode" );

    //    err = camera->GetFeatureByName( "AcquisitionStart", acquisitionStartCmd );
    //    err = acquisitionStartCmd->RunCommand ();
    //    ERR_CHK ( err, VmbErrorSuccess, "Error starting acquisition" );
    //}
    boost::lock_guard<boost::mutex> lock (*m_mutex);

    for ( auto camera = m_activeCameras.begin (); camera != m_activeCameras.end (); camera++ ) {
        if ( !camera->second->IsAcquiring () ) {
            camera->second->StartAcquisition ();
        }
    }
}

void PCCoreSystem::EndCapture ()
{
    //VmbErrorType err;
    //for ( unsigned int i = 0; i < m_cameras.size (); i++ ) {
    //    FeaturePtr acquisitionEndCmd;

    //    CameraPtr camera = m_cameras[i];

    //    //err = camera->FlushQueue ();

    //    err = camera->EndCapture ();

    //    //err = camera->GetFeatureByName( "AcquisitionEnd", acquisitionEndCmd );
    //    //err = acquisitionEndCmd->RunCommand ();
    //}
    boost::lock_guard<boost::mutex> lock (*m_mutex);
    
    for ( auto camera = m_activeCameras.begin (); camera != m_activeCameras.end (); camera++ ) {
        if ( camera->second->IsAcquiring () ) {
            camera->second->StopAcquisition ();
        }
    }
}

unsigned int PCCoreSystem::GetNumFrames ()
{
    return m_frames.size ();
}
PCCoreFramePtr const PCCoreSystem::GetFrameFromCamera ( std::string const& iCameraId )
{
    return m_frames.at ( iCameraId );
}
std::string PCCoreSystem::GetCameraStatus ( std::string const& iCameraId )
{
    return m_activeCameras.at ( iCameraId )->GetPtpStatus ();
}

double PCCoreSystem::GetCameraCalibrationProgress ( std::string const& iCameraId )
{
    return m_activeCameras.at ( iCameraId )->GetCalibrationProgress ();
}

void PCCoreSystem::SetFrame ( CameraPtr const& iCamera, FramePtr const& iFrame )
{
    VmbErrorType err;

    VmbUint32_t height;
    err = iFrame->GetHeight (height);
    ERR_CHK ( err, VmbErrorSuccess, "Error reading frame height." );

    VmbUint32_t width;
    err = iFrame->GetWidth (width);
    ERR_CHK ( err, VmbErrorSuccess, "Error reading frame width." );

    VmbUchar_t const* frameData;
    err = iFrame->GetImage(frameData);
    ERR_CHK ( err, VmbErrorSuccess, "Error reading frame image data." );

    std::string sCamId;
    iCamera->GetID ( sCamId );
    
    m_frames[sCamId]->Reset ( width, height, 1, frameData );
    
    //pcc::CalibrationHelper& calib = pcc::CalibrationHelper::GetInstance ();
    if ( m_activeCameras.at ( sCamId )->GetCalibrationState () == pcc::ACQUIRING ) {
        m_activeCameras.at ( sCamId )->TryPushFrame ( m_frames.at ( sCamId ) );
    }
    //memcpy ( m_data[uiCamId].data, frameData, width * height * sizeof ( unsigned char ) );

    //cv::Mat inImg ( height, width, CV_8UC1, frameData );
    //cv::medianBlur ( inImg, m_data[iCamId], 3 );

    //static cv::gpu::GpuMat I1, I2;
    //static cv::Mat kernel;
    //static bool hasKernel = false;
    //if ( !hasKernel ) {
    //    const int SIZE = 5;
    //    kernel = cv::getStructuringElement ( cv::MORPH_ELLIPSE, cv::Size ( 2*SIZE+1, 2*SIZE+1 ), cv::Point(SIZE,SIZE) );
    //}

    //I1.upload ( inImg );
    //cv::gpu::erode ( I1, I2, kernel );
    //cv::gpu::dilate ( I2, I1, kernel );
    //I2.download ( m_data[iCamId] );
    
    //cv::gpu::bilateralFilter ( I1, I2, 21, 42, 17 );
    //cv::gpu::GaussianBlur ( I1, I2, cv::Size (-1,-1), 2.5 );
    //I2.download ( m_data[iCamId] );
}

void PCCoreSystem::UnregisterCamera ( std::string const& iCameraId )
{
    if ( m_activeCameras.find ( iCameraId ) == m_activeCameras.end () ) {
        return;
    }
    PCCoreCameraPtr camera = m_activeCameras.at ( iCameraId );
    camera->StopAcquisition ();

    m_activeCameras.erase ( iCameraId );
    m_frames.erase ( iCameraId );
    for ( auto cam = m_activeCameras.begin (); cam != m_activeCameras.end (); cam++ ) {
        cam->second->AdjustBandwidth ( GetMaxPerCameraBandwidth () );
    }
}
void PCCoreSystem::RegisterCamera ( std::string const& iCameraId, CameraPtr const& iCamera )
{
    if ( m_activeCameras.find ( iCameraId ) != m_activeCameras.end () ) {
        return;
    }
    auto newCam = std::make_pair ( iCameraId, PCCoreCameraPtr ( new PCCoreCamera ( iCamera ) ) );
    auto lastCam = m_activeCameras.rbegin ();

    m_activeCameras.insert ( newCam );
    m_frames.insert ( std::make_pair ( iCameraId, PCCoreFramePtr ( new PCCoreFrame () ) ) );

    if ( m_activeCameras.size () && !(m_activeCameras.size () % 2) ) {
        m_stereo.push_back ( PCCoreStereoCameraPairPtr ( new PCCoreStereoCameraPair ( lastCam->second, newCam.second ) ) );
    }

    newCam.second->Setup ();
    for ( auto cam = m_activeCameras.begin (); cam != m_activeCameras.end (); cam++ ) {
        cam->second->AdjustBandwidth ( GetMaxPerCameraBandwidth () );
    }
    newCam.second->StartAcquisition ();
}

void PCCoreSystem::CameraListChanged ( CameraPtr iCamera, UpdateTriggerType iUpdateReason )
{
    boost::lock_guard<boost::mutex> lock (*m_mutex);

    VmbErrorType err;
    std::string sCamId;
    err = iCamera->GetID ( sCamId );
    
    std::cout << "Updated camera list." << std::endl;
    if ( iUpdateReason == UpdateTriggerPluggedOut ) {
        std::cout << "Camera unplugged: " << sCamId << std::endl;
        //m_removeQueue.push ( sCamId );
    } else if ( iUpdateReason == UpdateTriggerPluggedIn ) {
        std::cout << "Camera plugged: " << sCamId << std::endl;
        //m_addQueue.push ( iCamera );
    } else if ( iUpdateReason == UpdateTriggerOpenStateChanged ) {
        //m_removeQueue.push ( sCamId );
        //m_addQueue.push ( iCamera );

        std::cout << "Camera changed open state: " << sCamId << std::endl;
    }
}

int PCCoreSystem::GetMaxPerCameraBandwidth ()
{
    if ( !m_activeCameras.empty () ) {
        return MAX_BW / m_activeCameras.size ();
    } else {
        return MAX_BW;
    }
}

std::vector<std::string> PCCoreSystem::GetCameraList ()
{
    std::vector<std::string> list;

    for ( auto elem = m_activeCameras.begin (); elem != m_activeCameras.end (); elem++ ) {
        list.push_back ( elem->first );
    }

    return list;
}

void PCCoreSystem::UpdateCameras ()
{
    boost::lock_guard<boost::mutex> lock (*m_mutex);

    bool hasChanged = false;
    while ( !m_removeQueue.empty () ) {
        UnregisterCamera ( m_removeQueue.front () );

        m_removeQueue.pop ();

        hasChanged = true;
    }
    while ( !m_addQueue.empty () ) {
        CameraPtr cam = m_addQueue.front ();

        std::string sCamId;
        cam->GetID ( sCamId );
        RegisterCamera ( sCamId, cam );

        m_addQueue.pop ();

        hasChanged = true;
    }

    /*static bool printed = true;
    pcc::CalibrationHelper& calib = pcc::CalibrationHelper::GetInstance();
    if ( calib.CurrentState() == pcc::CalibrationHelper::ACQUIRING ) {
        m_frameCount++;
        if ( m_frameCount >= m_lastCalibrationFrame + calib.FrameDelay () ) {
            for ( auto frame = m_frames.begin (); frame != m_frames.end (); frame++ ) {
                calib.RegisterFrame ( frame->first, frame->second );
            }
            m_lastCalibrationFrame = m_frameCount;
        }
        printed = false;
    } else if ( !printed && calib.CurrentState() == pcc::CalibrationHelper::CALIBRATED ) {
        for ( auto camera = m_activeCameras.begin (); camera != m_activeCameras.end (); camera++ ) {
            std::cout << camera->first << ": " << std::endl;
            std::cout << camera->second.CameraMatrix () << std::endl;
            std::cout << camera->second.DistCoeffs () << std::endl;
        }
        printed = true;
    }*/

    if ( hasChanged ) {
        //SynchroniseCameras ();
    }
}

void PCCoreSystem::SynchroniseCameras ()
{
    for ( auto cam = m_activeCameras.begin (); cam != m_activeCameras.end (); cam++ ) {
        cam->second->StopAcquisition ();
    }

    boost::thread_group syncThreads;
    for ( auto cam = m_activeCameras.begin (); cam != m_activeCameras.end (); cam++ ) {
        syncThreads.create_thread ( boost::bind ( &PCCoreCamera::Synchronise, &(*(cam->second)) ) );
    }
    syncThreads.join_all ();
        
    for ( auto cam = m_activeCameras.begin (); cam != m_activeCameras.end (); cam++ ) {
        cam->second->StartAcquisition ();
    }
}

void PCCoreSystem::CalibrateCameras ()
{
    //pcc::CalibrationHelper& calib = pcc::CalibrationHelper::GetInstance ();
    //calib.StartCalibration ();
    for ( auto camera = m_activeCameras.begin (); camera != m_activeCameras.end (); camera++ ) {
        camera->second->StartCalibration ();
    }
}