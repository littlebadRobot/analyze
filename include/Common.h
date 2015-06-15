#ifndef _COMMON_H
#define _COMMON_H

#include <linux/videodev2.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <semaphore.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;
using namespace cv;

#define DEBUG

#ifdef DEBUG
        #define dbgprint(format,args...) \
        fprintf(stderr, format, ##args)
#else
        #define dbgprint(format,args...)
#endif

#endif

