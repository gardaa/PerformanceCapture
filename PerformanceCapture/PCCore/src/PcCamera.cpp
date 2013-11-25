#include "PcCamera.h"

#include "PcPtpSyncAgent.h"
#include "PcCommon.h"
#include "PcSystem.h"
#include "PcFrame.h"
#include "PcFrameObserver.h"
#include "PcCalibrationHelper.h"

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>

#include <iostream>

using namespace pcc;

PcCamera::PcCamera (
    VmbAPI::CameraPtr const&        iCamera
)   :   m_isSetup ( false )
    ,   m_isAcquiring ( false )
    ,   m_isSynced ( false )
    ,   m_camera ( iCamera )
    ,   m_cameraId ()
    ,   m_ptpStatus ()
    ,   m_frameSize ()
    ,   m_cameraMatrix ( 3, 3, CV_64F )
    ,   m_distCoeffs ( 8, 1, CV_64F )
    ,   m_frameCount ( 0u )
    ,   m_lastFrameCount ( 0u )
    ,   m_calibration ( (PcCameraCalibration*)0x0 )
{
    m_calibration = new PcCameraCalibration ( this );
}

//PcCamera::~PcCamera ()
//{
//    //Release ();
//}

//void PcCamera::DoCopy ( PcCamera const& iOther )
//{
//    GuardType lock ( *iOther.m_mutex );
//    
//    ++(*iOther.m_refCount);
//    m_mutex = iOther.m_mutex;
//    m_refCount = iOther.m_refCount;
//    m_isSetup = iOther.m_isSetup;
//    m_isAcquiring= iOther.m_isAcquiring;
//    m_isSynced = iOther.m_isSynced;
//    m_camera = iOther.m_camera;
//    m_cameraId = iOther.m_cameraId;
//    m_ptpStatus = iOther.m_ptpStatus;
//    m_frameSize = iOther.m_frameSize;
//    m_cameraMatrix = iOther.m_cameraMatrix;
//    m_distCoeffs = iOther.m_distCoeffs;
//    m_frameCount = iOther.m_frameCount;
//    m_lastFrameCount = iOther.m_lastFrameCount;
//    m_calibration = iOther.m_calibration;
//}
//
//PcCamera::PcCamera ( PcCamera const& iOther )
//{
//    DoCopy ( iOther );
//}
//
//PcCamera& PcCamera::operator= ( PcCamera const& iOther )
//{
//    if ( this != &iOther ) {
//        Release ();
//        DoCopy ( iOther );
//    }
//    return (*this);
//}
//
//void PcCamera::Release ()
//{
//    if ( m_refCount && --(*m_refCount) == 0u ) {
//        PCC_OBJ_FREE ( m_refCount );
//        PCC_OBJ_FREE ( m_isSetup );
//        PCC_OBJ_FREE ( m_isAcquiring );
//        PCC_OBJ_FREE ( m_isSynced );
//        m_camera->Close ();
//        PCC_OBJ_FREE ( m_cameraId );
//        PCC_OBJ_FREE ( m_ptpStatus );
//        PCC_OBJ_FREE ( m_frameSize );
//        PCC_OBJ_FREE ( m_cameraMatrix );
//        PCC_OBJ_FREE ( m_distCoeffs );
//        PCC_OBJ_FREE ( m_frameCount );
//        PCC_OBJ_FREE ( m_lastFrameCount );
//        PCC_OBJ_FREE ( m_calibration );
//        PCC_OBJ_FREE ( m_calibration );
//    }
//}

template<typename T>
inline bool PcCamera::TrySetFeature (
    std::string const&  iFeatureName,
    T const&            iFeatureValue,
    bool const&         iVerbose
) {
    VmbErrorType err;
    VmbAPI::FeaturePtr f;

    if (iVerbose)
        std::cout << "  - Acquiring feature [" << iFeatureName << "]... ";
    
    err = m_camera->GetFeatureByName ( iFeatureName.c_str (), f );
    if ( VmbErrorSuccess == err ) {
        if (iVerbose)
            std::cout << "OK" << std::endl;
        
        if (iVerbose)
            std::cout << "    - Setting feature [" << iFeatureName << "]... ";
        err = f->SetValue ( iFeatureValue );
        if ( VmbErrorSuccess == err ) {
            if (iVerbose)
                std::cout << "OK [" << iFeatureValue << "]" << std::endl;
            return true;
        } else {
            if (iVerbose)
                std::cout << "KO [" << err << "]" << std::endl;
        }
    } else {
        if (iVerbose)
            std::cout << "KO [" << err << "]" << std::endl;
    }
    return false;
}

template<typename T>
inline bool PcCamera::TryGetFeature (
    std::string const&  iFeatureName,
    T&                  oFeatureValue,
    bool const&         iVerbose
) {
    VmbErrorType err;
    VmbAPI::FeaturePtr f;

    if (iVerbose)
        std::cout << "  - Acquiring feature [" << iFeatureName << "]... ";
    
    err = m_camera->GetFeatureByName ( iFeatureName.c_str (), f );
    if ( VmbErrorSuccess == err ) {
        if (iVerbose)
            std::cout << "OK" << std::endl;
        
        if (iVerbose)
            std::cout << "    - Reading feature [" << iFeatureName << "]... ";
        err = f->GetValue ( oFeatureValue );
        if ( VmbErrorSuccess == err ) {
            if (iVerbose)
                std::cout << "OK [" << oFeatureValue << "]" << std::endl;
            return true;
        } else {
            if (iVerbose)
                std::cout << "KO [" << err << "]" << std::endl;
        }
    } else {
        if (iVerbose)
            std::cout << "KO [" << err << "]" << std::endl;
    }
    return false;
}

inline bool PcCamera::TryRunFeature (
    std::string const&  iFeatureName,
    bool const&         iVerbose
) {
    VmbErrorType err;
    VmbAPI::FeaturePtr f;

    if (iVerbose)
        std::cout << "  - Acquiring command feature [" << iFeatureName << "]... ";
    
    err = m_camera->GetFeatureByName ( iFeatureName.c_str (), f );
    if ( VmbErrorSuccess == err ) {
        if (iVerbose)
            std::cout << "OK" << std::endl;
        
        if (iVerbose)
            std::cout << "    - Running command feature [" << iFeatureName << "]... ";
        err = f->RunCommand ();
        if ( VmbErrorSuccess == err ) {
            if (iVerbose)
                std::cout << "OK" << std::endl;
            return true;
        } else {
            if (iVerbose)
                std::cout << "KO [" << err << "]" << std::endl;
        }
    } else {
        if (iVerbose)
            std::cout << "KO [" << err << "]" << std::endl;
    }
    return false;
}

inline bool PcCamera::TryRegisterObserver (
    std::string const&                  iFeatureName,
    VmbAPI::IFeatureObserverPtr const&  iObserver,
    bool const&                         iVerbose
) {
    VmbErrorType err;
    VmbAPI::FeaturePtr f;

    if (iVerbose)
        std::cout << "  - Acquiring feature [" << iFeatureName << "]... ";
    
    err = m_camera->GetFeatureByName ( iFeatureName.c_str (), f );
    if ( VmbErrorSuccess == err ) {
        if (iVerbose)
            std::cout << "OK" << std::endl;
        
        if (iVerbose)
            std::cout << "    - Registering feature observer [" << iFeatureName << "]... ";
        err = f->RegisterObserver ( iObserver );
        if ( VmbErrorSuccess == err ) {
            if (iVerbose)
                std::cout << "OK" << std::endl;
            return true;
        } else {
            if (iVerbose)
                std::cout << "KO [" << err << "]" << std::endl;
        }
    } else {
        if (iVerbose)
            std::cout << "KO [" << err << "]" << std::endl;
    }
    return false;
}

void PcCamera::Setup () {
    if ( !m_isSetup ) {
        VmbErrorType err;
        VmbInt64_t payload;
        VmbAPI::FeaturePtr f;

        err = m_camera->GetID ( m_cameraId );
        std::cout << "Camera " << m_cameraId << " { " << std::endl;

        std::cout << "  - Opening... ";
        err = m_camera->Open ( VmbAccessModeFull );
        if ( VmbErrorSuccess == err ) {
            std::cout << "OK" << std::endl;
            
            //FeaturePtrVector features;
            //std::cout << "  - Acquiring available features... ";
            //err = m_camera->GetFeatures ( features );
            //if ( VmbErrorSuccess == err ) {
            //    std::cout << "OK" << std::endl;
            //    std::cout << "  - List of available features: " << std::endl;
            //    for ( auto f = features.begin (); f != features.end (); f++ ) {
            //        std::string sFeatureName;
            //        (*f)->GetName ( sFeatureName );
            //        std::cout << "      - " << sFeatureName << std::endl;
            //    }
            //} else {
            //    std::cout << "KO [" << err << "]" << std::endl;
            //}

            TryGetFeature ( "PayloadSize", payload );
            
            VmbInt64_t f;
            TryGetFeature ( "Height", f );
            m_frameSize.height = f;
            TryGetFeature ( "Width", f );
            m_frameSize.width = f;

            TrySetFeature ( "TriggerSelector", "FrameStart" );
            TrySetFeature ( "TriggerMode", "On" );
            TrySetFeature ( "TriggerSource", "Freerun" );
            TrySetFeature ( "PixelFormat", VmbPixelFormatMono8 );
            TrySetFeature ( "GVSPPacketSize", (VmbInt32_t)1500 );
            TrySetFeature ( "ExposureTimeAbs", 15000.0 );
            TrySetFeature ( "GainAuto", "Continuous" );

        } else {
            std::cout << "KO [" << err << "]" << std::endl;
        }
        std::cout << "} // " << m_cameraId << std::endl;

        m_isSetup = true;
    }
}

void PcCamera::StartAcquisition ()
{
    VmbErrorType err = m_camera->StartContinuousImageAcquisition ( 10, VmbAPI::IFrameObserverPtr ( new PcFrameObserver ( m_camera ) ) );
    m_isAcquiring = ( err == VmbErrorSuccess );

    if ( m_isSynced ) {
        const bool verbose = false;

        VmbInt64_t timestamp, timestampClock;
        TryRunFeature ( "GevTimestampControlLatch", verbose );
        TryGetFeature ( "GevTimestampValue", timestamp, verbose );
        TryGetFeature ( "GevTimestampTickFrequency", timestampClock, verbose );
    
        timestamp += 2 * timestampClock;
        TrySetFeature ( "PtpAcquisitionGateTime", timestamp, verbose );
    }
}

void PcCamera::StopAcquisition ()
{
    m_camera->StopContinuousImageAcquisition ();
    m_isAcquiring = false;
}

void PcCamera::AdjustBandwidth ( unsigned int const& iBandwidth )
{
    TrySetFeature ( "StreamBytesPerSecond", (VmbInt32_t)iBandwidth, false );
}


void PcCamera::Synchronise ()
{
    bool verbose = false;

    TrySetFeature ( "PtpMode", "Off", verbose );
    TrySetFeature ( "TriggerSelector", "FrameStart", verbose );
    TrySetFeature ( "TriggerMode", "On", verbose );
    TrySetFeature ( "TriggerSource", "FixedRate", verbose );
    TrySetFeature ( "EventSelector", "PtpSyncLocked", verbose );
    TrySetFeature ( "EventNotification", "On", verbose );
    TrySetFeature ( "EventSelector", "PtpSyncLost", verbose );
    TrySetFeature ( "EventNotification", "On", verbose );

    //TryRunFeature ( "GevTimestampControlReset", verbose );
    
    VmbAPI::IFeatureObserverPtr ptpSyncAgent ( new PcPtpSyncAgent ( this ) );
    TryRegisterObserver ( "EventPtpSyncLocked", ptpSyncAgent, verbose );
    TryRegisterObserver ( "EventPtpSyncLost", ptpSyncAgent, verbose );
    TrySetFeature ( "PtpMode", "Auto", verbose );

    while ( !m_isSynced );// { std::cout << m_ptpStatus << std::endl; }
}

double PcCamera::DoCalibration ( cv::InputArrayOfArrays iChessboardPoints, cv::InputArrayOfArrays iDetectedChessboardPoints, cv::Size iImageSize )
{
    VEC(cv::Mat) rvecs, tvecs;

    double rms = cv::calibrateCamera (
        iChessboardPoints,
        iDetectedChessboardPoints,
        iImageSize,
        m_cameraMatrix,
        m_distCoeffs,
        rvecs,
        tvecs,
        CV_CALIB_FIX_K4|CV_CALIB_FIX_K5|CV_CALIB_FIX_K6
    );

    return rms;
}

void PcCamera::StartCalibration ()
{
    m_calibration->StartCalibration ();
}
void PcCamera::AbortCalibration ()
{
    m_calibration->AbortCalibration ();
}
void PcCamera::TryPushFrame ( PcFramePtr const& iFrame )
{
    PcCalibrationHelper& calib = PcCalibrationHelper::GetInstance ();

    unsigned int frameDelay = calib.FrameDelay () / 16;

    m_frameCount++;
    if ( m_frameCount >= m_lastFrameCount + frameDelay ) {
        m_calibration->PushFrame ( iFrame );
        m_lastFrameCount = m_frameCount;
    }
}
CalibrationState PcCamera::GetCalibrationState ()
{
    return m_calibration->GetCalibrationState ();
}
double PcCamera::GetCalibrationProgress () const
{
    return m_calibration->GetCalibrationProgress ();
}
VECOFVECS(cv::Point2f) const& PcCamera::GetChessboardCorners () const
{
    return m_calibration->GetChessboardCorners ();
}