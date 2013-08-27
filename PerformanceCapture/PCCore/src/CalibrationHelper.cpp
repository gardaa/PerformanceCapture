#include "CalibrationHelper.h"

#include "Chessboard.h"

#include "PCCoreCommon.h"
#include "PCCoreCamera.h"
#include "PCCoreSystem.h"

#include <opencv2/highgui/highgui.hpp>

// ----------------------------------------------------------------------
// CalibrationHelper
// ----------------------------------------------------------------------
// Private static
pcc::Chessboard pcc::CalibrationHelper::sm_chessboard ( 7u, 10u, 10u );
pcc::CalibrationHelper* pcc::CalibrationHelper::sm_pInstance;

// Public static
pcc::CalibrationHelper& pcc::CalibrationHelper::GetInstance ()
{
    if ( !sm_pInstance ) {
        sm_pInstance = new pcc::CalibrationHelper ();
    }
    return (*sm_pInstance);
}
void pcc::CalibrationHelper::DestroyInstance ()
{
    PCC_OBJ_FREE ( sm_pInstance );
}

// Public
pcc::CalibrationHelper::~CalibrationHelper ()
{}
std::vector< std::vector< cv::Point3f > > pcc::CalibrationHelper::GetChessboardPoints ()
{
    return sm_chessboard.CreateInputArray ( m_frameCount );
}

// Private
pcc::CalibrationHelper::CalibrationHelper ()
    :   m_frameDelay ( 1000 )
    ,   m_frameCount ( 15 )
{}
