#include "PcCalibrationHelper.h"

#include "PcChessboard.h"
#include "PcCamera.h"
#include "PcSystem.h"

#include <opencv2/highgui/highgui.hpp>

using namespace pcc;

// ----------------------------------------------------------------------
// PcCalibrationHelper
// ----------------------------------------------------------------------
// Private static
PcChessboard PcCalibrationHelper::sm_chessboard ( 7u, 10u, 10u );
PcCalibrationHelper* PcCalibrationHelper::sm_pInstance;

// Public static
PcCalibrationHelper& PcCalibrationHelper::GetInstance ()
{
    if ( !sm_pInstance ) {
        sm_pInstance = new PcCalibrationHelper ();
    }
    return (*sm_pInstance);
}
void PcCalibrationHelper::DestroyInstance ()
{
    PCC_OBJ_FREE ( sm_pInstance );
}

// Public
PcCalibrationHelper::~PcCalibrationHelper ()
{}
VECOFVECS(cv::Point3f) PcCalibrationHelper::GetChessboardPoints ()
{
    return sm_chessboard.CreateInputArray ( m_frameCount );
}

// Private
PcCalibrationHelper::PcCalibrationHelper ()
    :   m_frameDelay ( 1000 )
    ,   m_frameCount ( 15 )
{}
