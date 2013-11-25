#ifndef PCFRAMEVIEWER_H
#define PCFRAMEVIEWER_H

#include <QGLWidget>
#include <VimbaCPP/Include/VimbaCPP.h>
#include <opencv2/opencv.hpp>

#include "PcFrame.h"

class QTimer;

namespace pcm
{
    /// \ingroup    PCMAIN
    /// 
    /// \brief The frame display.
    /// 
    /// Displays current frames from the cameras in a grid. The number of columns in the grid can be specified by the user.
    class PcFrameViewer 
        :   public QGLWidget
    {
        Q_OBJECT
        
    public:
        /// \brief Constructor.
        /// 
        /// Instantiates a timer to automatically update the canvas and connects its timeout signal to the
        /// updateGL slot of this QGLWidget. Also, initialises the PcSystem singleton and the
        /// textures where frame images will be placed for displaying.
        /// 
        /// \param [in] iParent a pointer to the QWidget object that owns this widget
        explicit PcFrameViewer ( QWidget* iParent = 0 );
        
        /// \brief Default destructor.
        /// 
        /// Ends any currently running capture on the PcSystem, destroys the PcSystem singleton and
        /// frees memory used by the GLTextures.
        ~PcFrameViewer ();
        
    signals:

    public slots:
        /// \brief Overrides the QGLWidget default initializeGL() slot.
        /// 
        /// Performs the following actions:
        ///     - Enables GL texture mode
        ///     - Sets default shading model to smooth
        ///     - Sets the background clear color to CornflowerBlue
        ///     - Sets the clear depth
        ///     - Enables depth testing
        ///     - Sets the depth comparison function to LEQUAL (less than or equal)
        void initializeGL ();

        /// \brief Overrides the QGLWidget default resizeGL() slot.
        /// 
        /// Called whenever the window is resized to set the new width and height internally.
        /// \param [in] w   the new width
        /// \param [in] h   the new height
        void resizeGL ( int w, int h );

        /// \brief Overrides the QGLWidget default paintGL() slot.
        /// 
        /// Actually performs the drawing of the objects on the screen.
        /// 
        /// Gets the list of cameras from the PcSystem singleton and for every camera
        /// on that list, draw a separate viewport with its current frame.
        void paintGL ();

        /// \brief Called whenever the user changes the number of columns on the grid.
        /// 
        /// Sets the number of columns to the value given by the user and 
        /// recalculates the number of rows to match the number of cameras to be used.
        /// \param [in] c   the new number of columns
        void setNumColumns ( int c );

        /// \brief Called when the user clicks the calibrate button on the lefthand menu.
        /// 
        /// Initiates the calibration method on the PcSystem singleton.
        void calibrateCameras ();
        
    private:
        /// \brief Draw one cell of the grid.
        /// 
        /// Given a frame image and its position on the grid, uploads the frame image into the textre memory
        /// and draw a single textured rectangle on a separate GL viewport, using the frame image as texture.
        /// 
        /// Also, draws a small square on the top-right corner of the image to indicate the synchronisation status. A red square indicates
        /// PTP synchronisation is either disabled or has failed, a yellow square indicated the synchronisation is in progress, a green
        /// square indicates the master camera and a blue square indicares slave cameras.
        /// 
        /// Additionaly, writes the calibration progress over the image.
        /// 
        /// \param [in] row             the row of the grid cell
        /// \param [in] col             the column of the grid cell
        /// \param [in] frame           the frame to be displayed
        /// \param [in] cameraStatus    the status of the PTP synchronisation
        /// \param [in] progress        the progress of the calibration
        void DrawFrame ( int const& row, int const& col, pcc::PcFramePtr const& frame, std::string const& cameraStatus, float const& progress );

        /// \brief Adjusts the number of rows on the grid to match the number of elements that need to be displayed.
        /// 
        /// Recalculates the number of rows on the grid using the following formula:
        ///     \f[m\_nRows = \frac{nElems}{m\_nCols}+1\f]
        /// 
        /// \param [in] nElems  the new number of elements on the grid
        void AdjustGrid ( const int& nElems );

    private:
        int         m_nRows;        ///< The number of rows of the display grid.
        int         m_nCols;        ///< The number of columns of the display grid.
        int         m_width;        ///< The width of the QGLWidget.
        int         m_height;       ///< The height of the QGLWidget.
        
        GLuint*     m_textures;     ///< The array of GL textures representing each frame image.

        QTimer*     m_updateTimer;  ///< The timer to automatically update the QGLWidget.
    };
}

#endif // PCFRAMEVIEWER_H
