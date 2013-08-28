#ifndef PCCAMERACALIBRATION_H
#define PCCAMERACALIBRATION_H

#include "PcCommon.h"
#include "PcFrame.h"

namespace pcc {
class PcCamera;
}

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/function.hpp>

#include <queue>
#include <vector>

namespace pcc
{
    enum CalibrationState {
        UNKNOWN     =   0x00,
        ACQUIRING   =   0x01,
        CALIBRATING =   0x02,
        CALIBRATED  =   0x03,
    };

    class PcCameraCalibration {
    private:
        typedef boost::thread                                                           ThreadType;
        typedef boost::shared_mutex                                                     MutexType;
        typedef boost::upgrade_lock<MutexType>                                          UpgradeLockType;
        typedef boost::upgrade_to_unique_lock<MutexType>                                UniqueLockType;
        typedef boost::function3<void, PcCamera*, CalibrationState, CalibrationState>   CallbackFn;

    public:
        PcCameraCalibration ( PcCamera* iParent );
        ~PcCameraCalibration ();

        void StartCalibration ();
        void AbortCalibration ();
        void Process ();
        void PushFrame ( PcFramePtr const& iFrame );
        CalibrationState GetCalibrationState ();
        double GetCalibrationProgress () const;
        VECOFVECS(cv::Point2f) const& GetChessboardCorners () const { return m_corners; }

        inline void RegisterCallback ( void (*iFunc)(PcCamera*, CalibrationState, CalibrationState) )
        {
            m_listeners.push_back ( CallbackFn ( iFunc ) );
        }

        template<typename T>
        inline void RegisterCallback ( T* iObj, void (T::*iFunc)(PcCamera*, CalibrationState, CalibrationState) )
        {
            m_listeners.push_back ( CallbackFn ( boost::bind ( iFunc, iObj, _1, _2, _3 ) ) );
        }

    private:
        PcCameraCalibration ( PcCameraCalibration const& iOther) {}
        PcCameraCalibration& operator= ( PcCameraCalibration const& iOther ) {}

        void DoStartCalibration ();
        void DoAbortCalibration ();
        void SetCalibrationState ( CalibrationState const& iNewState );

    private:
        MutexType                       m_mutex;
        MutexType                       m_stateMutex;
        CalibrationState                m_calibState;
        ThreadType*                     m_calibThread;
        QUEUE(cv::Mat)                  m_frameQueue;
        VEC(cv::Mat)                    m_frameList;
        VECOFVECS(cv::Point2f)          m_corners;

        PcCamera*                       m_camera;

        unsigned int                    m_count;
        VEC(CallbackFn)                 m_listeners;
    };
}

#endif // PCCAMERACALIBRATION_H
