#include "ParameterHandler.h"

int ParameterHandler::m_refreshTimeout = 16;
const int& ParameterHandler::GetRefreshTimeout ()
{
    return m_refreshTimeout;
}
void ParameterHandler::SetRefreshTimeout ( const int& iTimeout )
{
    m_refreshTimeout = iTimeout;
}

int ParameterHandler::m_viewportCols = 2;
const int& ParameterHandler::GetViewportCols ()
{
    return m_viewportCols;
}
void ParameterHandler::SetViewportCols ( const int& iViewportCols )
{
    m_refreshTimeout = iViewportCols;
}
