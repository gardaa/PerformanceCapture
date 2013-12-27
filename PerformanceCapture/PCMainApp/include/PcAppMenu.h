#ifndef PCAPPMENU_H
#define PCAPPMENU_H

#include <QDockWidget>

class QSlider;
class QGroupBox;
class QPushButton;

namespace pcm
{
    /// \ingroup    PCMAIN
    /// 
    /// \brief The menu used to control the application.
    /// 
    /// Implements the control interface for the visualisation module.
    /// Contains all the buttons and settings with which the user can interact.
    class PcAppMenu :
        public QDockWidget
    {
        Q_OBJECT

    signals:
        /// \brief The Qt signal emitted when the calibration function is called.
        void calibrationClicked ();

        /// \brief The Qt signal emitted when the synchronisation function is called.
        void synchronisationClicked ();

    public slots:
        /// \brief The Qt slot called when the calibration button is clicked. Emits the calibrationClicked signal.
        void onCalibrationClicked ();

        /// \brief The Qt slot called when the synchronisation button is clicked. Emits the synchronisationClicked signal.
        void onSynchronisationClicked ();

    public:
        /// \brief The constructor.
        /// 
        /// Instantiates and initialises the app menu.
        ///
        /// \param iParent a pointer to the QWidget instance that owns the menu
        explicit PcAppMenu ( QWidget* iParent = 0 );

    private:
        // Main container
        QWidget*        m_mainWidget;               ///< The container of all the Qt layout objects.

        // Group boxes
        QGroupBox*      m_settingsWidget;           ///< A box containing the settings interface.
        QGroupBox*      m_controlsWidget;           ///< A box containing the controller buttons.

        // Buttons
        QPushButton*    m_calibrationButton;        ///< The calibration button.
        QPushButton*    m_synchronisationButton;    ///< The synchronisation button.

        // Components
        QSlider*        m_numColumns;               ///< A slider to select the number of columns displayed in the interface.
    };
}

#endif // PCAPPMENU_H
