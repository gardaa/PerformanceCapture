#ifndef PCCORECAMERA_H
#define PCCORECAMERA_H

#include "PCCoreExport.h"
#include "PCCoreCommon.h"

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <VimbaCPP/Include/VimbaCPP.h>
using namespace AVT::VmbAPI;

class PCCoreCamera
{
public:
    PCCORE_EXPORT explicit PCCoreCamera ( CameraPtr const& iCamera );
    PCCORE_EXPORT ~PCCoreCamera ();

    PCCORE_EXPORT PCCoreCamera ( PCCoreCamera const& iOther );
    PCCORE_EXPORT PCCoreCamera& operator= ( PCCoreCamera const& iOther );

    PCCORE_EXPORT void Setup ();
    PCCORE_EXPORT void StartAcquisition ();
    PCCORE_EXPORT void StopAcquisition ();
    PCCORE_EXPORT void AdjustBandwidth ( unsigned int const& iBandwidth );
    PCCORE_EXPORT void Synchronise ();
    PCCORE_EXPORT double Calibrate ( cv::InputArrayOfArrays iChessboardPoints, cv::InputArrayOfArrays iDetectedChessboardPoints, cv::Size iImageSize );

    inline std::string const& GetID () const { return m_cameraId; };
    
    inline cv::Mat& CameraMatrix () { return (*m_cameraMatrix); };
    inline cv::Mat const& CameraMatrix () const { return (*m_cameraMatrix); };
    inline cv::Mat& DistCoeffs () { return (*m_distCoeffs); };
    inline cv::Mat const& DistCoeffs () const { return (*m_distCoeffs); };

    inline std::string const& GetPtpStatus () const { return m_ptpStatus; }
    inline void SetPtpStatus ( std::string const& iPtpStatus ) { m_ptpStatus = iPtpStatus; }
    
    inline void SetSynced ( bool const& iSynced ) { m_isSynced = iSynced; }
    inline bool volatile const& IsSynced () const { return m_isSynced; }
    
private:
    void DoCopy ( PCCoreCamera const& iOther );

public:
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

private:
    unsigned int*                   m_refCount;

    bool                            m_isSetup;
    bool                            m_isAcquiring;
    volatile bool                   m_isSynced;

    CameraPtr                       m_camera;

    std::string                     m_cameraId;
    std::string                     m_ptpStatus;

    cv::Mat*                        m_cameraMatrix;
    cv::Mat*                        m_distCoeffs;
};

#endif // PCCORECAMERA_H