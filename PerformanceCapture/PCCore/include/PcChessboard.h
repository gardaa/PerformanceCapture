#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "PcCommon.h"
#include <vector>

#include <opencv2/calib3d/calib3d.hpp>

namespace pcc
{
    class PcChessboard {
    public:
        PcChessboard (
            unsigned int const&     iRows,
            unsigned int const&     iCols,
            float const&            iSquareSize
        );
        inline ~PcChessboard () {}

        inline VEC(cv::Point3f) const& GetPoints () const { return m_points; }
        inline cv::Size const& GetSize () const { return m_size; }

        VECOFVECS(cv::Point3f) CreateInputArray (
            unsigned int        iFrameCount
        );

    private:
        cv::Size                    m_size;
        float                       m_squareSize;
        VEC(cv::Point3f)            m_points;
    };
}

#endif // CHESSBOARD_H