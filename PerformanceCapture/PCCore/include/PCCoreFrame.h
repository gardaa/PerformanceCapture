#ifndef PCCOREFRAME_H
#define PCCOREFRAME_H

#include "PCCoreExport.h"

#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

class PCCoreFrame
{
public:
    // ctors {
    PCCORE_EXPORT PCCoreFrame ();
    PCCORE_EXPORT PCCoreFrame (
        unsigned int const&     iWidth,
        unsigned int const&     iHeight,
        unsigned int const&     iNumChannels,
        unsigned char const*&   iData
    );
    // }
    
    // dtor {
    PCCORE_EXPORT ~PCCoreFrame ();
    // }

    // Accessors {
    PCCORE_EXPORT int const& Width () const;
    PCCORE_EXPORT int const& Height () const;

    PCCORE_EXPORT void GetData (
        unsigned char const*&   oData,
        unsigned int&           oNumChannels
    ) const;
    PCCORE_EXPORT cv::Mat const& GetImagePoints () const { return (m_cpuImage); }
    // }

private:
    // Copy operations {
    PCCORE_EXPORT PCCoreFrame (
        PCCoreFrame const&      iOther
    );
    PCCORE_EXPORT PCCoreFrame& operator= (
        PCCoreFrame const&      iOther
    );
    // }

public:
    void Reset (
        unsigned int const&     iWidth,
        unsigned int const&     iHeight,
        unsigned int const&     iNumChannels,
        unsigned char const*&   iData
    );

    void Lock () { m_mutex.lock (); }
    void Unlock () { m_mutex.unlock (); }

private:
    boost::mutex                m_mutex;
    
    // Data containers
    cv::gpu::GpuMat             m_gpuImage;
    cv::Mat                     m_cpuImage;
};

typedef boost::shared_ptr<PCCoreFrame> PCCoreFramePtr;

#endif // PCCOREFRAME_H