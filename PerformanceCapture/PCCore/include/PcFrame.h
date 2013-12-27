#ifndef PCFRAME_H
#define PCFRAME_H

#include "PcExport.h"

#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

#define BOOST_ALL_DYN_LINK
#include <boost/thread/thread.hpp>

namespace pcc
{
    /// \ingroup PCCORE
    /// \brief A frame descriptor. Wraps around a cv::Mat object.
    /// 
    /// This class is used to manipulate frame data obtained from the cameras. It also provides
    /// an abstraction when dealing with GPU calculations.
    ///
    /// The PcFrame class ensures that, whenever data is changed on the computer's main memory,
    /// those changes are reflected on its GPU counterpart. It also provides access to the frame's
    /// metadata, such as width, height and number of color channels.
    class PcFrame
    {
    private:
        typedef boost::mutex    MutexType;  ///< The mutex type to be used to lock the shared resources.

    public:
        /// \brief Default constructor.
        /// Creates empty matrices to initialise the frame.
        PCCORE_EXPORT PcFrame ();
        
        /// \brief Creates a frame from its metadada and data.
        /// 
        /// Allocates memory for both the gpu and cpu components of a PcFrame.
        /// 
        /// The constructor creates a cv::Mat with dimensions iWidth x iHeight, and iNumChannels data channels.
        /// It then copies all data contained in the array pointed by the iData raw byte pointer to the cv::Mat
        /// itself and uploads it to the GPU.
        /// 
        /// \param [in] iWidth          the width of the frame image
        /// \param [in] iHeight         the height of the frame image
        /// \param [in] iNumChannels    the number of color channels in the image
        /// \param [in] iData           the byte pointer to the raw data array
        PCCORE_EXPORT PcFrame (
            unsigned int const&     iWidth,
            unsigned int const&     iHeight,
            unsigned int const&     iNumChannels,
            unsigned char const*&   iData
        );
        //PCCORE_EXPORT ~PcFrame ();

        /// \brief Gets the width of the image frame.
        /// \return a const reference to the width of the image frame
        PCCORE_EXPORT int const& Width () const;

        /// \brief Gets the height of the image frame.
        /// \return a const reference to the height of the image frame
        PCCORE_EXPORT int const& Height () const;

        /// \brief Gets a pointer to the data contained in the image frame.
        /// \param [out] oData          a byte pointer to the raw data contained in the frame
        /// \param [out] oNumChannels   the number of color channels of the image frame
        PCCORE_EXPORT void GetData (
            unsigned char const*&   oData,
            unsigned int&           oNumChannels
        ) const;

        /// \brief Gets the cv::Mat representation of the image frame.
        /// \return a const reference to the cv::Mat representation of the image frame
        PCCORE_EXPORT cv::Mat const& GetImagePoints () const { return (m_cpuImage); }

    private:
        /// \brief Private copy constructor.
        /// Disables copies of PcFrame objects.
        /// \param [in] iOther  the source object to copy from
        PcFrame (
            PcFrame const&      iOther
        );

        /// \brief Private assignment operator.
        /// Disables assignment to PcFrame objects.
        /// \param [in] iOther  the object from where to copy data on assignment
        PcFrame& operator= (
            PcFrame const&      iOther
        );

    public:
        /// \brief Replaces data inside the frame.
        /// Creates a new cv::Mat to hold the new data with the provided dimensions and copies the data from the byte array
        /// to the cv::Mat.
        /// \param [in] iWidth          the width of the new image frame
        /// \param [in] iHeight         the height of the new image frame
        /// \param [in] iNumChannels    the number of color channels on the new image frame's data
        /// \param [in] iData           the byte array representing the new image frame's data
        void Reset (
            unsigned int const&     iWidth,
            unsigned int const&     iHeight,
            unsigned int const&     iNumChannels,
            unsigned char const*&   iData
        );

        /// \brief Externally locks the frame for multithreaded usage.
        void Lock () { m_mutex.lock (); }

        /// \brief Externally unlocks the frame for multithreaded usage.
        void Unlock () { m_mutex.unlock (); }

    private:
        MutexType                   m_mutex         ///< The mutex to block concurrent access to the frame data.
    
        // Data containers
        cv::gpu::GpuMat             m_gpuImage;     ///< The GPU representation of the frame data.
        cv::Mat                     m_cpuImage;     ///< The CPU representation of the frame data.
    };

    typedef boost::shared_ptr<PcFrame> PcFramePtr;  ///< A reference-counted pointer to a PcFrame.
}

#endif // PCFRAME_H
