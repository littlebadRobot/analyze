#include "camera.h"
#include "algorithm.h"
#include "interface.h"
#include "SmokeDetector.h"
#include "Wavelet.h"
#include "SingleCamera.h"

int init_frame(VideoCapture &capture,framedeal &frame)
{
	frame.human.imageRoi = NULL;
	frame.human.imageThreshold = NULL;
	//打开视频文件：其实就是建立一个VideoCapture结构
	//frame.variable.videoStreamAddress = "rtsp://admin:123456@192.168.1.252:554/mpeg4cif";//"smoke.mov";//"k.avi";//"rtsp://admin:123456@192.168.1.252:554/mpeg4cif";//"rtsp://218.204.223.237:554/live/1/66251FC11353191F/e7ooqwcfbqjoo80j.sdp";//    121 123 252 254  "k.avi";//

	if(!capture.open(frame.variable.videoStreamAddress)) 
	{
        cout << "Error opening video stream or file" <<endl;
        return -1;
    }
	frame.features.framewidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);// 视频流帧的宽度
	//cout<<"frame width :"<<frame.features.framewidth<<endl;
	frame.features.framewidthresize = frame.features.framewidth/2;
    frame.features.frameheight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);// 视频流帧的高.
	//cout<<"frame height :"<<frame.features.frameheight<<endl;
	frame.features.frameheightresize = frame.features.frameheight/2;
	//获取帧率
	frame.features.rate = capture.get(CV_CAP_PROP_FPS);
	//cout<<"frame rate :"<<frame.features.rate<<endl;
	//两帧间的间隔时间:
	int delay = 1000/frame.features.rate;

	frame.features.stop = false;
	frame.human.RoiX = 0;
	frame.human.RoiY = 0;
	frame.human.RoiWidth = 440;
	frame.human.RoiHeight = 380;
	frame.human.ThresholdX = 100;
	frame.human.ThresholdY = 100;
	frame.human.ThresholdWidth = 1;
	frame.human.ThresholdHeight = 1;
	frame.human.resol = 2;
	frame.human.periodDetectable = 1;
	frame.human.detectable = 1;
	frame.human.initialized = false;
	memset (&(frame.humanstatis) , 0 , sizeof(frame.humanstatis));
	

	frame.smoke.detectable = true;
	frame.smoke.periodDetectable = true;
	frame.smoke.waveletThres = 35;
	frame.smoke.initialized = false;
	frame.smoke.element = getStructuringElement( cv::MORPH_RECT, Size( 3, 3 ));
	frame.smoke.blockSize = 2;

	frame.human.alarmCap = Mat(frame.variable.frame.rows, frame.variable.frame.cols, CV_8UC3);//三通道
	frame.variable.frame.copyTo(frame.human.alarmCap);
	frame.smoke.alarmCap = Mat(frame.variable.frame.rows, frame.variable.frame.cols, CV_8UC3);//涓夐�氶亾  z++
	frame.variable.frame.copyTo(frame.smoke.alarmCap);  
	return 0;
}

