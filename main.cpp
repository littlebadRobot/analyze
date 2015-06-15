#include "Common.h"
#include "CamThread.h"
#include "TcpCom.h"
#include "CmdDefine.h"

//全局变量
T_ClientMess    	t_ClientMcuMess;   	/* MCU 客户端 信息 */
T_TcpServer     	t_AnayServer;        	/* 本地视频分析服务器 */
T_ServerMess   	t_ServerProMess;     	/* 处理服务器的信息 */

T_CAMERA_ARRAY	g_CameraArray[4] ;

uint8          g_AnayRegister 		= 	0;
uint8          g_CameraArryNum  	=	0;
uint8          g_CameraFlag    		= 	0;
uint8          g_Disconnect               =	0;

const char*    server_port  	= 	"9097";
const char*    server_ip    	= 	"192.168.1.109";

int  re_start_connect_to_server(void )
{
	int iRet = -1;
 	
	pthread_t ClientID;
	
	iRet = pthread_create(&ClientID,NULL,tcp_client_thread,NULL);
	if(iRet != 0){
		 printf("create client_thread error!\n");
		 return -1;
	} 
	pthread_detach(ClientID);
	return 0;
}

int main(int argc ,char**argv)
{
 	int iRet = -1;
 	
	pthread_t ClientID;
	pthread_t ClientMcuID;
	
	iRet = pthread_create(&ClientID,NULL,tcp_client_thread,NULL);
	if(iRet != 0){
		 printf("create client_thread error!\n");
		 return -1;
	} 
	pthread_detach(ClientID);


	iRet = pthread_create(&ClientMcuID,NULL,tcp_mcu_thread,NULL);
	if(iRet != 0){
		 printf("create tcp_mcu_thread error!\n");
		 return -1;
	} 
	pthread_detach(ClientMcuID);

	while(1){
		sleep(2);
		if(g_Disconnect){
			
			re_start_connect_to_server();
		}
	}
	return 0;

}




