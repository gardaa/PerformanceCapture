#ifndef PCCORESTEREOCAMERAPAIR_H
#define PCCORESTEREOCAMERAPAIR_H

#include <string>

class PCCoreCamera;
class PCCoreFrame;
namespace cv {
    namespace gpu {
        class GpuMat;
    }
    class Mat;
}

class PCCoreStereoCameraPair
{
public:
    PCCoreStereoCameraPair ();
    ~PCCoreStereoCameraPair ();

    void Calibrate ();

private:
    unsigned int*                   m_refCount;

    PCCoreCamera*                   m_left;
    PCCoreCamera*                   m_right;
    
    cv::Mat*                        m_stereoRotation;
    cv::Mat*                        m_stereoTranslation;
    cv::Mat*                        m_stereoEssential;
    cv::Mat*                        m_stereoFundamental;
};

#endif // PCCORESTEREOCAMERAPAIR_H