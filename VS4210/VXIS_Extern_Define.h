/**
  * @brief VX889x configuration file
  *
  * Define the input / output / ISP / MCU and other profiles.
  *
  * @author Cunu-Yu
  * @date 2018/07/06 */

#ifndef _VXIS_EXTERN_DEFINE_H
#define _VXIS_EXTERN_DEFINE_H
#include <string.h>
#include <stdio.h>

//ISP init params
#define _EN_VS4210_Mode1_2_6
//#define _EN_VS4210_Modex26
#define _EN_VS4210_Mode88
#define HDMI_IN_NUMBER 2 //1////4   DavidCT 20201211-01
#define	RGBGAINOSD			        	OFF//ON
#define _IT66021_                                	1
#define VS4210_MUTICHIP_NUMBER 		1

#define DisplayOSDEnable       			OFF
//_MCU_ _MCU8051_ _IT66021_

//#define OSD_10SEC_OFF

#define VICTOR_DEBUG
#ifdef VICTOR_DEBUG
#define dbg_Printf(x, args...) printf("%s,%s,%d:" x, __FILE__, __FUNCTION__, __LINE__, ##args)
#else
#define dbg_Printf(x, args...)
#endif

#define EXTERNOSDNUMBER 15

//MCU def
#define EEPROM_24C02_addr 0xA0

#define  VS4210_INIT_NUMBER 88
//#define  VS4210_INIT_MIN_NUM 26
#define  VS4210_INIT_MIN_NUM 25

//HDMI
#define _SUPPORT_EDID_RAM_		TRUE

//BT1120 mode
#define _External_Sync				OFF

//Source Switch
#define _Source_Switch				ON
#define _Only_AHD_Mode				OFF//ON

//Input Source
#define _HDMI_Input     	0
#define _AHD_Input     		1

#ifndef TRUE
#define TRUE 1
#define _TRUE 1

#define MUTE 				1	
#define MENU				2	
#define TVVOL				4	
#define _JUMP_OSD			8	
#define _GAUGE_OSD			0x10	

#define UP					1
#define DN					2
#define RIGHT				3
#define LEFT  				4

//RX
#define HDMIRXADR 0x90
#define IT6602B0_HDMI_ADDR 0x90	//Hardware Fixed I2C address of IT6602 HDMI
#define IT6602A0_HDMI_ADDR 0x94	//Hardware Fixed I2C address of IT6602 HDMI
#define HDMIRXADR_B 0x92
#define MHL_ADDR 0xE0
#define ITE66121_ADDR 0x98

//Type of MenuItem
#define OSDMENU			1
#define CHOICEMENU		2
#define ACTION			3
#define BAR				4
#define NUMBER			5
#define CHOICE			6
#define ENUM_STRING		7 //YESNO			7
#define CHOICE1			8
#define TIME			9
#define NUMBER_STRING	10
#define TEXT_STR              11   // V1.1 02/28/07 simonsung
#define INFORMATION              12
#define EXIT					13

#define BG_COLOR_BLACK 		0x00	//Transparent //For background color of window, border, and char. 
#define BG_COLOR_Orange		0x40	
#define BG_COLOR_Med_AQUA	0x20
#define BG_COLOR_Silver		0x60 
#define BG_COLOR_Royal_Blue	0x10
#define BG_COLOR_Hot_Pink	0x50
#define BG_COLOR_Light_Green	0x30
#define BG_COLOR_Gray		0x70


#define BG_COLOR_MASK		0xf0	

#define CH_COLOR_BLACK 		0x00	//For color of char.
#define CH_COLOR_RED		0x04	
#define CH_COLOR_GREEN		0x02	
#define CH_COLOR_YELLOW 	0x06 
#define CH_COLOR_BLUE		0x01	
#define CH_COLOR_FUCHSIA	0x05	
#define CH_COLOR_AQUA		0x03	
#define CH_COLOR_WHITE		0x07  
#define CH_COLOR_TRANS		0x08  

#define VS4210_CONTRAST			0x32	
#define VS4210_BRIGHTNESS			0x31	
#define VS4210_SATURATION			0x33	
#define VS4210_SHARPNESS			0x4E
#define VS4210_HUE					0x34	

#endif

#ifndef FALSE
#define FALSE 0
#define _FALSE 0
#endif

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

//#define _EN_EEPROM_Save

//#define VS4210_ADDR 0x40
#define  VS4210_INIT_NUMBER 88
//#define  VS4210_INIT_MIN_NUM 26

//OSD-------------------------------------------------------------------------
#define _TitleOn            0x04    //xxxxx100
#define _ContentOn          0x02    //xxxxx010
#define _BottomOn           0x01    //xxxxx001
#define _TurnoffAll         0x00    //xxxxx000

#define _EN_DRAWBOX_COLOR_BLACK   0x70
#define _EN_DRAWBOX_COLOR_Blue    0x60
#define _EN_DRAWBOX_COLOR_Green   0x30
#define _EN_DRAWBOX_COLOR_Aqua    0x20
#define _EN_DRAWBOX_COLOR_Red     0x50
#define _EN_DRAWBOX_COLOR_Fuchsia 0x40
#define _EN_DRAWBOX_COLOR_Yellow  0x10
#define _EN_DRAWBOX_COLOR_White   0x00


#ifdef __GNUC__
//typedef unsigned char bit;
typedef int bit;
//typedef bool BOOL ;
typedef unsigned char tByte;
typedef unsigned short  tWord;
typedef unsigned long tLong;
typedef unsigned char tiByte;
typedef unsigned short  tiWord;
typedef unsigned long tiLong;
typedef const unsigned char tcByte;
typedef const unsigned short tcWord;
typedef unsigned char txByte;
typedef unsigned short  txWord;
typedef unsigned int  tDWord ;
typedef unsigned long txLong;

typedef	unsigned char	Register;
typedef	unsigned char	BYTE;
//typedef	unsigned short	WORD;
typedef	unsigned int WORD;
typedef	unsigned long	DWORD;
typedef unsigned short USHORT,*PUSHORT ;

#else
//Type Define------------------------------------------------------------------------
typedef unsigned char tByte;
typedef unsigned int  tWord;
typedef unsigned long tLong;
typedef unsigned char idata tiByte;
typedef unsigned int  idata tiWord;
typedef unsigned long idata tiLong;
typedef unsigned char code tcByte;
typedef unsigned int  code tcWord;
typedef unsigned char xdata txByte;
typedef unsigned int xdata txWord;
typedef unsigned long xdata txLong;

typedef	unsigned char	Register;
typedef	unsigned char	BYTE;
//typedef	unsigned short	WORD;
typedef	unsigned int WORD;

typedef	unsigned long	DWORD;

#endif


//--------------------------------------------------
// Enumerations of Timer Events
//--------------------------------------------------
#define SEC(x)                                  (1000 * (x))

typedef struct
{
    BYTE b4PowerAction : 4;
    BYTE b4PowerStatus : 4;
    BYTE b1GDIIsolationFlag : 1;
    BYTE b1TopIsolationFlag : 1;
    BYTE b1AnalogIsolationFlag : 1;
} StructPowerInfoType;

typedef struct
{
    BYTE b1EventValid : 1;
    BYTE b7EventID : 7;
    WORD usTime;
} StructTimerEventTableType;

typedef enum
{
    _USER_TIMER_EVENT_START,
    _USER_TIMER_EVENT_KEY_REPEAT_ENABLE = _USER_TIMER_EVENT_START,   
    _USER_TIMER_EVENT_KEY_SCAN_READY,
    _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_INC,
    _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_DEC,
    _USER_TIMER_EVENT_OSD_GET_MCU_VERSION,
    _USER_TIMER_EVENT_OSD_MCU_ENTRY_SLEEP_MODE,
    _USER_TIMER_EVENT_OSD_DISTANCE_RESET,
    _USER_TIMER_EVENT_OSD_DVR_REBOOT,
    _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN,
    _USER_TIMER_EVENT_OSD_ENCODER_DEVIATION_SETTING,
    //
     _USER_TIMER_EVENT_OSD_SHOW_NO_CABLE,
    _USER_TIMER_EVENT_OSD_SHOW_NO_SIGNAL,
    _USER_TIMER_EVENT_OSD_SHOW_POWER_SAVING,
    _USER_TIMER_EVENT_GO_TO_POWER_SAVING,
    _USER_TIMER_EVENT_OSD_DISABLE,   
    _USER_TIMER_EVENT_DISPLAY_DISABLE,   
	_USER_TIMER_EVENT_Encoder_Loop,	   
    _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON,
    _USER_TIMER_EVENT_PANEL_POWER_OFF,
    _USER_TIMER_EVENT_SEQ_CHECK,
    _USER_TIMER_EVENT_DO_AUTO_COLOR,
    _USER_TIMER_EVENT_PANEL_USED_TIMER,
    _USER_TIMER_EVENT_HLWIN_TYPE5_MOVE,    
	_USER_TIMER_EVENT_LED_POWER_SAVING,
	_USER_TIMER_EVENT_LIGHT_SENSER_ON,
	_USER_TIMER_EVENT_SHOW_BURN_IN,//ryan@20151230
	_USER_TIMER_EVENT_BURN_IN_ON,//ryan@20151230
	_USER_TIMER_EVENT_CHECK,//ryan@20151231
	_USER_TIMER_EVENT_DISPLAY_INFO,//ryan@20151231
	_USER_TIMER_EVENT_STANDBY_MODE,
	_USER_TIMER_EVENT_FACTORY_MODE,//ryan@20160113
	_USER_TIMER_EVENT_Monitor_ON,
    _USER_TIMER_EVENT_END,
    _USER_TIMER_EVENT_COUNT = _USER_TIMER_EVENT_END,
} EnumUserTimerEventID;

typedef enum
{
    _SYSTEM_TIMER_EVENT_START = _USER_TIMER_EVENT_COUNT,
    _SYSTEM_TIMER_EVENT_SOURCE_CLEAR_DETECTING = _SYSTEM_TIMER_EVENT_START,
    
    _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_POWER_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE,    
    _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE,
    _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE,
    _SYSTEM_TIMER_EVENT_JUDGE_DVR_COMM,
    _SYSTEM_TIMER_EVENT_JUDGE_TW8836_COMM,
    _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO,
     _SYSTEM_TIMER_EVENT_GRN_ON,    
    _SYSTEM_TIMER_EVENT_GRN_OFF,    
    _SYSTEM_TIMER_EVENT_RED_ON,      
    _SYSTEM_TIMER_EVENT_RED_OFF,	 
    _SYSTEM_TIMER_EVENT_GRN_BLINK,    
    _SYSTEM_TIMER_EVENT_RED_BLINK,	 
    _SYSTEM_TIMER_EVENT_GRN_RED_BLINK,	  
    _SYSTEM_TIMER_EVENT_GRN_RED_ON,    
     _SYSTEM_TIMER_EVENT_GRN_RED_OFF,    
    _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK,    
    _SYSTEM_TIMER_EVENT_LIGHT_SENSOR_CHECK,
    _SYSTEM_TIMER_EVENT_EQ_DETECT,
    _SYSTEM_TIMER_EVENT_KEY_CHECK,
    _SYSTEM_TIMER_EVENT_ADC_CHECK,
     _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE,   
	_SYSTEM_TIMER_EVENT_BOOT_SETUP,	 
	_SYSTEM_TIMER_EVENT_BOOT_VIDEO_DETECT,
	_SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT,
		_SYSTEM_TIMER_EVENT_UART2_CHECK,
    /*
    _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE,    
    _SYSTEM_TIMER_EVENT_BATT_HIGH_CHARGE,    
    _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE,	 
   */
_SYSTEM_TIMER_EVENT_TP_PRES_DOWN, 
    _SYSTEM_TIMER_EVENT_RESET_TOUCH_PANEL,
    _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER,   
    _SYSTEM_TIMER_EVENT_VIDEO_RESET,
	_SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS,   
	_SYSTEM_TIMER_EVENT_SET_EMBEDDED_SYNC,   
        _SYSTEM_TIMER_EVENT_END,
    _SYSTEM_TIMER_EVENT_COUNT,
} EnumSystemTimerEventID;


typedef enum
{
    _SCALER_TIMER_EVENT_START = _SYSTEM_TIMER_EVENT_COUNT,
    _SCALER_TIMER_EVENT_TMDS_HDMI_PACKET_DETECT = _SCALER_TIMER_EVENT_START, 
    _SCALER_TIMER_EVENT_TMDS_VIDEO_DETECT,  
    _SCALER_TIMER_EVENT_AUDIO_LOAD_STABLE_I_CODE,
    _SCALER_TIMER_EVENT_DP_HOTPLUG_ASSERTED,
    _SCALER_TIMER_EVENT_DP_HDCP_LONG_HOTPLUG_EVENT,
    _SCALER_TIMER_EVENT_DP_LINK_STATUS_IRQ,    
    _SCALER_TIMER_EVENT_D0_PORT_DIGITAL_SWITCH,
    _SCALER_TIMER_EVENT_D1_PORT_DIGITAL_SWITCH,  
    _SCALER_TIMER_EVENT_NR_DETECTION_FINISHED,  
    _SCALER_TIMER_EVENT_PHASE_CALIBRATION,    
    _SCALER_TIMER_EVENT_PHASE_CNT_CHECK,
    _SCALER_TIMER_EVENT_MHL_D0_READY_TO_TRANSMIT,
    _SCALER_TIMER_EVENT_MHL_D0_READY_TO_RESEND,
    _SCALER_TIMER_EVENT_MHL_D0_RECONNECT_1K,
    _SCALER_TIMER_EVENT_MHL_D0_ACTIVE_VBUS,
    _SCALER_TIMER_EVENT_MHL_D1_READY_TO_TRANSMIT,
    _SCALER_TIMER_EVENT_MHL_D1_READY_TO_RESEND,
    _SCALER_TIMER_EVENT_MHL_D1_RECONNECT_1K,
    _SCALER_TIMER_EVENT_MHL_D1_ACTIVE_VBUS,
    _SCALER_TIMER_EVENT_DCC_HISTOGRAM_THD_CHANGE,
    _SCALER_TIMER_EVENT_END,
    _SCALER_TIMER_EVENT_COUNT = _SCALER_TIMER_EVENT_END,
} EnumMCUTimerEventID;

//#define _SIMULTANEOUS_SCALER_EVENT_COUNT       0//     (5 + (_D0_INPUT_PORT_TYPE == _D0_MHL_PORT) + (_D1_INPUT_PORT_TYPE == _D1_MHL_PORT))
#define _SIMULTANEOUS_USER_EVENT_COUNT             5
#define _SIMULTANEOUS_SYSTEM_EVENT_COUNT            5

//#define _TIMER_EVENT_COUNT    (_SIMULTANEOUS_USER_EVENT_COUNT + _SIMULTANEOUS_SYSTEM_EVENT_COUNT + _SIMULTANEOUS_SCALER_EVENT_COUNT)
#define _TIMER_EVENT_COUNT    (_SIMULTANEOUS_USER_EVENT_COUNT + _SIMULTANEOUS_SYSTEM_EVENT_COUNT )


typedef enum
{

#if 1
VS4210_1080P_MODE0_w0 = 0x00,
 VS4210_1080P_MODE0_w1 = 0x01,
 VS4210_1080P_MODE0_w2 = 0x02,
 VS4210_1080P_MODE0_w3 = 0x03,
 VS4210_1080P_MODE1    = 0x10,
 VS4210_1080P_MODE2    = 0x20,
 VS4210_1080P_MODE6    = 0x30,
 VS4210_1080P_MODE12   = 0x40,

 VS4210_1080P_MODE13   = 0x50,
 VS4210_1080P_MODE24   = 0x60,

 VS4210_1080P_MODE_NUMBER=0x90,
 VS4210_1080P_MODE_SystemPowerDown=0xF0,
 VS4210_1080P_MODE_SystemPowerResume=0xF1
/*
VS4210_1080P_MODE0_w0 = 0x00,
VS4210_1080P_MODE0_w1 = 0x01,
VS4210_1080P_MODE0_w2 = 0x02,
VS4210_1080P_MODE0_w3 = 0x03,
VS4210_1080P_MODE1	  = 0x10,
VS4210_1080P_MODE2	  = 0x20,
VS4210_1080P_MODE8	  = 0x30,
VS4210_1080P_MODE9	  = 0x40,
VS4210_1080P_MODE10   = 0x50,
VS4210_1080P_MODE12   = 0x60,
VS4210_1080P_MODE14   = 0x70,
VS4210_1080P_MODE_NUMBER=0x90,
VS4210_1080P_MODE_SystemPowerDown=0xF0,
VS4210_1080P_MODE_SystemPowerResume=0xF1
*/
#else
    VS4210_1080P_MODE0_w0 = 0x00,
    VS4210_1080P_MODE0_w1 = 0x01,
    VS4210_1080P_MODE0_w2 = 0x02,
    VS4210_1080P_MODE0_w3 = 0x03,
    VS4210_1080P_MODE1    = 0x10,
//     VS4210_1080P_MODE2    = 0x20,
    VS4210_1080P_MODE10    = 0x25,
      VS4210_1080P_MODE8    = 0x30,
    VS4210_1080P_MODE9    = 0x40,
    VS4210_1080P_MODE12   = 0x50,
    VS4210_1080P_MODE14   = 0x60,
    VS4210_1080P_MODE_NUMBER=0x90,
    VS4210_1080P_MODE_SystemPowerDown=0xF0,
    VS4210_1080P_MODE_SystemPowerResume=0xF1
#endif
} HDMI_OUTPUT_MODE ;

typedef enum
{
    VS4210_LOOP_MODE0_w0 = 0,   
    VS4210_LOOP_MODE0_w1 ,      ///1
    VS4210_LOOP_MODE0_w2 ,   ///2
    VS4210_LOOP_MODE0_w3 ,///3
    VS4210_LOOP_MODE1 ,///4
    VS4210_LOOP_MODE2 ,///5
    VS4210_LOOP_MODE10_123 ,///6
    VS4210_LOOP_MODE8 ,///7
     VS4210_LOOP_MODE9 ,///8
    VS4210_LOOP_MODE12_12,///9
    VS4210_LOOP_MODE12_34,///10
    VS4210_LOOP_MODE12_14,///11
    VS4210_LOOP_MODE12_42,///12
    VS4210_LOOP_MODE12_43,///12
    VS4210_LOOP_MODE12_21,///12
    VS4210_LOOP_MODE12_23,///12
    VS4210_LOOP_MODE12_24,///12
      VS4210_LOOP_MODE12_13,///13
	VS4210_LOOP_MODE12_31,///12
		VS4210_LOOP_MODE12_32,///12		
   VS4210_LOOP_MODE8_412 ,///16
   VS4210_LOOP_MODE8_123 ,///16
   VS4210_LOOP_MODE8_124 ,///16
    VS4210_LOOP_MODE8_134 ,///16
    VS4210_LOOP_MODE8_213 ,///16
 VS4210_LOOP_MODE8_214 ,///16
  VS4210_LOOP_MODE8_234 ,///16
  	VS4210_LOOP_MODE8_312 ,///16
	VS4210_LOOP_MODE8_314 ,///16
	VS4210_LOOP_MODE8_324 ,///16
		VS4210_LOOP_MODE10_412 ,///6
		VS4210_LOOP_MODE10_413 ,///6
		VS4210_LOOP_MODE10_423 ,///6
		VS4210_LOOP_MODE8_413 ,///16
		VS4210_LOOP_MODE8_423 ,///16
	
         VS4210_LOOP_MODE14,///17
    VS4210_LOOP_MODE_NUMBER  //18
}VS4210_OUTPUT_MODE_LOOP ;

typedef enum
{

#if 1
VS4210_FULL_MODE0 =0x00,
VS4210_FULL_MODE1 =0x10,
VS4210_FULL_MODE2 =0x20,
VS4210_FULL_MODE8 =0x30,
VS4210_FULL_MODE9 =0x40,
VS4210_FULL_MODE10=0x50,
VS4210_FULL_MODE12=0x60,
VS4210_FULL_MODE14=0x70,
VS4210_FULL_MODE_NUMBER=0x90,
VS4210_SystemPowerDown=0xF0,
VS4210_SystemPowerResume=0xF1

#else
    VS4210_FULL_MODE0 =0x00,
    VS4210_FULL_MODE1 =0x10,
 //   VS4210_FULL_MODE2 =0x20,
  VS4210_FULL_MODE10 =0x20,   
    VS4210_FULL_MODE8 =0x30,
    VS4210_FULL_MODE9 =0x40,
    VS4210_FULL_MODE12=0x50,
    VS4210_FULL_MODE14 =0x60,
    VS4210_FULL_MODE_NUMBER=0x90,
    VS4210_SystemPowerDown=0xF0,
    VS4210_SystemPowerResume=0xF1
  #endif
} HDMI_FULL_OUTPUT_MODE ;

#if 1
typedef enum
{
    HDMI_AUDIO_1= 0 ,
    HDMI_AUDIO_2,
    HDMI_AUDIO_3,
    HDMI_AUDIO_4,
    HDMI_AUDIO_NONE,
    HDMI_AUDIO_NUMBER
} HDMI_AUDIO_SELECT ;
#else

typedef enum
{
    HDMI_AUDIO_0 = 0,
    HDMI_AUDIO_1,
    HDMI_AUDIO_2,
    HDMI_AUDIO_3,
    HDMI_AUDIO_NONE,
    HDMI_AUDIO_NUMBER
} HDMI_AUDIO_SELECT ;
#endif

#if 1

typedef enum
{
    _OSD_720X480I60=0,  //HDMI
    _OSD_720X576I50,
    _OSD_720X480P60,
    _OSD_720X576P50,
    _OSD_1920X1080I,
    _OSD_1280X720P,  //5
    _OSD_1920X1080P,
    _OSD_640X480P,  //DVI
    _OSD_800X600P,
    _OSD_1024X768P,
    _OSD_1280X768P, //10
    _OSD_1360X768P,
    _OSD_1366X768P,
    _OSD_1280X800P,
    _OSD_1440X900P,
    _OSD_1280X1024P, //15
    _OSD_1400X1050P,
    _OSD_1680X1050P,
    _OSD_1600X1200P,
    _OSD_1920X1200P,
    _OSD_No_Signal,  //20
    _OSD_OUT_OF_RANGE
} HDMI_INPUT_SIZE ;

#else
typedef enum
{
    _OSD_720X480I60=0,  //HDMI
    _OSD_720X576I50,
    _OSD_720X480P60,
    _OSD_720X576P50,
    _OSD_1920X1080I,
    _OSD_1280X720P,  //5
    _OSD_1920X1080P,    
    _OSD_No_Signal,
    _OSD_OUT_OF_RANGE
} HDMI_INPUT_SIZE ;
#endif

#if 0
typedef enum
{
    _OUTPUT_1080P60=0 ,
    _OUTPUT_720P60,
    _OUTPUT_800x480,
    _OUTPUT_NUMBER
} OUTPUT_SIZE;
#endif

typedef enum
{
    _OUTPUT_720P60=0,
    _OUTPUT_1080P60,
    _OUTPUT_NUMBER
} OUTPUT_SIZE;


typedef struct
{
    tByte WMap0 ;
    tByte WMap1 ;
    tByte WMap2 ;
    tByte WMap3 ;
} tWindowMap ;

typedef struct
{
    tByte OutputMode ;
    tWindowMap WindowMap ;
    tByte AudioChannel ;
    tByte OutputSize ;
    tByte DrawOSDFlg ;
    tByte DrawBoxFlg ;
    tByte DrawBoxColor ;
} T_VS4210_MutiWindows ;


#endif

//========= Start ===========
//Modifed FW records by DavidCT
//1. Enabled 2 ports.                        20201211-01
//2. Select vs4120 CH. 1 port.               20201211-02
//#define VS4210_SELECT_WIN1
//3. TP2824 selected 16-bit output.         20201211-05
//4. Fixed debugging command "dump".        20201211-03
//5. Fixed debugging can lock with the command "access". 20201211-04
//6. Added TP2824 program code.             20201214-01
//7. Created HS_Select_Win() function to select window 0 or window 1. 20201214-02
//========= End ===========
