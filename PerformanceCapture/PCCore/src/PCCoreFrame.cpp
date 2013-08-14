#include "PCCoreFrame.h"

#include "PCCoreErrChk.h"
#include "PCCoreCommon.h"

PCCoreFrame::PCCoreFrame ()
    :   m_gpuImage ()
    ,   m_cpuImage ( 1080, 1920, CV_8UC1, cv::Scalar(0) )
    ,   m_mutex ()
{}

PCCoreFrame::PCCoreFrame (
    unsigned int const&     iWidth,
    unsigned int const&     iHeight,
    unsigned int const&     iNumChannels,
    unsigned char const*&   iData
)   :   m_gpuImage ()
    ,   m_cpuImage ( iHeight, iWidth, CV_8UC(iNumChannels) )
    ,   m_mutex ()
{
    memcpy ( m_cpuImage.data, iData, iWidth * iHeight * iNumChannels * sizeof ( unsigned char ) );

    m_gpuImage.upload ( m_cpuImage );
}

//PCCoreFrame::PCCoreFrame (
//    PCCoreFrame const& iOther
//) {
//    //++(*(iOther.m_refCount));
//    //m_refCount = iOther.m_refCount;
//    //m_gpuImage = iOther.m_gpuImage;
//    //m_cpuImage = iOther.m_cpuImage;
//}

PCCoreFrame::~PCCoreFrame ()
{
    //if ( m_refCount && --(*m_refCount) == 0u ) {
    //    PCC_OBJ_FREE ( m_gpuImage );
    //    PCC_OBJ_FREE ( m_cpuImage );
    //    PCC_OBJ_FREE ( m_refCount );
    //}
}

void PCCoreFrame::Reset (
    unsigned int const&     iWidth,
    unsigned int const&     iHeight,
    unsigned int const&     iNumChannels,
    unsigned char const*&   iData
) {
    m_cpuImage.create ( iHeight, iWidth, CV_8UC(iNumChannels) );

    size_t size = m_cpuImage.dataend - m_cpuImage.datastart;

    memcpy ( m_cpuImage.data, iData, size * sizeof ( unsigned char ) );
    m_gpuImage.upload ( m_cpuImage );
}

int const& PCCoreFrame::Width () const
{
    return m_cpuImage.cols;
}

int const& PCCoreFrame::Height () const
{
    return m_cpuImage.rows;
}

void PCCoreFrame::GetData (
    unsigned char const*&   oData,
    unsigned int&           oNumChannels
) const {
    oData = m_cpuImage.data;
    oNumChannels = m_cpuImage.channels ();
}

//PCCoreFrame& PCCoreFrame::operator= (
//    PCCoreFrame const&      iOther
//) {
//    if ( this != &iOther ) {
//        this->~PCCoreFrame ();
//
//        ++(*(iOther.m_refCount));
//        m_refCount = iOther.m_refCount;
//        m_gpuImage = iOther.m_gpuImage;
//        m_cpuImage = iOther.m_cpuImage;
//    }
//    return (*this);
//}