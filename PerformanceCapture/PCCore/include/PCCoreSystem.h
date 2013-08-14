#ifndef PCCORESYSTEM_H
#define PCCORESYSTEM_H

#include "PCCoreExport.h"
#include "PCCoreFrame.h"
#include "PCCoreCamera.h"
#include "PCCoreStereoCameraPair.h"

#include <unordered_map>
#include <map>
#include <queue>

#include <VimbaCPP/Include/VimbaCPP.h>
#include <opencv2/opencv.hpp>

using namespace AVT::VmbAPI;


#include <boost/shared_ptr.hpp>

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
    PCCORE_EXPORT PCCoreFramePtr const GetFrameFromCamera ( std::string const& iCameraId );
    PCCORE_EXPORT std::string GetCameraStatus ( std::string const& iCameraId );
    PCCORE_EXPORT double GetCameraCalibrationProgress ( std::string const& iCameraId );
    PCCORE_EXPORT void CameraListChanged ( CameraPtr iCamera, UpdateTriggerType iUpdateReason );
    
    PCCORE_EXPORT int GetMaxPerCameraBandwidth ();

    PCCORE_EXPORT void Setup ();

    PCCORE_EXPORT void SynchroniseCameras ();
    PCCORE_EXPORT void CalibrateCameras ();

    void SetFrame ( CameraPtr const& iCamera, FramePtr const& iFrame );

private:
    void RegisterCamera ( std::string const& iCameraId, CameraPtr const& iCamera );
    void UnregisterCamera ( std::string const& iCameraId );

private:
    static ICameraListObserverPtr                       sm_pInstance;
    static int                                          sm_nAvailableCameras;

    boost::mutex*                                       m_mutex;
    std::queue<std::string>                             m_removeQueue;
    std::queue<CameraPtr>                               m_addQueue;

    std::map<std::string, PCCoreCameraPtr>              m_activeCameras;
    std::map<std::string, PCCoreFramePtr>               m_frames;

    std::vector<PCCoreStereoCameraPairPtr>              m_stereo;
};

#endif // PCCORESYSTEM_H
