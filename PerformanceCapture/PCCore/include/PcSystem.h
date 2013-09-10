#ifndef PCSYSTEM_H
#define PCSYSTEM_H

#include "PcCommon.h"
#include "PcExport.h"
#include "PcFrame.h"
#include "PcCamera.h"
#include "PcStereoCameraPair.h"

#include <unordered_map>
#include <map>
#include <queue>

#include <VimbaCPP/Include/VimbaCPP.h>
#include <opencv2/opencv.hpp>

using namespace AVT;

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>

/// The main namespace of the PerformanceCapture core library.
namespace pcc
{
    class PcSystem
        :   public VmbAPI::ICameraListObserver
    {
    private:
        typedef boost::mutex                    MutexType;
        typedef boost::lock_guard<MutexType>    GuardType;

    protected:
        PcSystem ();
        explicit PcSystem ( PcSystem const& iOther ) {}
    
    public:    
        ~PcSystem ();

    public:
        PCCORE_EXPORT static PcSystem& GetInstance ();
        PCCORE_EXPORT static void DestroyInstance ();

        PCCORE_EXPORT void UpdateCameras ();
        PCCORE_EXPORT void StartCapture ();
        PCCORE_EXPORT void EndCapture ();
        PCCORE_EXPORT unsigned int GetNumFrames ();
        PCCORE_EXPORT VEC(std::string) GetCameraList ();
        PCCORE_EXPORT PcFramePtr const GetFrameFromCamera ( std::string const& iCameraId );
        PCCORE_EXPORT std::string GetCameraStatus ( std::string const& iCameraId );
        PCCORE_EXPORT double GetCameraCalibrationProgress ( std::string const& iCameraId );
        PCCORE_EXPORT void CameraListChanged ( VmbAPI::CameraPtr iCamera, VmbAPI::UpdateTriggerType iUpdateReason );
    
        PCCORE_EXPORT int GetMaxPerCameraBandwidth ();

        PCCORE_EXPORT void Setup ();

        PCCORE_EXPORT void SynchroniseCameras ();
        PCCORE_EXPORT void CalibrateCameras ();

        void SetFrame ( VmbAPI::CameraPtr const& iCamera, VmbAPI::FramePtr const& iFrame );

    private:
        void RegisterCamera ( std::string const& iCameraId, VmbAPI::CameraPtr const& iCamera );
        void UnregisterCamera ( std::string const& iCameraId );

    private:
        static VmbAPI::ICameraListObserverPtr           sm_pInstance;
        static int                                      sm_nAvailableCameras;

        MutexType*                                      m_mutex;
        QUEUE(std::string)                              m_removeQueue;
        QUEUE(VmbAPI::CameraPtr)                        m_addQueue;

        STRMAP(PcCameraPtr)                             m_activeCameras;
        STRMAP(PcFramePtr)                              m_frames;

        VEC(PcStereoCameraPairPtr)                      m_stereo;
    };
}

#endif // PCSYSTEM_H
