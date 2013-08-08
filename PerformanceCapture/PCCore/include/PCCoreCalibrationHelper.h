#pragma once

#include "PCCoreFrame.h"
#include "PCCoreCamera.h"

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>

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

private:
    cv::Size                    m_size;
    float                       m_squareSize;
    std::vector< cv::Point3f >  m_points;
};

class PCCoreCalibrationHelper
{
private:
    template<class T>
    class StringMap {
    private:
        StringMap() {};
    public:
        typedef std::unordered_map<std::string, T> type;
    };

public:
    enum CalibrationState {
        UNKNOWN     =   0x00,
        ACQUIRING   =   0x01,
        CALIBRATING =   0x02,
        CALIBRATED  =   0x03,
    };

public:
    static PCCoreCalibrationHelper& GetInstance ();
    static void DestroyInstance ();
    
    ~PCCoreCalibrationHelper ();

    void StartCalibration ();
    void AbortCalibration ();
    bool RegisterCamera ( PCCoreCamera const& iCamera );
    bool UnregisterCamera ( PCCoreCamera const& iCamera );
    bool RegisterFrame ( std::string const& iCameraId, PCCoreFrame const& iFrame );
    void ProcessCalibration (
        std::string     iCamId
    );

    inline CalibrationState const& CurrentState () const { return m_state; }
    inline unsigned int const& FrameDelay () const { return m_frameDelay; }

private:
    PCCoreCalibrationHelper ();
    inline explicit PCCoreCalibrationHelper ( PCCoreCalibrationHelper const& iOther ) {}
    inline PCCoreCalibrationHelper& operator= ( PCCoreCalibrationHelper const& iOther ) {}

    void Reset ();

    void CheckCalibrationDone (
        PCCoreCamera&                               iCamera,
        std::vector< std::vector < cv::Point2f > >& iDetectedCorners
    );

private:
    static PCCoreCalibrationHelper*                                 sm_pInstance;
    static Chessboard                                               sm_chessboard;

    boost::shared_mutex                                             m_mutex;
    //boost::mutex                                                    m_mutex;
    StringMap< boost::shared_ptr<boost::mutex> >::type              m_camMutexes;

    boost::thread_group                                             m_workers;
    StringMap< boost::thread* >::type                               m_threads;

    unsigned int                                                    m_frameDelay;
    unsigned int                                                    m_frameCount;
    unsigned int                                                    m_calibratedCameras;

    CalibrationState                                                m_state;
    StringMap< PCCoreCamera >::type                                 m_cameras;
    StringMap< std::queue< PCCoreFrame > >::type                    m_frames;
    StringMap< std::vector< std::vector <cv::Point2f> > >::type     m_chessboardCorners;
};
