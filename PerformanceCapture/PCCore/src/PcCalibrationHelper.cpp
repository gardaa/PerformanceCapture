#include "PcCalibrationHelper.h"

#include "PcChessboard.h"
#include "PcCamera.h"
#include "PcSystem.h"

#include <opencv2/highgui/highgui.hpp>

using namespace pcc;

/// \brief  Default calibration chessboard dimensions are 7x10 with 10x10 sized squares.
PcChessboard PcCalibrationHelper::sm_chessboard ( 7u, 10u, 10u );
PcCalibrationHelper* PcCalibrationHelper::sm_pInstance = 0x0;

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

PcCalibrationHelper::~PcCalibrationHelper ()
{}
VECOFVECS(cv::Point3f) PcCalibrationHelper::GetChessboardPoints ()
{
    return sm_chessboard.CreateInputArray ( m_frameCount );
}

PcCalibrationHelper::PcCalibrationHelper ()
    :   m_frameDelay ( 1000 )
    ,   m_frameCount ( 15 )
{}
