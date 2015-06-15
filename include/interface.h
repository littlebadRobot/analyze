#ifndef _INTERFACE_H_
#define _INTERFACE_H_
 
#include "camera.h"

void onTrackbarSlide( int, void* );
void setroi(framedeal &frame);
void setthreshold(framedeal &frame);
void Swap(int &a,int &b);
int Partition(int a[],int p,int r);
void QuickSort(int a[],int p,int r);

#endif
