#ifndef PCCOREFRAME_H
#define PCCOREFRAME_H

#include "PCCoreExport.h"

namespace cv {
class Mat;
namespace gpu {
class GpuMat;
}}

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
    PCCORE_EXPORT cv::Mat const& GetImagePoints () const;
    // }

    // Copy operations {
    PCCORE_EXPORT PCCoreFrame (
        PCCoreFrame const&      iOther
    );
    PCCORE_EXPORT PCCoreFrame& operator= (
        PCCoreFrame const&      iOther
    );
    // }

    void Reset (
        unsigned int const&     iWidth,
        unsigned int const&     iHeight,
        unsigned int const&     iNumChannels,
        unsigned char const*&   iData
    );

private:
    // Reference counting for memory handling
    unsigned int*               m_refCount;

    // Data containers
    cv::gpu::GpuMat*            m_gpuImage;
    cv::Mat*                    m_cpuImage;
};

#endif // PCCOREFRAME_H