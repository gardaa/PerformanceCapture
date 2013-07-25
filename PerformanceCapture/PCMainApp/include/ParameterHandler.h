#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

class ParameterHandler {
public:
    static const int& GetRefreshTimeout ();
    static void SetRefreshTimeout ( const int& iTimeout );

    static const int& GetViewportCols ();
    static void SetViewportCols ( const int& iViewportCols );

private:
    static int          m_refreshTimeout;
    static int          m_viewportCols;
};

#endif // PARAMETERHANDLER_H
