///*****************************************
//  Copyright (C) 2009-2020
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <edid.c>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2020/05/27
//   @fileversion: ITE_HDMI1.4_RXSAMPLE_1.29
//******************************************/

//#include "it660x.h"
//#include "MCU.h"
//#include "nop.h"
#include <stdio.h>
#include "main.h"
#include "Printf.h"
#include "IT660x_reg.h"

#include "VXIS_Extern_Define.h"


#define DISABLE_PORT0_EDIDRAM 0
#define DISABLE_PORT1_EDIDRAM 0
#define EDID_ADDR 0xA8	//Software programmable I2C address of IT6602 EDID RAM

#define HDMI_DEV		0
#define SWADR	 	0x96
#define EDID_HEAD	0xA0

#define DELAY_TIME        1
#define IDLE_TIME        100

#define HIGH            1
#define LOW                0
#define ACTIVE          1

#define dealy11 300


typedef int /*bit*/ BOOL;



void SETIT66021EDID(void);
unsigned char  hdmirxset( unsigned char  offset, unsigned char  mask, unsigned char  ucdata );

//#if 1///def  _SUPPORT_EDID_RAM_

unsigned char HdmiI2cAddr2=IT6602B0_HDMI_ADDR;

unsigned char  txphyadr[2], txphyA, txphyB, txphyC, txphyD, txphylevel;	// for CEC function
unsigned char  rxphyadr[2][2];// for EDID RAM function

struct IT6602_dev_data IT6602DEV;

unsigned char const Default_Edid_Block[256] = {

#if 1
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,

0x22,0x70,0x28,0x0C,0x01,0x01,0x01,0x01,

0x34,0x16,0x01,0x03,0x80,0x73,0x41,0x78,

0x2A,0x7C,0x11,0x9E,0x59,0x47,0x9B,0x27,

0x10,0x50,0x54,0x00,0x00,0x00,0x01,0x01,

0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,

0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x3A,

0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,

0x45,0x00,0x10,0x09,0x00,0x00,0x00,0x1E,

0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,

0x10,0x3E,0x96,0x00,0x04,0x03,0x00,0x00,

0x00,0x18,0x00,0x00,0x00,0xFC,0x00,0x56,

0x69,0x64,0x65,0x6F,0x20,0x45,0x6E,0x63,

0x6F,0x64,0x65,0x72,0x00,0x00,0x00,0xFD,

0x00,0x30,0x7A,0x0F,0x50,0x10,0x00,0x0A,

0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xF9,

0x02,0x03,0x19,0x72,0x46,0x90,0x04,0x13,

0x01,0x02,0x03,0x23,0x09,0x07,0x07,0x83,

0x01,0x00,0x00,0x65,0x03,0x0C,0x00,0x10,

0x00,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,

0x20,0x6E,0x28,0x55,0x00,0x10,0x09,0x00,

0x00,0x00,0x1E,0xD6,0x09,0x80,0xA0,0x20,

0xE0,0x2D,0x10,0x10,0x60,0xA2,0x00,0x04,

0x03,0x00,0x00,0x00,0x18,0x8C,0x0A,0xD0,

0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,

0x00,0x10,0x09,0x00,0x00,0x00,0x18,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7B,

#else
// IT6602 with 640x480p ,720x480p ,1280x720p ,1920x1080p
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,// address 0x00
0x26,0x85,0x02,0x66,0x01,0x68,0x00,0x00,//
0x00,0x17,0x01,0x03,0x80,0x73,0x41,0x78,// address 0x10
0x2A,0x7C,0x11,0x9E,0x59,0x47,0x9B,0x27,//
0x10,0x50,0x54,0x00,0x00,0x00,0x01,0x01,// address 0x20
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//
0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x3A,// address 0x30
0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,//
0x45,0x00,0x10,0x09,0x00,0x00,0x00,0x1E,// address 0x40
0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,//
0x10,0x3E,0x96,0x00,0x04,0x03,0x00,0x00,// address 0x50
0x00,0x18,0x00,0x00,0x00,0xFC,0x00,0x49,//
0x54,0x45,0x36,0x38,0x30,0x32,0x0A,0x20,// address 0x60
0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFD,//
0x00,0x30,0x7A,0x0F,0x50,0x10,0x00,0x0A,// address 0x70
0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xF5,//
0x02,0x03,0x19,0x72,0x46,0x90,0x04,0x13,// address 0x80
0x01,0x02,0x03,0x23,0x09,0x07,0x07,0x83,//
0x01,0x00,0x00,0x65,0x03,0x0C,0x00,0x10,// address 0x90
0x00,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,//
0x20,0x6E,0x28,0x55,0x00,0x10,0x09,0x00,// address 0xA0
0x00,0x00,0x1E,0xD6,0x09,0x80,0xA0,0x20,//
0xE0,0x2D,0x10,0x10,0x60,0xA2,0x00,0x04,// address 0xB0
0x03,0x00,0x00,0x00,0x18,0x8C,0x0A,0xD0,//
0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,// address 0xC0
0x00,0x10,0x09,0x00,0x00,0x00,0x18,0x00,//
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// address 0xD0
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// address 0xE0
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// address 0xF0
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7B,//
#endif

};

typedef enum _Video_State_Type {
    VSTATE_Off=0,
    VSTATE_TerminationOff,
    VSTATE_TerminationOn,
    VSTATE_5VOff,
    VSTATE_SyncWait,  ///4
    VSTATE_SWReset,   //5
    VSTATE_SyncChecking,
    VSTATE_HDCPSet,
    VSTATE_HDCP_Reset,
    VSTATE_ModeDetecting,
    VSTATE_VideoOn,
    VSTATE_ColorDetectReset,
    VSTATE_HDMI_OFF,
    VSTATE_Unknown,
    VSTATE_Reserved
} Video_State_Type;

typedef enum _Audio_State_Type {
    ASTATE_AudioOff = 0,
    ASTATE_RequestAudio ,
    ASTATE_ResetAudio,
    ASTATE_WaitForReady,
    ASTATE_AudioOn ,
    ASTATE_Unknown,
    ASTATE_Reserved
} Audio_State_Type;


typedef enum _RxHDCP_State_Type {
    RxHDCP_PwrOff=0,
    RxHDCP_ModeCheck,
    RxHDCP_Receiver,
    RxHDCP_Repeater,
    RxHDCP_SetKSVFifoList,
    RxHDCP_GenVR,
    RxHDCP_WriteVR,
    RxHDCP_Auth_WaitRi,
    RxHDCP_Authenticated,
    RxHDCP_Reserved
} RxHDCP_State_Type;

typedef struct {
    BYTE AudioFlag;
    BYTE AudSrcEnable;
    BYTE SampleFreq;
    BYTE ChStat[5];
} AUDIO_CAPS;

typedef struct _3D_SourceConfiguration
{
    unsigned char          Format;              /**< Type of 3D source format expected or found.                                                        */
    unsigned char    LR_Reference;        /**< Source of the 3D L/R reference.                                                                    */
    unsigned char FrameDominance;      /**< Left or Right Eye is first in L/R image pair.                                                      */
    unsigned char     LR_Encoding;         /**< Type of 3D L/R encoding expected or detected.                                                      */
    unsigned char    TB_Reference;        /**< Top/Bottom reference for vertically sub-sampled sources.                                           */
    unsigned char    OE_Reference;        /**< Odd/Even reference for horizontally sub-sampled sources.                                           */
    unsigned char NumActiveBlankLines;                 /**< Number of lines separating vertically packed L/R data to be removed (cropped)
                                                  *  before being displayed. Does not include any embedded encoding.                                    */
    unsigned char NumberOfEncodedLines;                /**< Number of encoded lines in one L/R eye frame of the display data
                                                  *  to be blanked out with "Blanking Color". (assumed same number in second eye frame)                 */
    unsigned int  LeftEncodedLineLocation;             /**< Active line number of 1st encoded line in one Left eye frame of the display data (-1=unknown).     */
    unsigned int  RightEncodedLineLocation;            /**< Active line number of 1st encoded line in one Right eye frame of the display data (-1=unknown).
                                                  *  If format is Horizontally Packed, set RightEncodedLineLocation=LeftEncodedLineLocation             */
    unsigned char BlankingColor;                 /**< Color to use when blanking (or masking off) any embedded L/R encoding.                             */
} SRC_3D_SOURCE_CONFIG;

typedef struct _de3dframe
{
    unsigned char VIC;
    unsigned char HB0;
    unsigned char HB1;
    unsigned char HB2;
    unsigned char PB0;
    unsigned char PB1;
    unsigned char PB2;
    unsigned char PB3;
    unsigned char PB4;
    unsigned char PB5;
    unsigned char PB6;
    unsigned char PB7;
} DE3DFRAME;

typedef enum _Video_Output_Configure
{
    eRGB444_SDR=0,
    eYUV444_SDR,
    eRGB444_DDR,
    eYUV444_DDR,
    eYUV422_Emb_Sync_SDR,
    eYUV422_Emb_Sync_DDR,
    eYUV422_Sep_Sync_SDR,
    eYUV422_Sep_Sync_DDR,
    eCCIR656_Emb_Sync_SDR,
    eCCIR656_Emb_Sync_DDR,
    eCCIR656_Sep_Sync_SDR,
    eCCIR656_Sep_Sync_DDR,
    eRGB444_Half_Bus,
    eYUV444_Half_Bus,
    eBTA1004_SDR,
    eBTA1004_DDR,
    eVOMreserve
}Video_Output_Configure;

typedef enum _Video_OutputDataTrigger_Mode
{
    eSDR=0,
    eHalfPCLKDDR,
    eHalfBusDDR,
    eSDR_BTA1004,
    eDDR_BTA1004
}Video_DataTrigger_Mode;

typedef enum _Video_OutputSync_Mode
{
    eSepSync=0,
    eEmbSync,
    eCCIR656SepSync,
    eCCIR656EmbSync
}Video_OutputSync_Mode;

typedef enum _SYS_STATUS {
    ER_SUCCESS = 0,
    ER_FAIL,
    ER_RESERVED
} SYS_STATUS;

struct IT6602_dev_data
{
    Video_State_Type m_VState;
    Audio_State_Type m_AState;
    RxHDCP_State_Type m_RxHDCPState;
    AUDIO_CAPS m_RxAudioCaps;
    unsigned short m_SWResetTimeOut;
    unsigned short m_VideoCountingTimer;
    unsigned short m_AudioCountingTimer;
    unsigned char m_ucCurrentHDMIPort;
    unsigned char m_bOutputVideoMode;
    unsigned char m_bInputVideoMode;

//FIX_ID_039 xxxxx fix image flick when enable RGB limited / Full range convert
#ifdef _AVOID_REDUNDANCE_CSC_
    unsigned char m_Backup_OutputVideoMode;
    unsigned char m_Backup_InputVideoMode;
#endif
//FIX_ID_039 xxxxx

    unsigned char m_ucSCDTOffCount;
    unsigned char m_ucEccCount_P0;
    unsigned char m_ucEccCount_P1;
    unsigned char m_ucDeskew_P0;
    unsigned char m_ucDeskew_P1;

    SRC_3D_SOURCE_CONFIG de3dframe_config;
    DE3DFRAME s_Current3DFr;

    unsigned char oldVIC;
    unsigned char newVIC;
    unsigned char  f_de3dframe_hdmi;

//FIX_ID_001 xxxxx Add Auto EQ with Manual EQ
    #ifdef _SUPPORT_EQ_ADJUST_
    struct IT6602_eq_data EQPort[2];
    // FIX_ID_043
    unsigned char ToggleCount[2];
    //~FIX_ID_043
    #endif
//FIX_ID_001 xxxxx

    //FIX_ID_003 xxxxx	//Add IT6602 Video Output Configure setting
    Video_Output_Configure m_VidOutConfigMode;
    Video_DataTrigger_Mode m_VidOutDataTrgger;
    Video_OutputSync_Mode m_VidOutSyncMode;
    //FIX_ID_003 xxxxx

//FIX_ID_014 xxxxx	//Add Cbus Event Handler
    unsigned char HDMIIntEvent;
    unsigned char HDMIWaitNo[2];
//FIX_ID_014 xxxxx


    //AVI_info m_avi;
    unsigned char ColorMode;
    unsigned char Colorimetry;
    unsigned char ExtendedColorimetry;
    unsigned char RGBQuantizationRange;
    unsigned char YCCQuantizationRange;
    unsigned char VIC;



    unsigned char m_HDCP_ContentOff;
    //xxxxx

    unsigned char GCP_CD;	//Output Color Depth
    unsigned char  DE3DFormat_HDMIFlag:1;
    unsigned char  FramePacking_Flag:1;
    unsigned char  TopAndBottom_Flag:1;
    unsigned char  SideBySide_Flag:1;

    #ifdef _IT6607_GeNPacket_Usage_
    BYTE m_PollingPacket;
    BYTE m_PacketState;
    BYTE m_ACPState;
    BYTE m_GeneralRecPackType;
    BYTE m_GamutPacketRequest:1;
    #endif

//#if(_SUPPORT_HDCP_)
//    //HDCP
//    unsigned char HDCPEnable;
//    HDCPSts_Type Hdcp_state;
//    unsigned int HDCPFireCnt;
//#endif
    // 2015/07/01 added by jjtseng
    // for solving the new AVI interrupt issued by Audio Force MUTE.
    unsigned char prevAVIDB[2] ;



    unsigned char m_bRxAVmute:1;
    unsigned char m_bVideoOnCountFlag:1;
    unsigned char m_MuteAutoOff:1;
    unsigned char m_bUpHDMIMode:1;
    unsigned char m_bUpHDCPMode:1;
    unsigned char m_NewAVIInfoFrameF:1;
    unsigned char m_NewAUDInfoFrameF:1;
    unsigned char m_HDCPRepeater:1;
    unsigned char m_MuteByPKG:1;
    // pit660xCallbackList_t	pCallbackFunctionsList;
    unsigned char m_bHDCPrepeater:1;


#ifdef _SUPPORT_HDMI_REPEATER_
    unsigned char m_sinkHPD ;
#endif // _SUPPORT_HDMI_REPEATER_

};

void delay1(unsigned int i )
{  //5ms
    tWord j ;
    for (j = 0 ; j<i ; j++)
    {
       // HAL_Delay(5);
    }
}


void HotPlug(BYTE bEnable)
{
/*
     if(bEnable)
         gpHPD0 = HPDON;
     else
         gpHPD0 = HPDOFF;
*/	
}

void SetintActive(BOOL bactive)
{
    bactive =0;  //fake code

}

void chgbank( int bank )
{
#if 0
    switch( bank ) {
    case 0 :
         hdmirxset(0x0F, 0x03, 0x00);
         break;
    case 1 :
         hdmirxset(0x0F, 0x03, 0x01);
         break;
    case 2 :
         hdmirxset(0x0F, 0x03, 0x02);
         break;
    case 3:
         hdmirxset(0x0F, 0x03, 0x03);
         break;
    default :
         break;
    }
#else
         hdmirxset(0x0F, 0x03, bank&3);
#endif
}

void set_8051_scl( BOOL bit_value,BYTE device )
{
//     stall5us;
	 delay1(dealy11);

     switch(device)
     {
         case 0:
         //    DEV0_SCL_PORT=bit_value;
			 STM_SCL(bit_value);
            break;
         case 1:
          //   DEV1_SCL_PORT=bit_value;
            break;
        case 2:
          //   DEV2_SCL_PORT=bit_value;
            break;
         case 3:
        //     DEV3_SCL_PORT=bit_value;
            break;
         case 4:
         //    DEV4_SCL_PORT=bit_value;
            break;
        default:
          //   DEV0_SCL_PORT=bit_value;
            break;
     }

//     stall5us;
delay1(dealy11);

}

void set_8051_sda( BOOL bit_value,BYTE device )
{
//     stall5us;
	 delay1(dealy11);

     switch( device )
     {
         case 0:
         //    DEV0_SDA_PORT=bit_value;
	STM_SDA(bit_value);
            break;
         case 1:
           //  DEV1_SDA_PORT=bit_value;
            break;
         case 2:
            // DEV2_SDA_PORT=bit_value;
            break;
         case 3:
             //DEV3_SDA_PORT=bit_value;
            break;
         case 4:
            // DEV4_SDA_PORT=bit_value;
            break;

        default:
            // DEV0_SDA_PORT=bit_value;
             break;
     }

//     stall5us;
delay1(dealy11);

}

BOOL get_8051_sda( BYTE device )
{
BOOL ret=LOW;

     switch( device )
     {
         case 0:
	if(STM_GET_SDA()==HIGH)	 	
		ret=HIGH;
          //   DEV0_SDA_PORT=1;
             //return DEV0_SDA_PORT;
            break;
         case 1:
            // DEV1_SDA_PORT=1;
             //return DEV1_SDA_PORT;
            break;
         case 2:
            // DEV2_SDA_PORT=1;
             //return DEV2_SDA_PORT;
            break;

         case 3:
            // DEV3_SDA_PORT=1;
            // return DEV3_SDA_PORT;
            break;

         case 4:
             //DEV4_SDA_PORT=1;
             //return DEV4_SDA_PORT;
            break;

        default:
             //DEV0_SDA_PORT=1;
             //return DEV0_SDA_PORT;
             break;
     }

return ret;	
}

void i2c_8051_start( BYTE device )
{

    set_8051_sda( HIGH ,device);
    set_8051_scl( HIGH ,device);
    set_8051_sda( LOW  ,device);
    set_8051_scl( LOW  ,device);
	
}

void i2c_8051_write( BYTE byte_data,BYTE device )
{

 BYTE /*data*/ bit_cnt,tmp;
 BOOL /*data*/ bit_value;

     for(bit_cnt=0; bit_cnt<8; bit_cnt++) {
         tmp=(byte_data << bit_cnt) & 0x80;
         bit_value=tmp && 0x80;

         set_8051_sda( bit_value,device);
         set_8051_scl( HIGH     ,device);
         set_8051_scl( LOW      ,device);
     }
	
}

BOOL i2c_8051_wait_ack( BYTE device )
{

 BOOL /*data*/ ack_bit_value;

    set_8051_sda( HIGH,device );
    set_8051_scl( HIGH,device );
    ack_bit_value=get_8051_sda( device );
    set_8051_scl( LOW ,device);

    return ack_bit_value;
	
}

BYTE i2c_8051_read( BYTE device )
{

 BYTE /*data*/ bit_cnt,byte_data;
 BOOL /*data*/ bit_value;

     byte_data=0;
     for(bit_cnt=0; bit_cnt<8; bit_cnt++) {
         set_8051_scl( HIGH,device );

         bit_value=get_8051_sda( device );

         byte_data=(byte_data << 1) | bit_value;

         set_8051_scl( LOW,device );
     }

     return byte_data;
	
}

void i2c_8051_send_ack( BOOL bit_value,BYTE device )
{

     set_8051_sda( bit_value,device );
     set_8051_scl( HIGH ,device);
     set_8051_scl( LOW ,device);
     set_8051_sda( HIGH,device );
	
}

void i2c_8051_end( BYTE device )
{

     set_8051_sda( LOW,device );
     set_8051_scl( HIGH,device );
     set_8051_sda( HIGH,device );

}

BOOL i2c_write_byte( BYTE address,BYTE offset,BYTE byteno,BYTE *p_data,BYTE device )
{

     BYTE /*data*/ i;


     i2c_8051_start(device);

     i2c_8051_write(address&0xFE,device);
     if( i2c_8051_wait_ack(device)==1 )    {
         i2c_8051_end(device);
     return 0;
      }

     i2c_8051_write(offset,device);
     if( i2c_8051_wait_ack(device)==1 )    {
         i2c_8051_end(device);
     return 0;
     }

     for(i=0; i<byteno-1; i++) {
          i2c_8051_write(*p_data,device);
          if( i2c_8051_wait_ack(device)==1 ) {
              i2c_8051_end(device);
         return 0;
         }
         p_data++;
     }

     i2c_8051_write(*p_data,device);
     if( i2c_8051_wait_ack(device)==1 )    {
          i2c_8051_end(device);
     return 0;
     }
     else {
          i2c_8051_end(device);
     return 1;
     }
	 
	 return 1;
}

BOOL i2c_read_byte( BYTE address,BYTE offset,BYTE byteno,BYTE *p_data,BYTE device )
{

 BYTE /*data*/ i;

     i2c_8051_start(device);

     i2c_8051_write(address&0xFE,device);
     if( i2c_8051_wait_ack(device)==1 ) {
         i2c_8051_end(device);
         return 0;
     }

     i2c_8051_write(offset,device);
     if( i2c_8051_wait_ack(device)==1 ) {
         i2c_8051_end(device);
         return 0;
     }

     i2c_8051_start(device);

     i2c_8051_write(address|0x01,device);
     if( i2c_8051_wait_ack(device)==1 ) {
         i2c_8051_end(device);
         return 0;
     }

     for(i=0; i<byteno-1; i++) {
         *p_data=i2c_8051_read(device);
         i2c_8051_send_ack(LOW,device);

         p_data++;
     }

     *p_data=i2c_8051_read(device);
     i2c_8051_send_ack(HIGH,device);
     i2c_8051_end(device);

    return 1;
}


unsigned char hdmirxrd( unsigned char RegAddr)
{
    unsigned char	FLAG;
    unsigned char mDataIn;
//FIX_ID_002 xxxxx 	Check IT6602 chip version Identify for TogglePolarity and Port 1 Deskew
    FLAG= i2c_read_byte(HdmiI2cAddr2, RegAddr, 1, &mDataIn, HDMI_DEV);
//FIX_ID_002 xxxxx
    if(FLAG==0)
    {
    IT6602_DEBUG_INT_PRINTF(("HDMI Read ERROR !!!"));
    IT6602_DEBUG_INT_PRINTF(("=====  Read Reg0x%X=\n",(int) RegAddr));
    }
    return mDataIn;
}

unsigned char hdmirxwr( unsigned char RegAddr,unsigned char DataIn)
{

    unsigned char flag;
    // MHLRX_DEBUG_PRINTF(("HDMI_IIC_Write(%#BX,%#BX)\n", RegAddr, DataIn));

//FIX_ID_002 xxxxx 	Check IT6602 chip version Identify for TogglePolarity and Port 1 Deskew
    flag= i2c_write_byte(HdmiI2cAddr2, RegAddr, 1, &DataIn, HDMI_DEV);
//FIX_ID_002 xxxxx
    if(flag==0)
    {
    IT6602_DEBUG_INT_PRINTF(("====================================\n"));
    IT6602_DEBUG_INT_PRINTF(("HDMI I2C ERROR !!!"));
    IT6602_DEBUG_INT_PRINTF(("=====  Write Reg0x%X=%X =====\n",(int)RegAddr,(int)DataIn));
    IT6602_DEBUG_INT_PRINTF(("====================================\n"));
    }
    return !flag;
}

unsigned char  hdmirxset( unsigned char  offset, unsigned char  mask, unsigned char  ucdata )
{
    unsigned char  temp;
    temp = hdmirxrd(offset);
    temp = (temp&((~mask)&0xFF))+(mask&ucdata);
    return hdmirxwr(offset, temp);
}

void hdmirxbwr( unsigned char offset, unsigned char byteno, unsigned char *rddata )
{
    //unsigned char flag;
    if( byteno>0 )
//FIX_ID_002 xxxxx 	Check IT6602 chip version Identify for TogglePolarity and Port 1 Deskew
    //flag= 
    i2c_write_byte(HdmiI2cAddr2, offset, byteno, rddata, HDMI_DEV);
//FIX_ID_002 xxxxx
/*
    if(flag==0)
    {
    IT6602_DEBUG_INT_PRINTF(("====================================\n"));
    IT6602_DEBUG_INT_PRINTF(("IT6602 I2C ERROR !!!"));
    IT6602_DEBUG_INT_PRINTF(("=====  Write Reg0x%X=%X =====\n",(int)offset,(int)rddata));
    IT6602_DEBUG_INT_PRINTF(("====================================\n"));
    }
	*/
}

SYS_STATUS EDID_RAM_Write(unsigned char offset,unsigned char byteno,unsigned char *p_data )
{
    unsigned char flag;
    flag = i2c_write_byte(EDID_ADDR, offset, byteno, p_data, HDMI_DEV);
    return !flag;
}

struct IT6602_dev_data* get_IT6602_dev_data(void)
{
    return &IT6602DEV;
}

void IT6602HPDCtrl(unsigned char ucport,unsigned char ucEnable)
{

//    struct IT6602_dev_data *IT6602data = get_IT6602_dev_data();
    HDMIRX_VIDEO_PRINTF(("IT6602HPDCtrl(%X,%X)\n",(int)ucport,(int)ucEnable)) ;
    if(ucport == 0)
    {
        if(ucEnable == 0)
        {

            // Disable HDMI DDC Bus to access it660x EDID RAM

        // FIX_ID_048
            //else // HDMI mode.
            {
                if((hdmirxrd(REG_RX_P0_SYS_STATUS) & B_P0_PWR5V_DET))
                {
                    HDMIRX_VIDEO_PRINTF(("Port 0 HPD HDMI 00000\n"));
                    chgbank(1);
                    hdmirxset(REG_RX_1B0, 0x03, 0x01); //clear port 0 HPD=1 for EDID update
                    chgbank(0);
                }
                else
                {
                    HDMIRX_VIDEO_PRINTF(("Port 0 HPD Tri-State\n"));
                    chgbank(1);
                    hdmirxset(REG_RX_1B0, 0x03, 0x00); //clear port 0 HPD=1 for EDID update
                    chgbank(0);
                }
            } //
        //~FIX_ID_048

        }
        else
        {


            if((hdmirxrd(REG_RX_P0_SYS_STATUS) & B_P0_PWR5V_DET))
            {


                // Enable HDMI DDC bus to access it660x EDID RAM

            // FIX_ID_048 // only HDMI mode reply HPD pull high
                {
                    HDMIRX_VIDEO_PRINTF(("Port 0 HPD HDMI 11111\n"));
                    chgbank(1);
                    hdmirxset(REG_RX_1B0, 0x03, 0x03); //set port 0 HPD=1
                    chgbank(0);
                } //
            //~FIX_ID_048
            } // ((hdmirxrd(REG_RX_P0_SYS_STATUS) & B_P0_PWR5V_DET))
            else
            {
                HDMIRX_VIDEO_PRINTF(("Port 0 HPD but HPD Tristate by 5V.\n"));
                chgbank(1);
                hdmirxset(REG_RX_1B0, 0x03, 0x00); //set port 0 Tri-State
                chgbank(0);
            } // not ((hdmirxrd(REG_RX_P0_SYS_STATUS) & B_P0_PWR5V_DET))
        } // if(ucEnable == 0)
    } // if(ucport == 0)
#if defined(_IT6602_)
    else
    {
         if(ucEnable)
         {
            // Enable HDMI DDC bus to access it660x EDID RAM

            HDMIRX_VIDEO_PRINTF(("Port 1 HPD 11111\n"));
             gpHPD0 = HPDON;
         }
         else
         {
            // Disable HDMI DDC Bus to access it660x EDID RAM

            HDMIRX_VIDEO_PRINTF(("Port 1 HPD 00000\n"));
             gpHPD0 = HPDOFF;
         }
    }
#endif
}

#if 1///def _SUPPORT_EDID_RAM_
/*****************************************************************************/
/* EDID RAM  functions    *******************************************************/
/*****************************************************************************/

//static unsigned char UpdateEDIDRAM(_CODE unsigned char *pEDID,unsigned char BlockNUM)
unsigned char UpdateEDIDRAM(unsigned char *pEDID,unsigned char BlockNUM)
{
    unsigned char  i,offset,sum =0;

    if ( BlockNUM == 0x02 )
        offset = 0x00+128*0x01;
    else
        offset = 0x00+128*BlockNUM;

    EDID_DEBUG_PRINTF(("block No =%02X offset = %02X\n",(int) BlockNUM,(int) offset));


    for(i=0;i<0x7F;i++)
    {
        EDID_RAM_Write(offset,1 ,(pEDID+offset));

        EDID_DEBUG_PRINTF(("%02X ",(int) *(pEDID+offset)));
        sum += *(pEDID+offset);
        offset ++;
//		pEDID++;

            if((i % 16) == 15)
            {
            EDID_DEBUG_PRINTF(("\n"));
            }


    }
    sum = 0x00- sum;
//    EDID_RAM_Write(offset,1,&sum);
    return 	sum;
}

void EnableEDIDupdata(void)
{
    EDID_DEBUG_PRINTF(("EnableEDIDupdata()\n"));

//	HotPlug(0);	//clear port 1 HPD=0 for Enable EDID update

//	chgbank(1);
//	hdmirxset(REG_RX_1B0, 0x03, 0x01); //clear port 0 HPD=1 for EDID update
//	chgbank(0);

    IT6602HPDCtrl(0,0);	// HDMI port 0, set HPD = 0
    IT6602HPDCtrl(1,0);	// HDMI port 1, set HPD = 0
}

void DisableEDIDupdata(void)
{
    EDID_DEBUG_PRINTF(("DisableEDIDupdata()\n"));

//xxxxx 2013-0801 disable HPD Control
#if 0
    HotPlug(1);	//set port 1 HPD=1

    chgbank(1);
    hdmirxset(REG_RX_1B0, 0x03, 0x03); //set port 0 HPD=1
    chgbank(0);
#endif
//xxxxx
}


//static void EDIDRAMInitial(_CODE unsigned char *pIT6602EDID)
/*
void EDIDRAMInitial(unsigned char *pIT6602EDID)
{

    unsigned char Block0_CheckSum;
    unsigned char Block1_CheckSum;
    unsigned char u8_VSDB_Addr;
    unsigned char BlockNo;

    u8_VSDB_Addr=0;

    EnableEDIDupdata();

    for(BlockNo=0;BlockNo<2;BlockNo++){

    EDID_DEBUG_PRINTF(("IT6602 EDIDRAMInitial = %02X\n", (int) BlockNo));

        if(BlockNo==0)
        {
            Block0_CheckSum =  UpdateEDIDRAM(pIT6602EDID,0);
            hdmirxwr(REG_RX_0C4,Block0_CheckSum);		//Port 0 Bank 0 CheckSum
            hdmirxwr(REG_RX_0C8,Block0_CheckSum);		//Port 1 Bank 0 CheckSum

            EDID_DEBUG_PRINTF((" Block0_CheckSum = %02X\n", (int) Block0_CheckSum));
        }
        else
        {
            Block1_CheckSum =  UpdateEDIDRAM(pIT6602EDID,1);
            EDID_DEBUG_PRINTF((" Block1_CheckSum = %02X\n", (int) Block1_CheckSum));
            u8_VSDB_Addr=Find_Phyaddress_Location(pIT6602EDID,1);

            EDID_DEBUG_PRINTF(("u8_VSDB_Addr = %02X\n", (int) u8_VSDB_Addr));
            PhyAdrSet();

            if(u8_VSDB_Addr!=0)
            {

                UpdateEDIDReg(u8_VSDB_Addr, pIT6602EDID[u8_VSDB_Addr],pIT6602EDID[u8_VSDB_Addr+1], Block1_CheckSum);
                EDID_DEBUG_PRINTF(("EDID Parsing OK\n"));
            }
        }
    }

    DisableEDIDupdata();
}
*/

//static unsigned char Find_Phyaddress_Location(_CODE unsigned char *pEDID,unsigned char Block_Number)
unsigned char Find_Phyaddress_Location(unsigned char *pEDID,unsigned char Block_Number)
{
    unsigned char AddStart;
    unsigned char tag, count;
    unsigned char offset,End;
    unsigned char u8_VSDB_Addr;

    if ( Block_Number == 0x02 )
        AddStart = 0x00+128*0x01;
    else
        AddStart = 0x00+128*Block_Number;

    if((*(pEDID+AddStart))!=0x2 || (*(pEDID+AddStart+1))!=0x3)
        return 0;
    End = (*(pEDID+AddStart+2));
    u8_VSDB_Addr=0;

    for(offset=(AddStart+0x04);offset<(AddStart+End); )
    {


        tag=(*(pEDID+offset))>>5;
        count=(*(pEDID+offset)) & 0x1f;

        //#ifdef printf_EDID
        EDID_DEBUG_PRINTF(("offset = %X , Tag = %X , count =%X\n", (int) offset,(int)  tag, (int) count));
        //#endif

        offset++;
    if(tag==0x03)	// HDMI VSDB Block of EDID
        {
            //#ifdef printf_EDID
                EDID_DEBUG_PRINTF(("HDMI VSDB Block address = %X\n",(int)  offset));
            //#endif

            if(	(*(pEDID+offset  ))==0x03 &&
                (*(pEDID+offset+1))==0x0C &&
                (*(pEDID+offset+2))==0x0    )
            {
                u8_VSDB_Addr=offset+3;
                txphyadr[0]=(*(pEDID+offset+3));
                txphyadr[1]=(*(pEDID+offset+4));
                //#ifdef printf_EDID
                EDID_DEBUG_PRINTF(("txphyadr[0] = %X\n",(int)  txphyadr[0]));
                EDID_DEBUG_PRINTF(("txphyadr[1] = %X\n",(int)  txphyadr[1]));
                //#endif

                return u8_VSDB_Addr;
            }
        }

        offset=offset+count;
    }
    return 0;
}



void UpdateEDIDReg(unsigned char u8_VSDB_Addr, unsigned char CEC_AB, unsigned char CEC_CD, unsigned char Block1_CheckSum)
{

    unsigned char  A_Addr_AB, A_Addr_CD, A_Block1_CheckSum;
    unsigned char  B_Addr_AB, B_Addr_CD, B_Block1_CheckSum;

    A_Addr_AB=rxphyadr[0][0];
    A_Addr_CD=rxphyadr[0][1];

    B_Addr_AB=rxphyadr[1][0];
    B_Addr_CD=rxphyadr[1][1];


    A_Block1_CheckSum=(Block1_CheckSum+CEC_AB+CEC_CD -A_Addr_AB-A_Addr_CD)%0x100;
    B_Block1_CheckSum=(Block1_CheckSum+CEC_AB+CEC_CD -B_Addr_AB-B_Addr_CD)%0x100;


    hdmirxwr(REG_RX_0C1,u8_VSDB_Addr);			//VSDB Start Address
    hdmirxwr(REG_RX_0C2,A_Addr_AB);					//Port 0 AB
    hdmirxwr(REG_RX_0C3,A_Addr_CD);				//Port 0 CD
    hdmirxwr(REG_RX_0C5,A_Block1_CheckSum);		//Port 0 Bank 1 CheckSum

    hdmirxwr(REG_RX_0C6,B_Addr_AB);					//Port 1 AB
    hdmirxwr(REG_RX_0C7,B_Addr_CD);				//Port 1 CD
    hdmirxwr(REG_RX_0C9,B_Block1_CheckSum);		//Port 1 Bank 1 CheckSum



}
void PhyAdrSet(void)
{
//#ifdef Enable_IT6602_CEC
//	if(Myself_LogicAdr==DEVICE_ID_TV)
//#endif
    {
        rxphyadr[0][0] = 0x10;
        rxphyadr[0][1] = 0x00;
        rxphyadr[1][0] = 0x20;
        rxphyadr[1][1] = 0x00;

    }



}


#endif

//static void EDIDRAMInitial(_CODE unsigned char *pIT6602EDID)
void EDIDRAMInitial(unsigned char *pIT6602EDID)
{

    unsigned char Block0_CheckSum;
    unsigned char Block1_CheckSum;
    unsigned char u8_VSDB_Addr;
    unsigned char BlockNo;

    u8_VSDB_Addr=0;

    EnableEDIDupdata();

    for(BlockNo=0;BlockNo<2;BlockNo++){

    EDID_DEBUG_PRINTF(("IT6602 EDIDRAMInitial = %02X\n", (int) BlockNo));

        if(BlockNo==0)
        {
            Block0_CheckSum =  UpdateEDIDRAM(pIT6602EDID,0);
            hdmirxwr(REG_RX_0C4,Block0_CheckSum);		//Port 0 Bank 0 CheckSum
            hdmirxwr(REG_RX_0C8,Block0_CheckSum);		//Port 1 Bank 0 CheckSum

            EDID_DEBUG_PRINTF((" Block0_CheckSum = %02X\n", (int) Block0_CheckSum));
        }
        else
        {
            Block1_CheckSum =  UpdateEDIDRAM(pIT6602EDID,1);
            EDID_DEBUG_PRINTF((" Block1_CheckSum = %02X\n", (int) Block1_CheckSum));
            u8_VSDB_Addr=Find_Phyaddress_Location(pIT6602EDID,1);

            EDID_DEBUG_PRINTF(("u8_VSDB_Addr = %02X\n", (int) u8_VSDB_Addr));
            PhyAdrSet();

            if(u8_VSDB_Addr!=0)
            {

                UpdateEDIDReg(u8_VSDB_Addr, pIT6602EDID[u8_VSDB_Addr],pIT6602EDID[u8_VSDB_Addr+1], Block1_CheckSum);
                EDID_DEBUG_PRINTF(("EDID Parsing OK\n"));
            }
        }
    }

    DisableEDIDupdata();
}

void SETIT66021EDID(void)
{

	IT6602_DEBUG_INT_PRINTF(("\r\n(SETIT66021EDID)"));

    hdmirxset(REG_RX_0C0,	0x43,	0x40|(DISABLE_PORT1_EDIDRAM<<1)|(DISABLE_PORT0_EDIDRAM<<1));	//[0]1:Reg_P0DisableShadow
    hdmirxset(REG_RX_087,	0xFF,	(EDID_ADDR|0x01));	//[7:1] EDID RAM Slave Adr ,[0]1: Enable access EDID block

        //printf("!!!Use IT6602 EDID RAM !!!");
	EDIDRAMInitial(&Default_Edid_Block[0]);
	//FIX_ID_041 xxxxx Add EDID reset

	hdmirxset(REG_RX_0C0, 0x20, 0x20);	//xxxxx 2014-0731 [5] 1 for  reset edid
	//delay1ms(1);
	hdmirxset(REG_RX_0C0, 0x20, 0x00);
	//FIX_ID_041 xxxxx

}












//#endif
