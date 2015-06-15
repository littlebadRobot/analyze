
#include "SingleCamera.h"


void SingCameralabelBlobs(const cv::Mat &binary,framedeal &Frame)//传入二值图像做处理  标记联通区域
{
    Frame.smoke.m_MotionBlobs.clear();
    // Using labels from 2+ for each blob
    cv::Mat label_image;
    binary.convertTo(label_image, CV_32FC1);//灰度图像
 
    int label_count = 2; // starts at 2 because 0,1 are used already
 
    for(int y=0; y < binary.rows; y++) 
	{
		float* p = (float *)label_image.ptr(y);//行指针
        for(int x=0; x < binary.cols; x++) 
		{
            if((int)p[x] != 1) 
			{
                continue;
            }
 
            cv::Rect rect;
			//                                                         2
            cv::floodFill(label_image, cv::Point(x,y), cv::Scalar(label_count), &rect, cv::Scalar(0), cv::Scalar(0), 4);//漫水填充算法
 
            vector<Point>  blob;
 
            for(int i = rect.y; i < (rect.y+rect.height); i++) 
			{
				float* pp = (float *)label_image.ptr(i);
                for(int j=rect.x; j < (rect.x+rect.width); j++) 
				{
                    if((int)pp[j] != label_count)
					{
                        continue;
                    }
 
                    blob.push_back(cv::Point(j,i));//把点（2，i）插入到blob尾部
                }
            }
 
            Frame.smoke.m_MotionBlobs.push_back(blob);//在尾部插入块blob
            label_count++;
        }
    }
}

void SingCameraMorphology_Operations( Mat& src, Mat& dst )//将二值图像src中的区域个数统计出来，并去除噪声引起的区域；结果放到dst中
{
	Mat element = getStructuringElement( cv::MORPH_RECT, Size( 3, 3 ));//获得3*3的核
	Mat tmp = Mat::zeros(src.rows, src.cols, src.type());
	morphologyEx(src, tmp, MORPH_OPEN, element);//统计二值图像中的区域数，存放到tmp中，tmp为中间矩阵
	morphologyEx(tmp, dst, MORPH_CLOSE, element);//去除tmp中的噪声引起的区域，存放到dst（目标矩阵）
	tmp.release();
	element.release();
}

void operation(framedeal &Frame)
{
	Mat *frame = NULL;
	Frame.smoke.frame  = Frame.variable.frame.clone() ; 
	frame = &(Frame.smoke.frame);
	resize(*frame, *frame, Size(960, 540));
	try//系统异常情况的处理
	{
		if(!frame->empty()) 
		{
		//	Frame.smoke.alarmCap = Mat(frame->rows, frame->cols, CV_8UC3);//三通道  z++
			frame->copyTo(Frame.smoke.alarmCap);                                       //z++

			//烟雾检测
			if(Frame.smoke.periodDetectable && Frame.smoke.detectable)
			{
				vector<Rect> smokeRegion = detectSmoke(frame,Frame);//监测烟雾区域
				
				if(smokeRegion.size() > 0) 
				{
					Frame.smoke.alarmIs = true;

					for(unsigned int i = 0;i < smokeRegion.size();i++) 
					{
					//	rectangle(smokeAlarmCap, smokeRegion[i], Scalar(0, 0, 255),2, 8, 0);//red
						rectangle(Frame.smoke.alarmCap, smokeRegion[i], Scalar(0, 0, 255), 2, 8, 0);
					}
				}
			}
			else
			{
				Frame.smoke.alarmIs = false;
			}
			//所有监测处理后的处理
			IplImage iplImage = Frame.smoke.alarmCap.operator IplImage();
			//imshow("SmokeDetector", Frame.smoke.alarmCap);
		//	Frame.smoke.alarmCap.release();
		}

	}
	catch(Exception e) 
	{

	}

}

