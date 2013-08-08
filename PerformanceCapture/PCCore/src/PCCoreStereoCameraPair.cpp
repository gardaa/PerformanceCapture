#include "PCCoreStereoCameraPair.h"

#include "PCCoreCommon.h"

#include "PCCoreFrame.h"
#include "PCCoreCamera.h"
#include "PCCoreSystem.h"

#include <opencv2/calib3d/calib3d.hpp>

ChessboardDescriptor PCCoreStereoCameraPair::sm_chessboard ( 8u, 11u, 10u );

PCCoreStereoCameraPair::PCCoreStereoCameraPair ()
    :   m_refCount ( new unsigned int ( 1u ) )
    ,   m_left ( 0x0 )
    ,   m_right ( 0x0 )
    ,   m_stereoRotation    ( new cv::Mat () )
    ,   m_stereoTranslation ( new cv::Mat () )
    ,   m_stereoEssential   ( new cv::Mat () )
    ,   m_stereoFundamental ( new cv::Mat () )
{}


PCCoreStereoCameraPair::~PCCoreStereoCameraPair ()
{
    if ( m_refCount && --(*m_refCount) ) {
        PCC_OBJ_FREE (m_refCount);
        PCC_OBJ_FREE (m_left);
        PCC_OBJ_FREE (m_right);
        PCC_OBJ_FREE (m_stereoRotation);
        PCC_OBJ_FREE (m_stereoTranslation);
        PCC_OBJ_FREE (m_stereoEssential);
        PCC_OBJ_FREE (m_stereoFundamental);
    }
}

void PCCoreStereoCameraPair::Calibrate ()
{
    //PCCoreSystem& cs = PCCoreSystem::GetInstance ();
    //PCCoreFrame const& leftFrame = cs.GetFrameFromCamera ( m_left->GetID() );
    //PCCoreFrame const& rightFrame = cs.GetFrameFromCamera ( m_right->GetID() );

    //std::vector< cv::Point3f > chessboardPoints;
    //for ( unsigned int i = 0; i < sm_chessboard.rows; i++ ) {
    //    for ( unsigned int j = 0; j < sm_chessboard.cols; j++ ) {
    //        chessboardPoints.push_back ( cv::Point3f ( i * sm_chessboard.squareSize, j * sm_chessboard.squareSize, 0.0f ) );
    //    }   
    //}

    //cv::stereoCalibrate (
    //    chessboardPoints,
    //    leftFrame.GetImagePoints (),
    //    rightFrame.GetImagePoints (),
    //    m_left->CameraMatrix (),
    //    m_left->DistCoeffs (),
    //    m_right->CameraMatrix (),
    //    m_right->DistCoeffs (),
    //    cv::Size ( leftFrame.Width (), leftFrame.Height () ),
    //    *m_stereoRotation,
    //    *m_stereoTranslation,
    //    *m_stereoEssential,
    //    *m_stereoFundamental
    //);
}