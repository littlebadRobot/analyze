#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include "camera.h"

void algorithmMorphology_Operations( Mat& src, Mat& dst,framedeal &Frame );
void getMotionMask(Mat* frame, Mat* motionMask,framedeal &Frame);
void PreprocessInputImage( Mat* data, byte* buf,framedeal &Frame);
void PostprocessMotionMask(Mat* data, uchar* buf, Mat *motionMask,framedeal &Frame);
void algorithmlabelBlobs(const cv::Mat &binary,framedeal &Frame);
void algorithmoperation(framedeal &Frame);


#endif
