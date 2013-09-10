#ifndef PCSTEREOCAMERAPAIR_H
#define PCSTEREOCAMERAPAIR_H

#include <string>

namespace pcc {
    class PcFrame;
    enum CalibrationState;
}

#include "PcCommon.h"
#include "PcCamera.h"
#include <opencv2/opencv.hpp>

namespace pcc
{
    class PcStereoCameraPair
    {
    public:
        PcStereoCameraPair (
            PcCameraPtr         iLeft,
            PcCameraPtr         iRight
        );
        ~PcStereoCameraPair ();

        void Calibrate ();

        void OnCameraCalibrated ( pcc::PcCamera* iCamera, pcc::CalibrationState iOldState, pcc::CalibrationState iNewState );

        inline PcCameraPtr const GetLeft () const { return m_left; }
        inline void SetLeft ( PcCameraPtr const iNewLeft ) {
            m_left = iNewLeft;
            m_left->RegisterCallback ( this, &pcc::PcStereoCameraPair::OnCameraCalibrated );
        }
        inline PcCameraPtr const GetRight () const { return m_right; }
        inline void SetRight ( PcCameraPtr const iNewRight ) {
            m_right = iNewRight;
            m_right->RegisterCallback ( this, &pcc::PcStereoCameraPair::OnCameraCalibrated );
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
        PcStereoCameraPair ( PcStereoCameraPair const& iOther ) {}
        PcStereoCameraPair& operator= ( PcStereoCameraPair const& iOther ) {}

    private:
        PcCameraPtr                     m_left;
        PcCameraPtr                     m_right;
    
        cv::Mat                             m_stereoRotation;
        cv::Mat                             m_stereoTranslation;
        cv::Mat                             m_stereoEssential;
        cv::Mat                             m_stereoFundamental;
    };

    typedef boost::shared_ptr<PcStereoCameraPair> PcStereoCameraPairPtr;
}

#endif // PCSTEREOCAMERAPAIR_H
