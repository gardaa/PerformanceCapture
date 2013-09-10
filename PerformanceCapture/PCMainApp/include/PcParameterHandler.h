#ifndef PCPARAMETERHANDLER_H
#define PCPARAMETERHANDLER_H

#include <QObject>

/// Singleton
class PcParameterHandler
    :   public QObject
{
    Q_OBJECT

public:
    const int& GetRefreshTimeout ();
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
    PcParameterHandler ();
    explicit PcParameterHandler ( PcParameterHandler const& iOther ) {}

private:
    int                     m_refreshTimeout;
    int                     m_viewportCols;
};

#endif // PCPARAMETERHANDLER_H
