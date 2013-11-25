#ifndef PCSTEREOCAMERAPAIR_H
#define PCSTEREOCAMERAPAIR_H

#include <string>

#include "PcCommon.h"
#include "PcCamera.h"
#include <opencv2/opencv.hpp>

namespace pcc
{
    class PcFrame;
    enum CalibrationState;

    /// \ingroup PCCORE
    ///
    /// \brief Describes a stereo pair composed by two PcCameras.
    ///
    /// This is used to achieve external calibration of cameras while in a stereo pair.
    /// Contains pointers to the two cameras belonging to the stereo pair, as well as the four
    /// stereo calibration matrices:
    ///     - Rotation matrix from left camera to right camera.
    ///     - Translation matrix from left camera to right camera.
    ///     - Essential matrix, relating points from left to right camera images.
    ///     - Fundamental matrix, used to describe the line in the right image where the corresponding of a point in the left image must lie.
    ///
    /// Triggering of a stereo camera pair calibration is done via the callback system of the single camera
    /// calibration process. The OnCameraCalibrate function is registered as a callback on both cameras
    /// of the stereo pair, so that once both cameras report back as calibrated, the calibration process of 
    /// the stereo pair starts.
    class PcStereoCameraPair
    {
    public:
        /// \brief Constructor.
        ///
        /// Creates a stereo camera pair from its two component cameras.
        /// \param iLeft    the left eye of the stereo pair
        /// \param iRight   the right eye of the stereo pair
        PcStereoCameraPair (
            PcCameraPtr         iLeft,
            PcCameraPtr         iRight
        );

        /// \brief Triggers actual calibration of the stereo pair.
        /// 
        /// Calculates the calibration matrices of the stereo pair and prints them using the Boost.Log module of the Boost library.
        /// Called automatically once both camera's report back as calibrated.
        void Calibrate ();

        /// \brief Callback to be called upon calibration of the component cameras.
        ///
        /// This 
        void OnCameraCalibrated ( pcc::PcCamera* iCamera, pcc::CalibrationState iOldState, pcc::CalibrationState iNewState );

        /// \brief Gets the left-eye camera of the stereo pair.
        /// \return a pointer to the left eye of the stereo pair
        inline PcCameraPtr const GetLeft () const { return m_left; }
        
        /// \brief Sets the left-eye camera of the stereo pair.
        ///
        /// Sets the left eye of the stereo pair to a new camera and registers the calibration callback.
        /// \param iNewLeft    a pointer to the new left eye of the stereo pair
        inline void SetLeft ( PcCameraPtr const iNewLeft ) {
            m_left = iNewLeft;
            m_left->RegisterCallback ( this, &pcc::PcStereoCameraPair::OnCameraCalibrated );
        }

        /// \brief Gets the right-eye camera of the stereo pair.
        /// \return a pointer to the right eye of the stereo pair
        inline PcCameraPtr const GetRight () const { return m_right; }
        
        /// \brief Sets the right-eye camera of the stereo pair.
        ///
        /// Sets the right eye of the stereo pair to a new camera and registers the calibration callback.
        /// \param iNewRight    a pointer to the new right eye of the stereo pair
        inline void SetRight ( PcCameraPtr const iNewRight ) {
            m_right = iNewRight;
            m_right->RegisterCallback ( this, &pcc::PcStereoCameraPair::OnCameraCalibrated );
        }

        /// \brief Gets the status of the calibration process.
        ///
        /// If both cameras have the same calibration status, return it.
        /// Otherwise, the calibration state is unknown.
        ///
        /// \return the calibration state
        inline pcc::CalibrationState GetCalibrationState ()
        {
            if ( m_left->GetCalibrationState () == m_right->GetCalibrationState () ) {
                return m_left->GetCalibrationState ();
            } else {
                return pcc::UNKNOWN;
            }
        }

    private:
        /// \brief Private copy constructor.
        ///
        /// Disables copy of PcStereoCameraPair objects.
        /// \param iOther   the object to be copied
        PcStereoCameraPair ( PcStereoCameraPair const& iOther ) {}

        /// \brief Private assignment operator.
        ///
        /// Disables assignment operations on PcStereoCameraPair objects.
        /// \param iOther   the object to be assigned to this object.
        /// \return this object, unchanged
        PcStereoCameraPair& operator= ( PcStereoCameraPair const& iOther ) { return (*this); }

    private:
        PcCameraPtr                     m_left;                 ///< The left eye of the stereo pair.
        PcCameraPtr                     m_right;    			///< The right eye of the stereo pair.
    
        cv::Mat                         m_stereoRotation;       ///< The rotation matrix.
        cv::Mat                         m_stereoTranslation;    ///< The translation matrix.
        cv::Mat                         m_stereoEssential;      ///< The essential matrix.
        cv::Mat                         m_stereoFundamental;    ///< The fundamental matrix.
    };

    typedef boost::shared_ptr<PcStereoCameraPair> PcStereoCameraPairPtr;    ///< A reference-counted pointer to a PcStereoCameraPair object.
}

#endif // PCSTEREOCAMERAPAIR_H
