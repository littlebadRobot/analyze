#include "interface.h"

//void blobdeal(kp_p *kp,kp_p *kp_last,int kp_size,int kp_last_size);


void setroi(framedeal &frame)
{
	cvNamedWindow("SetRoi", CV_WINDOW_AUTOSIZE);
	createTrackbar("setROI x", "SetRoi", &frame.human.RoiX, 1024,onTrackbarSlide);
	createTrackbar("setROI y", "SetRoi", &frame.human.RoiY, 1024,onTrackbarSlide);
	createTrackbar("setROI width", "SetRoi", &frame.human.RoiWidth,1024, onTrackbarSlide);
	createTrackbar("setROI height", "SetRoi", &frame.human.RoiHeight, 1024,onTrackbarSlide);

}

void setthreshold(framedeal &frame)
{
	cvNamedWindow("SetThreshold", 0);
	createTrackbar("setTHRESHOLD x", "SetThreshold", &frame.human.ThresholdX, 1024,onTrackbarSlide);
	createTrackbar("setTHRESHOLD y", "SetThreshold", &frame.human.ThresholdY, 1024,onTrackbarSlide);
	createTrackbar("setTHRESHOLD width", "SetThreshold", &frame.human.ThresholdWidth,1024, onTrackbarSlide);
	createTrackbar("setTHRESHOLD height", "SetThreshold", &frame.human.ThresholdHeight, 1024,onTrackbarSlide);
}
void onTrackbarSlide( int, void* )
{

}
//øÏÀŸ≈≈–Ú
///////////////////////////////////////
void Swap(int &a,int &b)
{
    int temp;
    temp=a;
    a=b;
    b=temp;
}

int Partition(int a[],int p,int r)
{
    int i=p;
    int j=r+1;
    int x=a[p];
    while (true)
    {
        while(a[++i]<x&&i<r);
        while(a[--j]>x);
        if (i>=j)break;
        Swap(a[j],a[i]);

    }
    a[p]=a[j];
    a[j]=x;
    return j;
}

void QuickSort(int a[],int p,int r)
{
    if (p<r)
    {
        int q=Partition(a,p,r);
        QuickSort(a,p,q-1);
        QuickSort(a,q+1,r);
    }
}
#if 0
void blobdeal(kp_p *kp,kp_p *kp_last,int kp_size,int kp_last_size)
{
	int split = 0;
	int meet = 0;
	int id_count = 0;	
	
	for(int j=0;j<kp_last_size;j++)
	{
		for(int k=0;k<kp_size;k++)
		{
			if(abs(kp[k].x-kp_last[j].x) < (kp_last[j].w) && abs(kp[k].y-kp_last[j].y) < (kp_last[j].h))
			{
				split++;
				kp_last[j].id = id_count;
				kp[k].id = id_count;			
			}
		}	
		if(split >= 2) 
		{
			for(int k=0;k<c_size;k++)
			{
				if(kp[k].id == id_count)
				{
					kp[k].num = 1;		
//					if(kp[k].v == in)
				}
			}
		//	cout << "split........to......" << split << "       persons" << endl;
		}
		id_count++;	
		split = 0;
	}
	id_count = 0;
	
	for(int j=0;j<c_size;j++){
		kp[j].id = 500;
	}
	for(int j=0;j<10;j++){
                kp_last[j].id = 800;
        }
	
	for(int j=0;j<c_size;j++){
                for(int k=0;k<p_size;k++){
                        if(abs(kp_last[k].x-kp[j].x) < (kp[j].w) && abs(kp_last[k].y-kp[j].y) < (kp[j].h))
                        { 
				 meet++;
				 kp[j].id = id_count;
                                 kp_last[k].id = id_count;	
                	}	
		}
                if(meet >= 2) {
			for(int k=0;k<c_size;k++){
                                if(kp[k].id == id_count)
                                        kp[k].num = meet;
                        }
		//	cout << meet << "     persons.....meet............" << endl;
		}
		id_count++;
                meet = 0;
        }
	id_count = 0;

	for(int j=0;j<c_size;j++){
		if(kp[j].num == 0)
			kp[j].num = 1;
	}
	for(int j=0;j<c_size;j++){
		for(int k=0;k<p_size;k++){
			if(kp_last[k].id == kp[j].id){
				if((kp[j].v == out) && (kp_last[k].v == in)){
					if(update_count > 10){
						update_count = 0;
						out_n++;
						cout << "outtttttttttttttttt---------" << kp[j].num << endl;
					}
				}
				else if((kp[j].v == in) && (kp_last[k].v == out)){ 
					if(update_count > 10){
                                                update_count = 0;
                                                in_n++;
                                                cout << "innnnnnnnnnnnnnnnnn---------" << kp[j].num << endl;
                                        }
				}
			}
		
		}
	//	cout << "id::::" << kp[j].id << "    value::::" << kp[j].v << endl; 
	}
}
#endif
