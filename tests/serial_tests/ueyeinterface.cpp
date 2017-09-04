#include "ueyeinterface.h"

UeyeInterface::UeyeInterface(int id)
{
  format_id_ = 13; // 640 x 480 (VGA)
  pixel_clock_ = 18; // available values {9,18,30,35} measured in MHz
  cam_init_ = mem_alloc_ = false;
  hCam_ = id;
  status_ = is_InitCamera(&hCam_, NULL);
  if (isOk())
    status_ = is_PixelClock(hCam_, IS_PIXELCLOCK_CMD_SET, &pixel_clock_, 4);
  if (isOk())
  {
    cam_init_ = true;
    status_ = is_ImageFormat(hCam_, IMGFRMT_CMD_SET_FORMAT, &format_id_, 4);
  }
  if (isOk()) status_ = is_SetColorMode(hCam_, IS_CM_MONO8);
  if (isOk()) status_ = is_AllocImageMem(hCam_, 640, 480, 8, &data_, &mem_id_);
  if (isOk())
  {
    mem_alloc_ = true;
    status_ = is_SetImageMem(hCam_, data_, mem_id_);
  }
  if (isOk()) cv_frame_ = cv::Mat(480, 640, CV_8U, data_);
}

UeyeInterface::~UeyeInterface()
{
  if (mem_alloc_) is_FreeImageMem(hCam_, data_, mem_id_);
  if (cam_init_) is_ExitCamera(hCam_);
}

void UeyeInterface::setFrameRate(double fps)
{
  status_ = is_SetFrameRate(hCam_, fps, NULL);
}

void UeyeInterface::captureFrame()
{
  int left_attempts = 5;
  while ((status_=is_FreezeVideo(hCam_, IS_WAIT))==IS_TRANSFER_ERROR and
      left_attempts) --left_attempts;
}
