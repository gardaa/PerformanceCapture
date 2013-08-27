#ifndef PCCORESTEREOCAMERAPAIR_H
#define PCCORESTEREOCAMERAPAIR_H

#include <string>

class PCCoreFrame;

namespace pcc {
enum CalibrationState;
}

#include "PCCoreCamera.h"
#include <opencv2/opencv.hpp>

class PCCoreStereoCameraPair
{
public:
    PCCoreStereoCameraPair (
        PCCoreCameraPtr         iLeft,
        PCCoreCameraPtr         iRight
    );
    ~PCCoreStereoCameraPair ();

    void Calibrate ();

    void OnCameraCalibrated ( PCCoreCamera* iCamera, pcc::CalibrationState iOldState, pcc::CalibrationState iNewState );

    inline PCCoreCameraPtr const GetLeft () const { return m_left; }
    inline void SetLeft ( PCCoreCameraPtr const iNewLeft ) {
        m_left = iNewLeft;
        m_left->RegisterCallback ( this, &PCCoreStereoCameraPair::OnCameraCalibrated );
    }
    inline PCCoreCameraPtr const GetRight () const { return m_right; }
    inline void SetRight ( PCCoreCameraPtr const iNewRight ) {
        m_right = iNewRight;
        m_right->RegisterCallback ( this, &PCCoreStereoCameraPair::OnCameraCalibrated );
    }

    inline pcc::CalibrationState GetCalibrationState ()
    {
        if ( m_left->GetCalibrationState () == m_right->GetCalibrationState () ) {
            return m_left->GetCalibrationState ();
        } else {
            return pcc::UNKNOWN;
        }
    }

private:
    PCCoreStereoCameraPair ( PCCoreStereoCameraPair const& iOther ) {}
    PCCoreStereoCameraPair& operator= ( PCCoreStereoCameraPair const& iOther ) {}

private:
    PCCoreCameraPtr                     m_left;
    PCCoreCameraPtr                     m_right;
    
    cv::Mat                             m_stereoRotation;
    cv::Mat                             m_stereoTranslation;
    cv::Mat                             m_stereoEssential;
    cv::Mat                             m_stereoFundamental;
};

typedef boost::shared_ptr<PCCoreStereoCameraPair> PCCoreStereoCameraPairPtr;

#endif // PCCORESTEREOCAMERAPAIR_H
