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


//���ⳤ�ȶ���
#define SINGLE_IP_LEN_15        		15
#define SINGLE_PORT_LEN_6           6
#define SINGLE_URL_LEN_128          128

typedef unsigned char  		uint8;
typedef unsigned short 		uint16;
typedef unsigned int       	uint32;

//��ͷ����
typedef struct COMMON_PACKET_HEAD 
{
	uint32	magic;					//��ͷʶ����
	uint16      encrypt;				       //�������� MsgEncryptType
	uint16	cmd;					//����ID
	uint32      EncryptLen;				//�������ݰ�(���岿��)�ܳ��ȣ����ܺ�
	uint32      UnEncryptLen;			//����ǰ���ݰ�(���岿��)����
	uint32	CompressedLen;			//ѹ�������ݰ��峤��
	uint32	UnCompressedLen;		//δѹ�����峤
	uint16	chksum;					//У���
	uint16	unused;					//����

} T_PacketHead,*PT_PacketHead;

//////////////////////////////////////////////// MCU /////////////////////////////////////////////

enum
{
	//������������MCUͨ������
	
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
	uint16	Ack;    //ACK״̬ 0���ɹ� 1��ʧ��
	
} ST_SM_VDCS_MCU_REGISTER_ACK;

/*--------OPERATE-------*/

typedef struct 
{
	uint8   	UserName[USRNAME_LEN_20];
	uint8		MCUAddr [MCU_MAC_LEN_20];
	uint8		port;                     // 0~3   4��������
	uint8		TermType;          // 3/4 ������ ���ǵ����
	uint8		OpFlag;               // ����ָ����       0 / 1  kai /guan
	uint16	Res;     
	
} ST_SM_VDCS_MCU_OPERATE_TERM;

typedef struct
{
	uint8		UserName[USRNAME_LEN_20];
	uint8		MCUAddr[MCU_MAC_LEN_20];
	uint8		port;
	uint8		TermType;
	uint16	Ack;	                  //ACK״̬ 0���ɹ� 1��ʧ��
	
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
	uint16	Ack;			//ACK״̬ 0���ɹ� 1��ʧ��
	
} ST_SM_VDCS_MCU_ROUTING_INSPECTION_ACK;


////////////////////////////////////////////////////////������//////////////////////////////////////////////////////////////////

enum{
	// �����������봦���������ͨѶ����
	
	SM_ANAY_VDCS_REGISTER = 0x0400,      // �� MCU ��ַ���� ���������
	SM_VDCS_ANAY_REGISTER_ACK,       
	SM_VDCS_ANAY_DEVICE_CONTROL,        // ���Ƶ���� ��������
	SM_ANAY_VDCS_DEVICE_CONTROL_ACK, 
	SM_ANAY_VDCS_ROUTING_INSPECTION,   
	SM_VDCS_ANAY_ROUTING_INSPECTION_ACK,
	SM_ANAY_VDCS_DEVICE_STATUS,          // MCU ���߷���
	SM_VDCS_ANAY_DEVICE_STATUS_ACK,
	SM_ANAY_VDCS_WARN_INFO,              // �����������ﵽ���������������
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

