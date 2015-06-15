#include "algorithm.h"
#include "interface.h"
#include <iostream>

void algorithmoperation(framedeal &Frame)
{
	bool motionAlarmDone = false;
	bool vehicleAlarmDone = false;
	int totalnum = 0;
	int humannum = 0;
	int centroidX = 0;
	int centroidY = 0;

	int total = 0;
	vector<Point> object;

	Mat motionAlarmCap;
	Mat vehicleAlarmCap;
	Mat *frame = NULL;
	//*frame = Frame.variable.frame;
	Frame.human.frame  = Frame.variable.frame.clone() ; 
	frame = &(Frame.human.frame);
	Frame.human.frameNum++;
    try//系统异常情况的处理
	{
		if(!frame->empty()) 
		{	
			//if(!lastFrame.empty()) lastFrame.release();
			//frame->copyTo(lastFrame);
			//Frame.human.alarmCap = Mat(frame->rows, frame->cols, CV_8UC3);//三通道
			frame->copyTo(Frame.human.alarmCap);
			float w_Rate = (float)frame->cols / Frame.features.framewidthresize;
			float h_Rate = (float)frame->rows / Frame.features.frameheightresize;
			Mat tmp = Mat(Frame.features.framewidthresize, Frame.features.frameheightresize, frame->type());
			resize(*frame, tmp, Size(Frame.features.framewidthresize, Frame.features.frameheightresize));//tmp为frame缩放的320*240的矩阵
			Mat motionMask = Mat::zeros(tmp.rows, tmp.cols, CV_8UC1);//320*240的灰度图像
			getMotionMask(&tmp, &motionMask,Frame);
			tmp.release();

			Mat motionBlobs = Mat::zeros(motionMask.rows, motionMask.cols, CV_8UC1);//motionBlobs是从图像的最左开始高亮轮廓
			GaussianBlur(motionMask, motionMask, cv::Size( 3, 3 ), 0, 0 );
			threshold(motionMask, motionBlobs, 20, 255, THRESH_BINARY);

			motionMask.release();
			Mat morph = Mat(motionBlobs.rows, motionBlobs.cols, motionBlobs.type());
			algorithmMorphology_Operations(motionBlobs, morph,Frame);
			motionBlobs.release();

		//	blur(morph, morph, Size(1, 1));
		//	threshold(morph, morph, 20, 255, THRESH_BINARY);

			for(int i = 0;i < morph.rows;i++) 
			{
				uchar* p = morph.ptr(i);
				for(int j = 0;j < morph.cols;j++) {
					if(p[j] > 200) {
						p[j] = 1;
					}
					else{
						p[j] = 0;
					}
				}
			}	
			algorithmlabelBlobs(morph,Frame);
			resize(morph, morph, Size(frame->cols, frame->rows));
			Frame.human.m_BlobRects.clear();
			for(unsigned int i = 0;i < Frame.human.m_MotionBlobs.size();i++)//blob
			{
				Rect rect1 = boundingRect(Frame.human.m_MotionBlobs[i]);//cvBoundingRect计算点集的最外面（up-right）矩形边界--边框
				if(rect1.width == 0 || rect1.height == 0) continue;
				float x = (float)rect1.x * w_Rate;
				float width = (float)rect1.width * w_Rate;
				float y = (float)rect1.y * h_Rate;
				float height = (float)rect1.height * h_Rate;
				if(y > height * 0.1) 
				{
					y -= height * 0.1;
				}
				if(y + height * 1.2 < frame->rows) 
					height = height * 1.2; 
				else 
					height = frame->rows - y;
				if(x > width * 0.05) 
				{
					x -= width * 0.05;
				}
				if(x + width * 1.1 < frame->cols) 
					width = width * 1.1; 
				else 
					width = frame->cols - x;
				Frame.human.m_BlobRects.push_back(Rect(x, y, width, height));
			}	


				//运动监测
			if(Frame.human.periodDetectable&&Frame.human.detectable)//if(smokePeriodDetectable && smokeDetectable)
			{
				for(unsigned int i = 0;i < Frame.human.m_BlobRects.size();i++)//while(pos)
				{		
					
					Rect rt = Frame.human.m_BlobRects[i];
					if(rt.x + rt.width < morph.cols && rt.y + rt.height < morph.rows)
					{
						Mat tmp = morph(rt);//相当与tmp是rt的备份
						int ret = countNonZero(tmp);//计算非零数组元素个数   可以看成是面积
						if(rt.width * rt.height<500)
							continue;
						float rate = (float)ret / (float)(rt.width * rt.height);//
						if(rate > 0.7)//0.001)//
						{
							//rectangle(Frame.human.alarmCap, rt, Scalar( 0, 0, 255 ), 1, 8, 0);//红色
						}
						else
						{
							centroidX = rt.x + rt.width/2;
							centroidY = rt.y + rt.height/2;
							if((centroidX <(Frame.human.RoiX+Frame.human.RoiWidth))&&(centroidY > 100))
							{   
							    object.push_back(cv::Point(centroidX,centroidY));
								rectangle(Frame.human.alarmCap, rt, Scalar( 255, 0, 0 ), 2, 8, 0);//蓝
								rectangle(Frame.human.alarmCap,cvPoint(centroidX-1,centroidY-1),cvPoint(centroidX+1,centroidY+1),cvScalar(123,123,0),3,8,0); //画出目标的质心
							}

						}
					}
				}//for  end

			}
			else
			{

			}
		//	cout<<"object.size()======="<<object.size()<<endl;
			//所有监测处理后的处理
			for(unsigned int i=0; i<object.size(); i++)
			{
				//Point pt = object[i];
				if(object[i].x > (Frame.human.ThresholdX+Frame.human.ThresholdWidth)) 
				{
					humannum++;		
					
				}
			}

			for(int i=0; i<NUM; i++)
			{
				Frame.humanstatis.statis[i] = Frame.humanstatis.statis[i+1];
				Frame.humanstatis.statis[NUM-1] = humannum;
			}
			if(Frame.human.frameNum == (NUM-1))
			{
	
                QuickSort(Frame.humanstatis.statis,0,NUM-1);
#if 0
				for(int i=2; i<NUM-2; i++)
					total += humanstatis[i];
				humannumAll = total/(NUM-4);
#endif
				Frame.humanstatis.numAll = Frame.humanstatis.statis[(NUM-1)/3*2];
				Frame.human.frameNum = 0;

				if(Frame.humanstatis.numAll > Frame.humanstatis.prenum) 
					Frame.humanstatis.inAll += Frame.humanstatis.numAll - Frame.humanstatis.prenum;
				else if(Frame.humanstatis.numAll < Frame.humanstatis.prenum) 
					Frame.humanstatis.outAll += Frame.humanstatis.prenum - Frame.humanstatis.numAll;
				Frame.humanstatis.prenum = Frame.humanstatis.numAll;
			}		
			morph.release();
			char FrmNum[100]; 
		       snprintf(FrmNum, 100, "Num:%d,In:%d,Out:%d",Frame.humanstatis.numAll,Frame.humanstatis.inAll,Frame.humanstatis.outAll); 
			//sprintf_s(FrmNum, 100, "Num:%d,In:%d,Out:%d",Frame.humanstatis.numAll,Frame.humanstatis.inAll,Frame.humanstatis.outAll); 
			putText(Frame.human.alarmCap, FrmNum,cvPoint(0,25), CV_FONT_HERSHEY_COMPLEX, 1, cvScalar(0,0,255)); 
			//imshow("HumanDetector",Frame.human.alarmCap);//
			//rtptrans(Frame.human.alarmCap);
			//rtptrans(Frame.variable.frame);
			object.clear();
			vector<Point>(object).swap(object); //vector<Point>
			//Frame.human.alarmCap.release();
		}

	}
	catch(Exception e) 
	{
		//MessageBox(NULL, (LPCSTR)e.msg.c_str(), "Error", 1);
	}

}

void getMotionMask(Mat* frame, Mat* motionMask,framedeal &Frame)
{
#if 1
	static unsigned char counter=0;

	cv::cvtColor(*frame, *frame, CV_RGB2GRAY);

	if (Frame.human.initialized == false)
	{
		frame->copyTo(Frame.human.backgroundframe);
		Frame.human.initialized = true;
		return;
	}
	//cvRunningAvg(pFrameMat, pBkMat, 0.05, 0);
	if ( ++counter == 4 )
	{
		counter = 0;
		frame->copyTo(Frame.human.backgroundframe);
	}
	cv::absdiff(*frame, Frame.human.backgroundframe, *motionMask);
	//cv::cvtColor(*motionMask, *motionMask, CV_RGB2GRAY);
//	cv::GaussianBlur(result, result, cv::Size( 3, 3 ), 0, 0 );
//	cv::threshold(result, result, 10, 255, CV_THRESH_BINARY);
#endif
	return;
}

void algorithmMorphology_Operations( Mat& src, Mat& dst,framedeal &Frame )//将二值图像src中的区域个数统计出来，并去除噪声引起的区域；结果放到dst中
{
	Mat element = getStructuringElement( cv::MORPH_RECT, Size( 12,12));//获得3*3的核
	Mat element1 = getStructuringElement( cv::MORPH_RECT, Size( 3,3 ));//获得3*3的核
	Mat tmp = Mat::zeros(src.rows, src.cols, src.type());
	//morphologyEx(src, tmp, MORPH_OPEN, element);//统计二值图像中的区域数，存放到tmp中，tmp为中间矩阵
	//morphologyEx(tmp, dst, MORPH_CLOSE, element1);//去除tmp中的噪声引起的区域，存放到dst（目标矩阵）
	morphologyEx(src, tmp, MORPH_OPEN, element1);//统计二值图像中的区域数，存放到tmp中，tmp为中间矩阵
	dilate(tmp, dst, element);//膨胀一次
	//erode(dst, dst, element);
	tmp.release();
	element.release();
	element1.release();
}

//缩放为1/4,取均值
void PreprocessInputImage( Mat* data, byte* buf,framedeal &Frame)//将buf缩放为data的1/4，每2*2的邻域求均值（和/4）
{
	int len = (int)( ( data->cols - 1 ) / Frame.human.resol ) + 1;//原来的1/2或（原来的1/2-0.5）  Frame.human.resol是分辨率  （320-1）/2+1=160
	int rem = ( ( data->cols - 1 ) % Frame.human.resol ) + 1;    //（320-1）%2+1=2                        
	int* tmp = new int[500];
	int i, j, t1, t2, k = 0;

	try
	{			
		for ( int y = 0; y < data->rows;)
		{
			// collect pixels
			for(int m = 0;m < len;m++)
			{
				tmp[m] = 0;//tem[160]
			}
			for ( i = 0; ( i < Frame.human.resol ) && ( y < data->rows ); i++, y++ )
			{
				Vec3b* p = (Vec3b *)data->ptr(y);//行指针
				for ( int x = 0; x < data->cols; x++ )
				{
					// grayscale value using BT709
					tmp[(int) ( x / Frame.human.resol )] += (int)( 0.299f * p[x].val[2] + 0.587f * p[x].val[1] + 0.114f * p[x].val[0] );//从0行开始相邻两行两列的四个像素（3通道）转为灰度并相加
					//tmp[(int) ( x / Frame.human.resol )] += (int)( 0.2125f * p[x].val[2] + 0.7154f * p[x].val[1] + 0.0721f * p[x].val[0] );//从0行开始相邻两行两列的四个像素（3通道）转为灰度并相加
				}
			}
			// get average values
			t1 = i * Frame.human.resol;//2*2
			t2 = i * rem;//2*2
			for ( j = 0; j < len - 1; j++, k++ ) //len=160
			{
				if(t1 == 0) 
				{
					buf[k] = 0;
				}
				else
				{
					buf[k] = (byte)( tmp[j] / t1 );//buf[k]为原图像（上下左右四个像素点）的平均值
				}
			}
			if(t2 == 0) 
				buf[k++] = 0; 
			else
				buf[k++] = (byte)( tmp[j] / t2 );
		}
	}
	catch(Exception)
	{
	
	}
	delete[] tmp;
}
//显示轮廓
void PostprocessMotionMask(Mat* data, uchar* buf, Mat *motionMask,framedeal &Frame)
{
	int len = (int)( ( data->cols - 1 ) / Frame.human.resol ) + 1;//(（320-1）/2+1)=160
	int lenWM1 = len - 1;//159
	int lenHM1 = (int)( ( data->rows - 1 ) / Frame.human.resol);//（240-1）/2=119

	int rem = ( ( data->cols - 1 ) % Frame.human.resol ) + 1;//（320-1）%2+1=2
	int pi = 0, j = 0, k = 0;
	// for each line
	for ( int y = 0; y < data->rows; y++ )//240
	{
		pi = y / Frame.human.resol;
		uchar* p = motionMask->ptr(y);
		// for each pixel
		for ( int x = 0; x < data->cols; x++ )//320
		{
			j = x / Frame.human.resol;	
			k = pi * len + j;//pi*160+j

			// check if we need to highlight moving object   高亮边框
			if (buf[k] == 255)
			{
				if(( x % Frame.human.resol == 0 ) && ( ( j == 0 ) || ( buf[k - 1] == 0 ) ) ) {//最左两列或者每两列中像素一黑一白的高亮（使其像素值为255）
					p[x] += 3;//0000,0011
				}
				if(( x % Frame.human.resol == (Frame.human.resol - 1) ) && ( ( j == lenWM1 ) || ( buf[k + 1] == 0 ) ) ) {//最右边框或者每两列中像素一白一黑的
					p[x] += 12;//0000,1100
				}
				if(( y % Frame.human.resol == 0 ) && ( ( pi == 0 ) || ( buf[k - len] == 0 ) ) ) {//上边框
					p[x] += 48;//0011,0000
				}
				if( ( y % Frame.human.resol == (Frame.human.resol - 1) ) && ( ( pi == lenHM1 ) || ( buf[k + len] == 0 ) ) ) {//下边框
					p[x] += 192;//1100,0000
				}
			}
		}
	}
}

void algorithmlabelBlobs(const cv::Mat &binary,framedeal &Frame)//传入二值图像做处理  标记联通区域
{
	/*
	for(int i = 0; i < m_MotionBlobs.size(); ++i)
	{
		m_MotionBlobs[0].clear();
	}
	*/
	Frame.human.m_MotionBlobs.clear();
    // Using labels from 2+ for each blob
    cv::Mat label_image;
    binary.convertTo(label_image, CV_32FC1);
 
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
 
                    blob.push_back(cv::Point(j,i));
                }
            }
 
			Frame.human.m_MotionBlobs.push_back(blob);
            label_count++;
        }
    }
}

