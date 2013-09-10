#include "PcChessboard.h"

using namespace pcc;

// ----------------------------------------------------------------------
// PcChessboard
// ----------------------------------------------------------------------
// Public
PcChessboard::PcChessboard (
    unsigned int const&     iRows,
    unsigned int const&     iCols,
    float const&            iSquareSize
)   :   m_size (iCols, iRows)
    ,   m_squareSize (iSquareSize)
    ,   m_points ()
{
    for ( int i = 0; i < m_size.height; i++ ) {
        for ( int j = 0; j < m_size.width; j++ ) {
            m_points.push_back ( cv::Point3f ( i * m_squareSize, j * m_squareSize, 0.0f ) );
        }   
    }
}
VECOFVECS(cv::Point3f) PcChessboard::CreateInputArray (
    unsigned int        iFrameCount
) {
    VECOFVECS(cv::Point3f) srcArray;

    while ( iFrameCount-- ) {
        srcArray.push_back ( GetPoints () );
    }

    return srcArray;
}