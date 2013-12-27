#ifndef PCCALIBRATIONHELPER_H
#define PCCALIBRATIONHELPER_H

#include "PcCommon.h"

#include "PcChessboard.h"

#include <opencv2/calib3d/calib3d.hpp>

namespace pcc
{
    /// \class      PcCalibrationHelper
    ///
    /// \ingroup    PCCORE
    /// 
    /// \brief      Provides useful information for the calibration process.
    ///
    /// This class facilitates the camera calibration process by providing useful information 
    /// about the calibration process, such as:
    ///     - The time-delay between consecutive frame captures  
    ///     - The number of image frames to be used to calibrate the camera.  
    ///     - The points on R^3 describing the chessboard to be detected in calibration images.  
    ///     - The size of the chessboard to be detected.
    ///
    /// \todo   Allow m_frameDelay and m_frameCount to be set from outside.
    class PcCalibrationHelper
    {
    public:

        /// \brief  Gets a reference to the singleton PcCalibrationHelper instance.
        ///
        /// Allocates a new instance upon first invocation.
        /// 
        /// \return The singleton instance of PcCalibrationHelper
        static PcCalibrationHelper& GetInstance ();

        /// \brief  Destroys the currently allocated singleton instance of PcCalibrationHelper.
        static void DestroyInstance ();

        /// \brief  Gets the capture delay between two consecutive calibration frames.
        ///
        /// \return A constant reference to an unsigned int variable
        inline unsigned int const& FrameDelay () const { return m_frameDelay; }

        /// \brief  Gets the number of frames required for calibration.
        ///
        /// \return A constant reference to an unsigned int variable
        inline unsigned int const& FrameCount () const { return m_frameCount; }

        /// \brief  Gets a vector of point sets describing the chessboard to be detected within each frame.
        ///
        /// Assumes the chessboard is the same for all of the calibration frames, but since OpenCV won't 
        /// account for this use case, multiple copies of the same chessboard are added to the list for 
        /// every frame.
        /// 
        /// \return A vector of vectors of cv::Point3f values
        VECOFVECS(cv::Point3f) GetChessboardPoints ();

        /// \brief  Gets the calibration chessboard's inner dimensions.
        ///
        /// \return A cv::Size instance describing the chessboard dimensions.
        inline cv::Size const& GetChessboardSize () { return sm_chessboard.GetSize(); }

    private:

        /// \brief  Default constructor.
        /// Sets m_frameDelay and m_frameCount to their default values.
        PcCalibrationHelper ();

        /// \brief  Private copy constructor to prevent copy.
        ///
        /// Since PcCalibrationHelper is a singleton, we forbid copy operations.
        /// 
        /// \param [in] iOther      The instance to be copied.
        inline explicit PcCalibrationHelper ( PcCalibrationHelper const& iOther ) {}

        /// \brief  Private assignment operator to avoid unintentional assignment.
        ///
        /// Since PcCalibrationHelper is a singleton, we forbid copy operations.
        /// 
        /// \param [in] iOther      The instance from which to copy data.
        /// 
        /// \return This object, unmodified.
        inline PcCalibrationHelper& operator= ( PcCalibrationHelper const& iOther ) { return (*this); }

        /// \brief  Private destructor for the PcCalibrationHelper class.
        ~PcCalibrationHelper ();

    private:
        static PcCalibrationHelper*             sm_pInstance;   ///< The singleton instance of the calibration helper.
        static PcChessboard                     sm_chessboard;  ///< The chessboard descriptor to be used during the calibration process.

        unsigned int                            m_frameDelay;   ///< The delay in ms between consecutive frame captures. Defaults to 1000ms.
        unsigned int                            m_frameCount;   ///< The number of frames to be used for calibration. Defaults to 15.
    };
}

#endif // PCCALIBRATIONHELPER_H
