#ifndef PCCORECAMERA_H
#define PCCORECAMERA_H

#include "PCCoreExport.h"
#include "PCCoreCommon.h"

#include "CameraCalibration.h"
#include "CalibrationHelper.h"

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <VimbaCPP/Include/VimbaCPP.h>
using namespace AVT::VmbAPI;

namespace boost {
    class mutex;
}

class PCCoreCamera
{
private:
    typedef boost::mutex MutexType;
    typedef boost::lock_guard<MutexType> GuardType;

public:
    PCCORE_EXPORT explicit PCCoreCamera ( CameraPtr const& iCamera );
    PCCORE_EXPORT ~PCCoreCamera ();

    PCCORE_EXPORT void Setup ();
    PCCORE_EXPORT void StartAcquisition ();
    PCCORE_EXPORT void StopAcquisition ();
    PCCORE_EXPORT void AdjustBandwidth ( unsigned int const& iBandwidth );
    PCCORE_EXPORT void Synchronise ();
    PCCORE_EXPORT void StartCalibration ();
    PCCORE_EXPORT void AbortCalibration ();
    PCCORE_EXPORT void TryPushFrame ( PCCoreFramePtr const& iFrame );
    PCCORE_EXPORT pcc::CalibrationState GetCalibrationState ();
    PCCORE_EXPORT double GetCalibrationProgress () const;
    PCCORE_EXPORT std::vector< std::vector<cv::Point2f> > const& GetChessboardCorners () const;

    double DoCalibration ( cv::InputArrayOfArrays iChessboardPoints, cv::InputArrayOfArrays iDetectedChessboardPoints, cv::Size iImageSize );

    inline std::string const& GetID () const { return m_cameraId; };
    
    inline cv::Mat& CameraMatrix () { return (m_cameraMatrix); };
    inline cv::Mat const& CameraMatrix () const { return (m_cameraMatrix); };
    inline cv::Mat& DistCoeffs () { return (m_distCoeffs); };
    inline cv::Mat const& DistCoeffs () const { return (m_distCoeffs); };

    inline std::string const& GetPtpStatus () const { return m_ptpStatus; }
    inline void SetPtpStatus ( std::string const& iPtpStatus ) { m_ptpStatus = iPtpStatus; }
    
    inline void SetSynced ( bool const& iSynced ) { m_isSynced = iSynced; }
    inline bool volatile const& IsSynced () const { return m_isSynced; }

    inline bool const& IsAcquiring () { return m_isAcquiring; }
    
    inline cv::Size const& GetFrameSize () { return m_frameSize; }

private:
    void Release ();
    void DoCopy ( PCCoreCamera const& iOther );

    PCCoreCamera ( PCCoreCamera const& iOther ) {}
    PCCoreCamera& operator= ( PCCoreCamera const& iOther ) { return (*this); }

private:
    template<typename T>
    bool TrySetFeature (
        std::string const&          iFeatureName,
        T const&                    iFeatureValue,
        bool const&                 iVerbose=true
    );
    template<typename T>
    bool TryGetFeature (
        std::string const&          iFeatureName,
        T&                          oFeatureValue,
        bool const&                 iVerbose=true
    );
    bool TryRunFeature (
        std::string const&          iFeatureName,
        bool const&                 iVerbose=true
    );
    bool TryRegisterObserver (
        std::string const&          iFeatureName,
        IFeatureObserverPtr const&  iObserver,
        bool const&                 iVerbose=true
    );

public:
    inline void RegisterCallback ( void (*iFunc)(PCCoreCamera*, pcc::CalibrationState, pcc::CalibrationState) )
    {
        m_calibration->RegisterCallback ( iFunc );
    }

    template<typename T>
    inline void RegisterCallback ( T* iObj, void (T::*iFunc)(PCCoreCamera*, pcc::CalibrationState, pcc::CalibrationState) )
    {
        m_calibration->RegisterCallback ( iObj, iFunc );
    }

private:
    bool                                            m_isSetup;
    bool                                            m_isAcquiring;
    volatile bool                                   m_isSynced;

    CameraPtr                                       m_camera;
    std::string                                     m_cameraId;
    std::string                                     m_ptpStatus;

    cv::Size                                        m_frameSize;

    // Calibration
    cv::Mat                                         m_cameraMatrix;
    cv::Mat                                         m_distCoeffs;

    unsigned int                                    m_frameCount;
    unsigned int                                    m_lastFrameCount;
    pcc::CameraCalibration*                         m_calibration;
};

typedef boost::shared_ptr<PCCoreCamera> PCCoreCameraPtr;

#endif // PCCORECAMERA_H
