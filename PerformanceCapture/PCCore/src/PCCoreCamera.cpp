#include "PCCoreCamera.h"

#include "PCCoreCommon.h"
#include "PCCoreSystem.h"
#include "PCCoreFrame.h"
#include "PCCoreFrameObserver.h"

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>

#include <iostream>

PCCoreCamera::PCCoreCamera (
    CameraPtr const&        iCamera
)   :   m_camera ( iCamera )
    ,   m_refCount ( new unsigned int ( 1u ) )
    ,   m_isSetup ( false )
    ,   m_isAcquiring ( false )
    ,   m_isSynced( false )
    ,   m_cameraMatrix ( new cv::Mat ( 3, 3, CV_64F ) )
    ,   m_distCoeffs ( new cv::Mat ( 8, 1, CV_64F ) )
{}

PCCoreCamera::~PCCoreCamera ()
{
    if ( m_refCount && --(*m_refCount) == 0u ) {
        PCC_OBJ_FREE ( m_refCount );
        PCC_OBJ_FREE ( m_cameraMatrix );
        PCC_OBJ_FREE ( m_distCoeffs );
        m_camera->Close ();
    }
}

void PCCoreCamera::DoCopy ( PCCoreCamera const& iOther )
{
    ++(*iOther.m_refCount);
    m_refCount = iOther.m_refCount;
    m_camera = iOther.m_camera;
    m_cameraId = iOther.m_cameraId;
    m_isSetup = iOther.m_isSetup;
    m_isAcquiring= iOther.m_isAcquiring;
    m_isSynced = iOther.m_isSynced;
    m_cameraMatrix = iOther.m_cameraMatrix;
    m_distCoeffs = iOther.m_distCoeffs;
}

PCCoreCamera::PCCoreCamera ( PCCoreCamera const& iOther )
{
    DoCopy ( iOther );
}

PCCoreCamera& PCCoreCamera::operator= ( PCCoreCamera const& iOther )
{
    if ( this != &iOther ) {
        this->~PCCoreCamera ();
        DoCopy ( iOther );
    }
    return (*this);
}

template<typename T>
inline bool PCCoreCamera::TrySetFeature (
    std::string const&  iFeatureName,
    T const&            iFeatureValue,
    bool const&         iVerbose
) {
    VmbErrorType err;
    FeaturePtr f;

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
inline bool PCCoreCamera::TryGetFeature (
    std::string const&  iFeatureName,
    T&                  oFeatureValue,
    bool const&         iVerbose
) {
    VmbErrorType err;
    FeaturePtr f;

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

inline bool PCCoreCamera::TryRunFeature (
    std::string const&  iFeatureName,
    bool const&         iVerbose
) {
    VmbErrorType err;
    FeaturePtr f;

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

inline bool PCCoreCamera::TryRegisterObserver (
    std::string const&          iFeatureName,
    IFeatureObserverPtr const&  iObserver,
    bool const&                 iVerbose
) {
    VmbErrorType err;
    FeaturePtr f;

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

void PCCoreCamera::Setup () {
    if ( !m_isSetup ) {
        VmbErrorType err;
        VmbInt64_t payload;
        FeaturePtr f;

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

void PCCoreCamera::StartAcquisition ()
{
    VmbErrorType err = m_camera->StartContinuousImageAcquisition ( 3, IFrameObserverPtr ( new PCCoreFrameObserver ( m_camera ) ) );
    m_isAcquiring = ( err == VmbErrorSuccess );

    if ( m_isSynced ) {
        bool verbose = true;

        VmbInt64_t timestamp, timestampClock;
        TryRunFeature ( "GevTimestampControlLatch", verbose );
        TryGetFeature ( "GevTimestampValue", timestamp, verbose );
        TryGetFeature ( "GevTimestampTickFrequency", timestampClock, verbose );
    
        timestamp += 2 * timestampClock;
        TrySetFeature ( "PtpAcquisitionGateTime", timestamp, verbose );
    }
}

void PCCoreCamera::StopAcquisition ()
{
    m_camera->StopContinuousImageAcquisition ();
    m_isAcquiring = false;
}

void PCCoreCamera::AdjustBandwidth ( unsigned int const& iBandwidth )
{
    TrySetFeature ( "StreamBytesPerSecond", (VmbInt32_t)iBandwidth, false );
}

class PtpSyncAgent
    :   public IFeatureObserver {
public:
    void FeatureChanged ( FeaturePtr const& pFeature ) {
        boost::lock_guard<boost::mutex> lock (sm_mutex);

        std::string sPtpStatus;
        m_camera->TryGetFeature ( "PtpStatus", sPtpStatus, false );

        std::cout << m_camera->GetID() << " PtpStatus changed: " << sPtpStatus << std::endl;

        m_camera->SetPtpStatus ( sPtpStatus );
        bool verbose = true;
        if ( ( sPtpStatus.compare ( "Slave" ) == 0 ) || ( sPtpStatus.compare ( "Master" ) == 0 ) ) {
            m_camera->SetSynced ( true );
        } else if ( sPtpStatus.compare ( "Error" ) == 0 ) {
            m_camera->SetSynced ( false );
        }
    }

    PtpSyncAgent ( PCCoreCamera* iCamera ) : m_camera ( iCamera ) {}
private:
    static boost::mutex     sm_mutex;

    PCCoreCamera*           m_camera;
};
boost::mutex PtpSyncAgent::sm_mutex;

void PCCoreCamera::Synchronise ()
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
    
    IFeatureObserverPtr ptpSyncAgent ( new PtpSyncAgent ( this ) );
    TryRegisterObserver ( "EventPtpSyncLocked", ptpSyncAgent, verbose );
    TryRegisterObserver ( "EventPtpSyncLost", ptpSyncAgent, verbose );
    TrySetFeature ( "PtpMode", "Auto", verbose );

    while ( !m_isSynced );// { std::cout << m_ptpStatus << std::endl; }
}

double PCCoreCamera::Calibrate ( cv::InputArrayOfArrays iChessboardPoints, cv::InputArrayOfArrays iDetectedChessboardPoints, cv::Size iImageSize )
{
    std::vector<cv::Mat> rvecs, tvecs;

    double rms = cv::calibrateCamera (
        iChessboardPoints,
        iDetectedChessboardPoints,
        iImageSize,
        *m_cameraMatrix,
        *m_distCoeffs,
        rvecs,
        tvecs,
        CV_CALIB_FIX_K4|CV_CALIB_FIX_K5|CV_CALIB_FIX_K6
    );

    return rms;
}
