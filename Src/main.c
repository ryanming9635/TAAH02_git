/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#ifdef __GNUC__
    #include <sys/unistd.h>
#endif

#include "VXIS_Extern_Define.h"
#include "sw_i2c0.h"  //TP2824
#include "sw_i2c1.h"  //VS4210
#include "sw_i2c2.h"  //IT66121
#include "VX_swi2c.h"
#include "ite66121.h"
#include "HDMI_RX_lib.h"
#include "VS4210.h"
#include "IRdef.h"
#include "TP2802.h"
#include "VX_OSD_Simple.h"
#include "monitor.h"
#include "Printf.h"
#include "Etc_eep.h"
#include 	"audio.h"
#include "keyremo.h"
#include "VX8812.h"
#include "HDMI_RX_lib.h"

#include "TOA_Protocol.h"

//#include "VS4210_param_Mode88.h"
//#include "VS4210_param_Modex26.h"

#include "GT911.h"
//#include "exOSDfont.h"

//#include "it660x.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RXBUFMAX 50
#define IR_Vendor_Code_ 0xFF00

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile tByte VS4210_ADDR = 0x40 ;
volatile tByte VS8812_ADDR = 0x12 ;
#define OSD_OFF_SEC 10

//RX
tByte	ManVidRes;
tByte	half_scaler = 0;
tByte	DebugLevel;
tByte	DayNightLevel=0;
tWord LightSensor_ADC=0;
BYTE AutoDayNightDelay=0;
DATA BYTE PWMLevel=0x80;
BYTE TimeStep;
BYTE	PowerFlag=1;
DATA BYTE keytic=0,tptic=0;
BYTE	SelectModeType;
BYTE	Priority;
BYTE CAMA_M;	
BYTE CAMB_M;	
BYTE CAMC_M;	
BYTE CAMR_M;
BYTE OSD_MIRROR[4]={0,0,0,0};
BYTE OSD_SPEAKER[4]={0,0,0,0};
BYTE BOOT_ISSUE=1;
BYTE CURRENT_CH=0;
tByte ExOsdContent[256] ;
tByte PrebVXIS_InputMode[4]= {250,250,250,250} ;

int OSDPreSecCount =-1 ;
BYTE SelectInputMode=0;///0=HDMI , 1=AHD
BYTE CheckSelectInputMode=0;///0=HDMI , 1=AHD

//audio
tByte gPreAudioSampleFreq ;
tByte gPrebAUDO_INDEX  ;
tByte gbAUDO_INDEX = 0  ;
static tByte gAudioSampleFreq[4] ;

tWord gMode0LoopCount = 0 ;
tByte gMode0LoopFlg = 0 ;
tByte gMode0LoopSecCount = 0 ;
tByte gMode0LoopSecCmpVal = 10 ;

//Rx
static tByte bOldSignal[4] ;
tByte bSignal[4]= {0,0,0,0} ;
extern tByte PreVState[4] ;
extern tByte VState[4] ;
extern tByte AState[4] ;

//Tx
tByte TxCount = 0 ;

T_VS4210_MutiWindows pVS4210_MutiWindows1 ;
unsigned int volatile Tim1_count=0;
bit OSD_Show_Flg = 0 ;

static int TimeOutCount ;

extern DATA BYTE keytic;
extern 	DATA BYTE Key;
extern 	bit KeyReady, RepeatKey;

extern BYTE  updn_reg[4];
extern BYTE Time5ms;
extern bit ChangeKey;
extern tByte RS_buf[BUF_MAX];
extern  tByte RS2_buf[TOA_BUF_MAX];
extern  tByte	 RS_in, RS2_in;
extern BYTE tic02;
extern WORD tic03,tic04,tic05;
extern volatile WORD  g_usTimerCounter;
extern volatile bit g_bNotifyTimer0Int;
extern BYTE	AudioVol;
extern BYTE NowModeState;
extern BYTE Disp_Input;
extern BYTE OSD_LOCKF;
extern BYTE TriggerFlag;

extern BYTE	MainSetSelectMode[13];
/*extern*/ tByte gabVXIS_Input_mirrorH[4];
extern tByte gWindowNum ;

extern BYTE TP_ACTION;
extern BYTE TP_PRES_DOWN;
extern BYTE TouchP;
extern USHORT ucTickCount;
extern bit   AccessEnable;
extern tByte TP_InputMode;
extern tByte mode[0];

extern tcByte VS4210_TP_bt1120_1080P_1080P_mode0_w0_88param[VS4210_INIT_NUMBER] ;
extern tcByte VS4210_TP_bt1120_720P_1080P_mode0_w0_88param[VS4210_INIT_NUMBER] ;
extern tcByte VS4210_TP_bt1120_NTSC_1080P_mode0_w0_88param[VS4210_INIT_NUMBER] ;
extern tcByte VS4210_TP_bt1120_PAL_1080P_mode0_w0_88param[VS4210_INIT_NUMBER] ;

//Extern OSD 20200110-01
extern const unsigned char UserDefineFont1[40*(4+23+1)] ; //extern OSD

extern tcByte VS4210_hdmi1080P_1080P_mode0_w0_88param[VS4210_INIT_NUMBER];
extern tcByte VS4210_Addr_88param[VS4210_INIT_NUMBER];

tByte volatile gRxLoopFlg = 1 ;
T_VS4210_MutiWindows pVS4210_JointKind1 ;

static volatile tByte  u8PowerFlg  = 0 ;
/*static volatile */tByte  OSDSecCount = 0 ;
//static volatile 
	tByte  OSDStateFlg = 1 ;
static volatile tByte  IR_ShiftCount = 0 ;
static volatile tDWord IR_4BYTE = 0 ;
static volatile tWord  u16Vendor ;
static volatile tByte  u8IRcode ;
static volatile tByte  u8IRCheckcode ;
static volatile tByte  PreIR_Code = 0 ;
static volatile tByte  IR_Code = 0 ;
static volatile tByte  IR_StartFlg = 0 ;
static volatile tByte  IR_END_Flg = 0 ;
static volatile tByte  IR_IRQFlg = 0 ;
static volatile tByte  PreIR_IRQ_Index = 0 ;
static volatile tByte  IR_IRQ_Index = 0 ;
static volatile tWord  IR_PlusTimeVal = 0 ;
static volatile tDWord tim3count = 0 ; //10us

static volatile tDWord TxiCount = 0 ;
static volatile tDWord RxiCount = 0 ;

static char Rx_data[2],Rx2_data[2]/*, Rx_Buffer[RXBUFMAX]*/, Transfer_cplt;
static volatile int Rx_indx = 0 ;
static volatile int RxBufIndex= 0 ;
static char gRxbuf[RXBUFMAX] ;

static volatile int DebugFlg = 0 ;

//static int OutMapLoopModeIndex = VS4210_LOOP_MODE_NUMBER ;
//static tByte OutMapLoopOutSizeIndex ;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void MX_ADC1_Init(void);
//static void MX_TIM2_Init(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

extern void MCUTimerInitialTimerEvent(void);
extern void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID);
extern BYTE RS_ready(void);
extern BYTE RS2_ready(void);

extern void SysTimerHandler(void);


/* USER CODE BEGIN PFP */
#ifdef __GNUC__

// With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf

//   set to 'Yes') calls __io_putchar()

int _write(int file, char *data, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }
    // arbitrary timeout 1000
    HAL_StatusTypeDef status =
        HAL_UART_Transmit(&huart1, (uint8_t*)data, (uint16_t)len, 1000);
    // return # of bytes written - as best we can tell
    return (status == HAL_OK ? len : 0);
}
#else
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
    return ch;
}
#endif

void Audio_Sel(unsigned char mode )
{
    gbAUDO_INDEX = mode ;
    switch(mode)
    {
    case HDMI_AUDIO_NONE:
//        A_AUDIO = 1 ;
//        B_AUDIO = 1 ;
//        H1_AUDIO= 1 ;
//        H2_AUDIO= 1 ;
//        H3_AUDIO= 1 ;
//        H4_AUDIO= 1 ;
  //mask by ryan@20210126             HAL_GPIO_WritePin(A_AUDIO_GPIO_Port, A_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(B_AUDIO_GPIO_Port, B_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H1_AUDIO_GPIO_Port, H1_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H2_AUDIO_GPIO_Port, H2_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H3_AUDIO_GPIO_Port, H3_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H4_AUDIO_GPIO_Port, H4_AUDIO_Pin, GPIO_PIN_SET);
        break ;

    case HDMI_AUDIO_1:
//        A_AUDIO = 0 ;
//        B_AUDIO = 1 ;
//        H1_AUDIO= 0 ;
//        H2_AUDIO= 1 ;
//        H3_AUDIO= 1 ;
//        H4_AUDIO= 1 ;

  //mask by ryan@20210126      HAL_GPIO_WritePin(A_AUDIO_GPIO_Port, A_AUDIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(B_AUDIO_GPIO_Port, B_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H1_AUDIO_GPIO_Port, H1_AUDIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(H2_AUDIO_GPIO_Port, H2_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H3_AUDIO_GPIO_Port, H3_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H4_AUDIO_GPIO_Port, H4_AUDIO_Pin, GPIO_PIN_SET);

        HDMI_Tx_SetAudioFs(gAudioSampleFreq[0])  ;
        gPreAudioSampleFreq = gAudioSampleFreq[0] ;
        break ;

    case HDMI_AUDIO_2:
//        A_AUDIO = 0 ;
//        B_AUDIO = 1 ;
//        H1_AUDIO= 1 ;
//        H2_AUDIO= 0 ;
//        H3_AUDIO= 1 ;
//        H4_AUDIO= 1 ;

  //mask by ryan@20210126             HAL_GPIO_WritePin(A_AUDIO_GPIO_Port, A_AUDIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(B_AUDIO_GPIO_Port, B_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H1_AUDIO_GPIO_Port, H1_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H2_AUDIO_GPIO_Port, H2_AUDIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(H3_AUDIO_GPIO_Port, H3_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H4_AUDIO_GPIO_Port, H4_AUDIO_Pin, GPIO_PIN_SET);

        HDMI_Tx_SetAudioFs(gAudioSampleFreq[1])  ;
        gPreAudioSampleFreq = gAudioSampleFreq[1] ;
        break ;

    case HDMI_AUDIO_3:
//        A_AUDIO = 1 ;
//        B_AUDIO = 0 ;
//        H1_AUDIO= 1 ;
//        H2_AUDIO= 1 ;
//        H3_AUDIO= 0 ;
//        H4_AUDIO= 1 ;
   //mask by ryan@20210126            HAL_GPIO_WritePin(A_AUDIO_GPIO_Port, A_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(B_AUDIO_GPIO_Port, B_AUDIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(H1_AUDIO_GPIO_Port, H1_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H2_AUDIO_GPIO_Port, H2_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H3_AUDIO_GPIO_Port, H3_AUDIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(H4_AUDIO_GPIO_Port, H4_AUDIO_Pin, GPIO_PIN_SET);

        HDMI_Tx_SetAudioFs(gAudioSampleFreq[2])  ;
        gPreAudioSampleFreq = gAudioSampleFreq[2] ;
        break ;

    case HDMI_AUDIO_4:
//        A_AUDIO = 1 ;
//        B_AUDIO = 0 ;
//        H1_AUDIO= 1 ;
//        H2_AUDIO= 1 ;
//        H3_AUDIO= 1 ;
//        H4_AUDIO= 0 ;
  //mask by ryan@20210126             HAL_GPIO_WritePin(A_AUDIO_GPIO_Port, A_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(B_AUDIO_GPIO_Port, B_AUDIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(H1_AUDIO_GPIO_Port, H1_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H2_AUDIO_GPIO_Port, H2_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H3_AUDIO_GPIO_Port, H3_AUDIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(H4_AUDIO_GPIO_Port, H4_AUDIO_Pin, GPIO_PIN_RESET);

        HDMI_Tx_SetAudioFs(gAudioSampleFreq[3])  ;
        gPreAudioSampleFreq = gAudioSampleFreq[3] ;
        break ;
    }
}

void SET_Audio_Mute(unsigned char mode )
{

if(mode==ON)
	HAL_GPIO_WritePin(AUD_I2C_MUTE_Port,AUD_I2C_MUTE_Pin, GPIO_PIN_RESET);
else
	HAL_GPIO_WritePin(AUD_I2C_MUTE_Port,AUD_I2C_MUTE_Pin, GPIO_PIN_SET);

Printf("(*SET_Audio_Mute=%d)",(WORD)mode);

}

//Sleep
/*
void msleep(unsigned int i )
{
    HAL_Delay(i);
}
*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void InitialTimerEvent(void)
{
	MCUTimerInitialTimerEvent();  //clear all of timer event	

		#if (_DEBUG_MESSAGE_SysTimerEvent==ON)
  		MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER);//for timer
  		   //	MCUTimerActiveTimerEvent(SEC(6), _SYSTEM_TIMER_EVENT_BOOT_VIDEO_DETECT);//for timer
  		//MCUTimerActiveTimerEvent(SEC(5), _SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS);//for timer

//	MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_UART2_CHECK);//for test
		
     		#endif
//MCUTimerActiveTimerEvent(SEC(5), _SYSTEM_TIMER_EVENT_VIDEO_RESET);			
	/*		
     	MCUTimerActiveTimerEvent(SEC(5-2), _SYSTEM_TIMER_EVENT_BOOT_VIDEO_DETECT);//for timer
	MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_LIGHT_SENSOR_CHECK);//for timer
	MCUTimerActiveTimerEvent(100, _SYSTEM_TIMER_EVENT_GRN_ON);
	MCUTimerActiveTimerEvent(SEC(1),  _SYSTEM_TIMER_EVENT_ADC_CHECK);
	*/
}

void GetRS232_CMD(void)
{
    char achstr[3] ;
    char *endptr ;
    long IrNum  ;
    if (Transfer_cplt == 1)
    {
        Transfer_cplt = 0 ;
        printf("RxBufIndex=%d,str=%s\r\n",RxBufIndex, gRxbuf);
        if ( RxBufIndex >= 4 )
        {
            if ((gRxbuf[0] =='G') && (gRxbuf[3]=='g'))
            {
                achstr[2] = '\0' ;
                memcpy(&achstr[0], &gRxbuf[1], 2 ) ;
                IrNum = strtol(achstr, &endptr, 16) ;
                printf("IrNum=0x%02x\r\n",(int)IrNum);
            }
            else if ((gRxbuf[0] =='S') && (gRxbuf[5]=='s'))
            {                
            }
        }
    }
}

//static
	void setloopoutmode(int index)
{
/*
	pVS4210_JointKind1.AudioChannel = HDMI_AUDIO_NONE ;
	  pVS4210_JointKind1.DrawBoxFlg =1 ;
	  pVS4210_JointKind1.DrawOSDFlg =0x03 ; //bits 0 ,1
//	pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE1 ;
	  pVS4210_JointKind1.OutputSize = _OUTPUT_800x480;//_OUTPUT_1080P60 ;
*/
    switch(index)
    {
    case VS4210_LOOP_MODE0_w0:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w0 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
	 OSD_MIRROR[0]=CAMA_M;
	 gabVXIS_Input_mirrorH[0]=CAMA_M;
        printf("VS4210_MODE0_w0\r\n") ;
	  OSD_SPEAKER[0]=ON;
        break ;
    }
    case VS4210_LOOP_MODE0_w1:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w1 ;
        pVS4210_JointKind1.WindowMap.WMap0 =2 ;
   	 OSD_MIRROR[0]=CAMB_M;
	 gabVXIS_Input_mirrorH[1]=CAMB_M; 
	 OSD_SPEAKER[0]=ON;
        printf("VS4210_MODE0_w1\r\n") ;
        break ;
    }
    case VS4210_LOOP_MODE0_w2:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w2 ;
        pVS4210_JointKind1.WindowMap.WMap0 =3 ;
	 OSD_MIRROR[0]=CAMC_M;	
	gabVXIS_Input_mirrorH[2]=CAMC_M; 	 
	 OSD_SPEAKER[0]=ON;
        printf("VS4210_MODE0_w2\r\n") ;
        break ;
    }
    case VS4210_LOOP_MODE0_w3:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w3 ;
        pVS4210_JointKind1.WindowMap.WMap0 =4 ;
   	OSD_MIRROR[0]=CAMR_M;	
	gabVXIS_Input_mirrorH[3]=CAMR_M; 		
  	OSD_SPEAKER[0]=ON;
        printf("VS4210_MODE0_w3\r\n") ;
        break ;
    }
    case VS4210_LOOP_MODE1:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE1 ;
	
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
        pVS4210_JointKind1.WindowMap.WMap2 =3 ;
        pVS4210_JointKind1.WindowMap.WMap3 =4 ;
   	  OSD_MIRROR[0]=CAMA_M;	
   	  OSD_MIRROR[1]=CAMB_M;	
   	  OSD_MIRROR[2]=CAMC_M;	
   	  OSD_MIRROR[3]=CAMR_M;	
	gabVXIS_Input_mirrorH[0]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[3]=CAMR_M; 		  
	  OSD_SPEAKER[0]=OFF;
	  OSD_SPEAKER[1]=OFF;
	  OSD_SPEAKER[2]=OFF;
	  OSD_SPEAKER[3]=ON;

        printf("VS4210_MODE1\r\n") ;
        break ;
    }
	
    case VS4210_LOOP_MODE2:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE2 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
        pVS4210_JointKind1.WindowMap.WMap2 =3 ;
        pVS4210_JointKind1.WindowMap.WMap3 =4 ;
        printf("VS4210_MODE2\r\n") ;
        break ;
    }
	#if 0
    case VS4210_LOOP_MODE8:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
        pVS4210_JointKind1.WindowMap.WMap2 =3 ;
        pVS4210_JointKind1.WindowMap.WMap3 =4 ;
	  OSD_MIRROR[0]=CAMA_M;	
   	  OSD_MIRROR[1]=CAMB_M;	
   	  OSD_MIRROR[2]=CAMC_M;	
   	  OSD_MIRROR[3]=CAMR_M;	
	gabVXIS_Input_mirrorH[0]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[3]=CAMR_M;	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;
	  OSD_SPEAKER[2]=OFF;
        printf("VS4210_MODE8 ch1 ch2 ch3\r\n") ;
        break ;
    }
	    case VS4210_LOOP_MODE8_412:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
        pVS4210_JointKind1.WindowMap.WMap0 =4 ;
        pVS4210_JointKind1.WindowMap.WMap1 =1 ;
        pVS4210_JointKind1.WindowMap.WMap2 =2 ;
        pVS4210_JointKind1.WindowMap.WMap3 =0 ;
	  OSD_MIRROR[0]=CAMR_M;	
   	  OSD_MIRROR[1]=CAMA_M;	
   	  OSD_MIRROR[2]=CAMB_M;	
	gabVXIS_Input_mirrorH[0]=CAMR_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMB_M; 		  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;
	  OSD_SPEAKER[2]=OFF;
	  
      //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
        printf("VS4210_MODE8 ch4 ch1 ch2\r\n") ;
        break ;
    }
     case VS4210_LOOP_MODE8_123:
	 {
			 pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
			 pVS4210_JointKind1.WindowMap.WMap0 =1 ;
			 pVS4210_JointKind1.WindowMap.WMap1 =2 ;
			 pVS4210_JointKind1.WindowMap.WMap2 =3 ;
		   OSD_MIRROR[0]=CAMA_M; 
		   OSD_MIRROR[1]=CAMB_M; 
		   OSD_MIRROR[2]=CAMC_M; 
	gabVXIS_Input_mirrorH[0]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 		   
		   OSD_SPEAKER[0]=ON;
		   OSD_SPEAKER[1]=OFF;
		   OSD_SPEAKER[2]=OFF;
		   
		   //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
			 printf("VS4210_MODE8 ch1 ch2 ch3\r\n") ;
			 break ;
		 }
  case VS4210_LOOP_MODE8_124:
	 {
			 pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
			 pVS4210_JointKind1.WindowMap.WMap0 =1 ;
			 pVS4210_JointKind1.WindowMap.WMap1 =2 ;
			 pVS4210_JointKind1.WindowMap.WMap2 =4 ;
		   OSD_MIRROR[0]=CAMA_M; 
		   OSD_MIRROR[1]=CAMB_M; 
		   OSD_MIRROR[2]=CAMR_M; 
	gabVXIS_Input_mirrorH[0]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMR_M; 		   
		   OSD_SPEAKER[0]=OFF;
		   OSD_SPEAKER[1]=OFF;
		   OSD_SPEAKER[2]=ON;
		   
		   //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
			 printf("VS4210_MODE8 ch1 ch2 ch4\r\n") ;
			 break ;
		 }	 
  case VS4210_LOOP_MODE8_134:
	 {
			 pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
			 pVS4210_JointKind1.WindowMap.WMap0 =1 ;
			 pVS4210_JointKind1.WindowMap.WMap1 =3 ;
			 pVS4210_JointKind1.WindowMap.WMap2 =4 ;
		   OSD_MIRROR[0]=CAMA_M; 
		   OSD_MIRROR[1]=CAMC_M; 
		   OSD_MIRROR[2]=CAMR_M; 
	gabVXIS_Input_mirrorH[0]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMR_M; 		   
		   OSD_SPEAKER[0]=OFF;
		   OSD_SPEAKER[1]=OFF;
		   OSD_SPEAKER[2]=ON;
		   //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
			 printf("VS4210_MODE8 ch1 ch3 ch4\r\n") ;
			 break ;
		 }	 
//=============
case VS4210_LOOP_MODE8_213:
   {
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =2 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =1 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =3 ;
		 OSD_MIRROR[0]=CAMB_M; 
		 OSD_MIRROR[1]=CAMA_M; 
		 OSD_MIRROR[2]=CAMC_M; 
	gabVXIS_Input_mirrorH[0]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 			 
		 OSD_SPEAKER[0]=ON;
		 OSD_SPEAKER[1]=OFF;
		 OSD_SPEAKER[2]=OFF;
		 
		 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE8 ch2 ch1 ch3\r\n") ;
		   break ;
	   }   
case VS4210_LOOP_MODE8_214:
   {
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =2 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =1 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =4 ;
		 OSD_MIRROR[0]=CAMB_M; 
		 OSD_MIRROR[1]=CAMA_M; 
		 OSD_MIRROR[2]=CAMR_M; 
	gabVXIS_Input_mirrorH[0]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMR_M; 			 
		 OSD_SPEAKER[0]=OFF;
		 OSD_SPEAKER[1]=OFF;
		 OSD_SPEAKER[2]=ON;
		 
		 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE8 ch2 ch1 ch4\r\n") ;
		   break ;
	   }   
case VS4210_LOOP_MODE8_234:
   {
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =2 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =3 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =4 ;
		 OSD_MIRROR[0]=CAMA_M; 
		 OSD_MIRROR[1]=CAMC_M; 
		 OSD_MIRROR[2]=CAMR_M; 
	gabVXIS_Input_mirrorH[0]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMR_M; 			 
		 OSD_SPEAKER[0]=OFF;
		 OSD_SPEAKER[1]=OFF;
		 OSD_SPEAKER[2]=ON;
		 
		 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE8 ch2 ch3 ch4\r\n") ;
		   break ;
	   }   

//=============
case VS4210_LOOP_MODE8_312:
   {
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =3 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =1 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =2 ;
		 OSD_MIRROR[0]=CAMC_M; 
		 OSD_MIRROR[1]=CAMA_M; 
		 OSD_MIRROR[2]=CAMB_M; 
	gabVXIS_Input_mirrorH[0]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMB_M; 		 
		 OSD_SPEAKER[0]=ON;
		 OSD_SPEAKER[1]=OFF;
		 OSD_SPEAKER[2]=OFF;	 
		 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE8 ch3 ch1 ch2\r\n") ;
		   break ;
	   }   
case VS4210_LOOP_MODE8_314:
   {
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =3 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =1 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =4 ;
		 OSD_MIRROR[0]=CAMC_M; 
		 OSD_MIRROR[1]=CAMA_M; 
		 OSD_MIRROR[2]=CAMR_M; 
	gabVXIS_Input_mirrorH[0]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMR_M; 		 
		 OSD_SPEAKER[0]=OFF;
		 OSD_SPEAKER[1]=OFF;
		 OSD_SPEAKER[2]=ON;	 
		 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE8 ch3 ch1 ch4\r\n") ;
		   break ;
	   }   
case VS4210_LOOP_MODE8_324:
   {
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =3 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =2 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =4 ;
		 OSD_MIRROR[0]=CAMC_M; 
		 OSD_MIRROR[1]=CAMB_M; 
		 OSD_MIRROR[2]=CAMR_M; 
	gabVXIS_Input_mirrorH[0]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMR_M; 			 
		 OSD_SPEAKER[0]=OFF;
		 OSD_SPEAKER[1]=OFF;
		 OSD_SPEAKER[2]=ON;	 
		 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE8 ch3 ch2 ch4\r\n") ;
		   break ;
	   }   
//=============

		case VS4210_LOOP_MODE8_413:
		   {
				   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
				   pVS4210_JointKind1.WindowMap.WMap0 =4 ;
				   pVS4210_JointKind1.WindowMap.WMap1 =1 ;
				   pVS4210_JointKind1.WindowMap.WMap2 =3 ;
				 OSD_MIRROR[0]=CAMR_M; 
				 OSD_MIRROR[1]=CAMA_M; 
				 OSD_MIRROR[2]=CAMC_M; 
	gabVXIS_Input_mirrorH[0]=CAMR_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 				 
				 OSD_SPEAKER[0]=ON;
				 OSD_SPEAKER[1]=OFF;
				 OSD_SPEAKER[2]=OFF;  
				 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
				   printf("VS4210_MODE8 ch4 ch1 ch3\r\n") ;
				   break ;
			   }   
		case VS4210_LOOP_MODE8_423:
		   {
				   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8 ;
				   pVS4210_JointKind1.WindowMap.WMap0 =4 ;
				   pVS4210_JointKind1.WindowMap.WMap1 =2 ;
				   pVS4210_JointKind1.WindowMap.WMap2 =3 ;
				 OSD_MIRROR[0]=CAMR_M; 
				 OSD_MIRROR[1]=CAMB_M; 
				 OSD_MIRROR[2]=CAMC_M; 
	gabVXIS_Input_mirrorH[0]=CAMR_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 					 
				 OSD_SPEAKER[0]=ON;
				 OSD_SPEAKER[1]=OFF;
				 OSD_SPEAKER[2]=OFF;  
				 //  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
				   printf("VS4210_MODE8 ch4 ch2 ch3\r\n") ;
				   break ;
			   }   

		//=============
	    case VS4210_LOOP_MODE10_123:
    {
  pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10 ;

pVS4210_JointKind1.WindowMap.WMap0 =1 ;
pVS4210_JointKind1.WindowMap.WMap1 =2 ;
pVS4210_JointKind1.WindowMap.WMap2 =3 ;
	pVS4210_JointKind1.WindowMap.WMap3 =4 ;

	OSD_MIRROR[0]=CAMR_M; 
	OSD_MIRROR[1]=CAMA_M; 
	OSD_MIRROR[2]=CAMB_M; 
	OSD_MIRROR[3]=CAMC_M; 

gabVXIS_Input_mirrorH[0]=CAMR_M;		  
gabVXIS_Input_mirrorH[1]=CAMA_M;		  
gabVXIS_Input_mirrorH[2]=CAMB_M;		
gabVXIS_Input_mirrorH[3]=CAMC_M;		  

	OSD_SPEAKER[0]=ON;
	OSD_SPEAKER[1]=OFF;
	OSD_SPEAKER[2]=OFF;
	OSD_SPEAKER[3]=OFF;


     //   pVS4210_JointKind1.WindowMap.WMap3 =4 ;
        printf("VS4210_MODE10 ch1 ch2 ch3\r\n") ;
        break ;
    }
case VS4210_LOOP_MODE10_412:
	{
	   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10 ;
	   pVS4210_JointKind1.WindowMap.WMap0 =4 ;
	   pVS4210_JointKind1.WindowMap.WMap1 =1 ;
	   pVS4210_JointKind1.WindowMap.WMap2 =2 ;
	    pVS4210_JointKind1.WindowMap.WMap3 =0 ;
	  OSD_MIRROR[0]=CAMR_M;	
   	  OSD_MIRROR[1]=CAMA_M;	
   	  OSD_MIRROR[2]=CAMB_M;	
	gabVXIS_Input_mirrorH[0]=CAMR_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMB_M; 	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;
	  OSD_SPEAKER[2]=OFF;
	//	 pVS4210_JointKind1.WindowMap.WMap3 =4 ;
	   printf("VS4210_MODE10 ch4 ch1 ch2\r\n") ;
		break;
    	}
	case VS4210_LOOP_MODE10_413:
		{
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =4 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =1 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =3 ;
		  OSD_MIRROR[0]=CAMR_M; 
		  OSD_MIRROR[1]=CAMA_M; 
		  OSD_MIRROR[2]=CAMC_M; 
	gabVXIS_Input_mirrorH[0]=CAMR_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMA_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 			  
		  OSD_SPEAKER[0]=ON;
		  OSD_SPEAKER[1]=OFF;
		  OSD_SPEAKER[2]=OFF;
		//	 pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE10 ch4 ch1 ch3\r\n") ;
			break;
			}
	case VS4210_LOOP_MODE10_423:
		{
		   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10 ;
		   pVS4210_JointKind1.WindowMap.WMap0 =4 ;
		   pVS4210_JointKind1.WindowMap.WMap1 =2 ;
		   pVS4210_JointKind1.WindowMap.WMap2 =3 ;
		  OSD_MIRROR[0]=CAMR_M; 
		  OSD_MIRROR[1]=CAMB_M; 
		  OSD_MIRROR[2]=CAMC_M; 
	gabVXIS_Input_mirrorH[0]=CAMR_M; 		  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 			  
		  OSD_SPEAKER[0]=ON;
		  OSD_SPEAKER[1]=OFF;
		  OSD_SPEAKER[2]=OFF;
		//	 pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		   printf("VS4210_MODE10 ch4 ch2 ch3\r\n") ;
			break;
			}

    case VS4210_LOOP_MODE9:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
        pVS4210_JointKind1.WindowMap.WMap2 =3 ;
        pVS4210_JointKind1.WindowMap.WMap3 =4 ;
	  OSD_MIRROR[0]=CAMA_M;	
   	  OSD_MIRROR[1]=CAMB_M;	
   	  OSD_MIRROR[2]=CAMC_M;	
   	  OSD_MIRROR[3]=CAMR_M;	
       gabVXIS_Input_mirrorH[0]=CAMA_M;			  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 		  
	gabVXIS_Input_mirrorH[2]=CAMC_M; 		  
	gabVXIS_Input_mirrorH[3]=CAMR_M; 	  
	  OSD_SPEAKER[0]=OFF;
	  OSD_SPEAKER[1]=OFF;
	  OSD_SPEAKER[2]=OFF;
	  OSD_SPEAKER[3]=ON;	  
        printf("VS4210_MODE9\r\n") ;
        break ;
    }
	#endif
    case VS4210_LOOP_MODE12_12:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
	  OSD_MIRROR[0]=CAMA_M;	
   	  OSD_MIRROR[1]=CAMB_M;	
       gabVXIS_Input_mirrorH[0]=CAMA_M;			  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 	  
  	OSD_SPEAKER[0]=ON;
  	OSD_SPEAKER[1]=OFF;
        printf("VS4210_MODE12 ch1 ch2\r\n") ;
        break ;
    }
	case VS4210_LOOP_MODE12_14:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =4 ;
	  OSD_MIRROR[0]=CAMA_M;	
   	  OSD_MIRROR[1]=CAMR_M;
       gabVXIS_Input_mirrorH[0]=CAMA_M;			  
	gabVXIS_Input_mirrorH[1]=CAMR_M; 	  
  	OSD_SPEAKER[0]=OFF;
  	OSD_SPEAKER[1]=ON;	  
        printf("VS4210_MODE12 ch1 ch4\r\n") ;	
	break;
    	}
	case VS4210_LOOP_MODE12_42:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =4 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
	  OSD_MIRROR[0]=CAMR_M;	
   	  OSD_MIRROR[1]=CAMB_M;	
       gabVXIS_Input_mirrorH[0]=CAMR_M;			  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;		  
        printf("VS4210_MODE12 ch4 ch2\r\n") ;	
	break;
    	}
	case VS4210_LOOP_MODE12_43:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =4 ;
        pVS4210_JointKind1.WindowMap.WMap1 =3 ;
	  OSD_MIRROR[0]=CAMR_M;	
   	  OSD_MIRROR[1]=CAMC_M;	
       gabVXIS_Input_mirrorH[0]=CAMR_M;			  
	gabVXIS_Input_mirrorH[1]=CAMC_M; 	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;		  
        printf("VS4210_MODE12 ch4 ch3\r\n") ;	
	break;
    	}
	//==============
	case VS4210_LOOP_MODE12_21:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =2 ;
        pVS4210_JointKind1.WindowMap.WMap1 =1 ;
	  OSD_MIRROR[0]=CAMB_M;	
   	  OSD_MIRROR[1]=CAMA_M;	
       gabVXIS_Input_mirrorH[0]=CAMB_M;			  
	gabVXIS_Input_mirrorH[1]=CAMR_M; 	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;		  
        printf("VS4210_MODE12 ch2 ch1\r\n") ;	
	break;
    	}
	case VS4210_LOOP_MODE12_23:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =2 ;
        pVS4210_JointKind1.WindowMap.WMap1 =3 ;
	  OSD_MIRROR[0]=CAMB_M;	
   	  OSD_MIRROR[1]=CAMC_M;	
       gabVXIS_Input_mirrorH[0]=CAMB_M;			  
	gabVXIS_Input_mirrorH[1]=CAMC_M; 	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;		  
        printf("VS4210_MODE12 ch2 ch3\r\n") ;	
	break;
    	}
	case VS4210_LOOP_MODE12_24:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =2;
        pVS4210_JointKind1.WindowMap.WMap1 =4 ;
	  OSD_MIRROR[0]=CAMB_M;	
   	  OSD_MIRROR[1]=CAMR_M;	
       gabVXIS_Input_mirrorH[0]=CAMB_M;			  
	gabVXIS_Input_mirrorH[1]=CAMR_M; 	  
	  OSD_SPEAKER[0]=OFF;
	  OSD_SPEAKER[1]=ON;		  
        printf("VS4210_MODE12 ch2 ch4\r\n") ;	
	break;
    	}

	//==============
	case VS4210_LOOP_MODE12_13:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =3 ;
	  OSD_MIRROR[0]=CAMA_M;	
   	  OSD_MIRROR[1]=CAMC_M;	
       gabVXIS_Input_mirrorH[0]=CAMA_M;			  
	gabVXIS_Input_mirrorH[1]=CAMC_M; 	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;	
	  
        printf("VS4210_MODE12 ch1 ch3\r\n") ;	
	break;
    	}
	case VS4210_LOOP_MODE12_32:
      {
       pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =3 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
	  OSD_MIRROR[0]=CAMC_M;	
   	  OSD_MIRROR[1]=CAMB_M;	
       gabVXIS_Input_mirrorH[0]=CAMC_M;			  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 	  
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;	
	  
        printf("VS4210_MODE12 ch3 ch2\r\n") ;	
	break;
    	}

    case VS4210_LOOP_MODE12_34:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
        pVS4210_JointKind1.WindowMap.WMap0 =3 ;
        pVS4210_JointKind1.WindowMap.WMap1 =4 ;
	  OSD_MIRROR[0]=CAMC_M;	
   	  OSD_MIRROR[1]=CAMR_M;	
       gabVXIS_Input_mirrorH[0]=CAMC_M;			  
	gabVXIS_Input_mirrorH[1]=CAMR_M; 	  
	  OSD_SPEAKER[0]=OFF;
	  OSD_SPEAKER[1]=ON;	
	  
        printf("VS4210_MODE12 ch3 ch4\r\n") ;
        break ;
    }
//========================

case VS4210_LOOP_MODE12_31:
  {
   pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12 ;
	pVS4210_JointKind1.WindowMap.WMap0 =3 ;
	pVS4210_JointKind1.WindowMap.WMap1 =1 ;
  OSD_MIRROR[0]=CAMC_M; 
  OSD_MIRROR[1]=CAMA_M; 
       gabVXIS_Input_mirrorH[0]=CAMC_M;			  
	gabVXIS_Input_mirrorH[1]=CAMA_M;   
  OSD_SPEAKER[0]=ON;
  OSD_SPEAKER[1]=OFF;	
  
	printf("VS4210_MODE12 ch3 ch1\r\n") ;	
break;
	}

//=======================
#if 0
    case VS4210_LOOP_MODE14:
    {
        pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE14 ;
        pVS4210_JointKind1.WindowMap.WMap0 =1 ;
        pVS4210_JointKind1.WindowMap.WMap1 =2 ;
        pVS4210_JointKind1.WindowMap.WMap2 =3 ;
	  OSD_MIRROR[0]=CAMA_M;	
   	  OSD_MIRROR[1]=CAMB_M;	
   	  OSD_MIRROR[2]=CAMC_M;	
       gabVXIS_Input_mirrorH[0]=CAMA_M;			  
	gabVXIS_Input_mirrorH[1]=CAMB_M; 	  
	  gabVXIS_Input_mirrorH[2]=CAMC_M;		
	  OSD_SPEAKER[0]=ON;
	  OSD_SPEAKER[1]=OFF;	
	  OSD_SPEAKER[3]=OFF;	
	  
        printf("VS4210_MODE14 ch1 ch2\r\n") ;
        break ;
    }
#endif
    }

	
VS4210_SetMutiWindows(&pVS4210_JointKind1) ;

//AUDIO_SELECT(index);
AudioMute(ON);

MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT);
MCUTimerActiveTimerEvent(SEC(1),  _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT);
AUDIO_SELECT(index);

CURRENT_CH=index;

//ChangeVol(0);

}

/*
static int GetKet_State(void)
{
    int re = 0  ;

	#if 0
    if (HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin ) == 0)
    {
        re =1 ;
		  re =0 ;
     //   printf("SW1 Key\r\n") ;
    }
    else if (HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin ) == 0)
    {
        re =2 ;
		 re =0 ;
    //    printf("SW2 Key\r\n") ;
    }
    else if (HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin ) == 0)
    {
        re =3 ;
        printf("SW3 Key\r\n") ;
    }
    else if (HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin ) == 0)
    {
        re =4 ;
        printf("SW4 Key\r\n") ;
    }
    else if (HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin ) == 0)
    {
       // re =5 ;
      //  printf("SW5 Key\r\n") ;
      Monitor(); 
    }
    else if (HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin ) == 0)
    {
        re =6 ;
        printf("SW6 Key\r\n") ;
    }
    else if (HAL_GPIO_ReadPin(SW7_GPIO_Port, SW7_Pin ) == 0)
    {
        re =7 ;
        printf("SW7 Key\r\n") ;
    }
#endif

    return re ;
}
*/
	static void HDMI_VideoModeDetect(void)
	{
		tByte i  ;
	
		for(i = 0 ; i< 2/*4*/ ; i++ )
		{
			gHDMI_Index = i ;
			//--- Start --- 20201214-01
			if( i == 1 )
			{
				if( newInRes[0] == _OSD_No_Signal )
					bSignal[gHDMI_Index] = 0;
				else
					bSignal[gHDMI_Index] = 1;
			}
			else
			//--- End ---
			bSignal[gHDMI_Index] = CheckHDMIRX();

			//if( (i == 1 )&&(bSignal[1]==1))
			//{
			//dbg_Printf("ch%d, InputMode=%d \r\n", (int)(i+1),(int) gbVXIS_InputMode[i] ) ;
			//}
			
			if (PreVState[gHDMI_Index] != VState[gHDMI_Index]  )
			{
				PreVState[gHDMI_Index]	= VState[gHDMI_Index] ;
			}
	
			if(bSignal[gHDMI_Index] != bOldSignal[gHDMI_Index] )
			{
				bOldSignal[gHDMI_Index] = bSignal[gHDMI_Index] ;
	
				if(bSignal[gHDMI_Index] == 0 )
				{
					VS4210_Rx_NoSignal(i) ;
					dbg_Printf("##ch%d, VS4210_Rx_NoSignal\r\n", (int)(i+1) ) ;
					if (pVS4210_MutiWindows1.DrawOSDFlg != 0)
					{
						OSD_Show_Flg = 1 ;
						Tim1_count = 0 ;
						OSDStateFlg = 1 ;
						OSDSecCount = 0 ;
					}
					gbVXIS_InputMode[gHDMI_Index] =_OSD_No_Signal ;
					PrebVXIS_InputMode[gHDMI_Index] = _OSD_No_Signal ;

					if((SelectInputMode==_HDMI_Input)&&(gHDMI_Index==0))
						Set_Video_Loss_OSD(0,ON);
					//MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS);	
					//VXISI2CWrite(0x9C,0x0A);
						SET_Audio_Mute(ON);
				}
			}
	
			if( bSignal[gHDMI_Index] == 1  )
			{
				//--- Start --- 20201214-01
				if( i == 1 )
					gbVXIS_InputMode[gHDMI_Index] = newInRes[0];
				else
				//--- End ---
				gbVXIS_InputMode[gHDMI_Index] = GetHDMI_InputMode(gHDMI_Index ) ;

			}
	
			if (gbVXIS_InputMode[gHDMI_Index] != PrebVXIS_InputMode[gHDMI_Index] )
			{
				PrebVXIS_InputMode[gHDMI_Index] = gbVXIS_InputMode[gHDMI_Index] ;
	
				if (gbVXIS_InputMode[gHDMI_Index] != _OSD_No_Signal)
				{
					VS4210_SetInModeOutMode(i , gbVXIS_InputMode[i] , pVS4210_MutiWindows1.OutputMode );

					if((SelectInputMode==_AHD_Input)&&(gHDMI_Index==1))			
					VXISI2CWrite( 0x19, 0x01 );
							
							
					VS4210_SetInputSizeHV(i ,gHActive[i], gVActive[i], gHTotal[i] ,gVTotal[i] ) ;


					dbg_Printf("ch%d, InputMode=%d \r\n", (int)(i+1),(int) gbVXIS_InputMode[i] ) ;

						#if 0
						if(gbVXIS_InputMode[i]==6)
						{
						printf("(set reg.)");
						 VXISI2CWrite(0x08,0x21);
						 VXISI2CWrite(0x0A,0x21);
						 VXISI2CWrite(0x74,0x02);
						 VXISI2CWrite(0x82,0x40);
						}
						#endif
					if (pVS4210_MutiWindows1.DrawOSDFlg != 0)
					{
						OSD_Show_Flg = 1 ;
						Tim1_count = 0 ;
						OSDStateFlg = 1 ;
						OSDSecCount = 0 ;
					}

						if((SelectInputMode==_HDMI_Input)&&(gHDMI_Index==0))
						Set_Video_Loss_OSD(0,OFF);	

						if((SelectInputMode==_AHD_Input)&&(gHDMI_Index==1))
							{
							MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_VIDEO_RESET);
							}
						
						if(gHDMI_Index==0)
						SET_Audio_Mute(OFF);	
						//MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS);
				}
			}
		}
	}

static void HDMI_AudioModeDetect(void)
{
    tByte pbAudioSampleFreq, pbValidCh ;
    pbAudioSampleFreq = 0 ;

    getHDMIRXAudioInfo(&pbAudioSampleFreq , &pbValidCh ) ;

    if (gbAUDO_INDEX != HDMI_AUDIO_NONE )
    {
        if (gbAUDO_INDEX  == gHDMI_Index )
        {
            if (gPreAudioSampleFreq != pbAudioSampleFreq)
            {
                gPreAudioSampleFreq = pbAudioSampleFreq ;
                HDMI_Tx_SetAudioFs(gPreAudioSampleFreq )  ;
            }
        }
    }
    gAudioSampleFreq[gHDMI_Index] = pbAudioSampleFreq ;
}

//=============================================================================
//    Main Initialize Routine
//=============================================================================
void main_init (void)
{

	WORD	rev;

		rev = GetFWRevEE();
		Printf("\r\nPrev.FW:%04x\r\n", rev);
		
		if( GetFWRevEE()!=FWVER )	// exist EEPROM
		{					
			Printf("\r\nCurr.FW:%04x", (WORD)FWVER);
			SaveFWRevEE(FWVER);
			Printf("\r\nCurr.FW:%04x", (WORD)GetFWRevEE());
		}

		if( GetFWRevEE()==FWVER )
		{					// exist EEPROM
			if( rev != FWVER ) 
				{
				ClearBasicEE();
			
			Printf("\r\n[MSG]main_init()=> rev != FWVER & call ClearBasicEE()\r\n");  //pinchi test
				}
			else
				{
				;//DebugLevel = GetDebugLevelEE();

				}
		}
		else 
		{
			Printf("\r\nCannot access EEPROM!!!!");
			Printf("\r\nNo initialize");
			DebugLevel = 0xff;
			return;
		}


	delay(100);
	

//	SET_POWER_STATUS(_POWER_STATUS_OFF);
//	SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);

}


void AdjustAutoDayNight(BYTE val)
{
//	BYTE page;
	BYTE PWMLevelTemp=0;
	val=0xff-val;

	#if 1//ryan@20170928
	if(val>=0&&val<100)
		PWMLevelTemp=0+70;
	else if(val>=100&&val<140)
		PWMLevelTemp=20+70;//20;
	else if(val>=140&&val<180)
		PWMLevelTemp=40+70;//50;
	else if(val>=180&&val<220)
		PWMLevelTemp=60+70;//65;
	else if(val>=220&&val<=255)//andy 20110414
	{
		//#ifdef AWT
		PWMLevelTemp=75+70;//80;
		//#else
		//PWMLevelTemp=115;
		//#endif
		//	PWMLevelTemp=128;
	}

	#else
	if(val>=0&&val<100)
		PWMLevelTemp=0;
	else if(val>=100&&val<140)
		PWMLevelTemp=30;//20;
	else if(val>=140&&val<180)
		PWMLevelTemp=60;//50;
	else if(val>=180&&val<220)
		PWMLevelTemp=80;//65;
	else if(val>=220&&val<=255)//andy 20110414
	{
		//#ifdef AWT
		PWMLevelTemp=110;//80;
		//#else
		//PWMLevelTemp=115;
		//#endif
	}
	#endif
	//else
	//	PWMLevelTemp=0;


	if(DayNightLevel==1)
	{
		//if(AutoDayNightDelay<(5+5+5))
	//		AutoDayNightDelay++;
		//else
		{
			if(PWMLevel > PWMLevelTemp)
				PWMLevel-=1;
			else if(PWMLevel < PWMLevelTemp)
				PWMLevel+=1;

			AutoDayNightDelay=0;
		}

	//	PWMLevel=PWMLevelTemp;

		
	//	Printf("(PWMLevel1=%02x %02x)",(WORD)PWMLevel,(WORD)PWMLevelTemp);

	user_pwm_setvalue(PWMLevel*15);

	}
	else
	{
	//	PWMLevel=PWMLevelTemp;
		PWMLevel=255/*0x80*/;
	//		Printf("(PWMLevel2=%02x)",(WORD)PWMLevel);
	user_pwm_setvalue(_PWMLevelMax);
	}
	
#if 0

	//Printf("\r\nLADC0=%02x",(WORD)val);
	page = ReadTW88( 0xff );
	WriteTW88( 0xff, 0x00 );		// set page 0
	WriteTW88( 0xbe, ((ReadTW88(0xbe) & 0xcf) | 0x10) );	// set active Low control, if want active high clear 
	//WriteTW88( 0xc4, ((ReadTW88(0xc4) & 0x80)|(val>>2)) );	// set pwm number
	WriteTW88( 0xc4, ((ReadTW88(0xc4) & 0x80)|(PWMLevel)) );	// set pwm number
	WriteTW88( 0xff, page );
	
	#endif
	/*
	if(DayNightLevel==0)
	user_pwm_setvalue(4000);
	else
	user_pwm_setvalue(PWMLevel*15);
*/		
}
void LCDPowerON(void)
{
/*
	Printf("(LCDPowerON)");

	TTLI1_RES(OFF);
	PANEL_EN(ON);
	msleep(50) ;
	msleep(100) ;///for new 7in panel  	LCM : TH070800480NYR40S1-L
//	MCUTimerDelayXms(50);
	LCD_PWR(ON);

	msleep(50) ;

//	MCUTimerDelayXms(50);
	BL_ENABLE(ON);
HAL_GPIO_WritePin(PA7_GPIO_Port, PA7_Pin, 0);

//	BL_PWM(ON);

AudioMute(OFF);
*/

}

void LCDPowerOFF(void)
{
/*
	Printf("(LCDPowerOFF)");

#if 0

	TTLI1_RES(OFF);
	PANEL_EN(ON);
	msleep(50) ;
//	MCUTimerDelayXms(50);
	LCD_PWR(ON);

	msleep(50) ;

//	MCUTimerDelayXms(50);
	BL_ENABLE(ON);
//	BL_PWM(ON);

#else
	BL_ENABLE(OFF);
HAL_GPIO_WritePin(PA7_GPIO_Port, PA7_Pin, 1); 

//	BL_PWM(OFF);
//	msleep(50) ;
//	MCUTimerDelayXms(50);
	TTLI1_RES(ON);
	PANEL_EN(OFF);
//	MCUTimerDelayXms(50);
//	msleep(50) ;

	LCD_PWR(OFF);
#endif

AudioMute(ON);
*/
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

void uComOnChipInitial(void)
{

//power reset

//power
PCON3V3_TP(ON);      
PCON1V2_TP(ON);      

VS4210_RST(OFF);   

#if 0

	Set_USART1();


	//Set_USART2();
	//Set_USB();
	//USB_Interrupts_Config();
	Set_GPIO_Port();

	//AUDIO_OUT_Flag=ReadEEP(0xae);
	MUTE_OUT(1);



	Set_Timer();
	Set_Init_GPIO();

	TIM4_PWM(0);
	Set_Battery_ADC();

	EXTI_Configuration();

	//Set_I2C_Port();
	//Set_IR_Port();
	//Set_ADC_Port();
	
GPIO_SetBits(GPIOA, GPIO_Pin_1);//TW8816 reset
GPIO_ResetBits(GPIOA, GPIO_Pin_0);//TW8816 reset

GPIO_SetBits(GPIOA, GPIO_Pin_12);//ryan@20150525
GPIO_SetBits(GPIOA, GPIO_Pin_13);//ryan@20150525
GPIO_SetBits(GPIOA, GPIO_Pin_14);//ryan@20150525

GPIO_SetBits(GPIOC, GPIO_Pin_9);//ryan@20150525
GPIO_SetBits(GPIOC, GPIO_Pin_13);//ryan@20150525
GPIO_SetBits(GPIOC, GPIO_Pin_14);//ryan@20150525
GPIO_SetBits(GPIOC, GPIO_Pin_15);//ryan@20150525

GPIO_SetBits(GPIOC, GPIO_Pin_7);//ryan@20150525

//GPIO_ResetBits(GPIOC, GPIO_Pin_8);//ryan@20150525
PC_SW(1);//MP board
DC_OUT(0);//MP board
	

#if __USE_SPI_FLASH__ == 1
	Reset_SPI();
	Set_SPI();
#endif

	//Set_USBClock();
	//USB_Interrupts_Config();
	//USB_Init();

#endif

}

void user_pwm_setvalue(uint16_t value)
{
    TIM_OC_InitTypeDef sConfigOC;
  
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);  
}

void LoadEEPROM (void)
{
#if 0
	 PowerFlag = ReadEEP(EEP_Powerflag);
	 SelectModeType = ReadEEP(EEP_SelectMode);

	OSD_LOCKF= ReadEEP(EEP_ONOFFChoice);
	Disp_Input = ReadEEP(EEP_COSDMode); 
		
	 TimeStep = ReadEEP(EEP_TimeStep);
	 Priority=ReadEEP(EEP_PRIORITY);
	 DayNightLevel= ReadEEP(EEP_AutoMode);
	 NowModeState = ReadEEP(EEP_NowModeState);
	
	AudioVol = GetAudioVolEE();
	CAMA_M= ReadEEP(EEP_CAMAMode);
	CAMB_M= ReadEEP(EEP_CAMBMode);
	CAMC_M= ReadEEP(EEP_CAMCMode);
	CAMR_M= ReadEEP(EEP_CAMRMode);

	AudioVol = GetAudioVolEE();

#endif
}

void WaitPowerOn(void)
{
BYTE val;
	PowerLED(_SYSTEM_TIMER_EVENT_RED_ON);

	LCDPowerOFF();
	
	MCUTimerCancelTimerEvent( _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);

	Printf("\r\n(Power off..)");
		
	while(1) 
	{	

			CheckKeyIn();
			if(PowerFlag==ON)
			{
				//PowerFlag=ON;
				WriteEEP(EEP_Powerflag,PowerFlag);//2007.3.28 Justin
				break;		//poweron
			}
			
		  val=CheckTrigger();
		 
//		  TriggerHandler(val);

		if(val!=0)
		{	
			  PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);
			TriggerHandler(val);
			//	AudioMute(OFF);
			//DisplayInput();		//william v0.2 20120223
			/*
			if(!(TriggerMode==0x08))
			{
				//Printf("\r\n(**PowerFlag=%02x)",(WORD)PowerFlag);
				LCDPowerON(1);//修正power on時顯示太慢問題
				AudioMute(1);
			}
			*/
			
		}
		else
		{
			//Printf("\r\n(*NO-TRIG)");
			TriggerHandler(val);  //Pinchi 20140922 add for AWT ML072Q


				if(TriggerFlag==0)
				{
				//PowerOff();
			//	AudioMute(ON);
				
				//PowerLED(_SYSTEM_TIMER_EVENT_RED_ON);
				/*
				if(live_out_state==1)	////live out issue 
				{
				live_out_state=0;
				SetNowMode();
				Panel_power=0;
				}
				*/
				}
			
		//	TriggerHandler(val);  //Pinchi 20140922 move to above for AWT ML072Q	
		}		  

		   SysTimerHandler();
		   	  EQ_Detect();	 
			  
//			  	if((GT9147_Scan(1)>=_TOUCH_COUNT_1)&&(TP_ACTION==ON))
					if(GT9147_Scan(1)>=_TOUCH_COUNT_1)

					PowerFlag=ON;
					
   	}

	PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);
//	SHOWOSDINFO();	
	
//		if((NowModeState==JumpMode)&&(TriggerFlag==0))
//			 ShowJUMPOSD();
		
	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_DISPLAY_DISABLE);
	MCUTimerActiveTimerEvent(SEC(2),_USER_TIMER_EVENT_DISPLAY_DISABLE);

	

MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
//LCDPowerON();
Printf("\r\n(Power ON..)");		


}


int main(void)
{
  /* USER CODE BEGIN 1 */
  TimeOutCount = 0 ;

    tByte i ;
    tByte j ;
    tByte exosdLen ;
    tByte exosdStart ;
#ifdef VICTOR_DEBUG
    tByte val ;
#endif
  //  int keystate ;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the F
   sh interface and the Systick. */
  HAL_Init();
  
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  
  /* USER CODE BEGIN SysInit */
  InitialTimerEvent();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
 // LCDPowerOFF();
// AudioMute(ON);

//SET_PCA9554PW(_Shutter_SW,OFF);
 // SET_PCA9554PW(_CCD12_SW,ON);
//SET_PCA9554PW(_PCON5V_AUDIO,ON);
 //SET_PCA9554PW(_COL_PW_5V,OFF);

//  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin , GPIO_PIN_SET);
//  HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin , GPIO_PIN_SET); 

 // PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);

//  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
//  MX_ADC1_Init();
  
  /* USER CODE BEGIN 2 */

//HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);	
  
 SW_I2C0_Init() ;
 SW_I2C1_Init() ;
 SW_I2C2_Init() ;
 SW_I2C3_Init() ;

//user_pwm_setvalue(2000);

  //uComOnChipInitial();
  
// if((SelectInputMode==_HDMI_Input)&&(gHDMI_Index==0))
Set_Video_Loss_OSD(0,ON);
SET_Audio_Mute(ON);


 // GT9147_Init();
#if 1
  msleep(50) ;
  HAL_UART_Receive_IT(&huart1,(uint8_t*)Rx_data, 1);	//activate UART receive interrupt every time
  HAL_UART_Receive_IT(&huart2,(uint8_t*)Rx2_data, 1);	//activate UART receive interrupt every time

 // HAL_GPIO_WritePin(TP2824_RST_GPIO_Port, VS_RST_Pin, 0); //RX RST
 //HAL_GPIO_WritePin(TX_RST_GPIO_Port, VS_RST_Pin, 0); //TX RST
  HAL_GPIO_WritePin(VS_RST_GPIO_Port, VS_RST_Pin, 0); //VS4210 RST
 TP2824_SET_REST(0);

  msleep(100) ;
  //HAL_GPIO_WritePin(TP2824_RST_GPIO_Port, VS_RST_Pin, 1); //VS4210 RST
// HAL_GPIO_WritePin(TX_RST_GPIO_Port, VS_RST_Pin, 1); //TX RST
  HAL_GPIO_WritePin(VS_RST_GPIO_Port, VS_RST_Pin, 1); //VS4210 RST
 TP2824_SET_REST(1);

#endif
//LCDPowerON();
// SET_PCA9554PW(_COL_PW_5V,ON); 

  //I2CWrite(VS9988_ADDR, 0x01, 0x5A);  //reset
//  printf("\r\nVS4210 sdk2.0 Compiled %s %s !\r\n",__DATE__ , __TIME__);
// val = VXISI2CRead(0x00) ;
//  printf("VS4210 ID = 0x%02X !\r\n", (int)val );

  //Init_TP280x_RegSet();

  // RX Init
  for (i = 0 ; i < HDMI_IN_NUMBER ; i++ )
  {
      gHDMI_Index = i ;
      dbg_Printf("Init HDMI RX,Chip(%d)....\r\n",(int)i ) ;
      InitHDMIRX(0);
      HDMIRX_WriteI2C_Byte(0x89 ,0x80);

#ifdef VICTOR_DEBUG
      val = HDMIRX_ReadI2C_Byte(0x02) ;
      dbg_Printf("IT66021 chip num=%d , 02reg=0x%02x\r\n", (int )i , (int)val ) ;
#endif
  }
  
//  dbg_Printf("Init HDMI Tx....\r\n") ;
 //  HDMI_Tx_Init();

   if (HDMI_IN_NUMBER == 4 )
    pVS4210_MutiWindows1.OutputMode = VS4210_1080P_MODE1 ;  // 4windows
  else
	  #ifdef VS4210_SELECT_WIN1	//20201211-02
	  pVS4210_MutiWindows1.OutputMode =VS4210_1080P_MODE0_w1;
	  #else
      pVS4210_MutiWindows1.OutputMode =VS4210_1080P_MODE0_w0;/// VS4210_1080P_MODE12 ;  
	  #endif

  pVS4210_MutiWindows1.AudioChannel = HDMI_AUDIO_1 ;
  pVS4210_MutiWindows1.OutputSize = _OUTPUT_1080P60 ;
	#ifdef VS4210_SELECT_WIN1	//20201211-02
	pVS4210_MutiWindows1.WindowMap.WMap0 = 2 ;
  pVS4210_MutiWindows1.WindowMap.WMap1 = 1 ; //set ch2 = video2
  pVS4210_MutiWindows1.WindowMap.WMap2 = 3 ; //set ch3 = video3
  pVS4210_MutiWindows1.WindowMap.WMap3 = 4 ; //set ch4 = video4
	#else
  pVS4210_MutiWindows1.WindowMap.WMap0 = 1 ; //set ch1 = video1
  pVS4210_MutiWindows1.WindowMap.WMap1 = 2 ; //set ch2 = video2
  pVS4210_MutiWindows1.WindowMap.WMap2 = 3 ; //set ch3 = video3
  pVS4210_MutiWindows1.WindowMap.WMap3 = 4 ; //set ch4 = video4
	#endif
#if 1
//Extern OSD 20200110-01
VXOSD_UseFont_init(EXTERNOSDNUMBER ,&UserDefineFont1[0] ) ;

main_init();
#endif

#ifdef  _SUPPORT_EDID_RAM_
//interval EDID ryan@20200917
//SETIT66021EDID();
#endif

   Init_TP280x_RegSet();

   //test@start
#ifdef VICTOR_DEBUG
   val = VXISI2CRead(0x00) ;
   dbg_Printf("VS4210 chip ID = 0x%02x\r\n",(int)val) ;
   dbg_Printf("VS4210 Init. (Startting...)\r\n") ;
#endif

 pVS4210_MutiWindows1.DrawOSDFlg = 0x03 ;  //Enable OSD, bit1=Title, bit0 = bar
  //pVS4210_MutiWindows1.DrawOSDFlg = 0x00 ;  //Enable OSD, bit1=Title, bit0 = bar
  //pVS4210_MutiWindows1.OutputMode = VS4210_1080P_MODE1 ;  // 4windows
  //pVS4210_MutiWindows1.OutputMode = VS4210_1080P_MODE6 ;  // 1_3 windows
  //pVS4210_MutiWindows1.OutputMode = VS4210_1080P_MODE12 ;  // 4windows
//  pVS4210_MutiWindows1.WindowMap.WMap0 = 1 ; //set ch1 = video1
//  pVS4210_MutiWindows1.WindowMap.WMap1 = 2 ; //set ch2 = video2
//  pVS4210_MutiWindows1.WindowMap.WMap2 = 3 ; //set ch3 = video3
//  pVS4210_MutiWindows1.WindowMap.WMap3 = 4 ; //set ch4 = video4
  //pVS4210_MutiWindows1.AudioChannel = HDMI_AUDIO_1 ;
  pVS4210_MutiWindows1.OutputSize = _OUTPUT_1080P60 ;
  //pVS4210_MutiWindows1.OutputSize = _OUTPUT_720P60 ;
  //pVS4210_MutiWindows1.DrawBoxFlg =1 ;
  pVS4210_MutiWindows1.DrawBoxFlg =0 ;
  pVS4210_MutiWindows1.DrawBoxColor= _EN_DRAWBOX_COLOR_White ;
  VXIS_fSetOsdx2(0) ;

  VS4210_SetMutiWindows(&pVS4210_MutiWindows1) ;

  if (pVS4210_MutiWindows1.DrawOSDFlg != 0)
  {
        OSD_Show_Flg = 1 ;
        Tim1_count = 0 ;
        OSDStateFlg = 1 ;
        OSDSecCount = 0 ;
  }  

ExOsdContent[0] = 3*  ((EXTERNOSDNUMBER *2) +1) ;
memset(&ExOsdContent[1] ,_spc , (EXTERNOSDNUMBER+1) *2 ) ;
exosdLen = 4*EXTERNOSDNUMBER + 4  ;
exosdStart = (EXTERNOSDNUMBER +1) *2 +1 ;
j = 1 ;
for (i = exosdStart ; i <= exosdLen ;i = i+2 )
{
	ExOsdContent[i] = 0x7F + j ;
	ExOsdContent[i+1] = _spc ;
	j++ ;
}
memset(&ExOsdContent[4*EXTERNOSDNUMBER +3] ,_spc ,EXTERNOSDNUMBER *2 +1   ) ;
  exosdLen = EXTERNOSDNUMBER *2  ;

Audio_Sel(HDMI_AUDIO_1) ;
//test@end

/*
//xxxxx 2013-0801
IT6602HPDCtrl(1,0);	// HDMI port , set HPD = 0
//xxxxx

    delay1ms(1000);	//for power sequence

IT6602_fsm_init();
*/


#if 0
//PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);

//load eeprom data
//LoadEEPROM();

//RX
  ManVidRes = RES1920X1080 ; //0x01
//Init_TP280x_RegSet();

//TX
  //VS9988_Set_Format(gbVXIS_InputMode, gbVXIS_OuputModeflg, gbVXIS_OuputChannel  )	;
  //gbVXIS_InputMode = _OSD_NO_SINGLE ;
  //VS9988_BlueScreen() ;
  //VXOSD_SetOnOff(0) ;

  //HDMI_Tx_Init() ;

  //VS4210 SW Reset@start
    VXISI2CWrite( 0x01 , 0x5A); //Reset


  //VS4210 SW Reset@end
  //VS4210_SwitchChip(0x40, 0 ) ;c8 cc8
  pVS4210_JointKind1.AudioChannel = HDMI_AUDIO_NONE ;
  pVS4210_JointKind1.DrawBoxFlg =1 ;
  pVS4210_JointKind1.DrawOSDFlg =0x03 ; //bits 0 ,1
  //pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w0 ;
 // pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE1 ;
pVS4210_JointKind1.OutputMode =VS4210_1080P_MODE0_w0;/* VS4210_1080P_MODE1 ;*/
  pVS4210_JointKind1.OutputSize = _OUTPUT_1080P60 ;///*_OUTPUT_800x480;*/

//LCDPowerON();

//VS4210_Rx_NoSignal(3) ;

 setloopoutmode(0) ;
// SetNowMode();	

    VS4210_Rx_NoSignal(0) ;
  VS4210_Rx_NoSignal(1) ;
  VS4210_Rx_NoSignal(2) ;
  VS4210_Rx_NoSignal(3) ;
  
  #endif
// LCDPowerON();
// SetNowMode();  
  //void VS4210_Rx_NoSignal(tByte ch )

  HAL_TIM_Base_Start_IT(&htim3) ;
  /* USER CODE END 2 */
 
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

//SetNowMode();  

 // 	LoadVideoData();

//	  AudioMute(OFF);
	//  LCDPowerON();

//	SET_VX8812_SD_Video_Out(_NTSC);
//	SET_VX8812_SD_Video_Out(_PAL);

//  Printf("\r\nMain Loop...");
  printf("\r\nMain Loop...");

  //VXISI2CWrite(0x08,0x21);
  //VXISI2CWrite(0x0A,0x21);
  //VXISI2CWrite(0x74,0x02);
  //VXISI2CWrite(0x82,0x40);
//  VS4210_FULL88init2(VS4210_hdmi1080P_1080P_mode0_w0_88param);

  while (1)
  {
    /* USER CODE END WHILE */

//	if(PowerFlag==ON)///power offf
	 main_loop();
	
//	WaitPowerOn();

	//user_pwm_setvalue(500);

    /* USER CODE BEGIN 3 */




  }
  /* USER CODE END 3 */
}

void main_loop(void)
{

//BYTE val;
//USHORT loopinterval;

//VXISI2CWrite(0xf0,0xb5);

while(1)
{

#if (_DEBUG_MESSAGE_Monitor==ON)
	if(AccessEnable==1)
	#endif
{
	HDMI_VideoModeDetect() ;
	HDMI_AudioModeDetect() ;

	TxCount++ ;
	if (TxCount > 10 )
	{
		TxCount = 0 ;
		HDMITX_DevLoopProc() ;
	}
	
//	GetRS232_CMD();
	//getmcukeystate() ;
	/*
	if (IR_END_Flg ==1 )
	{
		IR_END_Flg = 0 ;
		if (u8IRcode == u8IRCheckcode )
		{
			dbg_Printf("Vendor=0x%04x,IR=0x%02x\r\n",(int) u16Vendor,(int) u8IRcode) ;
			if (u16Vendor == IR_Vendor_Code_)
			{
				ir_process(u8IRcode) ;
				u16Vendor =0 ;
			}
		}
	}
	*/

	msleep(10) ;

	   gMode0LoopCount++ ;
	   if (gMode0LoopCount > 100 )
	   {
		   gMode0LoopCount = 0 ;
		   OSDSecCount = OSDSecCount +1 ;
	   }
	
	   if (OSDPreSecCount != OSDSecCount)
	   {
		   OSDPreSecCount = OSDSecCount ;
		   if (gMode0LoopFlg == 1)
		   {
			   gMode0LoopSecCount ++ ;
			   if (gMode0LoopSecCount > gMode0LoopSecCmpVal)
			   {
				   gMode0LoopSecCount = 0 ;
				   pVS4210_MutiWindows1.OutputMode = VS4210_FULL_MODE0 ;
				   if (pVS4210_MutiWindows1.WindowMap.WMap0 < 4)
				   {
					   pVS4210_MutiWindows1.WindowMap.WMap0 = pVS4210_MutiWindows1.WindowMap.WMap0 +1 ;
				   }
				   else {
					   pVS4210_MutiWindows1.WindowMap.WMap0 = 1 ;
				   }
				   dbg_Printf("LOOP CH=%d,SMP Sec=%d\r\n",pVS4210_MutiWindows1.WindowMap.WMap0, gMode0LoopSecCmpVal )  ;			 
				   VS4210_SetMutiWindows(&pVS4210_MutiWindows1) ;
				   OSDSecCount = 0 ;
			   }
		   }
	
		   if (gMode0LoopFlg == 1)
		   {
		    dbg_Printf("Loop CH=%d,SecCount=%d\r\n",pVS4210_MutiWindows1.WindowMap.WMap0, OSDSecCount)  ;
		   }
		   else
		   {
		  //dbg_Printf("OSDSecCount=%d\r\n", OSDSecCount)  ;
		   }
	
		   if (OSDStateFlg == 1 )
		   {
			   if (OSDSecCount >= OSD_OFF_SEC )
			   {
				   OSDSecCount = 0 ;
				   OSDStateFlg = 0 ;
				   OSD_Clear_All();
				   ////VXIS_fSetContentOsdOnOff(0) ;
			   }
		   }

		if(VXISI2CRead(0x32)==0 )///CONTRAST
			VXISI2CWrite(0x32,0x80);
		   
	   }


	EQ_Detect();
	
#if (_Source_Switch==ON)

	#if (_Only_AHD_Mode==ON)
		if((SelectInputMode!=1))
		{
			msleep(50) ;

			if((SelectInputMode!=1))
				{
				SelectInputMode=1;

					HS_Select_Win(1);
				}
		}	

	#else
	CheckSelectInputMode=HDMI_SELECT_GET();
		if((CheckSelectInputMode!=SelectInputMode)/*&&(BOOT_ISSUE==0)*/)
		{
			msleep(50) ;
			CheckSelectInputMode=HDMI_SELECT_GET();

			if((CheckSelectInputMode!=SelectInputMode)/*&&(BOOT_ISSUE==0)*/)
				{
				SelectInputMode=CheckSelectInputMode;

			//	HS_Select_Win(HDMI_SELECT_GET());

				if(SelectInputMode==1)
					HS_Select_Win(1);
				else
				HS_Select_Win(0);
				}
		}	

		
	#endif

#endif
}

	#if (_DEBUG_MESSAGE_Monitor==ON)
	do {	//20201211-04
	while( RS_ready())	Monitor();	
	}while(AccessEnable == 0);
	#endif

	     TOAReceivePaser();
		
	      SysTimerHandler();

		Get_2824cIrq();//check tp2824 COC



#if 0
		  loopinterval += getloopTicCount();
		  
					  ///////////////////////////
					  // sys main flow
					  ///////////////////////////
					  if( loopinterval >=MS_LOOP) //execute once each 50m second
					  {
				    //printf("loopinterval\n");
		  
					   loopinterval =0;
				//	 IT6602_fsm();
					 }
#endif	
	/*
		CheckKeyIn();

	CheckTouchP();
	
	  EQ_Detect();	 

	if( GT9147_Scan(1)==_TOUCH_COUNT_4 ) break;		
	
		  val=CheckTrigger();
		  TriggerHandler(val);

	if( !CheckKeyIn() ) break;			// POWEROFF
	*/
	
	}

}

/* static*/ void VS4210_FULL88init2( tcByte *Valn )
 {
	 tByte i ;
	 tByte addr ;
	 tByte i2cVal ;
	 tcByte *Addrn ; 
	 Addrn = &VS4210_Addr_88param[0] ; 
 
	 for (i=0 ; i< VS4210_INIT_NUMBER ; i++)
	 {
		 addr	= Addrn[i] ;
		 i2cVal = Valn[i];
		 VXISI2CWrite( addr,i2cVal);
	 }
 }

void TP_bt1120_1080P_1080P_mode0_w0(void)
{
	Printf("\r\nLoading VS4210 AHD table 1080P... ");
#if 1

	VS4210_FULL88init2(VS4210_TP_bt1120_1080P_1080P_mode0_w0_88param);
#else

VXISI2CWrite(0x00,0x42); 
VXISI2CWrite(0x01,0x00); 
VXISI2CWrite(0x02,0x00); 
VXISI2CWrite(0x03,0x75); 
VXISI2CWrite(0x04,0x18); 
VXISI2CWrite(0x05,0x80); 
VXISI2CWrite(0x06,0x20); 
VXISI2CWrite(0x07,0x0C); 
VXISI2CWrite(0x08,0x40); 
VXISI2CWrite(0x09,0x0A); 
VXISI2CWrite(0x0A,0x40); 
VXISI2CWrite(0x0B,0x0A); 
VXISI2CWrite(0x0C,0x00); 
VXISI2CWrite(0x0D,0x20); 
VXISI2CWrite(0x0E,0x10); 
VXISI2CWrite(0x0F,0x00); 
VXISI2CWrite(0x10,0x81); 
VXISI2CWrite(0x11,0x82); 
VXISI2CWrite(0x12,0x74); 
VXISI2CWrite(0x13,0x80); 
VXISI2CWrite(0x14,0x38); 
VXISI2CWrite(0x15,0x78); 
VXISI2CWrite(0x16,0xFE); 
VXISI2CWrite(0x17,0x00); 
VXISI2CWrite(0x18,0xC0); 
VXISI2CWrite(0x19,0x01); 
VXISI2CWrite(0x1A,0x20); 
VXISI2CWrite(0x1B,0x30); 
VXISI2CWrite(0x1C,0x00); 
VXISI2CWrite(0x1D,0x00); 
VXISI2CWrite(0x1E,0x20); 
VXISI2CWrite(0x1F,0xA4); 
VXISI2CWrite(0x20,0x50); 
VXISI2CWrite(0x21,0xE2); 
VXISI2CWrite(0x22,0x87); 
VXISI2CWrite(0x23,0x98); 
VXISI2CWrite(0x24,0x80); 
VXISI2CWrite(0x25,0x44); 
VXISI2CWrite(0x26,0x65); 
VXISI2CWrite(0x27,0x38); 
VXISI2CWrite(0x28,0xF0); 
VXISI2CWrite(0x29,0x00); 
VXISI2CWrite(0x2A,0x00); 
VXISI2CWrite(0x2B,0x11); 
VXISI2CWrite(0x2C,0x00); 
VXISI2CWrite(0x2D,0x00); 
VXISI2CWrite(0x2E,0x47); 
VXISI2CWrite(0x2F,0x80); 
VXISI2CWrite(0x30,0x38); 
VXISI2CWrite(0x31,0x80); 
VXISI2CWrite(0x32,0x80); 
VXISI2CWrite(0x33,0x80); 
VXISI2CWrite(0x34,0xA0); 
VXISI2CWrite(0x35,0x18); 
VXISI2CWrite(0x36,0x46); 
VXISI2CWrite(0x37,0x64); 
VXISI2CWrite(0x38,0x00); 
VXISI2CWrite(0x39,0xFF); 
VXISI2CWrite(0x3A,0x00); 
VXISI2CWrite(0x3B,0x80); 
VXISI2CWrite(0x3C,0x80); 
VXISI2CWrite(0x3D,0x80); 
VXISI2CWrite(0x3E,0x80); 
VXISI2CWrite(0x3F,0x80); 
//VXISI2CWrite(0xVXISI2C)
VXISI2CWrite(0x41,0x60); 
VXISI2CWrite(0x42,0x5F); 
VXISI2CWrite(0x43,0x37); 
VXISI2CWrite(0x44,0x14); 
VXISI2CWrite(0x45,0x00); 
VXISI2CWrite(0x46,0x00); 
VXISI2CWrite(0x47,0x00); 
VXISI2CWrite(0x48,0x00); 
VXISI2CWrite(0x49,0x00); 
VXISI2CWrite(0x4A,0x00); 
VXISI2CWrite(0x4B,0x20); 
VXISI2CWrite(0x4C,0x00); 
VXISI2CWrite(0x4D,0x00); 
VXISI2CWrite(0x4E,0x00); 
VXISI2CWrite(0x4F,0x7F); 
VXISI2CWrite(0x50,0x20); 
VXISI2CWrite(0x51,0x05); 
VXISI2CWrite(0x52,0x00); 
VXISI2CWrite(0x53,0x06); 
VXISI2CWrite(0x54,0x00); 
VXISI2CWrite(0x55,0xB4); 
VXISI2CWrite(0x56,0x3D); 
VXISI2CWrite(0x57,0x04); 
VXISI2CWrite(0x58,0x00); 
VXISI2CWrite(0x59,0x08); 
VXISI2CWrite(0x5A,0x10); 
VXISI2CWrite(0x5B,0x00); 
VXISI2CWrite(0x5C,0x68); 
VXISI2CWrite(0x5D,0x40); 
VXISI2CWrite(0x5E,0x0A); 
VXISI2CWrite(0x5F,0x00); 
VXISI2CWrite(0x60,0xB4); 
VXISI2CWrite(0x61,0x51); 
VXISI2CWrite(0x62,0x33); 
VXISI2CWrite(0x63,0x01); 
VXISI2CWrite(0x64,0xFF); 
VXISI2CWrite(0x65,0x01); 
VXISI2CWrite(0x66,0xFF); 
VXISI2CWrite(0x67,0x89); 
VXISI2CWrite(0x68,0xAB); 
VXISI2CWrite(0x69,0x88); 
VXISI2CWrite(0x6A,0x6E); 
VXISI2CWrite(0x6B,0x6E); 
VXISI2CWrite(0x6C,0x6E); 
VXISI2CWrite(0x6D,0x00); 
VXISI2CWrite(0x6E,0x00); 
VXISI2CWrite(0x6F,0x00); 
VXISI2CWrite(0x70,0x00); 
VXISI2CWrite(0x71,0x00); 
VXISI2CWrite(0x72,0x00); 
VXISI2CWrite(0x73,0x00); 
VXISI2CWrite(0x74,0x06); 
VXISI2CWrite(0x75,0xFF); 
VXISI2CWrite(0x76,0xFF); 
VXISI2CWrite(0x77,0xFF); 
VXISI2CWrite(0x78,0xFF); 
VXISI2CWrite(0x79,0xFF); 
VXISI2CWrite(0x7A,0xFF); 
VXISI2CWrite(0x7B,0xFF); 
VXISI2CWrite(0x7C,0xFF); 
VXISI2CWrite(0x7D,0xF0); 
VXISI2CWrite(0x7E,0x80); 
VXISI2CWrite(0x7F,0x00); 
VXISI2CWrite(0x80,0x01); 
VXISI2CWrite(0x81,0x15); 
VXISI2CWrite(0x82,0x40); 
VXISI2CWrite(0x83,0x2D); 
VXISI2CWrite(0x84,0x57); 
VXISI2CWrite(0x85,0xFD); 
VXISI2CWrite(0x86,0x00); 
VXISI2CWrite(0x87,0x00); 
VXISI2CWrite(0x88,0x00); 
VXISI2CWrite(0x89,0x00); 
VXISI2CWrite(0x8A,0x00); 
VXISI2CWrite(0x8B,0x7E); 
VXISI2CWrite(0x8C,0x03); 
VXISI2CWrite(0x8D,0x17); 
VXISI2CWrite(0x8E,0x8C); 
VXISI2CWrite(0x8F,0x00); 
VXISI2CWrite(0x90,0xCC); 
VXISI2CWrite(0x91,0x41); 
VXISI2CWrite(0x92,0xC4); 
VXISI2CWrite(0x93,0x84); 
VXISI2CWrite(0x94,0x98); 
VXISI2CWrite(0x95,0x80); 
VXISI2CWrite(0x96,0x65); 
VXISI2CWrite(0x97,0x70); 
VXISI2CWrite(0x98,0x29); 
VXISI2CWrite(0x99,0x90); 
VXISI2CWrite(0x9A,0x80); 
VXISI2CWrite(0x9B,0x04); 
VXISI2CWrite(0x9C,0x00); 
VXISI2CWrite(0x9D,0x00); 
VXISI2CWrite(0x9E,0x04); 
VXISI2CWrite(0x9F,0x65); 
VXISI2CWrite(0xA0,0x80); 
VXISI2CWrite(0xA1,0xD8); 
VXISI2CWrite(0xA2,0x98); 
VXISI2CWrite(0xA3,0x87); 
VXISI2CWrite(0xA4,0x00); 
VXISI2CWrite(0xA5,0x80); 
VXISI2CWrite(0xA6,0x00); 
VXISI2CWrite(0xA7,0x70); 
VXISI2CWrite(0xA8,0x40); 
VXISI2CWrite(0xA9,0x1E); 
VXISI2CWrite(0xAA,0x2C); 
VXISI2CWrite(0xAB,0x00); 
VXISI2CWrite(0xAC,0x90); 
VXISI2CWrite(0xAD,0x65); 
VXISI2CWrite(0xAE,0x65); 
VXISI2CWrite(0xAF,0x99); 
VXISI2CWrite(0xB0,0x00); 
VXISI2CWrite(0xB1,0x00); 
VXISI2CWrite(0xB2,0x00); 
VXISI2CWrite(0xB3,0x00); 
VXISI2CWrite(0xB4,0x00); 
VXISI2CWrite(0xB5,0x00); 
VXISI2CWrite(0xB6,0x00); 
VXISI2CWrite(0xB7,0x00); 
VXISI2CWrite(0xB8,0x80); 
VXISI2CWrite(0xB9,0x00); 
VXISI2CWrite(0xBA,0x01); 
VXISI2CWrite(0xBB,0x00); 
VXISI2CWrite(0xBC,0x90); 
VXISI2CWrite(0xBD,0x01); 
VXISI2CWrite(0xBE,0x2C); 
VXISI2CWrite(0xBF,0x00); 
VXISI2CWrite(0xC0,0x00); 
VXISI2CWrite(0xC1,0x20); 
VXISI2CWrite(0xC2,0x20); 
VXISI2CWrite(0xC3,0x20); 
VXISI2CWrite(0xC4,0x20); 
VXISI2CWrite(0xC5,0x20); 
VXISI2CWrite(0xC6,0x00); 
VXISI2CWrite(0xC7,0x00); 
VXISI2CWrite(0xC8,0x00); 
VXISI2CWrite(0xC9,0x00); 
VXISI2CWrite(0xCA,0x00); 
VXISI2CWrite(0xCB,0x00); 
VXISI2CWrite(0xCC,0x00); 
VXISI2CWrite(0xCD,0x00); 
VXISI2CWrite(0xCE,0x00); 
VXISI2CWrite(0xCF,0x00); 
VXISI2CWrite(0xD0,0xFF); 
VXISI2CWrite(0xD1,0x3F); 
VXISI2CWrite(0xD2,0xC2); 
VXISI2CWrite(0xD3,0x3F); 
VXISI2CWrite(0xD4,0x00); 
VXISI2CWrite(0xD5,0x00); 
VXISI2CWrite(0xD6,0x00); 
VXISI2CWrite(0xD7,0x0F); 
VXISI2CWrite(0xD8,0x00); 
VXISI2CWrite(0xD9,0x00); 
VXISI2CWrite(0xDA,0x00); 
VXISI2CWrite(0xDB,0x00); 
VXISI2CWrite(0xDC,0x77); 
VXISI2CWrite(0xDD,0x01); 
VXISI2CWrite(0xDE,0x5F); 
VXISI2CWrite(0xDF,0x96); 
VXISI2CWrite(0xE0,0x00); 
VXISI2CWrite(0xE1,0x00); 
VXISI2CWrite(0xE2,0x00); 
VXISI2CWrite(0xE3,0x47); 
VXISI2CWrite(0xE4,0x3C); 
VXISI2CWrite(0xE5,0x80); 
VXISI2CWrite(0xE6,0x00); 
VXISI2CWrite(0xE7,0x00); 
VXISI2CWrite(0xE8,0x00); 
VXISI2CWrite(0xE9,0x00); 
VXISI2CWrite(0xEA,0x00); 
VXISI2CWrite(0xEB,0x00); 
VXISI2CWrite(0xEC,0x88); 
VXISI2CWrite(0xED,0x00); 
VXISI2CWrite(0xEE,0x00); 
VXISI2CWrite(0xEF,0x00); 
VXISI2CWrite(0xF0,0x30); 
VXISI2CWrite(0xF1,0x43); 
VXISI2CWrite(0xF2,0x1F); 
VXISI2CWrite(0xF3,0x1F); 
VXISI2CWrite(0xF4,0x22); 
VXISI2CWrite(0xF5,0x73); 
VXISI2CWrite(0xF6,0x53); 
VXISI2CWrite(0xF7,0x80); 
VXISI2CWrite(0xF8,0x01); 
VXISI2CWrite(0xF9,0x01); 
VXISI2CWrite(0xFA,0x00); 
VXISI2CWrite(0xFB,0x00); 
VXISI2CWrite(0xFC,0x00); 
VXISI2CWrite(0xFD,0x70); 
VXISI2CWrite(0xFE,0x18); 
VXISI2CWrite(0xFF,0x07); 


/*
VXISI2CWrite(0x01,0x00);
VXISI2CWrite(0x02,0x00);
VXISI2CWrite(0x03,0x75);
VXISI2CWrite(0x04,0x18);
VXISI2CWrite(0x05,0x80);
VXISI2CWrite(0x06,0x20);
VXISI2CWrite(0x07,0x0C);
VXISI2CWrite(0x08,0x21);
VXISI2CWrite(0x09,0x0A);
VXISI2CWrite(0x0A,0x21);
VXISI2CWrite(0x0B,0x0A);
VXISI2CWrite(0x0C,0x00);
VXISI2CWrite(0x0D,0x20);
VXISI2CWrite(0x0E,0x10);
VXISI2CWrite(0x0F,0x00);
VXISI2CWrite(0x10,0x81);
VXISI2CWrite(0x11,0x82);
VXISI2CWrite(0x12,0x00);
VXISI2CWrite(0x13,0x00);
VXISI2CWrite(0x14,0x00);
VXISI2CWrite(0x15,0x78);
VXISI2CWrite(0x16,0xFE);
VXISI2CWrite(0x17,0x00);
VXISI2CWrite(0x18,0xC0);
VXISI2CWrite(0x19,0x01);
VXISI2CWrite(0x1A,0x36);
VXISI2CWrite(0x1B,0x30);
VXISI2CWrite(0x1C,0x00);
VXISI2CWrite(0x1D,0x00);
VXISI2CWrite(0x1E,0x20);
VXISI2CWrite(0x1F,0x00);
VXISI2CWrite(0x20,0x00);
VXISI2CWrite(0x21,0x00);
VXISI2CWrite(0x22,0xA7);
VXISI2CWrite(0x23,0x50);
VXISI2CWrite(0x24,0x80);
VXISI2CWrite(0x25,0x44);
VXISI2CWrite(0x26,0x65);
VXISI2CWrite(0x27,0x38);
VXISI2CWrite(0x28,0xF0);
VXISI2CWrite(0x29,0x00);
VXISI2CWrite(0x2A,0x00);
VXISI2CWrite(0x2B,0x11);
VXISI2CWrite(0x2C,0x00);
VXISI2CWrite(0x2D,0x00);
VXISI2CWrite(0x2E,0x47);
VXISI2CWrite(0x2F,0x80);
VXISI2CWrite(0x30,0x38);
VXISI2CWrite(0x31,0x80);
//VXISI2CWrite(0x32,0x80);
VXISI2CWrite(0x33,0x80);
VXISI2CWrite(0x34,0xA0);
VXISI2CWrite(0x35,0x18);
VXISI2CWrite(0x36,0x46);
VXISI2CWrite(0x37,0x64);
VXISI2CWrite(0x38,0x00);
VXISI2CWrite(0x39,0xFF);
VXISI2CWrite(0x3A,0x1B);
VXISI2CWrite(0x3B,0x80);
VXISI2CWrite(0x3C,0x80);
VXISI2CWrite(0x3D,0x80);
VXISI2CWrite(0x3E,0x80);
VXISI2CWrite(0x3F,0x80);
VXISI2CWrite(0x40,0x80);
VXISI2CWrite(0x41,0x60);
VXISI2CWrite(0x42,0x5F);
VXISI2CWrite(0x43,0x37);
VXISI2CWrite(0x44,0x14);
VXISI2CWrite(0x45,0x06);
VXISI2CWrite(0x46,0x00);
VXISI2CWrite(0x47,0x00);
VXISI2CWrite(0x48,0x00);
VXISI2CWrite(0x49,0x00);
VXISI2CWrite(0x4B,0x20);
VXISI2CWrite(0x4C,0x00);
VXISI2CWrite(0x4D,0x00);
VXISI2CWrite(0x4E,0x00);
VXISI2CWrite(0x4F,0x7F);
VXISI2CWrite(0x50,0x20);
VXISI2CWrite(0x51,0x20);
VXISI2CWrite(0x52,0x00);
VXISI2CWrite(0x53,0x04);
VXISI2CWrite(0x54,0x00);
VXISI2CWrite(0x55,0xA0);
VXISI2CWrite(0x56,0x40);
VXISI2CWrite(0x57,0x04);
VXISI2CWrite(0x58,0x10);
VXISI2CWrite(0x59,0x08);
VXISI2CWrite(0x5A,0x10);
VXISI2CWrite(0x5B,0x00);
VXISI2CWrite(0x5C,0x68);
VXISI2CWrite(0x5D,0x84);
VXISI2CWrite(0x5E,0x08);
VXISI2CWrite(0x5F,0x21);
VXISI2CWrite(0x60,0x20);
VXISI2CWrite(0x61,0xB0);
VXISI2CWrite(0x62,0x33);
VXISI2CWrite(0x63,0x01);
VXISI2CWrite(0x64,0xFF);
VXISI2CWrite(0x65,0x01);
VXISI2CWrite(0x66,0xFF);
VXISI2CWrite(0x67,0x89);
VXISI2CWrite(0x68,0xAB);
VXISI2CWrite(0x69,0x28);
VXISI2CWrite(0x6A,0x6E);
VXISI2CWrite(0x6B,0x6E);
VXISI2CWrite(0x6C,0x6E);
VXISI2CWrite(0x6D,0x00);
VXISI2CWrite(0x6E,0x00);
VXISI2CWrite(0x6F,0x00);
VXISI2CWrite(0x70,0x00);
VXISI2CWrite(0x71,0x00);
VXISI2CWrite(0x72,0x00);
VXISI2CWrite(0x73,0x00);
VXISI2CWrite(0x74,0x00);
VXISI2CWrite(0x75,0xFF);
VXISI2CWrite(0x76,0xFF);
VXISI2CWrite(0x77,0xFF);
VXISI2CWrite(0x78,0xFF);
VXISI2CWrite(0x79,0xFF);
VXISI2CWrite(0x7A,0xFF);
VXISI2CWrite(0x7B,0xFF);
VXISI2CWrite(0x7C,0xFF);
VXISI2CWrite(0x7D,0xF0);
VXISI2CWrite(0x7E,0x80);
VXISI2CWrite(0x7F,0x00);
VXISI2CWrite(0x80,0x00);
VXISI2CWrite(0x81,0x34);
VXISI2CWrite(0x82,0x40);
VXISI2CWrite(0x83,0x05);
VXISI2CWrite(0x84,0x57);
VXISI2CWrite(0x85,0xFD);
VXISI2CWrite(0x86,0x04);
VXISI2CWrite(0x87,0x24);
VXISI2CWrite(0x88,0x94);
VXISI2CWrite(0x89,0x58);
VXISI2CWrite(0x8A,0x00);
VXISI2CWrite(0x8B,0x7E);
VXISI2CWrite(0x8C,0x03);
VXISI2CWrite(0x8D,0x2C);
VXISI2CWrite(0x8E,0x8C);
VXISI2CWrite(0x8F,0x00);
VXISI2CWrite(0x90,0xCC);
VXISI2CWrite(0x91,0x41);
VXISI2CWrite(0x92,0xC4);
VXISI2CWrite(0x93,0x84);
VXISI2CWrite(0x94,0x98);
VXISI2CWrite(0x95,0x80);
VXISI2CWrite(0x96,0x65);
VXISI2CWrite(0x97,0x70);
VXISI2CWrite(0x98,0x29);
VXISI2CWrite(0x99,0x90);
VXISI2CWrite(0x9A,0x00);
VXISI2CWrite(0x9B,0x04);
//VXISI2CWrite(0x9C,0x0A);
VXISI2CWrite(0x9D,0x00);
VXISI2CWrite(0x9E,0x04);
VXISI2CWrite(0x9F,0x65);
VXISI2CWrite(0xA0,0x80);
VXISI2CWrite(0xA1,0xD8);
VXISI2CWrite(0xA2,0x98);
VXISI2CWrite(0xA3,0x87);
VXISI2CWrite(0xA4,0x00);
VXISI2CWrite(0xA5,0x80);
VXISI2CWrite(0xA6,0x00);
VXISI2CWrite(0xA7,0x70);
VXISI2CWrite(0xA8,0x40);
VXISI2CWrite(0xA9,0x1E);
VXISI2CWrite(0xAA,0x2C);
VXISI2CWrite(0xAB,0x00);
VXISI2CWrite(0xAC,0x90);
VXISI2CWrite(0xAD,0x65);
VXISI2CWrite(0xAE,0x65);
VXISI2CWrite(0xAF,0x99);
VXISI2CWrite(0xB0,0x00);
VXISI2CWrite(0xB1,0x00);
VXISI2CWrite(0xB2,0x00);
VXISI2CWrite(0xB3,0x00);
VXISI2CWrite(0xB4,0x00);
VXISI2CWrite(0xB5,0x00);
VXISI2CWrite(0xB6,0x00);
VXISI2CWrite(0xB8,0x00);
VXISI2CWrite(0xB9,0x00);
VXISI2CWrite(0xBA,0x00);
VXISI2CWrite(0xBB,0x00);
VXISI2CWrite(0xBC,0x90);
VXISI2CWrite(0xBD,0x01);
VXISI2CWrite(0xBE,0xA5);
VXISI2CWrite(0xC0,0x00);
VXISI2CWrite(0xC1,0x20);
VXISI2CWrite(0xC2,0x20);
VXISI2CWrite(0xC3,0x20);
VXISI2CWrite(0xC4,0x20);
VXISI2CWrite(0xC5,0x20);
VXISI2CWrite(0xD0,0xFF);
VXISI2CWrite(0xD1,0x3F);
VXISI2CWrite(0xD2,0xC2);
VXISI2CWrite(0xD3,0x3F);
VXISI2CWrite(0xD4,0x00);
VXISI2CWrite(0xD5,0x00);
VXISI2CWrite(0xD6,0x00);
VXISI2CWrite(0xD7,0x00);
VXISI2CWrite(0xD8,0x00);
VXISI2CWrite(0xDC,0x77);
VXISI2CWrite(0xDD,0x01);
VXISI2CWrite(0xDE,0xA5);
VXISI2CWrite(0xDF,0xE5);
VXISI2CWrite(0xE0,0x00);
VXISI2CWrite(0xE1,0x00);
VXISI2CWrite(0xE2,0x00);
VXISI2CWrite(0xE3,0x47);
VXISI2CWrite(0xE4,0x3C);
VXISI2CWrite(0xE5,0x80);
VXISI2CWrite(0xE6,0x00);
VXISI2CWrite(0xE7,0x00);
VXISI2CWrite(0xE8,0x00);
VXISI2CWrite(0xE9,0x00);
VXISI2CWrite(0xEA,0x00);
VXISI2CWrite(0xEB,0x00);
VXISI2CWrite(0xEC,0x88);
VXISI2CWrite(0xED,0x00);
VXISI2CWrite(0xEE,0x00);
VXISI2CWrite(0xEF,0x00);
VXISI2CWrite(0xF0,0x35);
VXISI2CWrite(0xF1,0x43);
VXISI2CWrite(0xF2,0x1F);
VXISI2CWrite(0xF3,0x1F);
VXISI2CWrite(0xF4,0x22);
VXISI2CWrite(0xF5,0x73);
VXISI2CWrite(0xF6,0x53);
VXISI2CWrite(0xF7,0x80);
VXISI2CWrite(0xF8,0x01);
VXISI2CWrite(0xF9,0x01);
VXISI2CWrite(0xFA,0x00);
VXISI2CWrite(0xFB,0x00);
VXISI2CWrite(0xFC,0x00);
VXISI2CWrite(0xFD,0x00);
VXISI2CWrite(0xFE,0x00);
VXISI2CWrite(0xFF,0x00);
*/
#endif

}

void TP_bt1120_720P_1080P_mode0_w0(void)
{
	Printf("\r\nLoading VS4210 AHD table 720P... ");
	VS4210_FULL88init2(VS4210_TP_bt1120_720P_1080P_mode0_w0_88param);
	VXISI2CWrite(0xD7,0x10);

}

void TP_bt1120_NTSC_1080P_mode0_w0(void)
{
	Printf("\r\nLoading VS4210 AHD table NTSC... ");
	VS4210_FULL88init2(VS4210_TP_bt1120_NTSC_1080P_mode0_w0_88param);
}

void TP_bt1120_PAL_1080P_mode0_w0(void)
{
	Printf("\r\nLoading VS4210 AHD table PAL... ");
	VS4210_FULL88init2(VS4210_TP_bt1120_PAL_1080P_mode0_w0_88param);
	#if 0
	VXISI2CWrite(0x01,0x00);	
	VXISI2CWrite(0x02,0x00);	
	VXISI2CWrite(0x03,0x75);	
	VXISI2CWrite(0x04,0x18);	
	VXISI2CWrite(0x05,0x80);	
	VXISI2CWrite(0x06,0x20);	
	VXISI2CWrite(0x07,0x08);	
	VXISI2CWrite(0x08,0x40);	
	VXISI2CWrite(0x09,0x0A);	
	VXISI2CWrite(0x0A,0x40);	
	VXISI2CWrite(0x0B,0x09);	
	VXISI2CWrite(0x0C,0x00);	
	VXISI2CWrite(0x0D,0x20);	
	VXISI2CWrite(0x0E,0x10);	
	VXISI2CWrite(0x0F,0x00);	
	VXISI2CWrite(0x10,0xA1);	
	VXISI2CWrite(0x11,0x82);	
	VXISI2CWrite(0x12,0x21);	
	VXISI2CWrite(0x13,0xD0);	
	VXISI2CWrite(0x14,0x20);	
	VXISI2CWrite(0x15,0x78);	
	VXISI2CWrite(0x16,0xFF);	
	VXISI2CWrite(0x17,0x01);	
	VXISI2CWrite(0x18,0xC0);	
	VXISI2CWrite(0x19,0x01);	
	VXISI2CWrite(0x1A,0x26);	
	VXISI2CWrite(0x1B,0x14);	
	VXISI2CWrite(0x1C,0x00);	
	VXISI2CWrite(0x1D,0x00);	
	VXISI2CWrite(0x1E,0x20);	
	VXISI2CWrite(0x1F,0x31);	
	VXISI2CWrite(0x20,0x60);	
	VXISI2CWrite(0x21,0x39);	
	VXISI2CWrite(0x22,0x97);	
	VXISI2CWrite(0x23,0x48);	
	VXISI2CWrite(0x24,0xBC);	
	VXISI2CWrite(0x25,0x11);	
	VXISI2CWrite(0x26,0x38);	
	VXISI2CWrite(0x27,0x20);	
	VXISI2CWrite(0x28,0xF0);	
	VXISI2CWrite(0x29,0x40);	
	VXISI2CWrite(0x2A,0x00);	
	VXISI2CWrite(0x2B,0x00);	
	VXISI2CWrite(0x2C,0xFF);	
	VXISI2CWrite(0x2D,0x00);	
	VXISI2CWrite(0x2E,0x17);	
	VXISI2CWrite(0x2F,0xBC);	
	VXISI2CWrite(0x30,0x20);	
	VXISI2CWrite(0x31,0x80);	
	VXISI2CWrite(0x32,0x80);	
	VXISI2CWrite(0x33,0x80);	
	VXISI2CWrite(0x34,0xA0);	
	VXISI2CWrite(0x35,0x18);	
	VXISI2CWrite(0x36,0x46);	
	VXISI2CWrite(0x37,0x64);	
	VXISI2CWrite(0x38,0x00);	
	VXISI2CWrite(0x39,0xFF);	
	VXISI2CWrite(0x3A,0x15);	
	VXISI2CWrite(0x3B,0x80);	
	VXISI2CWrite(0x3C,0x80);	
	VXISI2CWrite(0x3D,0x80);	
	VXISI2CWrite(0x3E,0x80);	
	VXISI2CWrite(0x3F,0x80);	
	//VXISI2CWrite(0xVXI);SI2CWr
	VXISI2CWrite(0x41,0x60);	
	VXISI2CWrite(0x42,0x5F);	
	VXISI2CWrite(0x43,0x37);	
	VXISI2CWrite(0x44,0x14);	
	VXISI2CWrite(0x45,0x05);	
	VXISI2CWrite(0x46,0x00);	
	VXISI2CWrite(0x47,0x00);	
	VXISI2CWrite(0x48,0x00);	
	VXISI2CWrite(0x49,0x00);	
	VXISI2CWrite(0x4B,0x20);	
	VXISI2CWrite(0x4C,0x00);	
	VXISI2CWrite(0x4D,0x00);	
	VXISI2CWrite(0x4E,0x00);	
	VXISI2CWrite(0x4F,0x7F);	
	VXISI2CWrite(0x50,0x20);	
	VXISI2CWrite(0x51,0x00);	
	VXISI2CWrite(0x52,0x00);	
	VXISI2CWrite(0x53,0x04);	
	VXISI2CWrite(0x54,0x00);	
	VXISI2CWrite(0x55,0xE0);	
	VXISI2CWrite(0x56,0x40);	
	VXISI2CWrite(0x57,0x04);	
	VXISI2CWrite(0x58,0x00);	
	VXISI2CWrite(0x59,0x08);	
	VXISI2CWrite(0x5A,0x10);	
	VXISI2CWrite(0x5B,0x00);	
	VXISI2CWrite(0x5C,0x68);	
	VXISI2CWrite(0x5D,0x40);	
	VXISI2CWrite(0x5E,0x08);	
	VXISI2CWrite(0x5F,0x00);	
	VXISI2CWrite(0x60,0xE0);	
	VXISI2CWrite(0x61,0x54);	
	VXISI2CWrite(0x62,0x33);	
	VXISI2CWrite(0x63,0x01);	
	VXISI2CWrite(0x64,0xFF);	
	VXISI2CWrite(0x65,0x01);	
	VXISI2CWrite(0x66,0xFF);	
	VXISI2CWrite(0x67,0x89);	
	VXISI2CWrite(0x68,0xAB);	
	VXISI2CWrite(0x69,0x88);	
	VXISI2CWrite(0x6A,0x6E);	
	VXISI2CWrite(0x6B,0x6E);	
	VXISI2CWrite(0x6C,0x6E);	
	VXISI2CWrite(0x6D,0x00);	
	VXISI2CWrite(0x6E,0x00);	
	VXISI2CWrite(0x6F,0x00);	
	VXISI2CWrite(0x70,0x00);	
	VXISI2CWrite(0x71,0x00);	
	VXISI2CWrite(0x72,0x00);	
	VXISI2CWrite(0x73,0x00);	
	VXISI2CWrite(0x74,0x06);	
	VXISI2CWrite(0x75,0xFF);	
	VXISI2CWrite(0x76,0xFF);	
	VXISI2CWrite(0x77,0xFF);	
	VXISI2CWrite(0x78,0xFF);	
	VXISI2CWrite(0x79,0xFF);	
	VXISI2CWrite(0x7A,0xFF);	
	VXISI2CWrite(0x7B,0xFF);	
	VXISI2CWrite(0x7C,0xFF);	
	VXISI2CWrite(0x7D,0xF0);	
	VXISI2CWrite(0x7E,0x80);	
	VXISI2CWrite(0x7F,0x00);	
	VXISI2CWrite(0x80,0x01);	
	VXISI2CWrite(0x81,0x34);	
	VXISI2CWrite(0x82,0x40);	
	VXISI2CWrite(0x83,0x2D);	
	VXISI2CWrite(0x84,0x6F);	
	VXISI2CWrite(0x85,0x00);	
	VXISI2CWrite(0x86,0x00);	
	VXISI2CWrite(0x87,0x00);	
	VXISI2CWrite(0x88,0x13);	
	VXISI2CWrite(0x89,0x1B);	
	VXISI2CWrite(0x8A,0x00);	
	VXISI2CWrite(0x8B,0x7E);	
	VXISI2CWrite(0x8C,0x03);	
	VXISI2CWrite(0x8D,0x17);	
	VXISI2CWrite(0x8E,0xB0);	
	VXISI2CWrite(0x8F,0x14);	
	VXISI2CWrite(0x90,0xCC);	
	VXISI2CWrite(0x91,0x41);	
	VXISI2CWrite(0x92,0xC4);	
	VXISI2CWrite(0x93,0x81);	
	VXISI2CWrite(0x94,0xC9);	
	VXISI2CWrite(0x95,0xBC);	
	VXISI2CWrite(0x96,0xF5);	
	VXISI2CWrite(0x97,0x70);	
	VXISI2CWrite(0x98,0x15);	
	VXISI2CWrite(0x99,0x90);	
	VXISI2CWrite(0x9A,0x80);	
	VXISI2CWrite(0x9B,0x04);	
	VXISI2CWrite(0x9C,0x00);	
	VXISI2CWrite(0x9D,0x00);	
	VXISI2CWrite(0x9E,0x04);	
	VXISI2CWrite(0x9F,0x65);	
	VXISI2CWrite(0xA0,0x3B);	
	VXISI2CWrite(0xA1,0xD0);	
	VXISI2CWrite(0xA2,0x98);	
	VXISI2CWrite(0xA3,0x87);	
	VXISI2CWrite(0xA4,0x20);	
	VXISI2CWrite(0xA5,0x38);	
	VXISI2CWrite(0xA6,0xFD);	
	VXISI2CWrite(0xA7,0x50);	
	VXISI2CWrite(0xA8,0x40);	
	VXISI2CWrite(0xA9,0x1E);	
	VXISI2CWrite(0xAA,0x2E);	
	VXISI2CWrite(0xAB,0x00);	
	VXISI2CWrite(0xAC,0x84);	
	VXISI2CWrite(0xAD,0xF5);	
	VXISI2CWrite(0xAE,0x65);	
	VXISI2CWrite(0xAF,0x9B);	
	VXISI2CWrite(0xB0,0x00);	
	VXISI2CWrite(0xB1,0x00);	
	VXISI2CWrite(0xB2,0x00);	
	VXISI2CWrite(0xB3,0x00);	
	VXISI2CWrite(0xB4,0x00);	
	VXISI2CWrite(0xB5,0x00);	
	VXISI2CWrite(0xB6,0x00);	
	VXISI2CWrite(0xB8,0x01);	
	VXISI2CWrite(0xB9,0x00);	
	VXISI2CWrite(0xBA,0x01);	
	VXISI2CWrite(0xBB,0x00);	
	VXISI2CWrite(0xBC,0x90);	
	VXISI2CWrite(0xBD,0x01);	
	VXISI2CWrite(0xBE,0x1D);	
	VXISI2CWrite(0xC0,0x00);	
	VXISI2CWrite(0xC1,0x20);	
	VXISI2CWrite(0xC2,0x20);	
	VXISI2CWrite(0xC3,0x20);	
	VXISI2CWrite(0xC4,0x20);	
	VXISI2CWrite(0xC5,0x20);	
	VXISI2CWrite(0xD0,0xFF);	
	VXISI2CWrite(0xD1,0x3F);	
	VXISI2CWrite(0xD2,0xC2);	
	VXISI2CWrite(0xD3,0x3F);	
	VXISI2CWrite(0xD4,0x00);	
	VXISI2CWrite(0xD5,0x00);	
	VXISI2CWrite(0xD6,0x00);	
	VXISI2CWrite(0xD7,0x10);	
	VXISI2CWrite(0xD8,0x00);	
	VXISI2CWrite(0xDC,0x77);	
	VXISI2CWrite(0xDD,0x00);	
	VXISI2CWrite(0xDE,0xD3);	
	VXISI2CWrite(0xDF,0x49);	
	VXISI2CWrite(0xE0,0x00);	
	VXISI2CWrite(0xE1,0x00);	
	VXISI2CWrite(0xE2,0x00);	
	VXISI2CWrite(0xE3,0x17);	
	VXISI2CWrite(0xE4,0xE0);	
	VXISI2CWrite(0xE5,0xBC);	
	VXISI2CWrite(0xE6,0x00);	
	VXISI2CWrite(0xE7,0x00);	
	VXISI2CWrite(0xE8,0x00);	
	VXISI2CWrite(0xE9,0x00);	
	VXISI2CWrite(0xEA,0x00);	
	VXISI2CWrite(0xEB,0x00);	
	VXISI2CWrite(0xEC,0x88);	
	VXISI2CWrite(0xED,0x00);	
	VXISI2CWrite(0xEE,0x00);	
	VXISI2CWrite(0xEF,0x00);	
	VXISI2CWrite(0xF0,0x30);	
	VXISI2CWrite(0xF1,0x43);	
	VXISI2CWrite(0xF2,0x1F);	
	VXISI2CWrite(0xF3,0x1F);	
	VXISI2CWrite(0xF4,0x22);	
	VXISI2CWrite(0xF5,0x73);	
	VXISI2CWrite(0xF6,0x53);	
	VXISI2CWrite(0xF7,0x80);	
	VXISI2CWrite(0xF8,0x01);	
	VXISI2CWrite(0xF9,0x01);	
	VXISI2CWrite(0xFA,0x00);	
	VXISI2CWrite(0xFB,0x00);	
	VXISI2CWrite(0xFC,0x00);	
	VXISI2CWrite(0xFD,0x70);	
	VXISI2CWrite(0xFE,0x40);	
	VXISI2CWrite(0xFF,0x07);	
#endif

}
void SET_VS4210_720_480_out(void)
{
	Printf("\r\nSET_VS4210_720_480_out ");

	VXISI2CWrite(0x01,0x00);
	VXISI2CWrite(0x02,0x00);
	VXISI2CWrite(0x03,0x75);
	VXISI2CWrite(0x04,0x18);
	VXISI2CWrite(0x05,0x80);
	VXISI2CWrite(0x06,0x20);
	VXISI2CWrite(0x07,0x08);
	VXISI2CWrite(0x08,0x40);
	VXISI2CWrite(0x09,0x0A);
	VXISI2CWrite(0x0A,0x41);
	VXISI2CWrite(0x0B,0x07);
	VXISI2CWrite(0x0C,0x00);
	VXISI2CWrite(0x0D,0x20);
	VXISI2CWrite(0x0E,0x10);
	VXISI2CWrite(0x0F,0x00);
	VXISI2CWrite(0x10,0x80);
	VXISI2CWrite(0x11,0x82);
	VXISI2CWrite(0x12,0x21);
	VXISI2CWrite(0x13,0xD0);
	VXISI2CWrite(0x14,0xE0);
	VXISI2CWrite(0x15,0x78);
	VXISI2CWrite(0x16,0xFE);
	VXISI2CWrite(0x17,0x00);
	VXISI2CWrite(0x18,0xC8);
	VXISI2CWrite(0x19,0x00);
	VXISI2CWrite(0x1A,0x23);
	VXISI2CWrite(0x1B,0x00);
	VXISI2CWrite(0x1C,0x00);
	VXISI2CWrite(0x1D,0x00);
	VXISI2CWrite(0x1E,0x20);
	VXISI2CWrite(0x1F,0x32);
	VXISI2CWrite(0x20,0x5A);
	VXISI2CWrite(0x21,0x0D);
	VXISI2CWrite(0x22,0x32);
	VXISI2CWrite(0x23,0x5A);
	VXISI2CWrite(0x24,0xD0);
	VXISI2CWrite(0x25,0x21);
	VXISI2CWrite(0x26,0x0D);
	VXISI2CWrite(0x27,0xE0);
	VXISI2CWrite(0x28,0xF0);
	VXISI2CWrite(0x29,0x00);
	VXISI2CWrite(0x2A,0x00);
	VXISI2CWrite(0x2B,0x01);
	VXISI2CWrite(0x2C,0xFF);
	VXISI2CWrite(0x2D,0x00);
	VXISI2CWrite(0x2E,0x12);
	VXISI2CWrite(0x2F,0xD0);
	VXISI2CWrite(0x30,0xE0);
	VXISI2CWrite(0x31,0x80);
	VXISI2CWrite(0x32,0x80);
	VXISI2CWrite(0x33,0x80);
	VXISI2CWrite(0x34,0xA0);
	VXISI2CWrite(0x35,0x18);
	VXISI2CWrite(0x36,0x46);
	VXISI2CWrite(0x37,0x64);
	VXISI2CWrite(0x38,0x00);
	VXISI2CWrite(0x39,0xFF);
	VXISI2CWrite(0x3A,0x03);
	VXISI2CWrite(0x3B,0x80);
	VXISI2CWrite(0x3C,0x80);
	VXISI2CWrite(0x3D,0x80);
	VXISI2CWrite(0x3E,0x80);
	VXISI2CWrite(0x3F,0x80);
	//VXISI2CWrite(0xVXISI);2CWrite(0x80
	VXISI2CWrite(0x41,0x60);
	VXISI2CWrite(0x42,0x5F);
	VXISI2CWrite(0x43,0x37);
	VXISI2CWrite(0x44,0x14);
	VXISI2CWrite(0x45,0x04);
	VXISI2CWrite(0x46,0x00);
	VXISI2CWrite(0x47,0x00);
	VXISI2CWrite(0x48,0x00);
	VXISI2CWrite(0x49,0x00);
	VXISI2CWrite(0x4B,0x20);
	VXISI2CWrite(0x4C,0x00);
	VXISI2CWrite(0x4D,0x00);
	VXISI2CWrite(0x4E,0x00);
	VXISI2CWrite(0x4F,0x7F);
	VXISI2CWrite(0x50,0x20);
	VXISI2CWrite(0x51,0x00);
	VXISI2CWrite(0x52,0x00);
	VXISI2CWrite(0x53,0x04);
	VXISI2CWrite(0x54,0x00);
	VXISI2CWrite(0x55,0xE0);
	VXISI2CWrite(0x56,0x40);
	VXISI2CWrite(0x57,0x04);
	VXISI2CWrite(0x58,0x00);
	VXISI2CWrite(0x59,0x08);
	VXISI2CWrite(0x5A,0x10);
	VXISI2CWrite(0x5B,0x00);
	VXISI2CWrite(0x5C,0x68);
	VXISI2CWrite(0x5D,0x40);
	VXISI2CWrite(0x5E,0x08);
	VXISI2CWrite(0x5F,0x00);
	VXISI2CWrite(0x60,0xE0);
	VXISI2CWrite(0x61,0x54);
	VXISI2CWrite(0x62,0x33);
	VXISI2CWrite(0x63,0x01);
	VXISI2CWrite(0x64,0xFF);
	VXISI2CWrite(0x65,0x01);
	VXISI2CWrite(0x66,0xFF);
	VXISI2CWrite(0x67,0x89);
	VXISI2CWrite(0x68,0xAB);
	VXISI2CWrite(0x69,0x88);
	VXISI2CWrite(0x6A,0x6E);
	VXISI2CWrite(0x6B,0x6E);
	VXISI2CWrite(0x6C,0x6E);
	VXISI2CWrite(0x6D,0x00);
	VXISI2CWrite(0x6E,0x00);
	VXISI2CWrite(0x6F,0x00);
	VXISI2CWrite(0x70,0x00);
	VXISI2CWrite(0x71,0x00);
	VXISI2CWrite(0x72,0x00);
	VXISI2CWrite(0x73,0x00);
	VXISI2CWrite(0x74,0x00);
	VXISI2CWrite(0x75,0xFF);
	VXISI2CWrite(0x76,0xFF);
	VXISI2CWrite(0x77,0xFF);
	VXISI2CWrite(0x78,0xFF);
	VXISI2CWrite(0x79,0xFF);
	VXISI2CWrite(0x7A,0xFF);
	VXISI2CWrite(0x7B,0xFF);
	VXISI2CWrite(0x7C,0xFF);
	VXISI2CWrite(0x7D,0xF0);
	VXISI2CWrite(0x7E,0x80);
	VXISI2CWrite(0x7F,0x00);
	VXISI2CWrite(0x80,0x00);
	VXISI2CWrite(0x81,0x34);
	VXISI2CWrite(0x82,0x40);
	VXISI2CWrite(0x83,0x05);
	VXISI2CWrite(0x84,0x59);
	VXISI2CWrite(0x85,0xFD);
	VXISI2CWrite(0x86,0x04);
	VXISI2CWrite(0x87,0x24);
	VXISI2CWrite(0x88,0x94);
	VXISI2CWrite(0x89,0x58);
	VXISI2CWrite(0x8A,0x00);
	VXISI2CWrite(0x8B,0x7E);
	VXISI2CWrite(0x8C,0x03);
	VXISI2CWrite(0x8D,0x2C);
	VXISI2CWrite(0x8E,0x8C);
	VXISI2CWrite(0x8F,0x00);
	VXISI2CWrite(0x90,0xCC);
	VXISI2CWrite(0x91,0x41);
	VXISI2CWrite(0x92,0xC4);
	VXISI2CWrite(0x93,0x31);
	VXISI2CWrite(0x94,0x87);
	VXISI2CWrite(0x95,0xD0);
	VXISI2CWrite(0x96,0xF3);
	VXISI2CWrite(0x97,0x20);
	VXISI2CWrite(0x98,0x11);
	VXISI2CWrite(0x99,0x90);
	VXISI2CWrite(0x9A,0xFF);
	VXISI2CWrite(0x9B,0x04);
	VXISI2CWrite(0x9C,0x0A);
	VXISI2CWrite(0x9D,0x00);
	VXISI2CWrite(0x9E,0x04);
	VXISI2CWrite(0x9F,0x65);
	VXISI2CWrite(0xA0,0x2F);
	VXISI2CWrite(0xA1,0xD9);
	VXISI2CWrite(0xA2,0x98);
	VXISI2CWrite(0xA3,0x87);
	VXISI2CWrite(0xA4,0xE1);
	VXISI2CWrite(0xA5,0x38);
	VXISI2CWrite(0xA6,0xC2);
	VXISI2CWrite(0xA7,0x70);
	VXISI2CWrite(0xA8,0x00);
	VXISI2CWrite(0xA9,0x1E);
	VXISI2CWrite(0xAA,0x2E);
	VXISI2CWrite(0xAB,0x00);
	VXISI2CWrite(0xAC,0x84);
	VXISI2CWrite(0xAD,0xF3);
	VXISI2CWrite(0xAE,0x65);
	VXISI2CWrite(0xAF,0x99);
	VXISI2CWrite(0xB0,0x00);
	VXISI2CWrite(0xB1,0x00);
	VXISI2CWrite(0xB2,0x00);
	VXISI2CWrite(0xB3,0x00);
	VXISI2CWrite(0xB4,0x00);
	VXISI2CWrite(0xB5,0x00);
	VXISI2CWrite(0xB6,0x00);
	VXISI2CWrite(0xB8,0x80);
	VXISI2CWrite(0xB9,0x00);
	VXISI2CWrite(0xBA,0x01);
	VXISI2CWrite(0xBB,0x00);
	VXISI2CWrite(0xBC,0x90);
	VXISI2CWrite(0xBD,0x01);
	VXISI2CWrite(0xBE,0x2C);
	VXISI2CWrite(0xC0,0x00);
	VXISI2CWrite(0xC1,0x20);
	VXISI2CWrite(0xC2,0x20);
	VXISI2CWrite(0xC3,0x20);
	VXISI2CWrite(0xC4,0x20);
	VXISI2CWrite(0xC5,0x20);
	VXISI2CWrite(0xD0,0xFF);
	VXISI2CWrite(0xD1,0x3F);
	VXISI2CWrite(0xD2,0xC2);
	VXISI2CWrite(0xD3,0x3F);
	VXISI2CWrite(0xD4,0x00);
	VXISI2CWrite(0xD5,0x00);
	VXISI2CWrite(0xD6,0x00);
	VXISI2CWrite(0xD7,0x10);
	VXISI2CWrite(0xD8,0x00);
	VXISI2CWrite(0xDC,0x77);
	VXISI2CWrite(0xDD,0x00);
	VXISI2CWrite(0xDE,0xAF);
	VXISI2CWrite(0xDF,0xEF);
	VXISI2CWrite(0xE0,0x00);
	VXISI2CWrite(0xE1,0x00);
	VXISI2CWrite(0xE2,0x00);
	VXISI2CWrite(0xE3,0x12);
	VXISI2CWrite(0xE4,0xE2);
	VXISI2CWrite(0xE5,0xD0);
	VXISI2CWrite(0xE6,0x00);
	VXISI2CWrite(0xE7,0x00);
	VXISI2CWrite(0xE8,0x0F);
	VXISI2CWrite(0xE9,0x00);
	VXISI2CWrite(0xEA,0x00);
	VXISI2CWrite(0xEB,0x00);
	VXISI2CWrite(0xEC,0x88);
	VXISI2CWrite(0xED,0x00);
	VXISI2CWrite(0xEE,0x00);
	VXISI2CWrite(0xEF,0x00);
	VXISI2CWrite(0xF0,0x30);
	VXISI2CWrite(0xF1,0x43);
	VXISI2CWrite(0xF2,0x1F);
	VXISI2CWrite(0xF3,0x1F);
	VXISI2CWrite(0xF4,0x22);
	VXISI2CWrite(0xF5,0x73);
	VXISI2CWrite(0xF6,0x53);
	VXISI2CWrite(0xF7,0x80);
	VXISI2CWrite(0xF8,0x01);
	VXISI2CWrite(0xF9,0x01);
	VXISI2CWrite(0xFA,0x00);
	VXISI2CWrite(0xFB,0x00);
	VXISI2CWrite(0xFC,0x00);
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x40);
	VXISI2CWrite(0xFF,0x07);
					 


}

void SET_VS4210_1280_720_out(void)
{
	Printf("\r\nSET_VS4210_1280_720_out");

	VXISI2CWrite(0x01,0x00);
	VXISI2CWrite(0x02,0x00);
	VXISI2CWrite(0x03,0x75);
	VXISI2CWrite(0x04,0x18);
	VXISI2CWrite(0x05,0x80);
	VXISI2CWrite(0x06,0x20);
	VXISI2CWrite(0x07,0x08);
	VXISI2CWrite(0x08,0x40);
	VXISI2CWrite(0x09,0x0A);
	VXISI2CWrite(0x0A,0x41);
	VXISI2CWrite(0x0B,0x07);
	VXISI2CWrite(0x0C,0x00);
	VXISI2CWrite(0x0D,0x20);
	VXISI2CWrite(0x0E,0x10);
	VXISI2CWrite(0x0F,0x00);
	VXISI2CWrite(0x10,0x80);
	VXISI2CWrite(0x11,0x82);
	VXISI2CWrite(0x12,0x21);
	VXISI2CWrite(0x13,0xD0);
	VXISI2CWrite(0x14,0xE0);
	VXISI2CWrite(0x15,0x78);
	VXISI2CWrite(0x16,0xFE);
	VXISI2CWrite(0x17,0x00);
	VXISI2CWrite(0x18,0xC8);
	VXISI2CWrite(0x19,0x00);
	VXISI2CWrite(0x1A,0x23);
	VXISI2CWrite(0x1B,0x00);
	VXISI2CWrite(0x1C,0x00);
	VXISI2CWrite(0x1D,0x00);
	VXISI2CWrite(0x1E,0x20);
	VXISI2CWrite(0x1F,0x32);
	VXISI2CWrite(0x20,0x5A);
	VXISI2CWrite(0x21,0x0D);
	VXISI2CWrite(0x22,0x32);
	VXISI2CWrite(0x23,0x5A);
	VXISI2CWrite(0x24,0xD0);
	VXISI2CWrite(0x25,0x21);
	VXISI2CWrite(0x26,0x0D);
	VXISI2CWrite(0x27,0xE0);
	VXISI2CWrite(0x28,0xF0);
	VXISI2CWrite(0x29,0x00);
	VXISI2CWrite(0x2A,0x00);
	VXISI2CWrite(0x2B,0x01);
	VXISI2CWrite(0x2C,0xFF);
	VXISI2CWrite(0x2D,0x00);
	VXISI2CWrite(0x2E,0x12);
	VXISI2CWrite(0x2F,0xD0);
	VXISI2CWrite(0x30,0xE0);
	VXISI2CWrite(0x31,0x80);
	VXISI2CWrite(0x32,0x80);
	VXISI2CWrite(0x33,0x80);
	VXISI2CWrite(0x34,0xA0);
	VXISI2CWrite(0x35,0x18);
	VXISI2CWrite(0x36,0x46);
	VXISI2CWrite(0x37,0x64);
	VXISI2CWrite(0x38,0x00);
	VXISI2CWrite(0x39,0xFF);
	VXISI2CWrite(0x3A,0x03);
	VXISI2CWrite(0x3B,0x80);
	VXISI2CWrite(0x3C,0x80);
	VXISI2CWrite(0x3D,0x80);
	VXISI2CWrite(0x3E,0x80);
	VXISI2CWrite(0x3F,0x80);
	//VXISI2CWrite(0xVXISI);2CWrite(0x80
	VXISI2CWrite(0x41,0x60);
	VXISI2CWrite(0x42,0x5F);
	VXISI2CWrite(0x43,0x37);
	VXISI2CWrite(0x44,0x14);
	VXISI2CWrite(0x45,0x04);
	VXISI2CWrite(0x46,0x00);
	VXISI2CWrite(0x47,0x00);
	VXISI2CWrite(0x48,0x00);
	VXISI2CWrite(0x49,0x00);
	VXISI2CWrite(0x4B,0x20);
	VXISI2CWrite(0x4C,0x00);
	VXISI2CWrite(0x4D,0x00);
	VXISI2CWrite(0x4E,0x00);
	VXISI2CWrite(0x4F,0x7F);
	VXISI2CWrite(0x50,0x20);
	VXISI2CWrite(0x51,0x00);
	VXISI2CWrite(0x52,0x00);
	VXISI2CWrite(0x53,0x04);
	VXISI2CWrite(0x54,0x00);
	VXISI2CWrite(0x55,0xE0);
	VXISI2CWrite(0x56,0x40);
	VXISI2CWrite(0x57,0x04);
	VXISI2CWrite(0x58,0x00);
	VXISI2CWrite(0x59,0x08);
	VXISI2CWrite(0x5A,0x10);
	VXISI2CWrite(0x5B,0x00);
	VXISI2CWrite(0x5C,0x68);
	VXISI2CWrite(0x5D,0x40);
	VXISI2CWrite(0x5E,0x08);
	VXISI2CWrite(0x5F,0x00);
	VXISI2CWrite(0x60,0xE0);
	VXISI2CWrite(0x61,0x54);
	VXISI2CWrite(0x62,0x33);
	VXISI2CWrite(0x63,0x01);
	VXISI2CWrite(0x64,0xFF);
	VXISI2CWrite(0x65,0x01);
	VXISI2CWrite(0x66,0xFF);
	VXISI2CWrite(0x67,0x89);
	VXISI2CWrite(0x68,0xAB);
	VXISI2CWrite(0x69,0x88);
	VXISI2CWrite(0x6A,0x6E);
	VXISI2CWrite(0x6B,0x6E);
	VXISI2CWrite(0x6C,0x6E);
	VXISI2CWrite(0x6D,0x00);
	VXISI2CWrite(0x6E,0x00);
	VXISI2CWrite(0x6F,0x00);
	VXISI2CWrite(0x70,0x00);
	VXISI2CWrite(0x71,0x00);
	VXISI2CWrite(0x72,0x00);
	VXISI2CWrite(0x73,0x00);
	VXISI2CWrite(0x74,0x00);
	VXISI2CWrite(0x75,0xFF);
	VXISI2CWrite(0x76,0xFF);
	VXISI2CWrite(0x77,0xFF);
	VXISI2CWrite(0x78,0xFF);
	VXISI2CWrite(0x79,0xFF);
	VXISI2CWrite(0x7A,0xFF);
	VXISI2CWrite(0x7B,0xFF);
	VXISI2CWrite(0x7C,0xFF);
	VXISI2CWrite(0x7D,0xF0);
	VXISI2CWrite(0x7E,0x80);
	VXISI2CWrite(0x7F,0x00);
	VXISI2CWrite(0x80,0x00);
	VXISI2CWrite(0x81,0x34);
	VXISI2CWrite(0x82,0x40);
	VXISI2CWrite(0x83,0x05);
	VXISI2CWrite(0x84,0x59);
	VXISI2CWrite(0x85,0xFD);
	VXISI2CWrite(0x86,0x04);
	VXISI2CWrite(0x87,0x24);
	VXISI2CWrite(0x88,0x94);
	VXISI2CWrite(0x89,0x58);
	VXISI2CWrite(0x8A,0x00);
	VXISI2CWrite(0x8B,0x7E);
	VXISI2CWrite(0x8C,0x03);
	VXISI2CWrite(0x8D,0x2C);
	VXISI2CWrite(0x8E,0x8C);
	VXISI2CWrite(0x8F,0x00);
	VXISI2CWrite(0x90,0xCC);
	VXISI2CWrite(0x91,0x41);
	VXISI2CWrite(0x92,0xC4);
	VXISI2CWrite(0x93,0x31);
	VXISI2CWrite(0x94,0x87);
	VXISI2CWrite(0x95,0xD0);
	VXISI2CWrite(0x96,0xF3);
	VXISI2CWrite(0x97,0x20);
	VXISI2CWrite(0x98,0x11);
	VXISI2CWrite(0x99,0x90);
	VXISI2CWrite(0x9A,0xFF);
	VXISI2CWrite(0x9B,0x04);
	VXISI2CWrite(0x9C,0x0A);
	VXISI2CWrite(0x9D,0x00);
	VXISI2CWrite(0x9E,0x04);
	VXISI2CWrite(0x9F,0x65);
	VXISI2CWrite(0xA0,0x2F);
	VXISI2CWrite(0xA1,0xD9);
	VXISI2CWrite(0xA2,0x98);
	VXISI2CWrite(0xA3,0x87);
	VXISI2CWrite(0xA4,0xE1);
	VXISI2CWrite(0xA5,0x38);
	VXISI2CWrite(0xA6,0xC2);
	VXISI2CWrite(0xA7,0x70);
	VXISI2CWrite(0xA8,0x00);
	VXISI2CWrite(0xA9,0x1E);
	VXISI2CWrite(0xAA,0x2E);
	VXISI2CWrite(0xAB,0x00);
	VXISI2CWrite(0xAC,0x84);
	VXISI2CWrite(0xAD,0xF3);
	VXISI2CWrite(0xAE,0x65);
	VXISI2CWrite(0xAF,0x99);
	VXISI2CWrite(0xB0,0x00);
	VXISI2CWrite(0xB1,0x00);
	VXISI2CWrite(0xB2,0x00);
	VXISI2CWrite(0xB3,0x00);
	VXISI2CWrite(0xB4,0x00);
	VXISI2CWrite(0xB5,0x00);
	VXISI2CWrite(0xB6,0x00);
	VXISI2CWrite(0xB8,0x80);
	VXISI2CWrite(0xB9,0x00);
	VXISI2CWrite(0xBA,0x01);
	VXISI2CWrite(0xBB,0x00);
	VXISI2CWrite(0xBC,0x90);
	VXISI2CWrite(0xBD,0x01);
	VXISI2CWrite(0xBE,0x2C);
	VXISI2CWrite(0xC0,0x00);
	VXISI2CWrite(0xC1,0x20);
	VXISI2CWrite(0xC2,0x20);
	VXISI2CWrite(0xC3,0x20);
	VXISI2CWrite(0xC4,0x20);
	VXISI2CWrite(0xC5,0x20);
	VXISI2CWrite(0xD0,0xFF);
	VXISI2CWrite(0xD1,0x3F);
	VXISI2CWrite(0xD2,0xC2);
	VXISI2CWrite(0xD3,0x3F);
	VXISI2CWrite(0xD4,0x00);
	VXISI2CWrite(0xD5,0x00);
	VXISI2CWrite(0xD6,0x00);
	VXISI2CWrite(0xD7,0x10);
	VXISI2CWrite(0xD8,0x00);
	VXISI2CWrite(0xDC,0x77);
	VXISI2CWrite(0xDD,0x00);
	VXISI2CWrite(0xDE,0xAF);
	VXISI2CWrite(0xDF,0xEF);
	VXISI2CWrite(0xE0,0x00);
	VXISI2CWrite(0xE1,0x00);
	VXISI2CWrite(0xE2,0x00);
	VXISI2CWrite(0xE3,0x12);
	VXISI2CWrite(0xE4,0xE2);
	VXISI2CWrite(0xE5,0xD0);
	VXISI2CWrite(0xE6,0x00);
	VXISI2CWrite(0xE7,0x00);
	VXISI2CWrite(0xE8,0x0F);
	VXISI2CWrite(0xE9,0x00);
	VXISI2CWrite(0xEA,0x00);
	VXISI2CWrite(0xEB,0x00);
	VXISI2CWrite(0xEC,0x88);
	VXISI2CWrite(0xED,0x00);
	VXISI2CWrite(0xEE,0x00);
	VXISI2CWrite(0xEF,0x00);
	VXISI2CWrite(0xF0,0x30);
	VXISI2CWrite(0xF1,0x43);
	VXISI2CWrite(0xF2,0x1F);
	VXISI2CWrite(0xF3,0x1F);
	VXISI2CWrite(0xF4,0x22);
	VXISI2CWrite(0xF5,0x73);
	VXISI2CWrite(0xF6,0x53);
	VXISI2CWrite(0xF7,0x80);
	VXISI2CWrite(0xF8,0x01);
	VXISI2CWrite(0xF9,0x01);
	VXISI2CWrite(0xFA,0x00);
	VXISI2CWrite(0xFB,0x00);
	VXISI2CWrite(0xFC,0x00);
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x40);
	VXISI2CWrite(0xFF,0x07);
			 
}

void SET_AHD_MDOE(void)
{
	Printf("\r\nSET_AHD_MDOE...");

	VXISI2CWrite(0x00,0x42);
	VXISI2CWrite(0x01,0x00);
	VXISI2CWrite(0x02,0x00);
	VXISI2CWrite(0x03,0x75);
	VXISI2CWrite(0x04,0x18);
	VXISI2CWrite(0x05,0x80);
	VXISI2CWrite(0x06,0x20);
	VXISI2CWrite(0x07,0x0C);
	VXISI2CWrite(0x08,0x40);
	VXISI2CWrite(0x09,0x0A);
	VXISI2CWrite(0x0A,0x40);
	VXISI2CWrite(0x0B,0x0A);
	VXISI2CWrite(0x0C,0x00);
	VXISI2CWrite(0x0D,0x20);
	VXISI2CWrite(0x0E,0x10);
	VXISI2CWrite(0x0F,0x00);
	VXISI2CWrite(0x10,0x81);
	VXISI2CWrite(0x11,0x82);
	VXISI2CWrite(0x12,0x74);
	VXISI2CWrite(0x13,0x80);
	VXISI2CWrite(0x14,0x38);
	VXISI2CWrite(0x15,0x78);
	VXISI2CWrite(0x16,0xFE);
	VXISI2CWrite(0x17,0x00);
	VXISI2CWrite(0x18,0xC0);
	VXISI2CWrite(0x19,0x01);
	VXISI2CWrite(0x1A,0x20);
	VXISI2CWrite(0x1B,0x30);
	VXISI2CWrite(0x1C,0x00);
	VXISI2CWrite(0x1D,0x00);
	VXISI2CWrite(0x1E,0x20);
	VXISI2CWrite(0x1F,0xA4);
	VXISI2CWrite(0x20,0x50);
	VXISI2CWrite(0x21,0xE2);
	VXISI2CWrite(0x22,0x87);
	VXISI2CWrite(0x23,0x98);
	VXISI2CWrite(0x24,0x80);
	VXISI2CWrite(0x25,0x44);
	VXISI2CWrite(0x26,0x65);
	VXISI2CWrite(0x27,0x38);
	VXISI2CWrite(0x28,0xF0);
	VXISI2CWrite(0x29,0x00);
	VXISI2CWrite(0x2A,0x00);
	VXISI2CWrite(0x2B,0x11);
	VXISI2CWrite(0x2C,0x00);
	VXISI2CWrite(0x2D,0x00);
	VXISI2CWrite(0x2E,0x47);
	VXISI2CWrite(0x2F,0x80);
	VXISI2CWrite(0x30,0x38);
	VXISI2CWrite(0x31,0x80);
	VXISI2CWrite(0x32,0x80);
	VXISI2CWrite(0x33,0x80);
	VXISI2CWrite(0x34,0xA0);
	VXISI2CWrite(0x35,0x18);
	VXISI2CWrite(0x36,0x46);
	VXISI2CWrite(0x37,0x64);
	VXISI2CWrite(0x38,0x00);
	VXISI2CWrite(0x39,0xFF);
	VXISI2CWrite(0x3A,0x73);
	VXISI2CWrite(0x3B,0x80);
	VXISI2CWrite(0x3C,0x80);
	VXISI2CWrite(0x3D,0x80);
	VXISI2CWrite(0x3E,0x80);
	VXISI2CWrite(0x3F,0x80);
	VXISI2CWrite(0x40,0x80);
	VXISI2CWrite(0x41,0x60);
	VXISI2CWrite(0x42,0x5F);
	VXISI2CWrite(0x43,0x37);
	VXISI2CWrite(0x44,0x14);
	VXISI2CWrite(0x45,0x00);
	VXISI2CWrite(0x46,0x00);
	VXISI2CWrite(0x47,0x00);
	VXISI2CWrite(0x48,0x00);
	VXISI2CWrite(0x49,0x00);
	VXISI2CWrite(0x4A,0x00);
	VXISI2CWrite(0x4B,0x20);
	VXISI2CWrite(0x4C,0x00);
	VXISI2CWrite(0x4D,0x00);
	VXISI2CWrite(0x4E,0x00);
	VXISI2CWrite(0x4F,0x7F);
	VXISI2CWrite(0x50,0x20);
	VXISI2CWrite(0x51,0x00);
	VXISI2CWrite(0x52,0xA4);
	VXISI2CWrite(0x53,0xC0);
	VXISI2CWrite(0x54,0x42);
	VXISI2CWrite(0x55,0x60);
	VXISI2CWrite(0x56,0x5C);
	VXISI2CWrite(0x57,0x04);
	VXISI2CWrite(0x58,0x00);
	VXISI2CWrite(0x59,0x08);
	VXISI2CWrite(0x5A,0x10);
	VXISI2CWrite(0x5B,0x00);
	VXISI2CWrite(0x5C,0x68);
	VXISI2CWrite(0x5D,0xA8);
	VXISI2CWrite(0x5E,0xC0);
	VXISI2CWrite(0x5F,0x53);
	VXISI2CWrite(0x60,0xE0);
	VXISI2CWrite(0x61,0xCC);
	VXISI2CWrite(0x62,0x33);
	VXISI2CWrite(0x63,0x01);
	VXISI2CWrite(0x64,0xFF);
	VXISI2CWrite(0x65,0x01);
	VXISI2CWrite(0x66,0xFF);
	VXISI2CWrite(0x67,0x89);
	VXISI2CWrite(0x68,0xAB);
	VXISI2CWrite(0x69,0x88);
	VXISI2CWrite(0x6A,0x6E);
	VXISI2CWrite(0x6B,0x6E);
	VXISI2CWrite(0x6C,0x6E);
	VXISI2CWrite(0x6D,0x00);
	VXISI2CWrite(0x6E,0x00);
	VXISI2CWrite(0x6F,0x00);
	VXISI2CWrite(0x70,0x00);
	VXISI2CWrite(0x71,0x00);
	VXISI2CWrite(0x72,0x00);
	VXISI2CWrite(0x73,0x00);
	VXISI2CWrite(0x74,0x06);
	VXISI2CWrite(0x75,0xFF);
	VXISI2CWrite(0x76,0xFF);
	VXISI2CWrite(0x77,0xFF);
	VXISI2CWrite(0x78,0xFF);
	VXISI2CWrite(0x79,0xFF);
	VXISI2CWrite(0x7A,0xFF);
	VXISI2CWrite(0x7B,0xFF);
	VXISI2CWrite(0x7C,0xFF);
	VXISI2CWrite(0x7D,0xF0);
	VXISI2CWrite(0x7E,0x80);
	VXISI2CWrite(0x7F,0x00);
	VXISI2CWrite(0x80,0x01);
	VXISI2CWrite(0x81,0x15);
	VXISI2CWrite(0x82,0x40);
	VXISI2CWrite(0x83,0x2D);
	VXISI2CWrite(0x84,0x57);
	VXISI2CWrite(0x85,0xFD);
	VXISI2CWrite(0x86,0x00);
	VXISI2CWrite(0x87,0x00);
	VXISI2CWrite(0x88,0x00);
	VXISI2CWrite(0x89,0x00);
	VXISI2CWrite(0x8A,0x00);
	VXISI2CWrite(0x8B,0x7E);
	VXISI2CWrite(0x8C,0x03);
	VXISI2CWrite(0x8D,0x17);
	VXISI2CWrite(0x8E,0x8C);
	VXISI2CWrite(0x8F,0x00);
	VXISI2CWrite(0x90,0xCC);
	VXISI2CWrite(0x91,0x41);
	VXISI2CWrite(0x92,0xC4);
	VXISI2CWrite(0x93,0x84);
	VXISI2CWrite(0x94,0x98);
	VXISI2CWrite(0x95,0x80);
	VXISI2CWrite(0x96,0x65);
	VXISI2CWrite(0x97,0x70);
	VXISI2CWrite(0x98,0x29);
	VXISI2CWrite(0x99,0x90);
	VXISI2CWrite(0x9A,0x00);
	VXISI2CWrite(0x9B,0x04);
	VXISI2CWrite(0x9C,0x00);
	VXISI2CWrite(0x9D,0x00);
	VXISI2CWrite(0x9E,0x04);
	VXISI2CWrite(0x9F,0x65);
	VXISI2CWrite(0xA0,0x80);
	VXISI2CWrite(0xA1,0xD8);
	VXISI2CWrite(0xA2,0x98);
	VXISI2CWrite(0xA3,0x87);
	VXISI2CWrite(0xA4,0x00);
	VXISI2CWrite(0xA5,0x80);
	VXISI2CWrite(0xA6,0x00);
	VXISI2CWrite(0xA7,0x70);
	VXISI2CWrite(0xA8,0x40);
	VXISI2CWrite(0xA9,0x1E);
	VXISI2CWrite(0xAA,0x2C);
	VXISI2CWrite(0xAB,0x00);
	VXISI2CWrite(0xAC,0x90);
	VXISI2CWrite(0xAD,0x65);
	VXISI2CWrite(0xAE,0x65);
	VXISI2CWrite(0xAF,0x99);
	VXISI2CWrite(0xB0,0x00);
	VXISI2CWrite(0xB1,0x00);
	VXISI2CWrite(0xB2,0x00);
	VXISI2CWrite(0xB3,0x00);
	VXISI2CWrite(0xB4,0x00);
	VXISI2CWrite(0xB5,0x00);
	VXISI2CWrite(0xB6,0x00);
	VXISI2CWrite(0xB7,0x00);
	VXISI2CWrite(0xB8,0x80);
	VXISI2CWrite(0xB9,0x00);
	VXISI2CWrite(0xBA,0x01);
	VXISI2CWrite(0xBB,0x00);
	VXISI2CWrite(0xBC,0x90);
	VXISI2CWrite(0xBD,0x01);
	VXISI2CWrite(0xBE,0x2C);
	VXISI2CWrite(0xBF,0x00);
	VXISI2CWrite(0xC0,0x00);
	VXISI2CWrite(0xC1,0x20);
	VXISI2CWrite(0xC2,0x20);
	VXISI2CWrite(0xC3,0x20);
	VXISI2CWrite(0xC4,0x20);
	VXISI2CWrite(0xC5,0x20);
	VXISI2CWrite(0xC6,0x00);
	VXISI2CWrite(0xC7,0x00);
	VXISI2CWrite(0xC8,0x00);
	VXISI2CWrite(0xC9,0x00);
	VXISI2CWrite(0xCA,0x00);
	VXISI2CWrite(0xCB,0x00);
	VXISI2CWrite(0xCC,0x00);
	VXISI2CWrite(0xCD,0x00);
	VXISI2CWrite(0xCE,0x00);
	VXISI2CWrite(0xCF,0x00);
	VXISI2CWrite(0xD0,0xFF);
	VXISI2CWrite(0xD1,0x3F);
	VXISI2CWrite(0xD2,0xC2);
	VXISI2CWrite(0xD3,0x3F);
	VXISI2CWrite(0xD4,0x00);
	VXISI2CWrite(0xD5,0x00);
	VXISI2CWrite(0xD6,0x00);
	VXISI2CWrite(0xD7,0x0F);
	VXISI2CWrite(0xD8,0x00);
	VXISI2CWrite(0xD9,0x00);
	VXISI2CWrite(0xDA,0x00);
	VXISI2CWrite(0xDB,0x00);
	VXISI2CWrite(0xDC,0x77);
	VXISI2CWrite(0xDD,0x01);
	VXISI2CWrite(0xDE,0x5F);
	VXISI2CWrite(0xDF,0x96);
	VXISI2CWrite(0xE0,0x00);
	VXISI2CWrite(0xE1,0x00);
	VXISI2CWrite(0xE2,0x00);
	VXISI2CWrite(0xE3,0x47);
	VXISI2CWrite(0xE4,0x3C);
	VXISI2CWrite(0xE5,0x80);
	VXISI2CWrite(0xE6,0x00);
	VXISI2CWrite(0xE7,0x00);
	VXISI2CWrite(0xE8,0x00);
	VXISI2CWrite(0xE9,0x00);
	VXISI2CWrite(0xEA,0x00);
	VXISI2CWrite(0xEB,0x00);
	VXISI2CWrite(0xEC,0x88);
	VXISI2CWrite(0xED,0x00);
	VXISI2CWrite(0xEE,0x00);
	VXISI2CWrite(0xEF,0x00);
	VXISI2CWrite(0xF0,0x30);
	VXISI2CWrite(0xF1,0x43);
	VXISI2CWrite(0xF2,0x1F);
	VXISI2CWrite(0xF3,0x1F);
	VXISI2CWrite(0xF4,0x22);
	VXISI2CWrite(0xF5,0x73);
	VXISI2CWrite(0xF6,0x53);
	VXISI2CWrite(0xF7,0x80);
	VXISI2CWrite(0xF8,0x01);
	VXISI2CWrite(0xF9,0x01);
	VXISI2CWrite(0xFA,0x00);
	VXISI2CWrite(0xFB,0x00);
	VXISI2CWrite(0xFC,0x00);
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x40);
	VXISI2CWrite(0xFF,0x07);

}
/**
  * @brief System Clock Configuration
  * @retval None
  */
  #if 0//crystal

  void SystemClock_Config(void)
  {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	  Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
	  Error_Handler();
	}
  }

  #else
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif
/**
  * @brief ADC1 Initialization Function.
  * @param None
  * @retval None
  */
  #if 0
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  /*
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  */
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }


  /* USER CODE BEGIN ADC1_Init 2 */
HAL_ADC_MspInit(&hadc1);

  /* USER CODE END ADC1_Init 2 */

}
#endif
/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
  #if 0
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */
  ///TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = (35/4);
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; 
/*
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
	  Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
	  Error_Handler();
	}
*/
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
   Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
   Error_Handler();
  }
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
//  sConfigOC.Pulse = 4000;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
   Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

//TIMOCInitStructure.TIM_Pulse =(period); 						   

//TIMOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	   
//TIMOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//TIM_OC3Init(TIM3, &TIMOCInitStructure); 				   
				   

//TIM_Cmd(TIM3, ENABLE);

}
#endif
/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 35;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
//  huart1.Init.BaudRate = 9600;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
HAL_GPIO_WritePin(GPIOA, /*TX_RST_Pin|TP2824_RST_Pin|*/VS_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : TX_RST_Pin TP2824_RST_Pin VS_RST_Pin */
  GPIO_InitStruct.Pin = VIDEO_LOSS;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = HDMI_SELECT;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /*Configure GPIO pin : IR_IN_Pin */
/*
  GPIO_InitStruct.Pin = IR_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IR_IN_GPIO_Port, &GPIO_InitStruct);
*/
  /*Configure GPIO pin : RX1_Pin */
  /*
  GPIO_InitStruct.Pin = RX1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(RX1_GPIO_Port, &GPIO_InitStruct);
*/
#if 0
   GPIO_InitStruct.Pin =DOWN_KEY_Pin|UP_KEY_Pin|MENU_KEY_Pin|POWER_KEY_Pin/*|TOUCH_INT*/;
  // GPIO_InitStruct.Pin =MENU_KEY_Pin|POWER_KEY_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

  GPIO_InitStruct.Pin =(/*PANEL_EN_Pin|*//*TOUCH_RESET|*/VS_RST_Pin|STM_SCL_Pin|STM_SDA_Pin);
//  GPIO_InitStruct.Pin =(PANEL_EN_Pin|TOUCH_RESET);
//  GPIO_InitStruct.Pin =(PANEL_EN_Pin|BL_PWM_Pin);
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_OUTPUT_PP;///GPIO_MODE_OUTPUT_OD;
   GPIO_InitStruct.Pull =GPIO_PULLUP;// GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /*Configure GPIO pins : SW1_Pin SW2_Pin SW3_Pin SW4_Pin 
                           SW5_Pin SW6_Pin SW7_Pin */
//  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin|SW3_Pin|SW4_Pin 
   //                      |SW5_Pin|SW6_Pin|SW7_Pin|DOWN_KEY_Pin|UP_KEY_Pin|MENU_KEY_Pin|POWER_KEY_Pin;
  /*

  GPIO_InitStruct.Pin =DOWN_KEY_Pin|UP_KEY_Pin|MENU_KEY_Pin|POWER_KEY_Pin|TOUCH_INT;
 // GPIO_InitStruct.Pin =MENU_KEY_Pin|POWER_KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
*/
/*
  GPIO_InitStruct.Pin =DOWN_KEY_Pin|UP_KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
*/
  GPIO_InitStruct.Pin =TP2824_REST/*LCD_PWR_Pin|TTLI1_RES_Pin|AUD_SEL2_Pin|GREEN_LED_Pin |RED_LED_Pin*/;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLUP;

   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


//  GPIO_InitStruct.Pin =GREEN_LED_Pin|RED_LED_Pin;
  // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   //GPIO_InitStruct.Pull = GPIO_NOPULL;
   //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   //HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = TP2824_IRQ|MCU_WAKEUP|AUD_I2C_MUTE_Pin/*| JUMP_KEY_Pin*/;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/*
  GPIO_InitStruct.Pin =AUD_SEL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  */


	

  /* EXTI interrupt init*/
/*
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
*/
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//	BYTE TempKey=0;

    //10us
    if (htim->Instance == TIM3)
    {
	
	// Timer Counter 
	tic02++;
	tic03++;
	tic04++;
//	tic05++;
	
	if(tic02>=100)   
	{
	g_usTimerCounter += 1;     ///per 10us  interrupt
	tic02=0;
	g_bNotifyTimer0Int=_TRUE;
	}	
/*
	if(tic05>=100)////1ms
	{
	ucTickCount++;	//for IT66021
	tic05=0;
	}
	*/
        tim3count++ ;
        if (tim3count > 100000 )
        {
            tim3count = 0 ;
            //Test_tim3_printf_Flg  = 1;
            if (OSDStateFlg ==1 )
            {
                //static tByte enOSD10secOff = 1;   //En16wNumberOSD=1
                OSDSecCount++ ;
            }
        }
#if 0

		//Kane @HS 2007 0814 Ver1.31>>>>
	//	if(Time5ms)
	//	Time5ms--;
		//if(Time5ms==0)
		{
			//if(HAL_GPIO_ReadPin(UP_KEY_GPIO_Port,UP_KEY_Pin)==_FALSE)
			//	TempKey|=0x01;
			//if(HAL_GPIO_ReadPin(DOWN_KEY_GPIO_Port,DOWN_KEY_Pin)==_FALSE)
			//	TempKey|=0x02;
			//TempKey=(( (P2&0x60) >> 5) & 0x03);
//			TempKey=((~GPIOB->IDR)&(UP_KEY_Pin|DOWN_KEY_Pin))>>14;
			TempKey=(((~GPIOB->IDR)&(UP_KEY_Pin|DOWN_KEY_Pin))>>14)&0x03;

			if(updn_reg[3] != TempKey )  // (( (P2&0x60) >> 5) & 0x03))    //Abel for HS SW key ......951102									   
			{																						   
				updn_reg[0] = updn_reg[1];																
				updn_reg[1] = updn_reg[2];																
				updn_reg[2] = updn_reg[3];																
				updn_reg[3] = TempKey;// ( (P2&0x60) >> 5) & 0x03; 
				ChangeKey = 1;		
				//else			//william-v1.42-961130
				//ChangeKey=0;	//william-v1.42-961130
			//Time5ms = (48-8);
			//Time5ms = (48+100);
			
				
			}
		}
		#endif
#if 0
        if (IR_IRQFlg == 1)
        {
            if (PreIR_IRQ_Index == IR_IRQ_Index )
            {
                IR_PlusTimeVal++ ;

                //9.2xms + 4.5xms = 13.8ms
                if (IR_PlusTimeVal > 15000)  //10us
                {
//                    IR_PlusTimeVal = 0  ;
//                    PreIR_IRQ_Index = 0 ;
//                    IR_IRQ_Index = 0 ;
//                    IR_IRQFlg = 0 ;

                    if (IR_ShiftCount >= 32 )
                    {
                        u16Vendor = (tWord)(IR_4BYTE & 0x0000FFFF) ;
                        if (u16Vendor == IR_Vendor_Code_ )
                        {
                            u8IRcode = (tByte)((IR_4BYTE & 0x00FF0000 ) >> 16)  ;
                            u8IRCheckcode  = 0xFF - (tByte)((IR_4BYTE & 0xFF000000 ) >> 24)  ;
                            IR_END_Flg = 1 ;
                        }
                    }

                    IR_ShiftCount = 0 ;
                    IR_PlusTimeVal = 0  ;
                    PreIR_IRQ_Index = 0 ;
                    IR_IRQ_Index = 0 ;
                    IR_IRQFlg = 0 ;
                    //memset(aIR_PlusTime , 0 , 40 )  ;
                    IR_4BYTE = 0 ;
                    IR_StartFlg = 0 ;
                }
            }
            else
            {
                if (PreIR_IRQ_Index <= 33)
                {
                    //aIR_PlusTime[PreIR_IRQ_Index] = IR_PlusTimeVal ;
                    if (IR_StartFlg == 1 )
                    {
                        if (IR_PlusTimeVal >90 )
                        {
                            //IR_4BYTE = IR_4BYTE << 1 ;
                            IR_4BYTE = IR_4BYTE >> 1 ;

                            if (IR_PlusTimeVal > 160 )
                            {
                                //IR_4BYTE = IR_4BYTE | 0x01 ;
                                IR_4BYTE = IR_4BYTE | 0x80000000 ;
                            }
                            else
                            {
                                //IR_4BYTE = IR_4BYTE | 0x01 ;
                            }
                            IR_ShiftCount++ ;
                        }
                    }

                    if((IR_PlusTimeVal >1250 )&&(IR_PlusTimeVal < 1450))
                    {
                        IR_StartFlg =1 ;
                    }
                }
                IR_PlusTimeVal = 0 ;
                PreIR_IRQ_Index = IR_IRQ_Index ;
            }
        }
#endif
		///////////////////////////////////////////
	/*
		TempKey=0;
		if(HAL_GPIO_ReadPin(MENU_KEY_GPIO_Port,MENU_KEY_Pin)==_FALSE)
			TempKey|=0x08;
		if(HAL_GPIO_ReadPin(POWER_KEY_GPIO_Port,POWER_KEY_Pin)==_FALSE)
			TempKey|=0x04;
		if(HAL_GPIO_ReadPin(POWER_KEY_GPIO_Port,POWER_KEY_Pin)==_FALSE)
			TempKey|=0x04;
		if(HAL_GPIO_ReadPin(JUMP_KEY_GPIO_Port,JUMP_KEY_Pin)==_FALSE)
			TempKey|=0x10;
		if(HAL_GPIO_ReadPin(SOURCE_KEY_GPIO_Port,SOURCE_KEY_Pin)==_FALSE)
			Te			mpKey|=0x80;
*/
#if 0
	if(tic03==1000)
		{
		tic03=0;
		if(_ReadKey() ) {
			
			if( keytic==3 ) {
				Key = _ReadKey();
				RepeatKey = 0;
				KeyReady = 1;
			}
			else if( keytic==100 ) {
				Key = _ReadKey();
				KeyReady = 1;
				keytic = 80-70;

			}
			keytic++;
		
		}
		else 
		{
			keytic = 0;
			Key=0;
		}
		}	

	if(tic04==1000)
		{
		tic04=0;
				if(TP_PRES_DOWN ) {
					
					if( tptic==3) {
						TouchP=TP_PRES_DOWN;
					}
					else if(tptic==150)
						{
						TouchP=TP_PRES_DOWN;						
						
						tptic=4;						
						}
				tptic++;
				}
				else 
				{
					tptic = 0;
					TouchP=0;
				}

		}
	#endif
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)	//current UART
    {

/*
		if (Rx_indx==0)
        {
            memset(Rx_Buffer , 0  , RXBUFMAX ) ;
        }

        if (Rx_data[0] >= 0x30)
        {
            Rx_Buffer[Rx_indx]=Rx_data[0];	//add data to Rx_Buffer
            Rx_indx++ ;
            if (Rx_indx >= RXBUFMAX )
                Rx_indx = 0 ;
        }
        else if ((Rx_data[0] == 0x0d) || (Rx_data[0] == 0xa))//if received data = 13
        {
            RxBufIndex = Rx_indx ;
            memcpy(gRxbuf ,Rx_Buffer, Rx_indx ) ;
            Rx_indx=0;
            Transfer_cplt=1;//transfer complete, data is ready to read
        }
*/
#if 1
       RS_buf [RS_in++] = Rx_data[0];//USART_ReceiveData(USART1);	
    if( RS_in >= BUF_MAX)  RS_in = 0;
		
		//RS_tx(Rx_data[0]);


//HAL_UART_Receive_IT(&huart1,(uint8_t*)Rx_data, 1);	//activate UART receive interrupt every time
	HAL_UART_Receive_IT(&huart1,(uint8_t*)Rx_data, 1);	 //activate UART receive interrupt every time
	#endif
    }
#if 1
if (huart->Instance == USART2)	//current UART2
    {

       RS2_buf [RS2_in++] = Rx2_data[0];//USART_ReceiveData(USART1);	
    if( RS2_in >= TOA_BUF_MAX)  RS2_in = 0;
	HAL_UART_Receive_IT(&huart2,(uint8_t*)Rx2_data, 1);	 //activate UART receive interrupt every time
    }	
#endif
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
/*
    if(GPIO_Pin == IR_IN_Pin  )
    {
        IR_IRQFlg =1 ;

        IR_IRQ_Index++ ;
    }
	*/
}

void HS_Select_Win(unsigned char WinNum)	//20201214-02
{

	
	switch (WinNum)
	{
		default:
		case 0:
			VXISI2CWrite( 0x32, 0x00 );
			VXISI2CWrite( 0x19, 0x00 );
			SelectInputMode=0;
			printf("Selected window 0\r\n");
			pVS4210_MutiWindows1.OutputMode =VS4210_1080P_MODE0_w0;						
			pVS4210_MutiWindows1.WindowMap.WMap0 = 1 ;			
			VS4210_SetMutiWindows(&pVS4210_MutiWindows1) ;	
			//#if (_External_Sync==ON)
			//VXISI2CWrite( 0x32, 0x00 );
			  VXISI2CWrite( 0x19, 0x00 );
			//#endif
			//msleep(100) ;
		//	VXISI2CWrite( 0x32, 0x80 );

			if(bSignal[0]==1)
				{
				Set_Video_Loss_OSD(0,OFF);
				}
			else
				{
				Set_Video_Loss_OSD(0,ON);
				}

			#if (_External_Sync==OFF)
				//SET_HDMI_SYNC_PARA();			
			#endif
			break;
		
		case 1:
			VXISI2CWrite( 0x32, 0x00 );
			SelectInputMode=1;
			printf("Selected window 1\r\n");
			pVS4210_MutiWindows1.OutputMode =VS4210_1080P_MODE0_w1;
			pVS4210_MutiWindows1.WindowMap.WMap0 = 2 ;
			VS4210_SetMutiWindows(&pVS4210_MutiWindows1) ;		
			VXISI2CWrite( 0x19, 0x04 );

		if(SelectInputMode==1)
		{
			if(  mode[0]==7 )
				{
			
				TP_bt1120_1080P_1080P_mode0_w0();
					printf("(=1)");
				//VXISI2CWrite(0x10,0x01);
				//VXISI2CWrite(0x9c,0x0A);
				Set_Video_Loss_OSD(1,ON);

			}
			else
				{
				
				printf("(=2)");
								if(TP_InputMode==5)
									{
									TP_bt1120_720P_1080P_mode0_w0();
									}
								else if(TP_InputMode==6)
									{
									TP_bt1120_1080P_1080P_mode0_w0();
									}
								else if(TP_InputMode==0)
									{
									TP_bt1120_NTSC_1080P_mode0_w0();
									}
								else if(TP_InputMode==1)
									{
									TP_bt1120_PAL_1080P_mode0_w0();
									}
								else
									VXISI2CWrite(0x10,0x01);

										
				}

			#if (_External_Sync==OFF)
			//SET_AHD_SYNC_PARA();
			#endif
		}	
		// msleep(100) ;



		
			if(bSignal[1]==1)
				Set_Video_Loss_OSD(1,OFF);
			else
				Set_Video_Loss_OSD(1,ON);
			
			break;
		}
	
	

  		OSDSecCount = 0 ;
		   OSDStateFlg = 1 ;		
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
