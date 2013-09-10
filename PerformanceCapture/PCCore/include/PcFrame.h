#ifndef PCFRAME_H
#define PCFRAME_H

#include "PcExport.h"

#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>

namespace pcc
{
    class PcFrame
    {
    private:
        typedef boost::mutex MutexType;

    public:
        // ctors {
        PCCORE_EXPORT PcFrame ();
        PCCORE_EXPORT PcFrame (
            unsigned int const&     iWidth,
            unsigned int const&     iHeight,
            unsigned int const&     iNumChannels,
            unsigned char const*&   iData
        );
        // }
    
        // dtor {
        PCCORE_EXPORT ~PcFrame ();
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
        PCCORE_EXPORT PcFrame (
            PcFrame const&      iOther
        );
        PCCORE_EXPORT PcFrame& operator= (
            PcFrame const&      iOther
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
        MutexType                   m_mutex;
    
        // Data containers
        cv::gpu::GpuMat             m_gpuImage;
        cv::Mat                     m_cpuImage;
    };

    typedef boost::shared_ptr<PcFrame> PcFramePtr;
}

#endif // PCFRAME_H