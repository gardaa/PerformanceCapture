#include "PcPtpSyncAgent.h"
#include "PcCamera.h"

using namespace pcc;

boost::mutex PcPtpSyncAgent::sm_mutex;

void PcPtpSyncAgent::FeatureChanged ( VmbAPI::FeaturePtr const& pFeature ) {
    boost::lock_guard<boost::mutex> lock (sm_mutex);

    //std::string sPtpStatus;
    //m_camera->TryGetFeature ( "PtpStatus", sPtpStatus, false );

    //std::cout << m_camera->GetID() << " PtpStatus changed: " << sPtpStatus << std::endl;

    //m_camera->SetPtpStatus ( sPtpStatus );
    //bool verbose = true;
    //if ( ( sPtpStatus.compare ( "Slave" ) == 0 ) || ( sPtpStatus.compare ( "Master" ) == 0 ) ) {
    //    m_camera->SetSynced ( true );
    //} else if ( sPtpStatus.compare ( "Error" ) == 0 ) {
    //    m_camera->SetSynced ( false );
    //}
}