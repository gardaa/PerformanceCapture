#ifndef PCMAINWINDOW_H
#define PCMAINWINDOW_H

#include <QMainWindow>

class QTimer;

namespace pcm {

    class PcFrameViewer;
    class PcAppMenu;

    /// \ingroup    PCMAIN
    /// 
    /// \brief The visualisation module main window.
    ///
    /// This class implements the main window of the visualisation module, containing
    /// one instance of the application menu and one instance of the frame viewer.
    class PcMainWindow
        :   public QMainWindow
    {
        Q_OBJECT

    public:
        /// \brief Default constructor.
        /// 
        /// Initializes the underlying VimbaSystem, instantiates the main frame viewer and
        /// application menu.
        /// 
        /// Additionaly, performs signal-slot connections between the menu and the frame viewer:
        /// 
        /// \param [in] iParent the owner of this QMainWindow
        PcMainWindow ( QWidget* iParent = 0 );
        ~PcMainWindow();

    private:
        // Widgets
        PcFrameViewer*      m_frameViewer;  ///< The frame display instance.
        PcAppMenu*          m_leftMenu;     ///< The app menu instance.
    };
}

#endif // PCMAINWINDOW_H
