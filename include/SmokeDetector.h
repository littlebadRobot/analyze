//#pragma once
#ifndef _SMOKEDETECTOR_H_
#define _SMOKEDETECTOR_H_

#include "camera.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "Wavelet.h"
#include "SingleCamera.h"
#include <vector>



void SmokeDetectorlabelBlobs(const cv::Mat &binary,framedeal &Frame);
void initThreshold(int cols, int rows, float initValue,framedeal &Frame);
void SmokeDetectorMorphology_Operations( Mat& src, Mat& dst,framedeal &Frame );
void updateBackground(Mat& frame1, Mat& frame2, Mat& frame3,framedeal &Frame);
float getBinaryMotionMask(Mat& prevFrame,Mat& curFrame, Mat& motionMask, vector<Point> blob,framedeal &Frame);
float getWeberContrast(Mat& inputFrame, Mat& background, vector<Point> blob);
Rect getBoundaryofBlob(vector<Point> blob);
void smokeDetect(Mat& frame1, Mat& frame2, Mat& frame3,framedeal &Frame);
void getEdgeModel(Mat& frame,framedeal &Frame);
vector<Rect> detectSmoke(Mat* originFrame,framedeal &Frame);

#endif



