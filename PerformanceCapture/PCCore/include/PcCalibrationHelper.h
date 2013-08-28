#ifndef PCCALIBRATIONHELPER_H
#define PCCALIBRATIONHELPER_H

#include "PcCommon.h"

#include "PcChessboard.h"

#include <opencv2/calib3d/calib3d.hpp>

namespace pcc
{
    class PcCalibrationHelper
    {
    public:
        static PcCalibrationHelper& GetInstance ();
        static void DestroyInstance ();

        ~PcCalibrationHelper ();

        inline unsigned int const& FrameDelay () const { return m_frameDelay; }
        inline unsigned int const& FrameCount () const { return m_frameCount; }
        VECOFVECS(cv::Point3f) GetChessboardPoints ();
        inline cv::Size const& GetChessboardSize () { return sm_chessboard.GetSize(); }

    private:
        PcCalibrationHelper ();
        inline explicit PcCalibrationHelper ( PcCalibrationHelper const& iOther ) {}
        inline PcCalibrationHelper& operator= ( PcCalibrationHelper const& iOther ) {}

    private:
        static PcCalibrationHelper*             sm_pInstance;
        static PcChessboard                     sm_chessboard;

        unsigned int                            m_frameDelay;
        unsigned int                            m_frameCount;
        unsigned int                            m_calibratedCameras;
    };
}

#endif // PCCALIBRATIONHELPER_H
