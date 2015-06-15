//#include "stdafx.h"
#include "Wavelet.h"


float sgn(float x)
{
    float res=0;
    if(x==0)
    {
        res=0;
    }
    if(x>0)
    {
        res=1;
    }
    if(x<0)
    {
        res=-1;
    }
    return res;
}
//--------------------------------
// Soft shrinkage
//--------------------------------
float soft_shrink(float d,float T)
{
    float res;
    if(fabs(d)>T)
    {
        res=sgn(d)*(fabs(d)-T);
    }
    else
    {
        res=0;
    }

    return res;
}
//--------------------------------
// Hard shrinkage
//--------------------------------
float hard_shrink(float d,float T)
{
    float res;
    if(fabs(d)>T)
    {
        res=d;
    }
    else
    {
        res=0;
    }

    return res;
}
//--------------------------------
// Garrot shrinkage
//--------------------------------
float Garrot_shrink(float d,float T)
{
    float res;
    if(fabs(d)>T)
    {
        res=d-((T*T)/d);
    }
    else
    {
        res=0;
    }

    return res;
}
void cvHaarWavelet(Mat &src,Mat &dst,int NIter)
{
    float c,dh,dv,dd;
    assert( src.type() == CV_32FC1 );
    assert( dst.type() == CV_32FC1 );
    int width = src.cols;
    int height = src.rows;
    for (int k=0;k<NIter;k++) 
    {
        for (int y=0;y<(height>>(k+1));y++)
        {
            for (int x=0; x<(width>>(k+1));x++)
            {
                c=(src.at<float>(2*y,2*x)+src.at<float>(2*y,2*x+1)+src.at<float>(2*y+1,2*x)+src.at<float>(2*y+1,2*x+1))*0.5;//��ͷֱ��ʵ�Ƶ��Ϣ
                dst.at<float>(y,x)=c;

                dh=(src.at<float>(2*y,2*x)+src.at<float>(2*y+1,2*x)-src.at<float>(2*y,2*x+1)-src.at<float>(2*y+1,2*x+1))*0.5;//��ͷֱ��ʸ�Ƶ��Ϣ
                dst.at<float>(y,x+(width>>(k+1)))=dh;

                dv=(src.at<float>(2*y,2*x)+src.at<float>(2*y,2*x+1)-src.at<float>(2*y+1,2*x)-src.at<float>(2*y+1,2*x+1))*0.5;
                dst.at<float>(y+(height>>(k+1)),x)=dv;

                dd=(src.at<float>(2*y,2*x)-src.at<float>(2*y,2*x+1)-src.at<float>(2*y+1,2*x)+src.at<float>(2*y+1,2*x+1))*0.5;
                dst.at<float>(y+(height>>(k+1)),x+(width>>(k+1)))=dd;
            }
        }
        dst.copyTo(src);
    }   
}
//--------------------------------
//Inverse wavelet transform
void cvInvHaarWavelet(Mat &src,Mat &dst,int NIter, int SHRINKAGE_TYPE, float SHRINKAGE_T)
{
    float c,dh,dv,dd;
    assert( src.type() == CV_32FC1 );
    assert( dst.type() == CV_32FC1 );
    int width = src.cols;
    int height = src.rows;
    //--------------------------------
    // NIter - number of iterations 
    //--------------------------------
    for (int k=NIter;k>0;k--) 
    {
        for (int y=0;y<(height>>k);y++)
        {
            for (int x=0; x<(width>>k);x++)
            {
                c=src.at<float>(y,x);
                dh=src.at<float>(y,x+(width>>k));
                dv=src.at<float>(y+(height>>k),x);
                dd=src.at<float>(y+(height>>k),x+(width>>k));

               // (shrinkage)
                switch(SHRINKAGE_TYPE)
                {
                case HARD:
                    dh=hard_shrink(dh,SHRINKAGE_T);
                    dv=hard_shrink(dv,SHRINKAGE_T);
                    dd=hard_shrink(dd,SHRINKAGE_T);
                    break;
                case SOFT:
                    dh=soft_shrink(dh,SHRINKAGE_T);
                    dv=soft_shrink(dv,SHRINKAGE_T);
                    dd=soft_shrink(dd,SHRINKAGE_T);
                    break;
                case GARROT:
                    dh=Garrot_shrink(dh,SHRINKAGE_T);
                    dv=Garrot_shrink(dv,SHRINKAGE_T);
                    dd=Garrot_shrink(dd,SHRINKAGE_T);
                    break;
                }

                //-------------------
                dst.at<float>(y*2,x*2)=0.5*(c+dh+dv+dd);
                dst.at<float>(y*2,x*2+1)=0.5*(c-dh+dv-dd);
                dst.at<float>(y*2+1,x*2)=0.5*(c+dh-dv-dd);
                dst.at<float>(y*2+1,x*2+1)=0.5*(c-dh-dv+dd);            
            }
        }
        Mat C=src(Rect(0,0,width>>(k-1),height>>(k-1)));
        Mat D=dst(Rect(0,0,width>>(k-1),height>>(k-1)));
        D.copyTo(C);
    }   
}

void getEnergyMap(Mat& frame, Mat& ret) //����ֵ����
{
	if(frame.empty()) return;
	const int NIter = 2;

    Mat Src = Mat(frame.rows, frame.cols, CV_32FC1);
    Mat Dst = Mat(frame.rows, frame.cols, CV_32FC1);
	//Dst = 0;
    frame.convertTo(Src,CV_32FC1);
    cvHaarWavelet(Src,Dst,NIter);
	ret = Mat::zeros(frame.rows / 8, frame.cols / 8, CV_32FC1);

	int scale = 2;

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
	
	scale = 4;
	for(int i = 0;i < tmp1.rows;i += scale) 
	{
		for(int j = 0;j < tmp1.cols;j += scale)//32bit 
		{
			float total = 0;

			for(int x = 0;x < scale;x++) 
			{
				if(i + x >= tmp1.rows) continue;
				for(int y = 0;y < scale;y++) 
				{
					if(j + y >= tmp1.cols) continue;
					total += tmp1.at<float>(i + x, j + y);			
				}
			}
			int x = floor(i / scale);
			int y = floor(j / scale);
			if(x >= ret.rows || y >= ret.cols) continue;
			ret.at<float>(x, y) = total;
		}
	}
}

void getFilteredFrame(Mat& frame, Mat& filtered) //�õ�ÿ�����ص�Ŀ�ܣ�����ͼ���黯��һ��
{
	try{
		if (frame.empty()) return;
		const int NIter=2;

		filtered=Mat(frame.rows, frame.cols, CV_32FC1);
		Mat Src=Mat(frame.rows, frame.cols, CV_32FC1);
		Mat Dst=Mat(frame.rows, frame.cols, CV_32FC1);
		Mat Temp=Mat(frame.rows, frame.cols, CV_32FC1);

		//Dst = 0;
		frame.convertTo(Src,CV_32FC1);
		cvHaarWavelet(Src,Dst,NIter);
		double M=0,m=0;
		minMaxLoc(Dst,&m,&M);//Ѱ�Ҿ���(һά���鵱������,��Mat����) ����Сֵ�����ֵ��λ��
		if((M-m)>0) {Dst=Dst*(1.0/(M-m))-m/(M-m);}
		Dst.copyTo(Temp);
		cvInvHaarWavelet(Temp,filtered,NIter,GARROT,30);//С����任  ���Ϊ��ɫ��
		//----------------------------------------------------
		// Normalization to 0-1 range (for visualization)
		//----------------------------------------------------
		minMaxLoc(filtered,&m,&M);
		if((M-m)>0) {filtered=filtered*(1.0/(M-m))-m/(M-m);} //  ���Ϊ��ɫ��
		Src.release();
		Dst.release();
		Temp.release();
	}catch(exception) {
	
	}
}
