#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include "PCCoreFrame.h"

class PCCoreCamera;

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

    class CameraCalibration {
    private:
        typedef boost::function3<void, PCCoreCamera*, CalibrationState, CalibrationState> CallbackFn;

    public:
        CameraCalibration ( PCCoreCamera* iParent );
        ~CameraCalibration ();

        void StartCalibration ();
        void AbortCalibration ();
        void Process ();
        void PushFrame ( PCCoreFramePtr const& iFrame );
        CalibrationState GetCalibrationState ();
        double GetCalibrationProgress () const;
        std::vector< std::vector<cv::Point2f> > const& GetChessboardCorners () const { return m_corners; }

        inline void RegisterCallback ( void (*iFunc)(PCCoreCamera*, CalibrationState, CalibrationState) )
        {
            m_listeners.push_back ( CallbackFn ( iFunc ) );
        }

        template<typename T>
        inline void RegisterCallback ( T* iObj, void (T::*iFunc)(PCCoreCamera*, CalibrationState, CalibrationState) )
        {
            m_listeners.push_back ( CallbackFn ( boost::bind ( iFunc, iObj, _1, _2, _3 ) ) );
        }

    private:
        CameraCalibration ( CameraCalibration const& iOther) {}
        CameraCalibration& operator= ( CameraCalibration const& iOther ) {}

        void DoStartCalibration ();
        void DoAbortCalibration ();
        void SetCalibrationState ( CalibrationState const& iNewState );

    private:
        boost::shared_mutex                             m_mutex;
        boost::shared_mutex                             m_stateMutex;
        CalibrationState                                m_calibState;
        boost::thread*                                  m_calibThread;
        std::queue< cv::Mat >                           m_frameQueue;
        std::vector< cv::Mat >                          m_frameList;
        std::vector< std::vector<cv::Point2f> >         m_corners;

        PCCoreCamera*                                   m_camera;

        unsigned int                                    m_count;
        unsigned int                                    m_fuckupCount;
        std::vector<CallbackFn>                         m_listeners;
    };
}

#endif // CAMERACALIBRATION_H
