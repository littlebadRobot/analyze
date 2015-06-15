
#ifndef _RTPTRANS_H_
#define _RTPTRANS_H_

extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h> 
#include <libavformat/avformat.h>
}


typedef struct _process_camera_param_{
	
	//服务器传来的参数
	char    ip[15];
	uint16  port;
	char   url[128];
	uint8  WarnType;
	uint8  AutoWarn;
	uint16 WarnNumber;
	uint8  WarnFlag;
	uint8  WarnCount;
	
	//摄像头的参数
	VideoCapture  *vcap;
	int           rows;
	int           cols;
	
}T_ProcessCameraParam;

int open_camera_rtp_trans(ST_SM_SINGLE_CAMERA_PARAM* pt_CameraParam);

#endif

