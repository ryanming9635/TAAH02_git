/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  HS_DVRProtocol.c                                             										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef _TOA_Protocol_
#define _TOA_Protocol_

//DVR to MCU command 
#define MCU_PROTOCOL_CMD_SYS_READY_NOTICE		0x01	///DVR system ready notice
#define MCU_PROTOCOL_CMD_WATCH_DOG_KICK			0x02	///DVR WDT kick
#define MCU_PROTOCOL_CMD_ENTRY_SLEEP				0x03	///DVR sleep mode
#define MCU_PROTOCOL_CMD_DISTANCE_RESET			0x04	///DVR distance reset  0M
#define MCU_PROTOCOL_CMD_ENCODER_SET				0x05	///Encoder deviation setting
#define MCU_PROTOCOL_CMD_CAMERA_VOLTAGE			0x06	///Camera voltage adjust
#define MCU_PROTOCOL_CMD_WDT_SETTING                         0x07	///stop retoot 
#define MCU_PROTOCOL_CMD_DC12V_PWR_START               0x08	///DC12v power start

#define MCU_PROTOCOL_CMD_GET_MCU_FW			0x07	///MCU FW version

#define MCU_PROTOCOL_CMD_DVR_REBOOT			0x11	///DVR Reboot
#define MCU_PROTOCOL_CMD_DVR_SHUTDOWN		0x12	///DVR shutdown
#define MCU_PROTOCOL_PROTOCOL_CMD_ENTER_ISP  0x30 ///isp programming MCU

//MCU to DVR command 
#define MCU_PROTOCOL_CMD_REPLY_MCU_FW  		0x80	//Reply MCU firmware version
#define MCU_PROTOCOL_CMD_REGULAR_DATA   		0x81	//Regular data
#define MCU_PROTOCOL_CMD_SYSTEM_SHUTDOWN   	0x82	//system shutdown
#define MCU_PROTOCOL_CMD_ENTRY_FACTORY   		0x83	//Entry factory mode
#define MCU_PROTOCOL_CMD_SLEEP_WAKE_UP   		0x84	//notice DVR wake up from the sleep mode

///TOA MCU FW version
#define MCU_PROTOCOL_CMD_buff0				0x11	
#define MCU_PROTOCOL_CMD_buff1				0x12
#define MCU_PROTOCOL_CMD_buff2				0x13	
#define MCU_PROTOCOL_CMD_buff3				0x14	
#define MCU_PROTOCOL_CMD_buff4				0x15
#define MCU_PROTOCOL_CMD_buff5				0x16

#define PROTOCOL_STATE_NONE							0
#define PROTOCOL_STATE_Sync								1
#define PROTOCOL_STATE_ADDR							2
#define PROTOCOL_STATE_COM1							3
#define PROTOCOL_STATE_COM2							4
#define PROTOCOL_STATE_DATA1							5
#define PROTOCOL_STATE_DATA2							6

/*
#define MCU_PROTOCOL_CMD_Sync				0xFF
#define MCU_PROTOCOL_CMD_ADDR				0x00
#define MCU_PROTOCOL_CMD_COM1				0x00	
#define MCU_PROTOCOL_CMD_COM2				0x07	
#define MCU_PROTOCOL_CMD_DATA1				0x00
#define MCU_PROTOCOL_CMD_DATA2				0xC9	
*/

#define MCU_PROTOCOL_CMD_Sync				0xFF
#define MCU_PROTOCOL_CMD_ADDR				0x00
#define MCU_PROTOCOL_CMD_COM1			0x00	
#define MCU_PROTOCOL_CMD_COM2			0x07	
#define MCU_PROTOCOL_CMD_DATA1			0x00
#define MCU_PROTOCOL_CMD_DATA2			0xC9	

#define MCU_PROTOCOL_CMD_VERSION				0x0007
#define MCU_PROTOCOL_CMD_STOP					0x0000
#define MCU_PROTOCOL_CMD_UP					0x0008
#define MCU_PROTOCOL_CMD_RIGHT				0x0002
#define MCU_PROTOCOL_CMD_LEFT					0x0004
#define MCU_PROTOCOL_CMD_DOWN				0x0010
#define MCU_PROTOCOL_CMD_UP_RIGHT				0x000A
#define MCU_PROTOCOL_CMD_DOWN_RIGHT			0x0012
#define MCU_PROTOCOL_CMD_UP_LEFT				0x000C
#define MCU_PROTOCOL_CMD_DOWN_LEFT			0x0014
#define MCU_PROTOCOL_CMD_ZOOM_IN				0x0020
#define MCU_PROTOCOL_CMD_ZOOM_OUT			0x0040
#define MCU_PROTOCOL_CMD_FOCUS_NEAR			0x0100
#define MCU_PROTOCOL_CMD_FOCUS_FAR			0x0080
#define MCU_PROTOCOL_CMD_PRESET_SAVE			0x0003
#define MCU_PROTOCOL_CMD_PRESET_LOAD			0x0007
#define MCU_PROTOCOL_CMD_AUTO_SCAN			0x0007
#define MCU_PROTOCOL_CMD_GROUP_TOUR			0x0007
#define MCU_PROTOCOL_CMD_OSD_MENU			0x0007
#define MCU_PROTOCOL_CMD_FOCUS_ADJUST		0x0007

#define MCU_PROTOCOL_DATA						0x0000
#define MCU_PROTOCOL_DATA_VERSION				0x00C9
#define MCU_PROTOCOL_DATA_AUTO_SCAN			0x0042
#define MCU_PROTOCOL_DATA_FOCUS_ADJUST		0x0060

#define MCU_PROTOCOL_CMD_VS4210_W			0x1111
#define MCU_PROTOCOL_CMD_VS4210_R			0x2222

#define PROTOCOL_DATA_LENGTH								15
#define PROTOCOL_STATE_NONE								0
#define PROTOCOL_STATE_TYPE								1
#define PROTOCOL_STATE_LENGTH								2
#define PROTOCOL_STATE_DATA								3
#define PROTOCOL_STATE_CHECKSUM							7


void TOAReceivePaser(void);
extern void RS2_tx(BYTE tx_buf);
extern BYTE RS2_rx(void);

/*
void DvrReceivePaser(void);
extern void MCUTimerDelayXms(WORD usNum);
extern void MCUTimerReactiveTimerEvent(WORD usTime, BYTE ucEventID);
extern void MCUTimerCancelTimerEvent(BYTE ucEventID);
extern void UserInterfaceBatteryChargeMode(EnumBatteryStatus enumSwitch);
*/

#endif

