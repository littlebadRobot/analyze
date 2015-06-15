
#include "SmokeDetector.h"

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>



void SmokeDetectorlabelBlobs(const cv::Mat &binary,framedeal &Frame)//标记连通区域
{
    Frame.smoke.blobs.clear();
 
    // Using labels from 2+ for each blob
    cv::Mat label_image;
    binary.convertTo(label_image, CV_32FC1);
 
    int label_count = 2; // starts at 2 because 0,1 are used already     Scalar（0）=[0,0,0,0]    Scalar（1）=[1,0,0,0]   Scalar（2）=[2,0,0,0]

    for(int y=0; y < binary.rows; y++) 
	{
        for(int x=0; x < binary.cols; x++) 
		{
            if((int)label_image.at<float>(y,x) != 1) 
			{
                continue;
            }
 
            cv::Rect rect;
            cv::floodFill(label_image, cv::Point(x,y), cv::Scalar(label_count), &rect, cv::Scalar(0), cv::Scalar(0), 4);//将像素值为1附近的像素值为[0,0,0,0]的点赋值[2,0,0,0]
 
            vector<Point>  blob;
 
            for(int i=rect.y; i < (rect.y+rect.height); i++) 
			{
                for(int j=rect.x; j < (rect.x+rect.width); j++) 
				{
                    if((int)label_image.at<float>(i,j) != label_count)
					{
                        continue;
                    }
 	                
                    blob.push_back(cv::Point(j,i));
                }
            }
 
			Frame.smoke.blobs.push_back(blob);
            label_count++;
        }
    }
}

void initThreshold(int cols, int rows, float initValue,framedeal &Frame) {
	Frame.smoke.thres = Mat(rows, cols, CV_32FC1);

	for(int i = 0;i < rows;i++) {
		for(int j = 0;j < cols;j++) {
			Frame.smoke.thres.at<float>(i, j) = initValue;
		}
	}
}

void SmokeDetectorMorphology_Operations( Mat& src, Mat& dst,framedeal &Frame )
{
	Mat tmp = Mat::zeros(src.rows, src.cols, src.type());
	morphologyEx(src, tmp, MORPH_OPEN, Frame.smoke.element);
	morphologyEx(tmp, dst, MORPH_CLOSE, Frame.smoke.element);
	tmp.release();
}

void updateBackground(Mat& frame1, Mat& frame2, Mat& frame3,framedeal &Frame) {
	float alpha = 0.95;

	for(int i = 0;i < frame1.rows;i++) 
	{
		for(int j = 0;j < frame1.cols;j++) 
		{
			float f1 = frame1.at<float>(i, j);
			float f2 = frame2.at<float>(i, j);
			float f3 = frame3.at<float>(i, j);
			float t = Frame.smoke.thres.at<float>(i, j);

			
			if(abs(f1 - f2) > t && abs(f1 - f3) > t) //超出阈值时，背景和阈值更新
			{
				Frame.smoke.background.at<float>(i, j) = alpha * Frame.smoke.background.at<float>(i, j) + (1 - alpha) * f2;
				//thres.at<float>(i, j) = thres.at<float>(i, j) * alpha + (i - alpha) * (5 * abs(f2 - background.at<float>(i, j)));//z--
				Frame.smoke.thres.at<float>(i, j) = Frame.smoke.thres.at<float>(i, j) * alpha + (1 - alpha) * (5 * abs(f2 - Frame.smoke.background.at<float>(i, j)));//z++
			}
			Frame.smoke.foreground.at<float>(i, j) = (f1 - (0.62 * Frame.smoke.background.at<float>(i, j))) / 0.38;//不断更新前景色
			if(Frame.smoke.foreground.at<float>(i, j) > 0.9607) //显示出了图像变化的轮廓
				Frame.smoke.curRefinedFg.at<float>(i, j) = 1;
			else
				Frame.smoke.curRefinedFg.at<float>(i, j) = 0;
		}
	}
}

float getBinaryMotionMask(Mat& prevFrame,Mat& curFrame, Mat& motionMask, vector<Point> blob,framedeal &Frame) {
	int mCols = prevFrame.cols / Frame.smoke.blockSize;
	int mRows = prevFrame.rows / Frame.smoke.blockSize;
	int m = 0;
	int n = 0;
	float cs = 0;
	float c = 0;
	Mat tmp = Mat::zeros(mRows, mCols, CV_8UC1);

	for(unsigned int a = 0;a < blob.size();a++)
	{
		int i = floor(blob[a].y / Frame.smoke.blockSize);
		int j = floor(blob[a].x / Frame.smoke.blockSize);
		if(tmp.at<uchar>(i, j) == 1) continue;
		tmp.at<uchar>(i, j) = 1;
		c++;
		m = i - 1;
		if(m < 0) continue;
		for(int p = -1;p < 2;p++) 
		{
			n = j + p;
			if(n < 0 || n == mCols) continue;
			float max = 0;
			float min = 255;

			for(int x = 0;x < Frame.smoke.blockSize;x++) 
			{
				for(int y = 0;y < Frame.smoke.blockSize;y++) 
				{
					if(m * Frame.smoke.blockSize + x < 0 || m * Frame.smoke.blockSize + x >= prevFrame.rows || 
						n * Frame.smoke.blockSize + y < 0 ||  n * Frame.smoke.blockSize + y >= prevFrame.cols) continue;
					if(i * Frame.smoke.blockSize + x < 0 || i * Frame.smoke.blockSize + x >= prevFrame.rows || 
						j * Frame.smoke.blockSize + y < 0 ||  j * Frame.smoke.blockSize + y >= prevFrame.cols) continue;
					max += abs(curFrame.at<float>((m * Frame.smoke.blockSize) + x, (n * Frame.smoke.blockSize) + y) - prevFrame.at<float>((i * Frame.smoke.blockSize) + x, (j * Frame.smoke.blockSize) + y));
				}
			}

			max = max / pow(Frame.smoke.blockSize, 2);
			if(max < 0.005){
				motionMask.at<float>(i, j) = 1;
				motionMask.at<float>(m, n) = 1;
				cs++;
			}
		}
	}

	tmp.release();
	if(c == 0) 
	{
		return 0;
	}else
	{
		return cs * 100 / c;
	}
}

float getWeberContrast(Mat& inputFrame, Mat& background, vector<Point> blob) {//当前帧与背景对比
	float s = 0;

	for(unsigned int i = 0;i < blob.size();i++) {
		float iframe = inputFrame.at<float>(blob[i].y, blob[i].x);
		float bg = background.at<float>(blob[i].y, blob[i].x);

		iframe = abs(iframe - bg) / bg;
		s += iframe;
	}
	return s / blob.size();
}

Rect getBoundaryofBlob(vector<Point> blob) {
	int minX = 0;
	int maxX = 0;
	int minY = 0;
	int maxY = 0;
	
	for(unsigned int i = 0;i < blob.size();i++) {
		if(i == 0) {
			minX = blob[i].x;
			maxX = blob[i].x;
			minY = blob[i].y;
			maxY = blob[i].y;
		}else{
			if(blob[i].x > maxX) maxX = blob[i].x;
			if(blob[i].x < minX) minX = blob[i].x;
			if(blob[i].y > maxY) maxY = blob[i].y;
			if(blob[i].y < minY) minY = blob[i].y;
		}
	}
	return Rect(minX, minY, maxX - minX, maxY - minY);
}

void smokeDetect(Mat& frame1, Mat& frame2, Mat& frame3,framedeal &Frame) //frame1为当前帧，frame2为上一帧，frame3为上上帧
{
	updateBackground(frame1, frame2, frame3,Frame);//不断刷新背景、前景等
	SmokeDetectorMorphology_Operations(Frame.smoke.curRefinedFg, Frame.smoke.curMorph_Foreground,Frame);//进行形态学运算，先开运算再闭运算。获得图像形态并去除干扰   图像变化的轮廓
	SmokeDetectorlabelBlobs(Frame.smoke.curMorph_Foreground,Frame);//体现了图像结构，常用做掩码

	Mat frameGray;
	cvtColor(Frame.smoke.prevFrame, frameGray, CV_BGR2GRAY);//RGB转灰度图像
	
	equalizeHist(frameGray, frameGray);//灰度图像直方图均衡化   图像明亮  白色
	getEdgeModel(frameGray,Frame);//运用小波分析，得到边缘模型  EMap[0]~Emap[2]
	Mat curEMap;
	getEnergyMap(frameGray, curEMap);//能量分布,分析结果在curEMap中
	Mat diffEMap = Mat::zeros(curEMap.rows, curEMap.cols, CV_8UC1);
	frameGray.release();
	Mat blurMap = Mat::zeros(Frame.smoke.EMap[0].rows, Frame.smoke.EMap[0].cols, CV_8UC1);//全局数组EMap[3]
	/*对EMap[0]逐点分析，在EMap[0~3]中有一点超出阈值的情况下，
	如果EMap[1]该点的像素值不是最小，则如果EMap[0]小于阈值=>blurMap.at<char>(i, j) = 1*/
	for(int i = 0;i < Frame.smoke.EMap[0].rows;i++) 
	{
		for(int j = 0;j < Frame.smoke.EMap[0].cols;j++) 
		{
			if(Frame.smoke.EMap[0].at<float>(i, j) > Frame.smoke.waveletThres || Frame.smoke.EMap[1].at<float>(i, j) > Frame.smoke.waveletThres || Frame.smoke.EMap[2].at<float>(i, j) > Frame.smoke.waveletThres) //超出阈值
			{
				if((Frame.smoke.EMap[0].at<float>(i, j) < Frame.smoke.EMap[1].at<float>(i, j) && Frame.smoke.EMap[1].at<float>(i, j) < Frame.smoke.EMap[2].at<float>(i, j))
				|| (Frame.smoke.EMap[1].at<float>(i, j) > Frame.smoke.EMap[0].at<float>(i, j) && Frame.smoke.EMap[1].at<float>(i, j) > Frame.smoke.EMap[2].at<float>(i, j))) 
				{
					if(Frame.smoke.EMap[0].at<float>(i, j) < Frame.smoke.waveletThres) 
					{
						blurMap.at<char>(i, j) = 1;//白色
					}
				}
			}
		}
	}
      //如果bgEMap.at<float>(i, j)的像素值与能量分布图像素差大于90=>diffEMap.at<char>(i, j) = 1
	for(int i = 0;i < curEMap.rows;i++) 
	{
		for(int j = 0;j < curEMap.cols;j++) 
		{
			if(Frame.smoke.bgEMap.at<float>(i, j) - curEMap.at<float>(i, j) > 90) 
			{
				diffEMap.at<char>(i, j) = 1;
			}		
		}
	}

	Mat motion = Mat::zeros(frame1.rows / Frame.smoke.blockSize, frame1.cols / Frame.smoke.blockSize, CV_32FC1); //1/4图像
	for(unsigned int i = 0;i < Frame.smoke.lastblobs.size();i++) 
	{
		if(Frame.smoke.lastblobs[i].size() < 500) continue;
		Rect rect = getBoundaryofBlob(Frame.smoke.lastblobs[i]);//得到图像的边界及分辨率
		//if(rect.height < 24 || rect.height < 48) continue;
		int width = frame1.cols / 8;//40
		int height = frame1.rows / 8;//30

		Mat blob = Mat::zeros(height, width, CV_8UC1);//1/64
		float blobCount = 0;
		float blurCount = 0;

		for(unsigned int n = 0;n < Frame.smoke.lastblobs[i].size();n++) 
		{
			int y = (int)floor((float)Frame.smoke.lastblobs[i][n].y / 8);
			int x = (int)floor((float)Frame.smoke.lastblobs[i][n].x / 8);
			if(blob.at<char>(y, x) == 0) blobCount++;
			blob.at<char>(y, x) = 1;//wihte
		
		}

		for(int x = 0;x < curEMap.rows;x++) 
		{
			for(int y = 0;y < curEMap.cols;y++) 
			{
				if(blob.at<char>(x, y) == 1 && diffEMap.at<char>(x, y) == 1) 
				{//white，且像素差>90
					blurCount++;	
					
				}
			}
		}

		blurCount = blurCount / blobCount;
	//	std::cout << blurCount << std::endl;

		Mat contourBlur = Mat::zeros(height, width, CV_8UC1);//1/64
		float contourBlurCount = 0;

		for(int x = 0;x< blob.cols;x++) 
		{
			bool flag = false;

			for(int y = 0;y < blob.rows;y++) 
			{
				if(blob.at<char>(y, x) > 0)
				{
					if(blurMap.at<char>(y, x) > 0) 
					{
						flag = true;
						if(contourBlur.at<char>(y, x) == 0) contourBlurCount++;
						contourBlur.at<char>(y, x) = 1;
					}
					else
					{
						break;
					}		
				}
				else
				{
					if(flag) break;
				}
			}
		}

		for(int x = 0;x< blob.cols;x++) 
		{
			bool flag = false;

			for(int y = blob.rows - 1;y >= 0;y--) 
			{
				if(blob.at<char>(y, x) > 0) 
				{
					if(blurMap.at<char>(y, x) > 0) 
					{
						flag = true;
						if(contourBlur.at<char>(y, x) == 0) contourBlurCount++;
						contourBlur.at<char>(y, x) = 1;
					}else
					{
						break;
					}		
				}
				else
				{
					if(flag) break;
				}
			}
		}
		for(int y = 0;y< blob.rows;y++) 
		{
			bool flag = false;

			for(int x = 0;x < blob.cols;x++) 
			{
				if(blob.at<char>(y, x) > 0) 
				{
					if(blurMap.at<char>(y, x) > 0) 
					{
						flag = true;
						if(contourBlur.at<char>(y, x) == 0) contourBlurCount++;
						contourBlur.at<char>(y, x) = 1;
					}
					else
					{
						break;
					}		
				}
				else
				{
					if(flag) break;
				}
			}
		}
		for(int y = 0;y< blob.rows;y++) 
		{
			bool flag = false;

			for(int x = blob.cols - 1;x >= 0;x--) 
			{
				if(blob.at<char>(y, x) > 0) 
				{
					if(blurMap.at<char>(y, x) > 0) {
						flag = true;
						if(contourBlur.at<char>(y, x) == 0) contourBlurCount++;
						contourBlur.at<char>(y, x) = 1;
					}else{
						break;
					}		
				}else{
					if(flag) break;
				}
			}
		}

		contourBlurCount = contourBlurCount / blobCount;

		Mat motionMask = Mat(frame1.rows / Frame.smoke.blockSize, frame1.cols / Frame.smoke.blockSize, CV_32FC1);//1/4

		float p = getBinaryMotionMask(frame2, frame1, motionMask, Frame.smoke.lastblobs[i],Frame);

		add(motion, motionMask, motion);

		float cw = getWeberContrast(Frame.smoke.foreground, Frame.smoke.background, Frame.smoke.lastblobs[i]);
		/*
		std::cout << p << std::endl;
		std::cout << cw << std::endl;
		std::cout << blurCount << std::endl;
		std::cout << contourBlurCount << std::endl;
		*/
		float total = 0;	

		if(cw > 0.5 && p > 20 )//if(cw > 0.5 && p > 20 && blurCount > 0.3 && contourBlurCount > 0.15) 
		{
//			std::cout << "mmmmmm" << std::endl;
			for(unsigned int n = 0;n < Frame.smoke.lastblobs[i].size();n++) {
				float b = Frame.smoke.prevFrame.at<Vec3b>(Frame.smoke.lastblobs[i][n].y, Frame.smoke.lastblobs[i][n].x).val[0];
				float g = Frame.smoke.prevFrame.at<Vec3b>(Frame.smoke.lastblobs[i][n].y, Frame.smoke.lastblobs[i][n].x).val[1];
				float r = Frame.smoke.prevFrame.at<Vec3b>(Frame.smoke.lastblobs[i][n].y, Frame.smoke.lastblobs[i][n].x).val[2];

				float I = (r + g + b) / 3;
				float Cmax = std::max(r, std::max(g, b));
				float Cmin = std::min(r, std::min(g, b));
				if(Cmax - Cmin < 35) {
					if(I > 80 && I < 235) {
						total++;
					}
				}else{
					if(I > 80 && I < 235) {
						if(Cmax == b && (Cmax - Cmin) < 45) {
							total++;
						}
					}
				}
			}
			total = total / (float)Frame.smoke.lastblobs[i].size();
			if(total > 0) //if(total > 0.9)   原来的值
			{
	//			std::cout << "mmmmmm" << std::endl;
				Frame.smoke.SmokeRegion.push_back(rect);
			}
		}
		motionMask.release();
		contourBlur.release();
		blob.release();		
	}
	Frame.smoke.lastblobs.clear();
	for(unsigned int i = 0;i < Frame.smoke.blobs.size();i++) {
		vector<Point> tmp;
		for(unsigned int j = 0;j < Frame.smoke.blobs[i].size();j++) {
			tmp.push_back(Point(Frame.smoke.blobs[i][j].x, Frame.smoke.blobs[i][j].y));
		}
		Frame.smoke.lastblobs.push_back(tmp);
	}
	Frame.smoke.curMorph_Foreground.copyTo(Frame.smoke.prevMorph_Foreground);//
	curEMap.release();
	diffEMap.release();
	blurMap.release();
	motion.release();
}

void getEdgeModel(Mat& frame,framedeal &Frame) {
	if (frame.empty()) return;
	const int NIter = 3;

    Mat Src = Mat(frame.rows, frame.cols, CV_32FC1);
    Mat Dst = Mat(frame.rows, frame.cols, CV_32FC1);

    //Dst = 0;
    frame.convertTo(Src,CV_32FC1);//在缩放或不缩放的情况下转换为另一种数据类型
    cvHaarWavelet(Src,Dst,NIter);//哈尔小波转换应用于图像压缩
	
	for(int n = 0;n < 3;n ++) 
	{
		int scale = pow(2, n + 1);
		Mat tmp1 = Dst(Rect(0, frame.rows / scale, frame.cols / scale, frame.rows / scale));
		Mat tmp2 = Dst(Rect(frame.cols / scale , frame.rows / scale, frame.cols / scale, frame.rows / scale));
		Mat tmp3 = Dst(Rect(frame.cols / scale , 0, frame.cols / scale, frame.rows / scale));

		for(int i = 0;i < tmp1.rows;i++) 
		{
			for(int j = 0;j < tmp1.cols;j++) 
			{
				tmp1.at<float>(i, j) = sqrt(pow(tmp1.at<float>(i, j), 2) + pow(tmp2.at<float>(i, j), 2) + pow(tmp3.at<float>(i, j), 2));
			}
		}
		scale = pow(2, 2 - n);

		double M=0,m=0;
		minMaxLoc(tmp1,&m,&M);
		for(int i = 0;i < tmp1.rows;i += scale) 
		{
			for(int j = 0;j < tmp1.cols;j += scale) 
			{
				float max = m;

				for(int x = 0;x < scale;x++) 
				{
					for(int y = 0;y < scale;y++) 
					{
						if(max < tmp1.at<float>(i + x, j + y)) 
						{
							max = tmp1.at<float>(i + x, j + y);			
						}
					}
				}
				Frame.smoke.EMap[n].at<float>(i / scale, j / scale) = max;
			}
		}
		tmp1.release();
		tmp2.release();
		tmp3.release();
	}
	Src.release();
	Dst.release();
}

vector<Rect> detectSmoke(Mat* originFrame,framedeal &Frame) 
{
	Mat frameGray;
	Frame.smoke.SmokeRegion.clear();
	if(!Frame.smoke.initialized)
	{	
		resize(*originFrame, *originFrame, Size(320, 240));//将图像 src 改变尺寸得到与 dst 同样大小
		Frame.smoke.EMap[0] = Mat::zeros(originFrame->rows / 8, originFrame->cols / 8, CV_32FC1);
		Frame.smoke.EMap[1] = Mat::zeros(originFrame->rows / 8, originFrame->cols / 8, CV_32FC1);
		Frame.smoke.EMap[2] = Mat::zeros(originFrame->rows / 8, originFrame->cols / 8, CV_32FC1);
		initThreshold(originFrame->cols, originFrame->rows, 0.01,Frame);//all pixel value=0.01

		cvtColor(*originFrame, frameGray, CV_BGR2GRAY);//convert to gray
		equalizeHist(frameGray, frameGray);//灰度图象直方图均衡化  曝光均匀（更宽、理想统一的亮度值分布）
		getFilteredFrame(frameGray, Frame.smoke.frame1);// 获取过滤框架  

		getEnergyMap(frameGray, Frame.smoke.bgEMap);//能量图  明亮
		Frame.smoke.frame1.copyTo(Frame.smoke.background);
		Frame.smoke.foreground = Mat(Frame.smoke.background.rows, Frame.smoke.background.cols, Frame.smoke.background.type());
		Frame.smoke.curRefinedFg = Mat(Frame.smoke.background.rows, Frame.smoke.background.cols, Frame.smoke.background.type());
		Frame.smoke.prevMorph_Foreground = Mat::zeros(Frame.smoke.background.rows, Frame.smoke.background.cols, Frame.smoke.background.type());
		frameGray.release();
		Frame.smoke.initialized = true;
		return Frame.smoke.SmokeRegion;
	}

	if(!originFrame->empty())
	{
		
		float w_rate = (float)originFrame->cols / 320;
		float h_rate = (float)originFrame->rows / 240;
		resize(*originFrame, *originFrame, Size(320, 240));
		cvtColor(*originFrame, frameGray, CV_BGR2GRAY);
		equalizeHist(frameGray, frameGray);
		getFilteredFrame(frameGray, Frame.smoke.frame1);
		if(!Frame.smoke.frame1.empty() && !Frame.smoke.frame2.empty() && !Frame.smoke.frame3.empty()) 
		{
			smokeDetect(Frame.smoke.frame1, Frame.smoke.frame2, Frame.smoke.frame3,Frame);//frame1为当前帧，frame2为上一帧，frame3为上上帧
			for(unsigned int i = 0;i < Frame.smoke.SmokeRegion.size();i++) {

				Frame.smoke.SmokeRegion[i].x = Frame.smoke.SmokeRegion[i].x * w_rate;
				Frame.smoke.SmokeRegion[i].width = Frame.smoke.SmokeRegion[i].width * w_rate;
				Frame.smoke.SmokeRegion[i].y = Frame.smoke.SmokeRegion[i].y * h_rate;
				Frame.smoke.SmokeRegion[i].height = Frame.smoke.SmokeRegion[i].height * h_rate;
			}
		}
		if(!Frame.smoke.frame2.empty()) 
		{
			if(!Frame.smoke.frame3.empty()) 
			{
				Frame.smoke.frame3.release();
			}
			Frame.smoke.frame2.copyTo(Frame.smoke.frame3);//frame3为上上帧
			if(Frame.smoke.background.empty()) 
			{
						
			}
			Frame.smoke.frame2.release();
		}
		Frame.smoke.frame1.copyTo(Frame.smoke.frame2);//frame2为上一帧
		Frame.smoke.frame1.release();
		frameGray.release();
		if(!Frame.smoke.prevFrame.empty()) Frame.smoke.prevFrame.release();
		originFrame->copyTo(Frame.smoke.prevFrame);
	}
	return Frame.smoke.SmokeRegion;
}
