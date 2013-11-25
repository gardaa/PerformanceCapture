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
        void numColsChanged (int);
        void refreshTimeoutChanged (int);

    public slots:
        void SetRefreshTimeout ( const int& iTimeout );
        void SetViewportCols ( const int& iViewportCols );

    public:
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
        explicit PcParameterHandler ( PcParameterHandler const& iOther ) {}

    private:
        int                     m_refreshTimeout;   ///< The refresh rate of the screen in milisseconds.
        int                     m_viewportCols;     ///< The number of columns to be used on the frame viewer grid.
    };
}

#endif // PCPARAMETERHANDLER_H
