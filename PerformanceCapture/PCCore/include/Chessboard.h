#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>

#include <opencv2/calib3d/calib3d.hpp>

namespace pcc
{
    class Chessboard {
    public:
        Chessboard (
            unsigned int const&     iRows,
            unsigned int const&     iCols,
            float const&            iSquareSize
        );
        inline ~Chessboard () {}

        inline std::vector< cv::Point3f > const& GetPoints () const { return m_points; }
        inline cv::Size const& GetSize () const { return m_size; }

        std::vector< std::vector< cv::Point3f > > CreateInputArray (
            unsigned int        iFrameCount
        );

    private:
        cv::Size                    m_size;
        float                       m_squareSize;
        std::vector< cv::Point3f >  m_points;
    };
}

#endif // CHESSBOARD_H