#pragma once

#include "PCCoreFrame.h"

class PCCoreCamera;

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/function.hpp>

#include <unordered_map>
#include <queue>
#include <vector>

#include <opencv2/calib3d/calib3d.hpp>

class Chessboard {
public:
    Chessboard (
        unsigned int const&     iRows,
        unsigned int const&     iCols,
        float const&            iSquareSize
    );
    inline ~Chessboard () {}

    inline std::vector< cv::Point3f > const& GetPoints () const { return m_points; }
    inline cv::Size const& GetSize () const { return m_size; }

    std::vector< std::vector< cv::Point3f > > CreateInputArray (
        unsigned int        iFrameCount
    );

private:
    cv::Size                    m_size;
    float                       m_squareSize;
    std::vector< cv::Point3f >  m_points;
};

class PCCoreCalibrationHelper
{
public:
    static PCCoreCalibrationHelper& GetInstance ();
    static void DestroyInstance ();

    ~PCCoreCalibrationHelper ();

    inline unsigned int const& FrameDelay () const { return m_frameDelay; }
    inline unsigned int const& FrameCount () const { return m_frameCount; }
    std::vector< std::vector< cv::Point3f > > GetChessboardPoints ();
    inline cv::Size const& GetChessboardSize () { return sm_chessboard.GetSize(); }

private:
    PCCoreCalibrationHelper ();
    inline explicit PCCoreCalibrationHelper ( PCCoreCalibrationHelper const& iOther ) {}
    inline PCCoreCalibrationHelper& operator= ( PCCoreCalibrationHelper const& iOther ) {}

private:
    static PCCoreCalibrationHelper*     sm_pInstance;
    static Chessboard                   sm_chessboard;

    unsigned int                        m_frameDelay;
    unsigned int                        m_frameCount;
    unsigned int                        m_calibratedCameras;
};

enum CalibrationState {
    UNKNOWN     =   0x00,
    ACQUIRING   =   0x01,
    CALIBRATING =   0x02,
    CALIBRATED  =   0x03,
};

class PCCoreCameraCalibration {
private:
    typedef boost::function3<void, PCCoreCamera*, CalibrationState, CalibrationState> CallbackFn;

public:
    PCCoreCameraCalibration ( PCCoreCamera* iParent );
    ~PCCoreCameraCalibration ();

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
    PCCoreCameraCalibration ( PCCoreCameraCalibration const& iOther) {}
    PCCoreCameraCalibration& operator= ( PCCoreCameraCalibration const& iOther ) {}

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