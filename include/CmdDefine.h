#ifndef _CMDDEFINE_H
#define	_CMDDEFINE_H

#pragma pack(push) 
#pragma pack(1)

#define T_PACKETHEAD_MAGIC	0xfefefefe

#define  MAC_STR_LEN                         6
#define  PACKET_HEAD_LEN    		  28
#define  MCU_MAC_LEN_20         	  20
#define  MCU_MAC_LEN_32         	  32
#define  DEV_ADDR_LEN_32         	  32
#define  RE_CONTROL_LEN_20           20
#define  USRNAME_LEN_20             	   20


//特殊长度定义
#define SINGLE_IP_LEN_15        		15
#define SINGLE_PORT_LEN_6           6
#define SINGLE_URL_LEN_128          128

typedef unsigned char  		uint8;
typedef unsigned short 		uint16;
typedef unsigned int       	uint32;

//包头定义
typedef struct COMMON_PACKET_HEAD 
{
	uint32	magic;					//包头识别码
	uint16      encrypt;				       //加密类型 MsgEncryptType
	uint16	cmd;					//命令ID
	uint32      EncryptLen;				//传输数据包(包体部分)总长度（加密后）
	uint32      UnEncryptLen;			//加密前数据包(包体部分)长度
	uint32	CompressedLen;			//压缩后数据包体长度
	uint32	UnCompressedLen;		//未压缩包体长
	uint16	chksum;					//校验和
	uint16	unused;					//保留

} T_PacketHead,*PT_PacketHead;

//////////////////////////////////////////////// MCU /////////////////////////////////////////////

enum
{
	//分析服务器和MCU通信命令
	
	SM_MCU_VDCS_ENCRY_REQ = 0x1000,
	SM_VDCS_MCU_ENCRYPT_ACK,
	SM_MCU_VDCS_ENCRYPT_ACK,
	SM_VDCS_MCU_PUBLIC_KEY,
	SM_MCU_VDCS_ENCRYPT_KEY,
	SM_VDCS_MCU_ENCRYPT_KEY_ACK,
	SM_MCU_VDCS_REGISTER,
	SM_VDCS_MCU_REGISTER_ACK,
	SM_VDCS_MCU_OPERATE_TERM,
	SM_MCU_VDCS_OPERATE_TERM_ACK,
	SM_MCU_VDCS_ROUTING_INSPECTION,
	SM_VDCS_MCU_ROUTING_INSPECTION_ACK,
	
    	SM_MCU_HEARTBEAT = 0x8001
};

enum{
	  MCU_NOT_REGISTER  =0x01,
	  MCU_HAVE_REGISTER,
	  MCU_NEW_REGISTER
};

/*--------REG-------*/
typedef struct 
{
	uint8		MCUAddr[MCU_MAC_LEN_20];
	
} ST_SM_MCU_VDCS_REGISTER;

typedef struct 
{
	uint8		MCUAddr[MCU_MAC_LEN_20];
	uint16	Ack;    //ACK状态 0：成功 1：失败
	
} ST_SM_VDCS_MCU_REGISTER_ACK;

/*--------OPERATE-------*/

typedef struct 
{
	uint8   	UserName[USRNAME_LEN_20];
	uint8		MCUAddr [MCU_MAC_LEN_20];
	uint8		port;                     // 0~3   4个报警器
	uint8		TermType;          // 3/4 报警器 还是电控锁
	uint8		OpFlag;               // 操作指令码       0 / 1  kai /guan
	uint16	Res;     
	
} ST_SM_VDCS_MCU_OPERATE_TERM;

typedef struct
{
	uint8		UserName[USRNAME_LEN_20];
	uint8		MCUAddr[MCU_MAC_LEN_20];
	uint8		port;
	uint8		TermType;
	uint16	Ack;	                  //ACK状态 0：成功 1：失败
	
} ST_SM_MCU_VDCS_OPERATE_TERM_ACK;

/*--------ROUTING-------*/
typedef struct 
{
	uint8		ReControl[RE_CONTROL_LEN_20];
	uint8   	Action;
	
} ST_SM_MCU_VDCS_ROUTING_INSPECTION;

typedef struct 
{
	uint8		ReControl[RE_CONTROL_LEN_20];		
	uint8   	Action;
	uint16	Ack;			//ACK状态 0：成功 1：失败
	
} ST_SM_VDCS_MCU_ROUTING_INSPECTION_ACK;


////////////////////////////////////////////////////////服务器//////////////////////////////////////////////////////////////////

enum{
	// 分析服务器与处理服务器的通讯命令
	
	SM_ANAY_VDCS_REGISTER = 0x0400,      // 把 MCU 地址发给 处理服务器
	SM_VDCS_ANAY_REGISTER_ACK,       
	SM_VDCS_ANAY_DEVICE_CONTROL,        // 控制电控锁 报警器等
	SM_ANAY_VDCS_DEVICE_CONTROL_ACK, 
	SM_ANAY_VDCS_ROUTING_INSPECTION,   
	SM_VDCS_ANAY_ROUTING_INSPECTION_ACK,
	SM_ANAY_VDCS_DEVICE_STATUS,          // MCU 掉线发送
	SM_VDCS_ANAY_DEVICE_STATUS_ACK,
	SM_ANAY_VDCS_WARN_INFO,              // 分析服务器达到报警最大人数发送
	SM_VDCS_ANAY_WARN_INFO_ACK,
	SM_VDCS_ANAY_PUSH_CAMERA_PARAM,             // 
	SM_VDCS_ANAY_RENEW_REGISTER_MCU,
	SM_VDCS_ANAY_RENEW_REGISTER_MCU_ACK,

	
	SM_ANAY_HEATBEAT = 0X8003

};

enum DeviceType
{
	DeviceTypeNetCamera =1,
	DeviceTypeUsbCamera,
	DeviceTypeWarn,
	DeviceTypeLock,
	DeviceTypeMcu,
	DeviceTypeNetControl
};

typedef struct _SM_SINGLE_CAMERA_PARAM{
	
	char  	IP[SINGLE_IP_LEN_15];
	char   	PORT[SINGLE_PORT_LEN_6];
	char   	URL[SINGLE_URL_LEN_128];
	uint8 	WarnType;
	uint8  	AutoWarn;
	uint16      WarnNumber;
		
}ST_SM_SINGLE_CAMERA_PARAM;


typedef struct _SM_VDCS_ANAY_CAMERA_PARAM
{
	char   	MCUAddr[MCU_MAC_LEN_32];
	char   	IPList[200];
	char   	PortList[200];
	char   	UrlList[500];
	uint8  	WarnType;
	uint8  	AutoWarn;
	uint16 	WarnNumber;
	uint8  	CameCount;

}ST_SM_VDCS_ANAY_CAMERA_PARAM;

typedef struct _SM_ANAY_VDCS_REGISTER
{
	char 	MCUAddr[MCU_MAC_LEN_32];
	
} ST_SM_ANAY_VDCS_REGISTER;

typedef struct _SM_VDCS_ANAY_REGISTER_ACK
{
	char 	MCUAddr[MCU_MAC_LEN_32];
	uint8 	Ack;
	
} ST_SM_VDCS_ANAY_REGISTER_ACK;

typedef struct _SM_VDCS_ANAY_DEVICE_CONTROL
{
	char   	UserName[USRNAME_LEN_20];
	char   	DeviceAddr[DEV_ADDR_LEN_32];
	char   	MCUAddr[MCU_MAC_LEN_32];
	uint8  	DeviceType;
	uint8  	OpFlag;
	uint8  	Port;

} ST_SM_VDCS_ANAY_DEVICE_CONTROL;

typedef struct _SM_ANAY_VDCS_DEVICE_CONTROL_ACK
{
	char 	UserName[USRNAME_LEN_20];
	char 	DeviceAddr[DEV_ADDR_LEN_32];
	char 	MCUAddr[MCU_MAC_LEN_32];
	uint8  	DeviceType;
	uint8 	OpFlag;
	uint8  	Port;
	uint8  	Ack;

} ST_SM_ANAY_VDCS_DEVICE_CONTROL_ACK;

typedef struct _SM_ANAY_VDCS_DEVICE_STATUS
{
	char  	MCUAddr[MCU_MAC_LEN_32];
	uint8 	DeviceType;
	uint8 	port;
	
}ST_SM_ANAY_VDCS_DEVICE_STATUS;

typedef struct _SM_VDCS_ANAY_DEVICE_STATUS_ACK
{
	char 	MCUAddr[MCU_MAC_LEN_32];
	uint8 	DeviceType;
	uint8 	port;
	uint8 	Ack;

}ST_SM_VDCS_ANAY_DEVICE_STATUS_ACK;

typedef struct _SM_ANAY_VDCS_ROUTING_INSPECTION
{
	char		ReControl[RE_CONTROL_LEN_20];
	char    	McuAddr[MCU_MAC_LEN_32];
	uint8		Action;

} ST_SM_ANAY_VDCS_ROUTING_INSPECTION;

typedef struct _SM_VDCS_ANAY_ROUTING_INSPECTION_ACK
{
	char		ReControl[RE_CONTROL_LEN_20];	
	char    	McuAddr[MCU_MAC_LEN_32];
	uint8		Action;
	uint8   	Ack;

} ST_SM_VDCS_ANAY_ROUTING_INSPECTION_ACK;

typedef struct _SM_ANAY_VDCS_WARN_INFO
{
	char   	MCUAddr[MCU_MAC_LEN_32];	
	char   	CameUrl[SINGLE_URL_LEN_128];
	uint16 	InCount;
	uint16 	OutCount;
	uint16	MaxCount;
	uint8 	WarnType;

} ST_SM_ANAY_VDCS_WARN_INFO;

typedef struct _SM_VDCS_ANAY_WARN_INFO_ACK
{
	char   	MCUAddr[MCU_MAC_LEN_32];
	uint16 	Ack;

} ST_SM_VDCS_ANAY_WARN_INFO_ACK;

typedef struct _SM_VDCS_ANAY_RENEW_REGISTER_MCU
{
	char   	MCUAddr[MCU_MAC_LEN_32];
	uint8        ConnectType;                // 0  reconnect \\ 1 new connect

} ST_SM_VDCS_ANAY_RENEW_REGISTER_MCU;

typedef struct _SM_VDCS_ANAY_RENEW_REGISTER_MCU_ACK
{
	char   	MCUAddr[MCU_MAC_LEN_32];
	uint8        ConnectType;                // 0  reconnect \\ 1 new connect
	uint8        Ack;

} ST_SM_VDCS_ANAY_RENEW_REGISTER_MCU_ACK;

#pragma pack(pop)

#endif

