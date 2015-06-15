#ifndef _CAMERA_H_
#define _CAMERA_H_ 

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/contrib/contrib.hpp"
#include <opencv2/core/core.hpp>

#include "highgui.h"
#include <vector>
#include <stdio.h>
#include<time.h>

using namespace std;
using namespace cv;

#define NUM  20
typedef unsigned char byte;
typedef struct _features_
{
	int framewidth;
	int frameheight;
	int framewidthresize ;
	int frameheightresize;
	double rate;
	double totaltime;
	bool stop;   //stream is open or closed
}t_features;

typedef struct _variable_
{
	char videoStreamAddress[128];
	Mat frame;
	int frameNum;
}t_variable;

typedef struct _humanalgorithm_
{
	IplImage *imageRoi;
	IplImage *imageThreshold;
	Mat alarmCap;
	Mat frame;
	Mat backgroundframe;
	bool detectorEn;
	bool periodDetectable;
	bool detectable;
	char resol;
	bool initialized;
	bool alarmIs;
	vector< vector<cv::Point> > m_MotionBlobs;
	vector< cv::Rect >			m_BlobRects;
	byte*	backgroundFrame;
	byte*	currentFrame;
	byte*	currentFrameDilatated;
	int RoiX;
	int RoiY;
	int RoiWidth;
	int RoiHeight;
	int ThresholdX;
	int ThresholdY;
	int ThresholdWidth;
	int ThresholdHeight;
	unsigned char frameNum;
	unsigned char  AutoWarn;
	int WarnNumber;		
}humanalgorithm;

typedef struct _smokealgorithm_
{
	IplImage *imageRoi;
	IplImage *imageThreshold;
	Mat alarmCap;
	Mat frame;
	Mat backgroundframe;
	bool detectorEn;
	bool periodDetectable;
	bool detectable;
	char resol;
	bool initialized;
	bool alarmIs;
	unsigned char frameNum;
	vector< vector<cv::Point> > m_MotionBlobs;
	Mat element;
	vector<vector<Point> > blobs;
	vector<vector<Point> > lastblobs;
	vector<Rect> SmokeRegion;
	Mat thres;
	Mat background;
	Mat foreground;
	Mat curRefinedFg;
	Mat curMorph_Foreground;
	Mat prevMorph_Foreground;
	Mat prevFrame;
	Mat frame1;
	Mat frame2;
	Mat frame3;
	Mat EMap[3];
	float waveletThres;
	Mat	bgEMap;
	int blockSize;
}smokealgorithm;

typedef struct _statistics_
{
	int totalnumMax;
	int inAll;
	int outAll ;
	int numAll ;
	int prenum ;
	int statis[NUM];
}statistics;

typedef struct _frame_deal
{
	t_features     features;
	t_variable     variable;	
	smokealgorithm    smoke;
	humanalgorithm    human;
	statistics   humanstatis;
}framedeal;

int init_frame(VideoCapture &capture,framedeal &frame);

#endif
