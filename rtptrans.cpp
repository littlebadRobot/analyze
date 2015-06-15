#include "algorithm.h"
#include "Common.h"
#include "CamThread.h"
#include "Rtptrans.h"
#include "camera.h"
#include "SingleCamera.h"
#include "Common.h"
#include "TcpCom.h"
#include "CmdDefine.h"

extern T_ServerMess  t_ServerProMess; 
extern T_ClientMess   t_ClientMcuMess; 

extern       uint8    g_CameraFlag ;
extern const char*    server_ip;


int open_camera_stream(T_ProcessCameraParam* pt_PosCameraParam)
{
	Mat image;

	if(!(pt_PosCameraParam->vcap->open(pt_PosCameraParam->url))) {   //打开rtsp流
			printf("Error opening video stream or file!\n");
		    return -1;
	} 

	if(!(pt_PosCameraParam->vcap->read(image))) {   //读出图像  Mat类型
			printf("read frame error!\n");
			return -1;
	} 
	
	pt_PosCameraParam->rows  	=  image.rows;
	pt_PosCameraParam->cols  	=  image.cols;
	
	return 0;
}

int send_alarm_to_server(framedeal *pRawFrame,T_ProcessCameraParam* pt_PosCameraParam)
{
	T_PacketHead               t_PackHeadWarn;
	ST_SM_ANAY_VDCS_WARN_INFO  t_WarnInfo;
	char AlarmBuff[28+167]={0};
	int 	iRet = -1;

	memset(&t_PackHeadWarn,0,sizeof(T_PacketHead));
	memset(&t_WarnInfo,0,sizeof(ST_SM_VDCS_MCU_OPERATE_TERM));

	t_PackHeadWarn.cmd          		= SM_ANAY_VDCS_WARN_INFO;
	t_PackHeadWarn.magic        		= T_PACKETHEAD_MAGIC;
	t_PackHeadWarn.UnEncryptLen 	= sizeof(ST_SM_ANAY_VDCS_WARN_INFO);
	memcpy(AlarmBuff,&t_PackHeadWarn,PACKET_HEAD_LEN);
	
	memcpy(t_WarnInfo.MCUAddr ,t_ClientMcuMess.cMac,MCU_MAC_LEN_20);
	memcpy(t_WarnInfo.CameUrl,pt_PosCameraParam->url,SINGLE_URL_LEN_128);
	
	t_WarnInfo.MaxCount 	= 	(uint16)pRawFrame->humanstatis.numAll;
	t_WarnInfo.InCount   	= 	(uint16)pRawFrame->humanstatis.inAll;
	t_WarnInfo.OutCount  	= 	(uint16)pRawFrame->humanstatis.outAll;
	t_WarnInfo.WarnType  	= 	pt_PosCameraParam->WarnType;

	memcpy(AlarmBuff+PACKET_HEAD_LEN,&t_WarnInfo,sizeof(ST_SM_ANAY_VDCS_WARN_INFO));

	iRet = send(t_ServerProMess.iClientFd,AlarmBuff,sizeof(AlarmBuff),0);
	if(iRet <0){
		printf("send error!\n");
		return -1;
	}
	printf("warn info send %d types to server!\n",iRet);
	
	return iRet;

}

int send_alarm_to_mcu() 
{
	T_PacketHead                 t_PackHeadMcuOpr;
	ST_SM_VDCS_MCU_OPERATE_TERM  t_OprMcu;
	char   ControlBuff[28 + 45]={0};
	int	 iRet = -1;
	
	memset(&t_PackHeadMcuOpr,0,sizeof(T_PacketHead));
	memset(&t_OprMcu,0,sizeof(ST_SM_VDCS_MCU_OPERATE_TERM));
	memcpy(t_OprMcu.MCUAddr,t_ClientMcuMess.cMac,MCU_MAC_LEN_20);
	t_OprMcu.port      		=	0x01;
	t_OprMcu.TermType  	=	0x03;
	t_OprMcu.OpFlag    	=	0x00;
	
	t_PackHeadMcuOpr.magic		  = T_PACKETHEAD_MAGIC;
	t_PackHeadMcuOpr.cmd		  = SM_VDCS_MCU_OPERATE_TERM;
	t_PackHeadMcuOpr.UnEncryptLen = sizeof(ST_SM_VDCS_MCU_OPERATE_TERM);
	
	memcpy(ControlBuff,&t_PackHeadMcuOpr,PACKET_HEAD_LEN);
	memcpy(ControlBuff+PACKET_HEAD_LEN,&t_OprMcu,sizeof(ST_SM_VDCS_MCU_OPERATE_TERM));

	iRet = send(t_ClientMcuMess.iFd,ControlBuff,sizeof(ControlBuff),0);
	if(iRet <0){
		printf("send error!\n");
		return -1;
	}
	printf("send %d types to mcu!\n",iRet);
	
	return iRet;
}

int send_alarm_mcu_server(framedeal *pRawFrame,T_ProcessCameraParam* pt_PosCameraParam)
{
	int iNumAll;
	int iRet = -1;

	iNumAll = pRawFrame->humanstatis.numAll;

	if(iNumAll >= pRawFrame->human.WarnNumber){
		pt_PosCameraParam->WarnCount ++;
	}

	if(pt_PosCameraParam->WarnCount > 3){
		dbgprint("---warn---\n");
		if((pt_PosCameraParam->AutoWarn  == 0)&&(!pt_PosCameraParam->WarnFlag)){// 手动
			iRet = send_alarm_to_server(pRawFrame,pt_PosCameraParam);
			pt_PosCameraParam->WarnFlag =1;
		}else if((pt_PosCameraParam->AutoWarn  == 1)&&(!pt_PosCameraParam->WarnFlag)){ // 自动
		 	iRet = send_alarm_to_mcu();
			pt_PosCameraParam->WarnFlag =1;
		} 
		pt_PosCameraParam->WarnCount =0;
	}
	
	return iRet;
}

int decode_process_cam(T_ProcessCameraParam* pt_PosCameraParam)
{
	Mat 	image;
	int 	iRet = -1;

	uint8 	 warntype 	=	0; 
	int 		frameNum 	=	0;
 	framedeal rawframe;
	VideoCapture capture = *(pt_PosCameraParam->vcap);

	
	rawframe.human.detectorEn =0;
	rawframe.smoke.detectorEn =0;
	
	warntype =pt_PosCameraParam->WarnType;
	switch (warntype){
		case 1: 
			rawframe.human.detectorEn =1;
			break;
		case 2: 
			rawframe.smoke.detectorEn =1;
			break;
			
		default : break;
	}

	rawframe.human.WarnNumber  =(int )pt_PosCameraParam->WarnNumber;
	rawframe.human.AutoWarn =(unsigned char)pt_PosCameraParam->AutoWarn ; 
   
	memcpy((char *)rawframe.variable.videoStreamAddress,pt_PosCameraParam->url,SINGLE_URL_LEN_128);
    	init_frame(capture,rawframe);	

	image     = Mat(rawframe.variable.frame.rows, rawframe.variable.frame.cols, CV_8UC3);//三通道
	
	pt_PosCameraParam->WarnFlag =0;

	while(g_CameraFlag)
	{ 
		if(!(pt_PosCameraParam->vcap->read(rawframe.variable.frame))) {   //读出图像  Mat类型
			printf("No frame!\n");
			return -1;
		} 

		if(rawframe.human.detectorEn)
		{	   
			algorithmoperation(rawframe);

			image =rawframe.human.alarmCap.clone();		
		}
		if(rawframe.smoke.detectorEn)
		{
			operation(rawframe);
			image =rawframe.smoke.alarmCap.clone();
		}
		
		if(pt_PosCameraParam->WarnFlag){
			frameNum++;
		}
		
		if(frameNum>70){
			frameNum          =  0;
			pt_PosCameraParam->WarnFlag = 0;
		}
		
		send_alarm_mcu_server(& rawframe,pt_PosCameraParam);	
		
	}
	
	delete (pt_PosCameraParam->vcap);
	dbgprint("--IPCamera thread is down!--\n");
	return 0;
}

int open_camera_rtp_trans(ST_SM_SINGLE_CAMERA_PARAM* pt_CameraParam)
{
	char port[6] = {0};
	int  iRet = -1;
	T_ProcessCameraParam t_PosCameraParam;
	
    	memset(&t_PosCameraParam,0,sizeof(T_ProcessCameraParam));	
	t_PosCameraParam.vcap    = new VideoCapture();
	
	memcpy(port,pt_CameraParam->PORT, SINGLE_PORT_LEN_6);
	
	t_PosCameraParam.port=(port[0]-'0')*10000+(port[1]-'0')*1000+(port[2]-'0')*100+(port[3]-'0')*10+(port[4]-'0');
	memcpy(t_PosCameraParam.url, pt_CameraParam->URL,  SINGLE_URL_LEN_128);

	t_PosCameraParam.WarnNumber 	= pt_CameraParam->WarnNumber;
	t_PosCameraParam.AutoWarn   		= pt_CameraParam->AutoWarn;
	t_PosCameraParam.WarnType   		= pt_CameraParam->WarnType;
	memcpy(t_PosCameraParam.ip,pt_CameraParam->IP,15);

	iRet = open_camera_stream(&t_PosCameraParam);
	if(iRet < 0)
		return -1;

	decode_process_cam(&t_PosCameraParam);
	return 0;
}

