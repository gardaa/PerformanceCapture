#ifndef PCCORESYSTEM_H
#define PCCORESYSTEM_H

#include "PCCoreExport.h"

#include <unordered_map>
#include <map>
#include <queue>

#include <VimbaCPP/Include/VimbaCPP.h>
#include <opencv2/opencv.hpp>

using namespace AVT::VmbAPI;

#include "PCCoreFrame.h"
#include "PCCoreCamera.h"

namespace boost {
    class mutex;
}

class PCCoreSystem
    :   public ICameraListObserver
{
protected:
    PCCoreSystem ();
    explicit PCCoreSystem ( PCCoreSystem const& iOther ) {}
    
public:    
    ~PCCoreSystem ();

public:
    PCCORE_EXPORT static PCCoreSystem& GetInstance ();
    PCCORE_EXPORT static void DestroyInstance ();

    PCCORE_EXPORT void UpdateCameras ();
    PCCORE_EXPORT void StartCapture ();
    PCCORE_EXPORT void EndCapture ();
    PCCORE_EXPORT unsigned int GetNumFrames ();
    PCCORE_EXPORT std::vector<std::string> GetCameraList ();
    PCCORE_EXPORT PCCoreFrame const& GetFrameFromCamera ( std::string const& iCameraId );
    PCCORE_EXPORT std::string GetCameraStatus ( std::string const& iCameraId );

    PCCORE_EXPORT void CameraListChanged ( CameraPtr iCamera, UpdateTriggerType iUpdateReason );
    
    PCCORE_EXPORT int GetMaxPerCameraBandwidth ();

    PCCORE_EXPORT void Setup ();

    PCCORE_EXPORT void SynchroniseCameras ();
    PCCORE_EXPORT void CalibrateCameras ();

    void SetFrame ( CameraPtr const& iCamera, FramePtr const& iFrame );

//public:
//    PCCORE_EXPORT void ScheduleRegisterCameraById ( std::string const& iCameraId );
//    PCCORE_EXPORT void ScheduleUnregisterCameraById ( std::string const& iCameraId );

private:
    void RegisterCamera ( std::string const& iCameraId, CameraPtr const& iCamera );
    void UnregisterCamera ( std::string const& iCameraId );

private:
    static ICameraListObserverPtr                       sm_pInstance;
    static int                                          sm_nAvailableCameras;

    boost::mutex*                                       m_mutex;
    std::queue<std::string>                             m_removeQueue;
    std::queue<CameraPtr>                               m_addQueue;

    std::map<std::string, PCCoreCamera>       m_activeCameras;
    std::map<std::string, PCCoreFrame>        m_frames;

    unsigned int    m_frameCount;
    unsigned int    m_lastCalibrationFrame;
};

#endif // PCCORESYSTEM_H
