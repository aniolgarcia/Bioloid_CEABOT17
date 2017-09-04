#ifndef _UEYEINTERFACE_H_
#define _UEYEINTERFACE_H_
#include <uEye.h>
#include <opencv2/core/core.hpp>

class UeyeInterface
{
  private:
    HIDS hCam_;
    INT mem_id_;
    INT status_;
    UINT format_id_;
    UINT pixel_clock_;
    char* data_;
    cv::Mat cv_frame_;
    bool cam_init_;
    bool mem_alloc_;

  public:
    UeyeInterface(int id);

    ~UeyeInterface();

    void setFrameRate(double fps);

    void captureFrame();

    inline int getStatus() const
    {
      return status_;
    }

    inline bool isOk() const
    {
      return status_ == IS_SUCCESS;
    }

    inline cv::Mat getFrame() const
    {
      return cv_frame_;
    }

};

#endif
