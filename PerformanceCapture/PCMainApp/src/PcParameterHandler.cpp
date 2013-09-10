#include "PcParameterHandler.h"

const int& PcParameterHandler::GetRefreshTimeout ()
{
    return m_refreshTimeout;
}
void PcParameterHandler::SetRefreshTimeout ( const int& iTimeout )
{
    m_refreshTimeout = iTimeout;

    emit refreshTimeoutChanged ( iTimeout );
}

const int& PcParameterHandler::GetViewportCols ()
{
    return m_viewportCols;
}
void PcParameterHandler::SetViewportCols ( const int& iViewportCols )
{
    m_viewportCols = iViewportCols;

    emit numColsChanged ( iViewportCols );
}

PcParameterHandler::PcParameterHandler ()
    :   QObject ()
    ,   m_refreshTimeout ( 16 )
    ,   m_viewportCols ( 2 )
{}
