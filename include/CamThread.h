#ifndef _CAM_THREAD_H
#define _CAM_THREAD_H

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "CmdDefine.h"
#include "TcpCom.h"

typedef struct _CAMERA_ARRAY{

	pthread_t               threadID;
	uint8                      ialive;
	ST_SM_SINGLE_CAMERA_PARAM  	t_CameraParam;
	
}T_CAMERA_ARRAY;

int camera_process(char * buffer,int len,PT_TcpClient instance);

#endif
