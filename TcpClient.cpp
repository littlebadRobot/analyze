#include "Common.h"
#include "TcpCom.h"
#include "CmdDefine.h"
#include "CamThread.h"

extern T_ClientMess   	t_ClientMcuMess;
extern T_ServerMess   	t_ServerProMess; 

extern const char*      server_port ;
extern const char*      server_ip   ;

extern T_CAMERA_ARRAY	g_CameraArray[4] ;
extern uint8          g_AnayRegister;
extern uint8          g_CameraArryNum ;
extern uint8          g_CameraFlag ;
extern uint8          g_Disconnect ;

int anay_register_ack(char * buffer,PT_TcpClient instance)
{
	ST_SM_VDCS_ANAY_RENEW_REGISTER_MCU_ACK  t_AnayRegAck;
	uint8 	ack;

	memset(&t_AnayRegAck,0,sizeof(ST_SM_VDCS_ANAY_RENEW_REGISTER_MCU_ACK));
	memcpy(&t_AnayRegAck,buffer+PACKET_HEAD_LEN,sizeof(ST_SM_VDCS_ANAY_RENEW_REGISTER_MCU_ACK));

	ack = t_AnayRegAck.Ack;
	if(ack == 0){
			dbgprint("anay register sucess!\n");
			g_AnayRegister = 1;
			return 0;
	}else{
			dbgprint("anay register failed!\n");
			return -1;
	}
	return -1;
}

int anay_re_register_ack(char * buffer,PT_TcpClient instance)
{
	ST_SM_VDCS_ANAY_REGISTER_ACK t_AnayReRegAck;
	uint8 ack;

	memset(&t_AnayReRegAck,0,sizeof(ST_SM_VDCS_ANAY_REGISTER_ACK));
	memcpy(&t_AnayReRegAck,buffer+PACKET_HEAD_LEN,sizeof(ST_SM_VDCS_ANAY_REGISTER_ACK));

	ack = t_AnayReRegAck.Ack;
	if(ack == 0){
			dbgprint("--anay re_register sucess!--\n");
			g_AnayRegister = 1;
			return 0;
	}else{
			dbgprint("--anay re_register  failed!--\n");
			return -1;
	}
	return -1;
}

void mcu_inactive_ack(char * buffer,PT_TcpClient instance)
{
	ST_SM_VDCS_ANAY_DEVICE_STATUS_ACK t_AnayStatusAck;
	uint8 ack;

   	 memset(&t_AnayStatusAck,0,sizeof(ST_SM_VDCS_ANAY_DEVICE_STATUS_ACK));
	memcpy(&t_AnayStatusAck,buffer+PACKET_HEAD_LEN,sizeof(ST_SM_VDCS_ANAY_DEVICE_STATUS_ACK));
	ack = t_AnayStatusAck.Ack;
	if(ack == 0){
			dbgprint("mcu inactive report sucess!\n");
	}else{
		dbgprint("mcu inactive report error!\n");
	}
}

int device_control_process(char * buffer,PT_TcpClient instance)
{
	T_PacketHead					t_PackHeadMcuOpr;
	ST_SM_VDCS_MCU_OPERATE_TERM     t_McuOpr;

	ST_SM_VDCS_ANAY_DEVICE_CONTROL  t_DeviceControl;
	int McuFd = -1;
	char   ControlBuff[28 + 45]={0};
	int   iRet = -1;

	memset(&t_PackHeadMcuOpr,0,sizeof(T_PacketHead));
	memset(&t_McuOpr,0,sizeof(ST_SM_VDCS_MCU_OPERATE_TERM));
		
	memset(&t_DeviceControl,0,sizeof(ST_SM_VDCS_ANAY_DEVICE_CONTROL));
	memcpy(&t_DeviceControl,buffer+PACKET_HEAD_LEN,sizeof(ST_SM_VDCS_ANAY_DEVICE_CONTROL));

	memcpy((char *)t_McuOpr.MCUAddr,t_DeviceControl.MCUAddr,MCU_MAC_LEN_20);

	McuFd = search_mcu_client((char *)t_McuOpr.MCUAddr);
	if(McuFd <0)
			return -1;	
	dbgprint("have searched mcu !\n");
	
	t_McuOpr.TermType      = t_DeviceControl.DeviceType;
	t_McuOpr.OpFlag        = t_DeviceControl.OpFlag;
	t_McuOpr.port          = t_DeviceControl.Port;

	memcpy((char *)t_McuOpr.UserName,t_DeviceControl.UserName ,USRNAME_LEN_20);
	memcpy((char *)t_McuOpr.MCUAddr,t_ClientMcuMess.cMac,MCU_MAC_LEN_20);

	
	t_PackHeadMcuOpr.magic        		= T_PACKETHEAD_MAGIC;
	t_PackHeadMcuOpr.cmd          		= SM_VDCS_MCU_OPERATE_TERM;
	t_PackHeadMcuOpr.UnEncryptLen 	= sizeof(ST_SM_VDCS_MCU_OPERATE_TERM);
	
	memcpy(ControlBuff,&t_PackHeadMcuOpr,PACKET_HEAD_LEN);
	memcpy(ControlBuff+PACKET_HEAD_LEN,&t_McuOpr,sizeof(ST_SM_VDCS_MCU_OPERATE_TERM));

	iRet = send(McuFd,ControlBuff,sizeof(ControlBuff),0);
	if(iRet <0){
		printf("send error!\n");
		return -1;
	}
	printf("send %d types to mcu!\n",iRet);
	
	return iRet;
}

int warn_info_ack(char * buffer)
{
	ST_SM_VDCS_ANAY_WARN_INFO_ACK  t_AnayWarnAck;
	uint8 ack;
	
    memset(&t_AnayWarnAck,0,sizeof(ST_SM_VDCS_ANAY_WARN_INFO_ACK));
	memcpy(&t_AnayWarnAck,buffer+PACKET_HEAD_LEN,sizeof(ST_SM_VDCS_ANAY_WARN_INFO_ACK));
		
	ack = t_AnayWarnAck.Ack;
	if(ack == 0){
			dbgprint("ipCamera warninfo report sucess!\n");
	}else{
		dbgprint("ipCamera warninfo report error!\n");
	}
	return 0;
}

int routing_inspection_ack(char * buffer)
{
	ST_SM_VDCS_ANAY_ROUTING_INSPECTION_ACK  t_AnayRoutingAck;
	uint8 ack;
	
    	memset(&t_AnayRoutingAck,0,sizeof(ST_SM_VDCS_ANAY_ROUTING_INSPECTION_ACK));
	memcpy(&t_AnayRoutingAck,buffer+PACKET_HEAD_LEN,sizeof(ST_SM_VDCS_ANAY_ROUTING_INSPECTION_ACK));
	
	dbgprint("t_AnayRoutingAck.Action is %c\n",t_AnayRoutingAck.Action);
	
	ack = t_AnayRoutingAck.Ack;
	if(ack == 0){
			dbgprint("routing inspection ack report sucess!\n");
	}else{
		dbgprint("routing inspection ack report error!\n");
	}
	return 0;	
}

int server_data_analyse(char * buffer,int len,PT_TcpClient instance)
{
	uint16 	cmd;
	T_PacketHead   t_packet_head;
	
	if(len < PACKET_HEAD_LEN){
		 printf("receive error!\n");
		 return SERVER_DATA_ERR;
	}

	memcpy(&t_packet_head,buffer,PACKET_HEAD_LEN);
	cmd = t_packet_head.cmd;
	
	switch (cmd ){
		case SM_VDCS_ANAY_PUSH_CAMERA_PARAM: 
		      	  camera_process(buffer,len,instance);
			  break;
		case SM_VDCS_ANAY_REGISTER_ACK:      
			  anay_register_ack(buffer,instance);
		      break;
		case SM_VDCS_ANAY_DEVICE_CONTROL:    
			  device_control_process(buffer,instance);
			  break;
		case SM_VDCS_ANAY_DEVICE_STATUS_ACK: 
			  mcu_inactive_ack(buffer,instance);
		      break;
		case SM_VDCS_ANAY_WARN_INFO_ACK:      
			  warn_info_ack(buffer);
			  break;
		case SM_VDCS_ANAY_ROUTING_INSPECTION_ACK:
			  routing_inspection_ack(buffer);
			  break;
		case SM_ANAY_HEATBEAT:              

		case SM_VDCS_ANAY_RENEW_REGISTER_MCU_ACK:
			  anay_re_register_ack(buffer,instance);
			  break;
		default: break;
	}
	dbgprint("server cmd = %x\n", cmd);
	return 0;
}

static void client_read_cb(struct bufferevent* bev, void* arg)	
{
	char buf[1024];  
	int 	iLen;
	PT_TcpClient instance  =  (PT_TcpClient)arg; 
	
	memset(buf,0,1024);
	struct evbuffer *input = bufferevent_get_input(bev);
	if (evbuffer_get_length(input) == 0) {
		printf("no data read!\n");
		bufferevent_free(bev);
	} 
	
	iLen = evbuffer_remove(input, buf, sizeof(buf)); 

	server_data_analyse(buf,iLen,instance);
	
	return ;
}

static void client_event_cb(struct bufferevent *bev, short event, void *arg)	
{
	PT_TcpClient instance  =  (PT_TcpClient)arg; 
	struct timeval delay = { 0, 200 };

	if (event & BEV_EVENT_TIMEOUT) {
		printf("--process server connectTimed out--\n"); 
	}
	else if (event & BEV_EVENT_EOF)	{
		printf("connection closed\n");
	}
	else if (event & BEV_EVENT_ERROR) { 
		printf("process server close connection\n"); 
	}
	else if( event & BEV_EVENT_CONNECTED)  {  
		printf("--the client has connected to process server--\n");
		return;
	}    
	
	//bufferevent_free(bev);
	g_Disconnect =1;
	event_base_loopexit(instance->pt_PServerBase, &delay);

	return ;
}

int mcu_empty()
{
	int i=0;
	
	for(i;i<6;i++)
	{
		if(t_ClientMcuMess.cMac[i] != 0x00)
			                 return 0;
	}
	return 1;
}

int analyze_register(PT_TcpClient instance)
{

	ST_SM_ANAY_VDCS_REGISTER st_AnayReg;
	T_PacketHead             t_PackHeadAnayReg;
	char AnayRegBuff[60] = {0};
	int iRet = -1;
	memset(&t_PackHeadAnayReg,0 ,sizeof(t_PackHeadAnayReg));
	memset(&st_AnayReg,0 ,sizeof(st_AnayReg));
	t_PackHeadAnayReg.magic          = T_PACKETHEAD_MAGIC;
	t_PackHeadAnayReg.cmd	         = SM_ANAY_VDCS_REGISTER;
	t_PackHeadAnayReg.UnEncryptLen   = sizeof(ST_SM_ANAY_VDCS_REGISTER);

	while(mcu_empty()){
		   sleep(2);
	}
	
	memcpy(st_AnayReg.MCUAddr,t_ClientMcuMess.cMac,MCU_MAC_LEN_20);

	memcpy(AnayRegBuff,&t_PackHeadAnayReg,sizeof(t_PackHeadAnayReg));
	memcpy(AnayRegBuff+sizeof(t_PackHeadAnayReg),&st_AnayReg,sizeof(st_AnayReg));
	iRet = send(t_ServerProMess.iClientFd,AnayRegBuff,sizeof(AnayRegBuff),0);
	dbgprint("send server %d \n",iRet);
	return iRet;
}

int send_hreatbeat(PT_TcpClient instance)
{
	int iRet = -1;
	T_PacketHead heartbeat;
	memset(&heartbeat, 0 ,sizeof(heartbeat));
	heartbeat.magic        	=  T_PACKETHEAD_MAGIC;
	heartbeat.cmd	       		=  SM_ANAY_HEATBEAT;
	heartbeat.UnEncryptLen =  0;
	iRet = send(t_ServerProMess.iClientFd,&heartbeat,sizeof(T_PacketHead),0);
	return iRet;
}

static void client_timer_task(int fd, short events, void * arg) 
{
	PT_TcpClient instance  =  (PT_TcpClient)arg; 
	
	if((!g_AnayRegister))
	{
		analyze_register(instance);
		sleep(1);
	}
	if(g_AnayRegister){
		send_hreatbeat(instance);
	}
	event_add(instance->pt_PServerTimerEv, &(instance->t_PServerTv));
	return ;
}

void *tcp_client_thread(void*)
{
	T_TcpClient   t_ClientInstance;
	int                   Client_fd   = -1;
	
	g_Disconnect =0;

	memset(&t_ClientInstance, 0 ,sizeof(T_TcpClient));


	memset(&t_ServerProMess, 0 ,sizeof(T_ServerMess));
	t_ServerProMess.iClientFd = -1 ;

	t_ClientInstance.pt_PServerBase  = event_base_new();  
	if (!t_ClientInstance.pt_PServerBase) {
		printf("Could not initialize libevent!\n");
		return NULL;
	}

	t_ClientInstance.pt_PServerBev = bufferevent_socket_new(t_ClientInstance.pt_PServerBase, -1, BEV_OPT_CLOSE_ON_FREE);  
	if (!t_ClientInstance.pt_PServerBev) {
		printf("Error constructing bufferevent!\n");
		return NULL;
	}

	t_ClientInstance.t_PServerTv.tv_sec 	= 	5;
	t_ClientInstance.t_PServerTv.tv_usec 	=	0;	

	memset(&t_ClientInstance.t_PSeverAddr, 0, sizeof(t_ClientInstance.t_PSeverAddr) );	
	t_ClientInstance.t_PSeverAddr.sin_family 	= 	AF_INET;  
	t_ClientInstance.t_PSeverAddr.sin_port 	= 	htons(atoi(server_port));  
	inet_aton(server_ip, &t_ClientInstance.t_PSeverAddr.sin_addr);

	bufferevent_socket_connect(t_ClientInstance.pt_PServerBev, (struct sockaddr *)&t_ClientInstance.t_PSeverAddr,  sizeof(t_ClientInstance.t_PSeverAddr));

	Client_fd = bufferevent_getfd(t_ClientInstance.pt_PServerBev); 
	while (connect(Client_fd, (struct sockaddr *)&t_ClientInstance.t_PSeverAddr, sizeof(t_ClientInstance.t_PSeverAddr)) < 0){
		sleep(2);
	}

	t_ServerProMess.iClientFd  	= Client_fd;

	bufferevent_setcb(t_ClientInstance.pt_PServerBev ,client_read_cb, NULL, client_event_cb, (void*)&t_ClientInstance);  
	bufferevent_enable(t_ClientInstance.pt_PServerBev, EV_READ |EV_WRITE);

	t_ClientInstance.pt_PServerTimerEv = evtimer_new(t_ClientInstance.pt_PServerBase,client_timer_task,(void*)&t_ClientInstance);
	if (!t_ClientInstance.pt_PServerTimerEv || event_add(t_ClientInstance.pt_PServerTimerEv, &t_ClientInstance.t_PServerTv)<0) {
		printf("Could not create/add a timer_event!\n");
		return NULL;
	}
		
	event_base_dispatch	(t_ClientInstance.pt_PServerBase);	
	// 释放资源
	event_base_free		(t_ClientInstance.pt_PServerBase);
	bufferevent_free          (t_ClientInstance.pt_PServerBev);	
	evtimer_del                   (t_ClientInstance.pt_PServerTimerEv);

        //对于全局变量的资源处理
	if(t_ServerProMess.iClientFd > 0){
		close (t_ServerProMess.iClientFd);
		t_ServerProMess.iClientFd = -1;
	}
	
	t_ServerProMess.ialive   = 0;

	for(int i=0 ;i<4;i++){
			memset(&g_CameraArray[i], 0 ,sizeof(T_CAMERA_ARRAY));
	}

	 g_AnayRegister 		= 	0;
	 g_CameraArryNum  	=	0;
	 g_CameraFlag    		= 	0;
	 
	 printf("-- tcp_client_thread is down--\n");
	 return NULL;
}


