#ifndef COREFRAMEBUFFER_H
#define COREFRAMEBUFFER_H

#include <VimbaCPP/Include/VimbaCPP.h>

#include "PCCoreExport.h"

using namespace AVT::VmbAPI;

class CoreFrameObserver;

class CoreFrameBuffer
{
protected:
    CoreFrameBuffer();
    ~CoreFrameBuffer();

public:
    PCCORE_EXPORT static CoreFrameBuffer& GetInstance ();
    PCCORE_EXPORT static void DestroyInstance ();

    PCCORE_EXPORT void ReadSingleFrame ();
    PCCORE_EXPORT void StartCapture ();
    PCCORE_EXPORT void EndCapture ();
    PCCORE_EXPORT const FramePtrVector& GetAllFrames () const;
    PCCORE_EXPORT FramePtr const GetFrameFromCamera ( const unsigned int& iCameraId ) const;

    void SetFrame ( const unsigned int& iCamId, FramePtr iFrame );

private:
    static CoreFrameBuffer*             sm_pInstance;

    CameraPtrVector                     m_cameras;

    std::vector<FramePtrVector>         m_camFrames;
    FramePtrVector                      m_data;

    std::vector<IFrameObserverPtr>      m_observers;
};

#endif // COREFRAMEBUFFER_H
