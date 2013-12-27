#ifndef PCPARAMETERHANDLER_H
#define PCPARAMETERHANDLER_H

#include <QObject>

namespace pcm
{
    
    /// \ingroup    PCMAIN
    /// 
    /// \brief Interface for easier manipulation of customisation parameters. Singleton.
    /// 
    /// Keeps track of variable parameters and provides an interface for internal classes
    /// to ask for the values of those parameters.
    class PcParameterHandler : public QObject
    {
        Q_OBJECT

    public:
        /// \brief Read the screen refresh rate.
        /// \return a constant reference to the variable representing the screen refresh rate
        const int& GetRefreshTimeout ();

        /// \brief Read the number of columns of the frame viewer grid.
        /// \return a constant reference to the variable containing the number of columns for the frame viewer grid
        const int& GetViewportCols ();

    signals:
        /// \brief Signals listeners that the number of columns has been changed.
        void numColsChanged (int);

        /// \brief Signals listeners that the screen refresh rate has changed.
        void refreshTimeoutChanged (int);

    public slots:
        /// \brief Changes the screen refresh timeout.
        /// \param [in] iTimeout    the new time to wait between screen refreshes.
        void SetRefreshTimeout ( const int& iTimeout );

        /// \brief Changes the number of columns on the viewport grid.
        /// \param [in] iViewportCols   the new number of columns on the grid.
        void SetViewportCols ( const int& iViewportCols );

    public:
        /// \brief Gets the singleton instance of the parameter handler.
        ///
        /// Allocates and returns a new static instance of the parameter handler.
        ///
        /// \return the singleton instance of the parameter handler.
        static inline PcParameterHandler& Instance () {
            static PcParameterHandler _instance;
            
            return _instance;
        }

    private:
        /// \brief Default constructor.
        /// 
        /// Initialises variables to their default values:
        ///     - Refresh rate of 16ms
        ///     - 2 columns on the grid
        PcParameterHandler ();

        /// \brief Private copy constructor.
        ///
        /// Prevents copy of the ParameterHandler singleton object.
        ///
        /// \param [in] iOther  the instance from where to copy data.
        explicit PcParameterHandler ( PcParameterHandler const& iOther ) {}

    private:
        int                     m_refreshTimeout;   ///< The refresh rate of the screen in milisseconds.
        int                     m_viewportCols;     ///< The number of columns to be used on the frame viewer grid.
    };
}

#endif // PCPARAMETERHANDLER_H
