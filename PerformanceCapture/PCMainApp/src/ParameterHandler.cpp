#include "ParameterHandler.h"

const int& ParameterHandler::GetRefreshTimeout ()
{
    return m_refreshTimeout;
}
void ParameterHandler::SetRefreshTimeout ( const int& iTimeout )
{
    m_refreshTimeout = iTimeout;

    emit refreshTimeoutChanged ( iTimeout );
}

const int& ParameterHandler::GetViewportCols ()
{
    return m_viewportCols;
}
void ParameterHandler::SetViewportCols ( const int& iViewportCols )
{
    m_viewportCols = iViewportCols;

    emit numColsChanged ( iViewportCols );
}

ParameterHandler::ParameterHandler ()
    :   QObject ()
    ,   m_refreshTimeout ( 16 )
    ,   m_viewportCols ( 2 )
{}
