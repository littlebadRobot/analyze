
#include "Common.h"
#include "CamThread.h"
#include "Rtptrans.h"

extern T_CAMERA_ARRAY   g_CameraArray[4];
extern uint8                             g_CameraArryNum;
extern uint8         			g_CameraFlag  ;

void *Camera_thread(void * arg)
{
	int iRet = -1;
	ST_SM_SINGLE_CAMERA_PARAM*  pt_CameraParam  = (ST_SM_SINGLE_CAMERA_PARAM* )arg;

	iRet = open_camera_rtp_trans(pt_CameraParam);
 	return NULL;
}

int add_camera_array(ST_SM_SINGLE_CAMERA_PARAM t_CameraParam)
{
	int iCamNum = 0;
	
	for(iCamNum=0;iCamNum<4;iCamNum++){
		if((g_CameraArray[iCamNum].ialive == 1) &&(!strcmp(t_CameraParam.PORT ,g_CameraArray[iCamNum].t_CameraParam.PORT))){
			printf("PORT is  in active!\n");
			return -1;
		}
		
		if(g_CameraArray[iCamNum].ialive == 0){
			    	g_CameraArray[iCamNum].t_CameraParam = t_CameraParam;
				g_CameraArray[iCamNum].ialive       = 1;
				g_CameraArryNum++;
				dbgprint("g_CameraArryNum is %d\n",g_CameraArryNum);
				return 0;
		}
	}
	return -1;
}

int del_camera_array(pthread_t ID ,uint8 status)
{
	int iCamNum = 0;
	for(iCamNum=0;iCamNum<4;iCamNum++){

				g_CameraArryNum--;
				dbgprint("g_CameraArryNum is %d\n",g_CameraArryNum);
				return 0;
	}
	return -1;
}

int multi_camera_process(int Count,ST_SM_VDCS_ANAY_CAMERA_PARAM * param)
{
	ST_SM_SINGLE_CAMERA_PARAM t_CameraParam[4];
	int iRet = -1;
	int num=0;
	int i;

	for(i=0;i<4;i++){
		memset(&t_CameraParam[i], 0 ,sizeof(ST_SM_SINGLE_CAMERA_PARAM));
	}
	
	for(i=0;i<4;i++){
		t_CameraParam[i].WarnType   = param->WarnType;
		t_CameraParam[i].AutoWarn   = param->AutoWarn;
		t_CameraParam[i].WarnNumber = param->WarnNumber;
	}

	//IPList PortList UrlList
	strcpy( (char *)(t_CameraParam[0].IP), strtok(param->IPList, ","));
	for(i=1;i<Count;i++){
		 strcpy( t_CameraParam[i].IP, strtok(NULL, ","));
	}


	strcpy( (char *)t_CameraParam[0].PORT , strtok(param->PortList, ","));
	for(i=1;i<Count;i++){
		 strcpy( t_CameraParam[i].PORT , strtok(NULL, ","));
	}
	
	strcpy((char *)t_CameraParam[0].URL ,strtok(param->UrlList, ","));
	for(i=1;i<Count;i++){
		  strcpy( t_CameraParam[i].URL , strtok(NULL, ","));
	}

	for(i=0;i<Count;i++)
	{
		dbgprint("camera%d ip       is %s\n",i,t_CameraParam[i].IP);
		dbgprint("camera%d port     is %s\n",i,t_CameraParam[i].PORT);
		dbgprint("camera%d URL      is %s\n",i,t_CameraParam[i].URL);
		dbgprint("camera%d WarnType is %d\n",i,t_CameraParam[i].WarnType);
		dbgprint("camera%d AutoWarn is %d\n",i,t_CameraParam[i].AutoWarn);
	}

	for(i=0;i<Count;i++)
	{		
	 	iRet =add_camera_array(t_CameraParam[i]);
		if(iRet == 0)
				num++;
	}
	return num;
}

int ip_port_url_process(int Count,ST_SM_VDCS_ANAY_CAMERA_PARAM * param) //解析 参数
{
	int iRet = -1; 
	int num=0;

	if(Count <= 0){
		printf("camera num error!\n");
		return -1;
	}else {
		num = multi_camera_process(Count,param);
	}
	return num;
}

int create_camera_thread(int iAddnum)
{
	pthread_t pthread_ID;
	int iRet = -1;
	int num =0;
    	int iCamNum = 0;
	
	for(iCamNum=0;iCamNum<4;iCamNum++){
		if(g_CameraArray[iCamNum].threadID == 0)
											break;
	}
	g_CameraFlag   =	1;
	while(iAddnum){
		if(g_CameraArray[iCamNum].threadID == 0){
			iRet = pthread_create(&pthread_ID,NULL,Camera_thread,(void*)&(g_CameraArray[iCamNum].t_CameraParam));
			if(iRet != 0){
				 printf("create Camera_thread error!\n");
				 continue;
			}
			dbgprint("pthread_create !\n");
			g_CameraArray[iCamNum].threadID = pthread_ID;
			pthread_detach(pthread_ID);	
			num++;
		}
		iAddnum--;
		iCamNum++;
		
		if(iCamNum>3)
					break;
	}
	return num;
}

int camera_process(char * buffer,int len,PT_TcpClient instance)
{
	//int iRet;
	int 	num 		 =	-1;
	int    CameraCount = 	-1; 
	
	ST_SM_VDCS_ANAY_CAMERA_PARAM  t_CameraParam;

	for(int i=0 ;i<4;i++){
			memset(&g_CameraArray[i], 0 ,sizeof(T_CAMERA_ARRAY));
	}

        // restart camera
	g_CameraFlag  		= 	0;
	g_CameraArryNum  	=	0;

	sleep(1);
	
	if(len != (sizeof(ST_SM_VDCS_ANAY_CAMERA_PARAM)+PACKET_HEAD_LEN)){
		         	printf("camera_process receive error!\n");  
				 return -1;
	}
	
	memset(&t_CameraParam,0,sizeof(ST_SM_VDCS_ANAY_CAMERA_PARAM));
	memcpy(&t_CameraParam,buffer+PACKET_HEAD_LEN,sizeof(ST_SM_VDCS_ANAY_CAMERA_PARAM));
	CameraCount = t_CameraParam.CameCount;
	
	if(CameraCount > 4){
		CameraCount  =	4;
	}
	
	dbgprint("camera count is %d \n",CameraCount);
	num = ip_port_url_process(CameraCount,&t_CameraParam);
	
	if(num < 1){
		printf("cant add camera!\n");
		return -1;
	}
	//打开线程
	create_camera_thread(num);
	return 0;
}

