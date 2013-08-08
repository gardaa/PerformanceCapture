#include "PCCoreFrame.h"

#include "PCCoreErrChk.h"
#include "PCCoreCommon.h"

#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

PCCoreFrame::PCCoreFrame ()
    :   m_refCount ( new unsigned int ( 1u ) ),
        m_gpuImage ( new cv::gpu::GpuMat () ),
        m_cpuImage ( new cv::Mat ( 1080, 1920, CV_8UC1, cv::Scalar(0) ) )
{}

PCCoreFrame::PCCoreFrame (
    unsigned int const&     iWidth,
    unsigned int const&     iHeight,
    unsigned int const&     iNumChannels,
    unsigned char const*&   iData
)   :   m_refCount ( new unsigned int ( 1u ) ),
        m_gpuImage ( new cv::gpu::GpuMat () ),
        m_cpuImage ( new cv::Mat ( iHeight, iWidth, CV_8UC(iNumChannels) ) )
{
    memcpy ( m_cpuImage->data, iData, iWidth * iHeight * iNumChannels * sizeof ( unsigned char ) );

    m_gpuImage->upload ( *m_cpuImage );
}

PCCoreFrame::PCCoreFrame (
    PCCoreFrame const& iOther
) {
    ++(*(iOther.m_refCount));
    m_refCount = iOther.m_refCount;
    m_gpuImage = iOther.m_gpuImage;
    m_cpuImage = iOther.m_cpuImage;
}

PCCoreFrame::~PCCoreFrame ()
{
    if ( m_refCount && --(*m_refCount) == 0u ) {
        PCC_OBJ_FREE ( m_gpuImage );
        PCC_OBJ_FREE ( m_cpuImage );
        PCC_OBJ_FREE ( m_refCount );
    }
}

void PCCoreFrame::Reset (
    unsigned int const&     iWidth,
    unsigned int const&     iHeight,
    unsigned int const&     iNumChannels,
    unsigned char const*&   iData
) {
    m_cpuImage->create ( iHeight, iWidth, CV_8UC(iNumChannels) );

    size_t size = m_cpuImage->dataend - m_cpuImage->datastart;

    memcpy ( m_cpuImage->data, iData, size * sizeof ( unsigned char ) );
    m_gpuImage->upload ( *m_cpuImage );
}

int const& PCCoreFrame::Width () const
{
    return m_cpuImage->cols;
}

int const& PCCoreFrame::Height () const
{
    return m_cpuImage->rows;
}

void PCCoreFrame::GetData (
    unsigned char const*&   oData,
    unsigned int&           oNumChannels
) const {
    oData = m_cpuImage->data;
    oNumChannels = m_cpuImage->channels ();
}

cv::Mat const& PCCoreFrame::GetImagePoints () const
{
    return (*m_cpuImage);
}

//void PCCoreFrame::FrameReceived ( FramePtr iFrame )
//{
//    VmbFrameStatusType status;
//    iFrame->GetReceiveStatus ( status );
//
//    if ( status == VmbFrameStatusComplete ) {
//        VmbErrorType err;
//
//        VmbUint32_t height;
//        err = iFrame->GetHeight (height);
//        ERR_CHK ( err, VmbErrorSuccess, "Error reading frame height." );
//
//        VmbUint32_t width;
//        err = iFrame->GetWidth (width);
//        ERR_CHK ( err, VmbErrorSuccess, "Error reading frame width." );
//
//        VmbUchar_t* frameData;
//        err = iFrame->GetImage(frameData);
//        ERR_CHK ( err, VmbErrorSuccess, "Error reading frame image data." );
//
//        Reset ( width, height, 1, frameData );
//    }
//
//    m_pCamera->QueueFrame ( iFrame );
//}

PCCoreFrame& PCCoreFrame::operator= (
    PCCoreFrame const&      iOther
) {
    if ( this != &iOther ) {
        this->~PCCoreFrame ();

        ++(*(iOther.m_refCount));
        m_refCount = iOther.m_refCount;
        m_gpuImage = iOther.m_gpuImage;
        m_cpuImage = iOther.m_cpuImage;
    }
    return (*this);
}