#ifndef _SINGLECAMERA_H_
#define _SINGLECAMERA_H_

#include "camera.h"
#include "SmokeDetector.h"
#include "Wavelet.h"

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>

void SingCameralabelBlobs(const cv::Mat &binary,framedeal &Frame);//�����ֵͼ��������  �����ͨ����
void SingCameraMorphology_Operations( Mat& src, Mat& dst );
void operation(framedeal &Frame);


using namespace std;
using namespace cv;

#endif



