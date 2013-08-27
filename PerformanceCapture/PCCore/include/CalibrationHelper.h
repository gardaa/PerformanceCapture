#ifndef CALIBRATIONHELPER_H
#define CALIBRATIONHELPER_H

#include "Chessboard.h"

#include <opencv2/calib3d/calib3d.hpp>

namespace pcc
{
    class CalibrationHelper
    {
    public:
        static CalibrationHelper& GetInstance ();
        static void DestroyInstance ();

        ~CalibrationHelper ();

        inline unsigned int const& FrameDelay () const { return m_frameDelay; }
        inline unsigned int const& FrameCount () const { return m_frameCount; }
        std::vector< std::vector< cv::Point3f > > GetChessboardPoints ();
        inline cv::Size const& GetChessboardSize () { return sm_chessboard.GetSize(); }

    private:
        CalibrationHelper ();
        inline explicit CalibrationHelper ( CalibrationHelper const& iOther ) {}
        inline CalibrationHelper& operator= ( CalibrationHelper const& iOther ) {}

    private:
        static CalibrationHelper*           sm_pInstance;
        static Chessboard                   sm_chessboard;

        unsigned int                        m_frameDelay;
        unsigned int                        m_frameCount;
        unsigned int                        m_calibratedCameras;
    };
}

#endif // CALIBRATIONHELPER_H
