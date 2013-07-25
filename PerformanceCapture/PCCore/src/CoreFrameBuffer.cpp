#include "CoreFrameBuffer.h"
#include "CoreFrameObserver.h"
#include "ErrChk.h"

#define BOOST_ALL_DYN_LINK
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace AVT::VmbAPI;

// Maximum total allowed bandwidth in bytes
const unsigned int MAX_BW = 124000000;

CoreFrameBuffer* CoreFrameBuffer::sm_pInstance = (CoreFrameBuffer*)0x0;

CoreFrameBuffer::CoreFrameBuffer()
{
    VmbErrorType err;

    VimbaSystem& vmbs = VimbaSystem::GetInstance ();

    err = vmbs.GetCameras ( m_cameras );
    ERR_CHK ( err, VmbErrorSuccess, "Error getting camera list." );

    m_data.resize ( m_cameras.size () );
    m_observers.resize ( m_cameras.size () );
    m_camFrames.resize ( m_cameras.size () );

    const VmbInt32_t maxCameraBandwidth = MAX_BW / m_cameras.size ();
    std::cout << m_cameras.size () << " cameras found" << std::endl;
    std::cout << "Maximum per-camera bandwidth: " << maxCameraBandwidth << std::endl;
    for ( unsigned int i = 0; i < m_cameras.size (); i++ ) {
        VmbInt64_t payload;
        FeaturePtr f;
        CameraPtr camera = m_cameras[i];
        err = camera->Open ( VmbAccessModeFull );
        
        err = camera->GetFeatureByName ( "PixelFormat", f );
        err = f->SetValue ( VmbPixelFormatMono8 );

        err = camera->GetFeatureByName ( "StreamBytesPerSecond", f );
        err = f->SetValue ( maxCameraBandwidth );

        err = camera->GetFeatureByName ( "GVSPPacketSize", f );
        err = f->SetValue ( 1500 );

        err = camera->GetFeatureByName ( "PayloadSize", f );
        err = f->GetValue ( payload );
        std::cout << "Camera Payload " << payload << std::endl;

        FramePtrVector frameList;
        frameList.resize ( 3 );

        IFrameObserverPtr observer ( new CoreFrameObserver ( i, camera ) );
        for ( unsigned int j = 0; j < 3; j++ ) {
            FramePtr newFrame ( new Frame ( payload ) );
            
            err = newFrame->RegisterObserver ( observer );

            err = camera->AnnounceFrame ( newFrame );

            frameList[j] = newFrame;
        }
        m_camFrames[i] = frameList;
        m_observers[i] = observer;
    }
}

CoreFrameBuffer::~CoreFrameBuffer ()
{
    for ( unsigned int i = 0; i < m_cameras.size (); i++ ) {
        m_cameras[i]->Close ();
    }
}

CoreFrameBuffer& CoreFrameBuffer::GetInstance ()
{
    if ( sm_pInstance == 0x0 ) {
        sm_pInstance = new CoreFrameBuffer ();
    }
    return *sm_pInstance;
}
void CoreFrameBuffer::DestroyInstance ()
{
    if ( sm_pInstance ) {
        delete sm_pInstance;
        sm_pInstance = (CoreFrameBuffer*)0x0;
    }
}

void CoreFrameBuffer::ReadSingleFrame ()
{
    for ( unsigned int i = 0; i < m_cameras.size (); i++ ) {
        m_cameras[i]->AcquireSingleImage ( m_data[i], 1000 );
    }
}

void CoreFrameBuffer::StartCapture ()
{
    VmbErrorType err;
    for ( unsigned int i = 0; i < m_cameras.size (); i++ ) {
        FeaturePtr acquisitionStartCmd;
        FeaturePtr acquisitionMode;

        CameraPtr camera = m_cameras[i];
        err = camera->StartCapture ();
        ERR_CHK ( err, VmbErrorSuccess, "Error starting capture" );

        for ( FramePtrVector::iterator iter = m_camFrames[i].begin ();
            m_camFrames[i].end() != iter;
            ++iter )
            {
            err = camera-> QueueFrame( *iter );
            ERR_CHK ( err, VmbErrorSuccess, "Error queueing frame" );
        }

        //err = camera->GetFeatureByName( "AcquisitionMode", acquisitionMode );
        //err = acquisitionMode->SetValue ( "Continuous" );
        //ERR_CHK ( err, VmbErrorSuccess, "Error setting camera acquisition mode" );

        err = camera->GetFeatureByName( "AcquisitionStart", acquisitionStartCmd );
        err = acquisitionStartCmd->RunCommand ();
        ERR_CHK ( err, VmbErrorSuccess, "Error starting acquisition" );
    }
}

void CoreFrameBuffer::EndCapture () {
    VmbErrorType err;
    for ( unsigned int i = 0; i < m_cameras.size (); i++ ) {
        FeaturePtr acquisitionEndCmd;

        CameraPtr camera = m_cameras[i];

        //err = camera->FlushQueue ();

        err = camera->EndCapture ();

        //err = camera->GetFeatureByName( "AcquisitionEnd", acquisitionEndCmd );
        //err = acquisitionEndCmd->RunCommand ();
    }
}

const FramePtrVector& CoreFrameBuffer::GetAllFrames () const
{
    return m_data;
}
FramePtr const CoreFrameBuffer::GetFrameFromCamera ( const unsigned int& iCameraId ) const
{
    return m_data[iCameraId];
}

void CoreFrameBuffer::SetFrame ( const unsigned int& iCamId, FramePtr iFrame )
{
    m_data[iCamId] = iFrame;
}