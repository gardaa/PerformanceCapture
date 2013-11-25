#ifndef PCCHESSBOARD_H
#define PCCHESSBOARD_H

#include "PcCommon.h"
#include <vector>

#include <opencv2/calib3d/calib3d.hpp>

namespace pcc
{
    /// \ingroup PCCORE
    /// 
    /// \brief Describes a chessboard to be used on calibration.
    ///
    /// The chessboard is described by its inner corners and square sizes. Therefore, a 5x7 chessboard would look like this:
    ///  \verbatim
    ///    +--+--+--+--+--+--+--+--+
    ///    ||||  ||||  ||||  ||||  |
    ///  1 +--+--+--+--+--+--+--+--+
    ///    |  ||||  ||||  ||||  ||||
    ///  2 +--+--+--+--+--+--+--+--+
    ///    ||||  ||||  ||||  ||||  |
    ///  3 +--+--+--+--+--+--+--+--+
    ///    |  ||||  ||||  ||||  ||||
    ///  4 +--+--+--+--+--+--+--+--+
    ///    ||||  ||||  ||||  ||||  |
    ///  5 +--+--+--+--+--+--+--+--+
    ///    |  ||||  ||||  ||||  ||||
    ///    +--+--+--+--+--+--+--+--+
    ///       1  2  3  4  5  6  7    \endverbatim
    class PcChessboard {
    public:
        /// \brief Constructor.
        ///
        /// Sets up the metadata of the chessboard descriptor (inner chessboard dimensions and square size),
        /// and calculates all of the chessboard's points coordinates in 3D space.
        /// \param [in] iRows       the number of inner corners in a column of the chessboard
        /// \param [in] iCols       the number of inner corners in a row of the chessboard
        /// \param [in] iSquareSize the size of the square side in a user-defined scale
        PcChessboard (
            unsigned int const&     iRows,
            unsigned int const&     iCols,
            float const&            iSquareSize
        );

        /// \brief Gets the list of points in 3D space describing the chessboard.
        /// \return a vector containing all of the points describing the chessboard
        inline VEC(cv::Point3f) const& GetPoints () const { return m_points; }
        
        /// \brief Gets dimensions of the chessboard.
        /// \return the dimensions of the chessboard
        inline cv::Size const& GetSize () const { return m_size; }

        /// \brief Creates an array with a given number of chessboard corner coordinates.
        /// 
        /// Copies the vector describing the coordinates of the internal corners of the chessboard
        /// many times in an output array to be used directly by the calibration method of the cameras.
        /// 
        /// \param [in] iFrameCount     the number of descriptors needed (the size of the output array)
        VECOFVECS(cv::Point3f) CreateInputArray (
            unsigned int        iFrameCount
        );

    private:
        cv::Size                    m_size;         ///< The dimensions of the chessboard.
        float                       m_squareSize;   ///< The size of the side of the chessboard's squares.
        VEC(cv::Point3f)            m_points;       ///< The real-world coordinates of the chessboard's internal corners
    };
}

#endif // PCCHESSBOARD_H
