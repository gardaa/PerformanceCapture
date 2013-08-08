#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

#include <QObject>

//! Singleton
class ParameterHandler
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
    static inline ParameterHandler& Instance () {
        static ParameterHandler _instance;
        
        return _instance;
    }

private:
    ParameterHandler ();
    explicit ParameterHandler ( ParameterHandler const& iOther ) {}

private:
    int                     m_refreshTimeout;
    int                     m_viewportCols;
};

#endif // PARAMETERHANDLER_H
