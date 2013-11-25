#include "PcStereoCameraPair.h"

#include "PcCommon.h"

#include "PcFrame.h"
#include "PcCamera.h"
#include "PcSystem.h"
#include "PcCalibrationHelper.h"

#include <boost/log/trivial.hpp>

#include <opencv2/calib3d/calib3d.hpp>

using namespace pcc;

PcStereoCameraPair::PcStereoCameraPair (
    PcCameraPtr         iLeft,
    PcCameraPtr         iRight
)   :   m_left ()
    ,   m_right ()
    ,   m_stereoRotation ()
    ,   m_stereoTranslation ()
    ,   m_stereoEssential ()
    ,   m_stereoFundamental ()
{
    SetLeft ( iLeft );
    SetRight ( iRight );
}

void PcStereoCameraPair::OnCameraCalibrated ( PcCamera* iCamera, CalibrationState iOldState, CalibrationState iNewState )
{
    std::string const& camId = iCamera->GetID ();
    if ( iNewState == CALIBRATED && camId.compare ( m_left->GetID () ) == 0 ) {
        if ( m_right.get () != 0x0 && m_right->GetCalibrationState () == CALIBRATED ) {
            Calibrate ();
        }
    } else if ( iNewState == CALIBRATED && camId.compare ( m_right->GetID () ) == 0 ) {
        if ( m_left.get () != 0x0 && m_left->GetCalibrationState () == CALIBRATED ) {
            Calibrate ();
        }
    }
}

void PcStereoCameraPair::Calibrate ()
{
    cv::stereoCalibrate (
        PcCalibrationHelper::GetInstance ().GetChessboardPoints (),
        m_left->GetChessboardCorners (),
        m_right->GetChessboardCorners (),
        m_left->CameraMatrix (),
        m_left->DistCoeffs (),
        m_right->CameraMatrix (),
        m_right->DistCoeffs (),
        m_left->GetFrameSize (),
        m_stereoRotation,
        m_stereoTranslation,
        m_stereoEssential,
        m_stereoFundamental
    );

    BOOST_LOG_TRIVIAL (trace) << std::endl << "Rstereo: " << std::endl << m_stereoRotation << std::endl;
    BOOST_LOG_TRIVIAL (trace) << std::endl << "Tstereo: " << std::endl << m_stereoTranslation << std::endl;
    BOOST_LOG_TRIVIAL (trace) << std::endl << "Estereo: " << std::endl << m_stereoEssential << std::endl;
    BOOST_LOG_TRIVIAL (trace) << std::endl << "Fstereo: " << std::endl << m_stereoFundamental << std::endl;

    m_left->StopAcquisition ();
    m_left->StartAcquisition ();

    m_right->StopAcquisition ();
    m_right->StartAcquisition ();
}