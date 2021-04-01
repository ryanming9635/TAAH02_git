// *****************************************************
// Company : Techpoint Inc
// $Date: 2014-03-10
// $Log:  $
// ******************************************************
#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"
#include "Printf.h"

extern tByte ManVidRes;
extern tByte ManSelCh;
extern tByte gRxLoopFlg;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//TP2802D EQ for short cable option
#define TP2802D_EQ_SHORT 0x0d
#define TP2802D_CGAIN_SHORT 0x74
//#define TP2802D_EQ_SHORT 0x01
//#define TP2802D_CGAIN_SHORT 0x70

//#define CVI_TEST 0
//#define AHD_TEST 1

#define CH_PER_CHIP 0x01 //0x04
#define MAX_COUNT 	0xff

//extern void CreateMDIN340VideoInstance(void);

tByte newInRes[4] = {/*7*/20,7,7,7} ;

extern bit DetAutoRes;
tByte	state[CH_PER_CHIP];
tByte	count[CH_PER_CHIP];
tByte	mode[CH_PER_CHIP];
tByte	std[CH_PER_CHIP];
tByte	egain[CH_PER_CHIP][4];
tByte	revision;
tByte	chip;
tByte	flag_LOCKED;
tByte   TVII2CAddress;
tByte	PTZ_buf[16];
tWord   PTZ_pelco;
tByte   CurRes;
tByte   CurCH;
tByte crc;
bit sywd;
bit flag_CVBS;
bit flag_AUDIO;
bit flag_MEGA;

tcByte SYS_MODE[]= {0x01,0x02,0x04,0x08,0x0f};
tcByte SYS_AND[] = {0xfe,0xfd,0xfb,0xf7,0xf0};

tcByte REG_F6[4]= {0x00,0x11,0x22,0x33};
tcByte REG_F7[4]= {0x44,0x55,0x66,0x77};
tcByte REG_FA[4]= {0xBB,0xCC,0xDD,0xEE};

tByte TP_InputMode=6;
tByte Pre_InputMode=0xff;

//cocrx
tByte     tpRxD[26];
BYTE COCRX1[12];
BYTE COCRX2[12];
BYTE COCRX3[12];
BYTE COCRX4[12];

BYTE AHDcmd1,AHDcmd2,AHDcmd3,AHDcmd4;
BYTE TVI_Command;
BYTE presetNum;
BYTE TVI_Chksum;
BYTE TVI_speed;
bit ptztxflag;

BYTE PtzTxChNum=0;//0:ch1 / 1:ch2 / 2:ch3 / 3:ch4
//extern BYTE QuadBoxId;//1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
BYTE QuadBoxId=0;//1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
BYTE QuadBoxMode=0;

extern BYTE SelectInputMode;
extern BYTE BOOT_ISSUE;
extern tByte  OSDSecCount;
extern tByte  OSDStateFlg;

enum
{
    TP2802C=0x02,
    TP2804 =0x04,
    TP2802D=0x05,
    TP2822 =0x22,
    TP2823 =0x23,
    TP2834 =0x24,
    TP2833 =0x25,
    TP2853C=0x26,
    TP2824C=0x27,
    TP2826 =0x2f
};

tcByte TP2824C_DataSet[] =
{
    0x40, 0x04,
    0x07, 0xC0,
    0x0B, 0xC0,
    0x21, 0x84,
    0x38, 0x00,
    0x3A, 0x22,
    0x3B, 0x05,
    0x26, 0x04,
    //0x4d, 0x07,
    //0x4e, 0x05,
    0x4d, 0x0F,
    0x4e, 0x0F,  //chunyu
    0xf1, 0x00,//different with TP2826
    0xff, 0xff
};

tcByte TP2826_DataSet[] =
{
    0x40, 0x04,
    0x07, 0xC0,
    0x0B, 0xC0,
    0x21, 0x84,
    0x38, 0x00,
    0x3A, 0x22,
    0x3B, 0x05,
    0x26, 0x04,
    0x4d, 0x07,
    0x4e, 0x05,
    0xf1, 0x07,
    0xff, 0xff
};


tcByte TP2826_MODE_DataSet[5][20]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0x2c,0x21,0x22,0x23,0x25,0x27,0x28,0xff}, //ADDR
    {0x03,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x1c,0x0a,0x84,0x36,0x3c,0xff,0x2d,0x00,0xff}, //V1
    {0x13,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x18,0x0a,0x84,0x36,0x3c,0xff,0x2d,0x00,0xff}, //V2
    {0x13,0x50,0x40,0x05,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x04,0x2a,0x84,0x36,0x3c,0xff,0x2d,0x00,0xff}, //NTSC
    {0x13,0x51,0x48,0x05,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x04,0x2a,0x84,0x37,0x3f,0xff,0x2d,0x00,0xff}  //PAL
};
tcByte TP2824C_MODE_DataSet[5][20]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0x2c,0x21,0x22,0x23,0x25,0x27,0x28,0xff}, //ADDR
    {0x03,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x1c,0x0a,0x84,0x36,0x3c,0xff,0x2d,0x00,0xff}, //V1
    {0x13,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x18,0x0a,0x84,0x36,0x3c,0xff,0x2d,0x00,0xff}, //V2
    {0x13,0x50,0x40,0x05,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x04,0x2a,0x84,0x36,0x3c,0xff,0x2d,0x00,0xff}, //NTSC
    {0x13,0x51,0x48,0x05,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x04,0x2a,0x84,0x37,0x3f,0xff,0x2d,0x00,0xff}  //PAL
};

tcByte TP2826_HDA_DataSet[5][12]=
{
    {0x0c,0x20,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x21,0x25,0xff}, //ADDR
    {0x03,0x40,0x60,0x48,0x40,0x27,0x72,0x80,0x77,0x46,0xfe,0xff}, //720p30
    {0x03,0x40,0x60,0x48,0x40,0x27,0x88,0x04,0x23,0x46,0xfe,0xff}, //720p25
    {0x03,0x38,0x60,0x44,0x40,0x29,0x65,0x78,0x16,0x46,0xfe,0xff}, //1080p30
    {0x03,0x3c,0x60,0x44,0x40,0x29,0x61,0x78,0x16,0x46,0xfe,0xff}, //1080p25
};
tcByte TP2824C_HDA_DataSet[5][12]=
{
    {0x0c,0x20,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x21,0x25,0xff}, //ADDR
    {0x03,0x40,0x60,0x48,0x40,0x27,0x72,0x80,0x77,0x46,0xfe,0xff}, //720p30
    {0x03,0x40,0x60,0x48,0x40,0x27,0x88,0x04,0x23,0x46,0xfe,0xff}, //720p25
    {0x03,0x38,0x60,0x44,0x40,0x29,0x65,0x78,0x16,0x46,0xfe,0xff}, //1080p30
    {0x03,0x3c,0x60,0x44,0x40,0x29,0x61,0x78,0x16,0x46,0xfe,0xff}, //1080p25
};
tcByte TP2826_HDC_DataSet[5][20]=
{
    {0x0c,0x15,0x16,0x17,0x18,0x19,0x1a,0x1c,0x1d,0x20,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x27,0x28,0xff}, //ADDR
    {0x03,0x13,0x08,0x00,0x19,0xd0,0x25,0x06,0x72,0x30,0x60,0x37,0x40,0x48,0x67,0x6f,0x30,0x5a,0x04,0xff}, //720p30
    {0x03,0x13,0x0a,0x00,0x19,0xd0,0x25,0x06,0x7a,0x3a,0x60,0x42,0x40,0x48,0x67,0x6f,0x33,0x5a,0x04,0xff}, //720p25
    {0x03,0x13,0x60,0x80,0x29,0x38,0x47,0x09,0x60,0x3c,0x60,0x47,0x40,0x41,0x82,0x27,0xa4,0x5a,0x04,0xff}, //1080p30
    {0x03,0x13,0x60,0x80,0x29,0x38,0x47,0x09,0x60,0x50,0x60,0x54,0x40,0x41,0x82,0x27,0xa2,0x5a,0x04,0xff}, //1080p25
};
tcByte TP2824C_HDC_DataSet[5][20]=
{
    {0x0c,0x15,0x16,0x17,0x18,0x19,0x1a,0x1c,0x1d,0x20,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x27,0x28,0xff}, //ADDR
    {0x03,0x13,0x08,0x00,0x19,0xd0,0x25,0x06,0x72,0x30,0x60,0x37,0x40,0x48,0x67,0x6f,0x30,0x5a,0x04,0xff}, //720p30
    {0x03,0x13,0x0a,0x00,0x19,0xd0,0x25,0x06,0x7a,0x3a,0x60,0x42,0x40,0x48,0x67,0x6f,0x33,0x5a,0x04,0xff}, //720p25
    {0x03,0x13,0x60,0x80,0x29,0x38,0x47,0x09,0x60,0x3c,0x60,0x47,0x40,0x41,0x82,0x27,0xa4,0x5a,0x04,0xff}, //1080p30
    {0x03,0x13,0x60,0x80,0x29,0x38,0x47,0x09,0x60,0x50,0x60,0x54,0x40,0x41,0x82,0x27,0xa2,0x5a,0x04,0xff}, //1080p25
};
tcByte TP2853C_DataSet[] =
{
    0x40, 0x04,
    0x07, 0xC0,
    0x0B, 0xC0,
    0x21, 0x84,
    0x38, 0x00,
    0x3A, 0x32,
    0x3B, 0x05,
    0x26, 0x04,
    0x4d, 0x03,
    0x4e, 0x03,
    0x4f, 0x01,
    0xf5, 0xf0,
    0xf1,0x07,//tp2826c

    0xff, 0xff
};
tcByte TP2853C_MODE_DataSet[5][18]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0x2c,0x21,0x25,0x27,0x28,0xff}, //ADDR
    {0x03,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x0c,0x0a,0x84,0xf0,0x2d,0x00,0xff}, //V1
    {0x13,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x08,0x0a,0x84,0xf0,0x2d,0x00,0xff},	//V2
    {0x13,0x50,0x40,0x05,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x04,0x2a,0x84,0xf0,0x2d,0x00,0xff}, //NTSC
    {0x13,0x51,0x48,0x05,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x04,0x2a,0x84,0xf0,0x2d,0x00,0xff}  //PAL
};
tcByte TP2853C_HDA_DataSet[5][10]=
{
    {0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x21,0x25,0xff}, //ADDR
    {0x70,0x48,0x5e,0x27,0x72,0x80,0x77,0x44,0xf0,0xff}, //720p30
    {0x70,0x48,0x5e,0x27,0x88,0x04,0x23,0x44,0xf0,0xff}, //720p25
    {0x58,0x45,0x50,0x29,0x65,0x78,0x16,0x44,0xf0,0xff}, //1080p30
    {0x58,0x45,0x40,0x29,0x61,0x78,0x16,0x44,0xf0,0xff}, //1080p25
};
tcByte TP2853C_HDC_DataSet[5][13]=
{
    {0x15,0x16,0x20,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x27,0x28,0xff}, //ADDR
    {0x13,0x40,0x50,0x60,0x42,0x40,0x48,0x67,0x2e,0x90,0x5a,0x04,0xff}, //720p30
    {0x13,0x40,0x50,0x60,0x42,0x40,0x48,0x67,0x2e,0x90,0x5a,0x04,0xff}, //720p25
    {0x13,0x20,0x48,0x60,0x45,0x40,0x41,0x83,0x2e,0x90,0x5a,0x04,0xff}, //1080p30
    {0x13,0x84,0x48,0x60,0x45,0x40,0x41,0x83,0x2e,0x90,0x5a,0x04,0xff}, //1080p25
};

tcByte TP2834_Audio_DataSet[]=
{
//audio
    0x40, 0x40, //audio page
    0x00, 0x01, //A1
    0x01, 0x02, //A2
    0x02, 0x03, //A3
    0x03, 0x04, //A4
    0x04, 0x11, //AUX
    0x17, 0x00,
    0x18, 0x10, //5ch mode
    0x19, 0x0f,
    0x1A, 0x15, //PB to DAC
    0x1b, 0x01, //PB master
    0x37, 0x20,

    0x1d, 0x08,
    0x1d, 0x00,

    0x3c, 0x3f,
    0x3c, 0x00,

    0x3d, 0x01, //ADC reset
    0xff, 0xff
};

tcByte TP2833_DataSet[] =
{

    0x40, 0x04,
    0x07, 0xC0,
    0x0B, 0xC0,
    0x22, 0x36,
    0x38, 0x40,
    0x26, 0x04,
    0x4d, 0x03,
    0x4e, 0x03,
    0x4f, 0x01,
    0xff, 0xff
};
tcByte TP2833_MODE_DataSet[5][14]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0x18,0xff},
    {0x03,0x50,0x30,0x05,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x0c,0x12,0xff},
    {0x13,0x50,0x30,0x05,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x08,0x12,0xff},
    {0x13,0x50,0x40,0x05,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x04,0x11,0xff},
    {0x13,0x51,0x48,0x05,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x04,0x11,0xff}
};
tcByte TP2833_HDA_DataSet[5][8]=
{
    {0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0xff}, //ADDR
    {0x70,0x48,0x5e,0x27,0x72,0x80,0x77,0xff}, //720p30
    {0x70,0x48,0x5e,0x27,0x88,0x04,0x23,0xff}, //720p25
    {0x58,0x45,0x50,0x29,0x65,0x78,0x16,0xff}, //1080p30
    {0x58,0x45,0x40,0x29,0x61,0x78,0x16,0xff}, //1080p25
};

tcByte TP2834_DataSet[] =
{

    //video
    0x40, 0x04,
    0x07, 0xc0,
    0x0b, 0xc0,
    0x3a, 0x70,
    0x26, 0x01,

    0x4d, 0x0f,
    0x4e, 0x0f,

    0xff, 0xff
};
tcByte TP2834_MODE_DataSet[5][13]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0xff},
    {0x03,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x8c,0xff},
    {0x03,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x88,0xff},
    {0x43,0x10,0xa0,0x12,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x84,0xff},
    {0x43,0x11,0xb0,0x02,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x84,0xff}
};

//////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////

tcByte TP2823_DataSet[] =
{

    //video
    0x40, 0x04,
    0x0b, 0x60,
    0x22, 0x34,
    0x23, 0x44,
    0x26, 0x01,
    0x38, 0x40,
    0x3f, 0x08,

    0x4d, 0x03,
    0x4e, 0x33,
    0xfa, 0x00,

    0xff, 0xff
};
tcByte TP2823_MODE_DataSet[5][13]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0xff},
    {0x43,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x30,0xff},
    {0x53,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x20,0xff},
    {0x53,0x10,0xa0,0x12,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x10,0xff},
    {0x53,0x11,0xb0,0x02,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x10,0xff}
};
tcByte TP2822_DataSet[] =
{

    0x40, 0x04,
    0x39, 0x00,
    0x3d, 0x08,
    0x3a, 0x01,

    0x30, 0x48,
    0x31, 0xbb,
    0x32, 0x2e,
    0x33, 0x90,

    0x4d, 0x03,
    0x4e, 0x33,
    0x3d, 0x00,

    0xff, 0xff
};

tcByte TP2802D_DataSet[] =
{
    0x40, 0x04,
//		0x07, 0xc0,
//		0x0b, 0xc0,
    0x2b, 0x44,
    0x23, 0x50,
    0x39, 0x42,
    0x3a, 0x01,

    0x30, 0x48,
    0x31, 0xbb,
    0x32, 0x2e,
    0x33, 0x90,

    0x4d, 0x0f,
    0x4e, 0xff,

    //now TP2801A just support 2 lines, to disable line3&4, else IRQ is in trouble.
    0x40, 0x01,
    0x50, 0x00,
    0x51, 0x00,
    0x52, 0x00,
    0x7F, 0x00,
    0x80, 0x00,
    0x81, 0x00,

    //0x50~0xb2 need bank switch
    0x40, 0x00,
    //TX total 34bit, valid load 32bit
    0x50, 0x00,
    0x51, 0x0b,
    0x52, 0x0c,
    0x53, 0x19,
    0x54, 0x78,
    0x55, 0x21,
    0x7e, 0x0f,   //

    // RX total 40bit, valid load 39bit
    0x7F, 0x00,
    0x80, 0x07,
    0x81, 0x08,
    0x82, 0x04,
    0x83, 0x00,
    0x84, 0x00,
    0x85, 0x60,
    0x86, 0x10,
    0x87, 0x06,
    0x88, 0xBE,
    0x89, 0x39,
    0x8A, 0x27,   //
    0xB9, 0x0F,   //RX channel enable
    0xff, 0xff
};

/*
//total 34bit, valid load 32bit
tcByte TP2822_PTZ_DataSet[] = {

		//PTZ
		0x40, 0x10,
		0xc8, 0x21,
		0xe0, 0x21,
		0x40, 0x00,
		0xc8, 0x21,
		0xe0, 0x21,
		0x7e, 0x0f,
		0xb9, 0x0f,

	0xff, 0xff
};
*/

//chunyu
void I2CDeviceSet( tcByte *RegSet)
{
	int	cnt=0;
	tByte index, val;

	// Output discription
	#ifdef TASKING
	while( *RegSet ) 
		RS_tx( *RegSet++ );
	RegSet++;
	#endif

	while ( *RegSet != 0xFF ) {

		index = *RegSet;
		val = *(RegSet+1);
//		MonWriteI2C(addr, index, val);
		tp28xx_byte_write(index, val);

//		Puts("\r\n");

		RegSet+=2;

		cnt--;
	}	
    delay(50); //50*5 =250ms
}

void Init_TP280x_RegSet()
{

    tcByte *ptr_TP2802;
    tByte i;

    TVII2CAddress = 0x88;
    //TVII2CAddress = 0x8A;
    //TVII2CAddress = 0x8C;
    //TVII2CAddress = 0x8E;
#if 0//TAAH01 issue, with HDMI I2S address error ryan@20201221
    for(i=0; i<4; i++)
    {
        if(0x28 == tp28xx_byte_read(0xfe)) break;
        TVII2CAddress++;
        TVII2CAddress++;
    }
#endif
    chip = tp28xx_byte_read(0xff);

    revision = tp28xx_byte_read(0xfd);
    revision &= 0x07;

    printf("\r\nchip %02x revision %02x\r\n", (tWord)chip,(tWord)revision);
    flag_CVBS = 0;
    flag_AUDIO = 0;
    flag_MEGA = 0;
	ptr_TP2802 = TP2824C_DataSet; //def  chunyu
    switch(chip)
    {

    case TP2802C:
    case TP2804:
    case TP2822:
        printf("EOL ID\r\n");
        break;

    case TP2823:
        flag_CVBS = 1;
        flag_AUDIO = 1;
        if(0x01 == revision)
        {
            chip=TP2833;
            ptr_TP2802 = TP2833_DataSet;
        }
        else
        {
            ptr_TP2802 = TP2823_DataSet;
        }
        break;

    case TP2834:
        flag_CVBS = 1;
        flag_AUDIO = 1;
        ptr_TP2802 = TP2834_DataSet;
        break;

    case TP2853C:
        flag_CVBS = 1;
        flag_AUDIO = 1;

        if(0x01 == revision)
        {
            chip=TP2826;
            //flag_MEGA = 1;
			flag_MEGA = 0;  //chunyu 
            ptr_TP2802 = TP2826_DataSet;
        }
        else if(0x00 == revision)
        {
            flag_MEGA = 1;
            ptr_TP2802 = TP2853C_DataSet;
        }
        else
        {
            ptr_TP2802 = TP2853C_DataSet;
        }
        break;

    case TP2824C:
        flag_CVBS = 1;
        flag_AUDIO = 1;

        if(0x04 == revision)
        {
            //flag_MEGA = 1;
			flag_MEGA = 0 ; //chunyu
            ptr_TP2802 = TP2824C_DataSet;
            printf("TP2824C test\r\n");
        }
        break;

    default:		
        printf("unknown ID\r\n");
        break;
    }

    Set_PLL_StartUp();

    I2CDeviceSet( ptr_TP2802 );

    if(flag_AUDIO)	I2CDeviceSet( TP2834_Audio_DataSet );

    mode[CH1]= R1080P25;	//chunyu
    mode[CH2]= R1080P25;
   mode[CH3]= R1080P25;
   mode[CH4]= R1080P25;

	//mode[CH1]= R720P25;	
	//mode[CH1]= R1080P25;	
    Set_VidRes(CH1);
//    Set_VidRes(CH2);
//    Set_VidRes(CH3);
//    Set_VidRes(CH4);
	
    CurRes=RES1920X1080;
	//CurRes=RES1280X720;

    for( i = 0; i < CH_PER_CHIP; i++)
    {
        count[i] = 0;
        state[i] = VIDEO_UNPLUG;
        std[i] = TVI;
        mode[i] = NoDet;
    }
}


void I2CDeviceSet2( tcByte *RegSet, tcByte *DatSet)
{
	int	cnt=0;
	tByte index, val;

	while ( *RegSet != 0xFF ) {

		index = *RegSet;
		val = *DatSet;
//		MonWriteI2C(addr, index, val);
		tp28xx_byte_write(index, val);

//		Puts("\r\n");

		RegSet++;
		DatSet++;

		cnt--;
	}
    delay(50); //50*5 =250ms
}

void SetRegPage(tByte channel)
{
    //tByte tmp;
    //tmp = tp28xx_byte_read(tByte index);
    tp28xx_byte_write(0x40,channel);
    //tp28xx_byte_write(0x40,0x04); //chunyu
    //printf("\r\nChannel-%02x ",(tWord)channel);
}

/*
void Write_AllRegPage(tByte addr, tByte bitpos, tByte bitval){
	tByte ch;
	tByte val;
	tByte mask=0x00;
	tByte maskb;
	tByte Reg40;

	Reg40 = tp28xx_byte_read(TVII2CAddress,0x40);

	mask = 0x01 << bitpos;
	maskb = ~mask;

	for(ch=CH1;ch<=CH4;ch++){
		tp28xx_byte_write(TVII2CAddress,0x40,ch);
		val = tp28xx_byte_read(TVII2CAddress,addr);
		if(bitval)
			tp28xx_byte_write(TVII2CAddress,addr,(val|mask));
		else
			tp28xx_byte_write(TVII2CAddress,addr,(val&maskb));
	}

	tp28xx_byte_write(TVII2CAddress,0x40,Reg40);
}
*/

/*
void Set_16Bits_VDCLK_Out(){

	Write_AllRegPage(0x02,7,0);
	tp28xx_byte_write(TVII2CAddress,0x4b,0x40);
}

void Set_16Bits_VD_Out(tByte channel){
	tByte Reg02,Reg4b;
	tByte VD2SEL;
	tByte VD1SEL;
	tByte dat;

	Reg02 = tp28xx_byte_read(TVII2CAddress,0x02);
	Reg4b = tp28xx_byte_read(TVII2CAddress,0x4b);


	tp28xx_byte_write(TVII2CAddress,0x02,(Reg02&0x7f));

	switch (channel){
		case CH1:
			VD2SEL = VIN1_C;
			VD1SEL = VIN1_Y;
			break;
		case CH2:
			VD2SEL = VIN2_C;
			VD1SEL = VIN2_Y;
			break;
		case CH3:
			VD2SEL = VIN3_C;
			VD1SEL = VIN3_Y;
			break;
		case CH4:
			VD2SEL = VIN4_C;
			VD1SEL = VIN4_Y;
			break;
		}
	dat = (VD2SEL<<4)|VD1SEL;
	tp28xx_byte_write(TVII2CAddress,0x4b,dat);

}
*/

tcByte TP2802_FORMAT_DataSet[][12] =
{
    {0x02,0x15,0x16,0x17,0x18,0x19,0x1a,0x1c,0x1d,0x35,0x36,0xff}, //reg
    {0x48,0x03,0xd3,0x80,0x29,0x38,0x47,0x08,0x98,0x05,0xdc,0xff}, //1080p30
    {0x48,0x03,0xd3,0x80,0x29,0x38,0x47,0x0a,0x50,0x05,0xdc,0xff}, //1080p25
    {0x4a,0x13,0x16,0x00,0x19,0xd0,0x25,0x06,0x72,0x05,0xdc,0xff}, //720p60 720p30V2
    {0x4a,0x13,0x16,0x00,0x19,0xd0,0x25,0x07,0xbc,0x05,0xdc,0xff}, //720p50 720p25V2
    {0x4a,0x13,0x16,0x00,0x19,0xd0,0x25,0x0c,0xe4,0x05,0xdc,0xff}, //720p30    //5
    {0x4a,0x13,0x16,0x00,0x19,0xd0,0x25,0x0f,0x78,0x05,0xdc,0xff}, //720p25
    {0x4f,0x13,0x4e,0xbc,0x15,0xf0,0x07,0x09,0x38,0x25,0xdc,0xff}, //NTSC
    {0x4f,0x13,0x5f,0xbc,0x17,0x20,0x17,0x09,0x48,0x25,0xdc,0xff}, //PAL
    {0x48,0x13,0x6c,0x00,0x2c,0x00,0x68,0x09,0xc4,0x16,0x30,0xff}, //3M18
    {0x48,0x13,0x1f,0x20,0x34,0x98,0x7a,0x0b,0x9a,0x17,0xd0,0xff}, //5M12      //10
    {0x4a,0x13,0x16,0x00,0x19,0xd0,0x25,0x06,0x72,0x25,0xdc,0xff}, //720p30V2
    {0x4a,0x13,0x16,0x00,0x19,0xd0,0x25,0x07,0xbc,0x25,0xdc,0xff}, //720p25V2
    {0x48,0x13,0x1f,0x80,0x7d,0xf0,0x5a,0x0b,0xb8,0x16,0x72,0xff}, //4M15
    {0x48,0x13,0x6c,0x00,0x2c,0x00,0x68,0x09,0xc4,0x16,0x72,0xff}, //3M20
    {0x48,0x23,0x34,0x78,0x8c,0xf0,0x5a,0x0c,0xe4,0x17,0x08,0xff}, //4M12
    {0x48,0x13,0xec,0x80,0xb0,0x08,0x7c,0x0e,0xa6,0x17,0xbc,0xff}, //6M10
    {0x48,0x13,0x0f,0x00,0x32,0xa0,0x55,0x06,0x72,0x15,0xdc,0xff}, //4M30
    {0x48,0x13,0x20,0x00,0x20,0xa0,0x55,0x07,0xbc,0x15,0xdc,0xff}  //4M25

    //{0xc8,0x13,0x6c,0x20,0x2c,0xf0,0x5a,0x0b,0xb8,0x16,0x72,0xff}, //2592x1520p15
};

static void tp28xx_SYSCLK_V2(unsigned char ch)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(0xf5);
    tmp |= SYS_MODE[ch];
    tp28xx_byte_write(0xf5, tmp);
    tmp = tp28xx_byte_read(0x35);
    tmp |= 0x20;
    tp28xx_byte_write(0x35, tmp);
}
static void tp28xx_SYSCLK_V1(unsigned char ch)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(0xf5);
    tmp &= SYS_AND[ch];
    tp28xx_byte_write(0xf5, tmp);
    tmp = tp28xx_byte_read(0x35);
    tmp &= 0xdf;
    tp28xx_byte_write(0x35, tmp);
}

void Set_VidRes(tByte ch)
{
    tByte index, sys, tmp;
	sys = 1;  
    //printf("Set_VidRes");
    switch(mode[ch])
    {
    case R1080P30:
        index=1;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("1080p30");
        break;

    case R1080P25:
        index=2;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("1080p25");
        break;

    case R720P60:
        index=3;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p60");
        break;

    case R720P30V2:
        index=11;
        sys=2;
        ManVidRes=RES1280X720;
        printf("720p30V2");
        break;

    case R720P50:
        index=4;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p50");
        break;

    case R720P25V2:
        index=12;
        sys=2;
        ManVidRes=RES1280X720;
        printf("720p25V2");
        break;

    case R720P30:
        index=5;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p30");
        break;

    case R720P25:
        index=6;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p25");
        break;

    case PAL:
        index=8;
        sys=4;
        ManVidRes=RES1920X576;
        printf("PAL");
        break;

    case NTSC:
        index=7;
        sys=3;
        ManVidRes=RES1920X480;
        printf("NTSC");
        break;

    case R3M18:
        index=9;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("3M18");
        break;

    case R5M12:
        index=10;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("5M12");
        break;

    case R4M15:
        index=13;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("4M15");
        break;

    case R3M20:
        index=14;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("3M20");
        break;

    case R4M12:
        index=15;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("4M12");
        break;

    case R6M10:
        index=16;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("6M10");
        break;

    case R4M30:
        index=17;
        sys=1;
        ManVidRes=RES1280X720;
        printf("4M30");
        break;

    default:
        index=1;
        break;
    }
    printf("...\r\n");

    //I2CDeviceSet(TVII2CAddress, ptr_TP2802_Res );
    I2CDeviceSet2( TP2802_FORMAT_DataSet[0],TP2802_FORMAT_DataSet[index] );
    //	printf("chip= 0x%4x\r\n",chip);
    if(TP2853C == chip)
    {
        I2CDeviceSet2( TP2853C_MODE_DataSet[0],TP2853C_MODE_DataSet[sys] );
    }
    else if(TP2833 == chip)
    {
        I2CDeviceSet2( TP2833_MODE_DataSet[0],TP2833_MODE_DataSet[sys] );
    }
    else if(TP2834 == chip)
    {
        I2CDeviceSet2( TP2834_MODE_DataSet[0],TP2834_MODE_DataSet[sys] );
    }
    if(TP2826 == chip)
    {
        printf("tp2826MODEDATASET\r\n");
        I2CDeviceSet2( TP2826_MODE_DataSet[0],TP2826_MODE_DataSet[sys] );
    }
    if(TP2824C == chip)
    {
        printf("tp2824C MODEDATASET\r\n");
        I2CDeviceSet2( TP2824C_MODE_DataSet[0],TP2824C_MODE_DataSet[sys] );
    }

    if(1 == sys)
        tp28xx_SYSCLK_V1(ch);
    else
        tp28xx_SYSCLK_V2(ch);

    tp28xx_byte_write(0x13, 0x00);
    tmp=tp28xx_byte_read(0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(0x14, tmp);

#if(OUTPUT == Bits16out)
    tmp=tp28xx_byte_read(0x02);
    tmp &= 0x7f;
    tp28xx_byte_write(0x02, tmp);
#else
    tmp=tp28xx_byte_read(0x02);
    tmp |= 0x80;
    tp28xx_byte_write(0x02, tmp);
#endif
}

tByte Set_HDA(tByte ch)
{

    tByte index,tmp;
    index=0;

    switch(mode[ch])
    {

    case R1080P30:
        index=3;
        break;

    case R1080P25:
        index=4;
        break;

    case R720P30V2:
        index=1;
        break;

    case R720P25V2:
        index=2;
        break;
    }
    if(index)
    {
        Set_VidRes(ch);

        if(TP2853C == chip)
        {
            I2CDeviceSet2(TP2853C_HDA_DataSet[0],TP2853C_HDA_DataSet[index] );
        }
        else if(TP2833 == chip || TP2834 == chip )
        {
            I2CDeviceSet2(TP2833_HDA_DataSet[0],TP2833_HDA_DataSet[index] );
        }
        else if(TP2826 == chip)
        {
            I2CDeviceSet2(TP2826_HDA_DataSet[0],TP2826_HDA_DataSet[index] );
        }
        else if(TP2824C == chip)
        {
            I2CDeviceSet2(TP2824C_HDA_DataSet[0],TP2824C_HDA_DataSet[index] );
        }
        tmp=tp28xx_byte_read(0x14);
        tmp |= 0x40;
        tp28xx_byte_write(0x14, tmp);

    }
    return index;
}

tByte Set_HDC(tByte ch)
{

    tByte index;

    index=0;

    switch(mode[ch])
    {

    case R1080P30:
        index=3;
        break;

    case R1080P25:
        index=4;
        break;

    case R720P30V2:
        index=1;
        break;

    case R720P25V2:
        index=2;
        break;
    }
    if(index)
    {
        Set_VidRes(ch);

        if(TP2853C == chip)
        {
            I2CDeviceSet2( TP2853C_HDC_DataSet[0],TP2853C_HDC_DataSet[index] );
        }
        else if(TP2833 == chip || TP2834 == chip )
        {
            I2CDeviceSet2( TP2853C_HDC_DataSet[0],TP2853C_HDC_DataSet[index] );
        }
        else if(TP2826 == chip)
        {
            I2CDeviceSet2( TP2826_HDC_DataSet[0],TP2826_HDC_DataSet[index] );
        }
        else if(TP2824C == chip)
        {
            I2CDeviceSet2( TP2824C_HDC_DataSet[0],TP2824C_HDC_DataSet[index] );
        }
        tp28xx_byte_write(0x13, 0x40);
    }
    return index;
}

tByte Det_VidRes()
{
    tByte CVSTD;
    tByte RES;

    CVSTD = tp28xx_byte_read(0x03) & 0x07;

    switch (CVSTD)
    {

    case 0x00:
        RES = R720P60;
        break;
    case 0x01:
        RES = R720P50;
        break;
    case 0x02:
        RES = R1080P30;
        break;
    case 0x03:
        RES = R1080P25;
        break;
    case 0x04:
        RES = R720P30;
        break;
    case 0x05:
        RES = R720P25;
        break;
    case 0x06:
        printf("\r\nN/P detected");
        RES = NTPAL;
        break;
    default:
        printf("\r\nNo res detect...");
        RES = NoDet;
        break;
    }

    return RES;

}
tByte Det_VideoLoss(tByte ch)
{

    tByte VDLOSS;

    tp28xx_byte_write(0x40,ch&0x03);
    VDLOSS = tp28xx_byte_read(0x01);
//	printf("video status 0x%02x on ch%01x\r\n",(tWord)VDLOSS,(tWord)ch);

    if(VDLOSS)	printf("No ");

    printf("video detected");


    VDLOSS = VDLOSS>>7;
    return VDLOSS;
}

void ManualAGC(unsigned char ch)
{
    unsigned int agc;
    unsigned char tmp;
    //unsigned int i;

    tp28xx_byte_write(0x2F, 0x02);
    agc = tp28xx_byte_read(0x04);
    agc += tp28xx_byte_read(0x04);
    agc += tp28xx_byte_read(0x04);
    agc += tp28xx_byte_read(0x04);
    agc &= 0x3f0;
    agc >>=1;
    if(agc > 0x1ff) agc = 0x1ff;

    printf("result AGC=0X%04x ch%02X\r\n", (tWord)agc, (tWord)ch);
    tp28xx_byte_write(0x08, agc&0xff);
    tmp = tp28xx_byte_read(0x06);
    tmp &=0xf9;
    tmp |=(agc>>7)&0x02;
    tmp |=0x04;
    tp28xx_byte_write(0x06,tmp);
    
//test Dump Reg 
/*
    printf("DumpReg@strat\r\n") ;  
    tp28xx_byte_write(0x40,01); //
    for (i = 2 ; i< 255 ;i++)
    {
        tmp = tp28xx_byte_read((tByte)i);
        printf("88,%02X,%02X\r\n", i , (int)tmp ) ; 
    }
    printf("DumpReg@end\r\n") ; 
*/
    
}


/*
static void tp2802D_reset_default(unsigned char ch)
{
    unsigned char tmp;
    tp28xx_byte_write(0x3A, 0x01);
    tp28xx_byte_write(0x0B, 0xC0);
    tp28xx_byte_write(0x07, 0xC0);
    tp28xx_byte_write(0x2e, 0x70);
    tp28xx_byte_write(0x39, 0x42);
    tp28xx_byte_write(0x09, 0x24);
    tmp = tp28xx_byte_read(0x06);   // soft reset and auto agc when cable is unplug
    tmp &= 0x7b;
    tp28xx_byte_write(0x06, tmp);

    tmp = tp28xx_byte_read(0xf5);
    if( tmp & SYS_MODE[ch])
    {
        tmp &= SYS_AND[ch];
        tp28xx_byte_write(0xf5, tmp);
    }
}
*/
/*  //chunyu
static void tp282x_reset_AGC_CLK(unsigned char ch)
{
    unsigned char tmp;

    tmp = tp28xx_byte_read(0x06);
    tmp &= 0xfb;
    tp28xx_byte_write(0x06, tmp);

    tmp = tp28xx_byte_read(0xf5);
    tmp &= SYS_AND[ch];
    tp28xx_byte_write(0xf5, tmp);

    tp28xx_byte_write(0x35, 0x05);
    tp28xx_byte_write(0x26, 0x01);
}
*/
static void tp2824_reset_default(void)
{
    //unsigned char tmp;
    //tp28xx_byte_write(addr, 0x07, 0x40);
    //tp28xx_byte_write(addr, 0x0b, 0x40);
    //tp28xx_byte_write(addr, 0x3A, 0x88);
    tp28xx_byte_write(0x26, 0x01);
}
static void tp2853C_reset_default(void)
{
    unsigned char tmp;
    tp28xx_byte_write(0x07, 0xC0);
    tmp = tp28xx_byte_read(0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(0x14, tmp);
    tp28xx_byte_write(0x13, 0x00);
    tp28xx_byte_write(0x26, 0x04);
    tp28xx_byte_write(0x06, 0xb2);
}
static void TP28xx_reset_default(int chip, unsigned char ch)
{
    /*    if(TP2823 == chip )
    	{
               tp282x_reset_AGC_CLK(ch);
    	}
        else if(TP2822 == chip )
    	{
               tp282x_reset_AGC_CLK(ch);
    		   tp28xx_byte_write(0x07, 0x40);
    	}
        else if(TP2802D == chip )
        {
               tp2802D_reset_default( ch);
        }
    */
    if(TP2834 == chip )
    {
        tp2824_reset_default();
    }
    else if(TP2833 == chip )
    {
        tp2853C_reset_default();
    }
    else if(TP2853C == chip )
    {
        tp2853C_reset_default();
    }
	else  //chunyu
	{	
        tp2824_reset_default() ;   //chunyu
	}
}
////////////////////////////////////////
tByte ReadGain(void)
{
    tp28xx_byte_write(0x2f, 0x00);
    return tp28xx_byte_read(0x04);
}
////////////////////////////////////////
void SetCGain(unsigned char CGAIN_STD)
{
    unsigned char tmp, cgain;
    unsigned char retry = 30;

    tp28xx_byte_write(0x2f, 0x06);
    cgain = tp28xx_byte_read(0x04);

    if(cgain < CGAIN_STD )
    {
        while(retry)
        {
            retry--;

            tmp = tp28xx_byte_read(0x07);
            tmp &=0x3f;
            while(abs(CGAIN_STD-cgain))
            {
                if(tmp) tmp--;
                else break;
                cgain++;
            }

            tp28xx_byte_write(0x07, 0x80|tmp);
            if(0 == tmp) break;
            delay(8); //delay 40ms
            tp28xx_byte_write(0x2f, 0x06);
            cgain = tp28xx_byte_read(0x04);

            if(cgain > (CGAIN_STD+1))
            {
                tmp = tp28xx_byte_read(0x07);
                tmp &=0x3f;
                //tmp +=0x02;
                tmp++;
                tmp++;
                if(tmp > 0x3f) tmp = 0x3f;
                tp28xx_byte_write(0x07, 0x80|tmp);
                if(0x3f == tmp) break;
                delay(8); //delay 40ms
                cgain = tp28xx_byte_read(0x04);

            }
            if(abs(cgain-CGAIN_STD) < 2)  break;
        }

    }
}
////////////////////////////////////////
void EQ_Detect()
{
    //tByte tmpval;
    tByte cvstd,status,i;
    tByte gain, tmp, fmt/*, tmpab*/;

    for(i = 0; i < CH_PER_CHIP; i++)//scan four inputs:
    {
        SetRegPage(i);
        status = tp28xx_byte_read(0x01);

        //state machine for video checking
        if(status & FLAG_LOSS)
        {
            if(VIDEO_UNPLUG == state[i])
            {
                if(count[i] < MAX_COUNT) count[i]++;
                continue;
            }
            else
            {
                state[i] = VIDEO_UNPLUG;
                count[i] = 0;
                mode[i] = NoDet;
                TP28xx_reset_default(chip, i);
                printf("video loss ch%02x \r\n", (tWord)i);                
              	VS4210_Rx_NoSignal(1/*i*/) ; //chunyu

		TP_InputMode=7;
		
		if(SelectInputMode==_AHD_Input)
		Set_Video_Loss_OSD(1,ON);	
		//MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS);
		//MCUTimerActiveTimerEvent(SEC(3), _SYSTEM_TIMER_EVENT_VIDEO_RESET);

		OSDSecCount = 0 ;
		 OSDStateFlg = 1 ;		

            }
        }
        else
        {
            if(PAL == mode[i] || NTSC == mode[i])
                flag_LOCKED = CVBS_FLAG_LOCKED;
            else
                flag_LOCKED = TVI_FLAG_LOCKED;

            if( flag_LOCKED == (status & flag_LOCKED) ) //video locked
            {
                if(VIDEO_LOCKED == state[i])
                {
                    if(count[i] < MAX_COUNT) count[i]++;
                }
                else if(VIDEO_UNPLUG == state[i])
                {
                    state[i] = VIDEO_IN;
                    count[i] = 0;
                    printf("1video in ch%02x \r\n", (tWord)i);
                }
                else
                {
                    state[i] = VIDEO_LOCKED;
                    count[i] = 0;
                    printf("video locked ch%02x \r\n", (tWord)i);
                }
            }
            else  //video in but unlocked
            {
                if(VIDEO_UNPLUG == state[i])
                {
                    state[i] = VIDEO_IN;
                    count[i] = 0;
                    printf("video in ch%02x \r\n", (tWord)i);
                }
                else if(VIDEO_LOCKED == state[i])
                {
                    state[i] = VIDEO_UNLOCK;
                    count[i] = 0;
                    printf("video unstable ch%02x \r\n", (tWord)i);
                }
                else
                {
                    if(count[i] < MAX_COUNT) count[i]++;
                    if(VIDEO_UNLOCK == state[i] && count[i] > 2)
                    {
                        state[i] = VIDEO_IN;
                        count[i] = 0;
                        TP28xx_reset_default(chip, i);
                        printf("video unlocked ch%02x \r\n", (tWord)i);                        
                        VS4210_Rx_NoSignal(i) ;//chunyu
                    }
                }
            }
        }
        
        if( VIDEO_IN == state[i])
        {
            cvstd = tp28xx_byte_read(0x03);
            sywd = cvstd & 0x08;
            cvstd &= 0x07;
            fmt = mode[i];
            if(NTPAL == cvstd && flag_CVBS)
            {
                if(count[i] & 0x01)
                    fmt = PAL;
                else
                    fmt = NTSC;
            }
            else if(cvstd == R720P30 )
            {
                if( flag_MEGA && (count[i]&0x02) )// 3M auto detect test
                    fmt = R4M12;
                else
                    fmt = R720P30V2;
            }
            else if(cvstd == R720P25 )
            {
                fmt = R720P25V2;
            }
            else if(cvstd == R1080P30 )
            {
                if( flag_MEGA && (count[i]&0x02) )//TP2853C 3M auto detect test
                    fmt = R3M20;
                else
                    fmt = R1080P30;
            }
            else if(cvstd == R720P60 )
            {
                if( flag_MEGA && (count[i]&0x02) )//TP2853C 3M auto detect test
                    fmt = R4M30;
                else
                    fmt = R720P60;
            }
            else if(cvstd < 4 )
            {
                fmt = cvstd;
            }
            else
            {
                if( flag_MEGA )//TP2853C 3M auto detect test
                {
                    tp28xx_byte_write(0x2f, 0x09);
                    tmp = tp28xx_byte_read(0x04);
                    if((tmp > 0x48) && (tmp < 0x55))
                    {
                        fmt = R3M18;
                    }
                    else if((tmp > 0x55) && (tmp < 0x62))
                    {
                        fmt = R4M15;
                    }
                    else if((tmp > 0x70) && (tmp < 0x80))
                    {
                        fmt = R6M10;
                    }
                }
            }

            if( mode[i] != fmt )
            {
                mode[i] = fmt;
                printf("Set_VidResmode[%d] =%d\r\n",(int)i, (int)fmt);
                Set_VidRes(i);
            }
        }

        if( VIDEO_LOCKED == state[i]) //check signal lock
        {
            if(0 == count[i])
            {
                std[i] = TVI;
                CurCH = i;
                if( (R720P30V2 == mode[i]) || (R720P25V2 == mode[i]) )
                {
                    if( 0x00 == (0x08 & tp28xx_byte_read(0x03)))
                    {
                        mode[i] &= 0xf7;
                        Set_VidRes(i);
                        //printf("TVI_Set_VidRes\r\n");
                    }
                }

                //tp28xx_byte_write(0xf6  , REG_F6[i]);
                tp28xx_byte_write(0xf6 +i  , REG_F6[i]);  //chunyu

#if(OUTPUT == Bits16out)
                if( TP2826 == chip )
                {
                    tp28xx_byte_write(0xf8, REG_F7[i]);
                    tp28xx_byte_write(0xfa, REG_FA[i]);
                    tp28xx_byte_write(0xfb, REG_FA[i]);
                }
                else
                {                    
                    tp28xx_byte_write(0xf7, REG_F7[i]);
                    tp28xx_byte_write(0xfa, REG_FA[i]);
                    //printf("set output ch%02x \r\n", (tWord)CH[i]);
                }
#else //BT656																				

                tmp=tp28xx_byte_read(0xf5);
                if( tmp &= SYS_MODE[i])
                {
                    printf("//75.25M output\r\n") ;

                    if (i == 0)
                    {
                        tmpab = tp28xx_byte_read(0xfa) ;
                        tmpab = (tmpab & 0xF0) | 0x0A ;
                        tp28xx_byte_write(0xfa, tmpab);
                    }
                    else if (i == 1)
                    {
                        tmpab = tp28xx_byte_read(0xfa) ;
                        tmpab = (tmpab & 0x0F) | 0xA0 ;
                        tp28xx_byte_write(0xfa, tmpab);
                    }

                    if (i == 2)
                    {
                        tmpab = tp28xx_byte_read(0xfb) ;
                        tmpab = (tmpab & 0xF0) | 0x0A ;
                        tp28xx_byte_write(0xfb, tmpab);
                    }
                    else if (i == 3)
                    {
                        tmpab = tp28xx_byte_read(0xfb) ;
                        tmpab = (tmpab & 0x0F) | 0xA0 ;
                        tp28xx_byte_write(0xfb, tmpab);
                    }
                }
                else
                {
                    extern tByte half_scaler;
                    if(half_scaler) //down scaler output
                    {
                        tmp=tp28xx_byte_read(0x35);
                        tmp |= 0x40;
                        tp28xx_byte_write(0x35, tmp);                        

                        if (i == 0)
                        {
                            tmpab = tp28xx_byte_read(0xfa) ;
                            tmpab = (tmpab & 0xF0) | 0x0A ;
                            tp28xx_byte_write(0xfa, tmpab);
                        }
                        else if (i == 1)
                        {
                            tmpab = tp28xx_byte_read(0xfa) ;
                            tmpab = (tmpab & 0x0F) | 0xA0 ;
                            tp28xx_byte_write(0xfa, tmpab);
                        }


                        if (i == 2)
                        {
                            tmpab = tp28xx_byte_read(0xfb) ;
                            tmpab = (tmpab & 0xF0) | 0x0A ;
                            tp28xx_byte_write(0xfb, tmpab);
                        }
                        else if (i == 3)
                        {
                            tmpab = tp28xx_byte_read(0xfb) ;
                            tmpab = (tmpab & 0x0F) | 0xA0 ;
                            tp28xx_byte_write(0xfb, tmpab);
                        }

                    }
                    else
                    {
                        tmp=tp28xx_byte_read(0x35);
                        tmp &= 0xbf;
                        tp28xx_byte_write(0x35, tmp);


                        if (i == 0)
                        {
                            tmpab = tp28xx_byte_read(0xfa) ;
                            tmpab = (tmpab & 0xF0) | 0x08 ;
                            tp28xx_byte_write(0xfa, tmpab);
                        }
                        else if (i == 1)
                        {
                            tmpab = tp28xx_byte_read(0xfa) ;
                            tmpab = (tmpab & 0x0F) | 0x80 ;
                            tp28xx_byte_write(0xfa, tmpab);
                        }


                        if (i == 2)
                        {
                            tmpab = tp28xx_byte_read(0xfb) ;
                            tmpab = (tmpab & 0xF0) | 0x08 ;
                            tp28xx_byte_write(0xfb, tmpab);
                        }
                        else if (i == 3)
                        {
                            tmpab = tp28xx_byte_read(0xfb) ;
                            tmpab = (tmpab & 0x0F) | 0x80 ;
                            tp28xx_byte_write(0xfb, tmpab);
                        }
                    }
                }
#endif
                if( CurRes != ManVidRes)
                {
                    CurRes = ManVidRes;
                    //CreateMDIN340VideoInstance();  //chunyu
                    printf("ManVidRes=0x%02x\r\n",ManVidRes ) ;

                }
            }
            else if(1 == count[i])
            {
                if( chip > TP2823 )
                {
                    tmp = tp28xx_byte_read(0x01);
                    printf("Reg01 %02x\r\n", (tWord)tmp);
                    if( 0x60 == (0x64 & tmp))
                    {
                        //if(Set_HDA(mode[i])) printf("turn to A ch%02x\r\n", (tWord)i);
                        if(Set_HDC(i))
                        {
                            std[i] = HDC;
                            printf("turn to C ch%02x\r\n", (tWord)i);
                        }
                    }
                }
            }
            else if(2 == count[i])
            {
                if( chip > TP2823 )
                {
                    tmp = tp28xx_byte_read(0x01);
                    printf("Reg01 %02x\r\n", (tWord)tmp);
                    if( 0x10 != (0x11 & tmp) && HDC == std[i])
                    {
                        if(Set_HDA(i))
                        {
                            std[i] = HDA;
                            printf("turn to A ch%02x\r\n", (tWord)i);
                        }
                    }

                }

            }
            else if(3 == count[i])
            {
                if( chip > TP2823 )
                {
                    tmp = tp28xx_byte_read(0x01);
                    printf("Reg01 %02x\r\n", (tWord)tmp);
                    if( 0x00 != (0x01 & tmp))
                    {
                       // Set_VidRes(mode[i]); //chunyu
                             Set_VidRes(i); //is it right? ryan@20200904
                        printf("back to T ch%02x\r\n", (tWord)i); //chunyu          
                    }
                }
            }
            else if(count[i] < 7) //skip first 3 second
            {
                egain[i][3] = egain[i][2];
                egain[i][2] = egain[i][1];
                egain[i][1] = egain[i][0];

                gain = ReadGain();
                egain[i][0] = gain;
            }
            else if( count[i] < EQ_COUNT)
            {
                egain[i][3] = egain[i][2];
                egain[i][2] = egain[i][1];
                egain[i][1] = egain[i][0];

                gain = ReadGain();
                egain[i][0] = gain;

                if(abs(egain[i][3] - egain[i][0])< EQ_DIFF && abs(egain[i][2] - egain[i][0])< EQ_DIFF && abs(egain[i][1] - egain[i][0])< EQ_DIFF )
                {
                    count[i] = EQ_COUNT-1;
                }
            }
            else if(EQ_COUNT == count[i])
            {
                gain = ReadGain();
                printf("Egain=0x%02x ch%02x,std=%d,mode=0x%02X\r\n", (tWord)gain, (tWord)i, (tWord)std[i],(tWord)mode[i] ); //chunyu

                if ((mode[i] == R1080P30 ) || (mode[i] == R1080P25 ))
                {
                    newInRes[i] = _OSD_1920X1080P ;
                }
                else if ((mode[i] == R720P50 ) || (mode[i] == R720P60 ))
                {
                    newInRes[i] = _OSD_1280X720P ;
                }
                else if ((mode[i] == R720P25 ) || (mode[i] == R720P30 ))
                {
                    newInRes[i] = _OSD_1280X720P ;
                }
                else if ((mode[i] == R720P30V2 ) || (mode[i] == R720P25V2 ))
                {
                    newInRes[i] = _OSD_1280X720P ;
                }
                else if (mode[i] == NTSC )
                {
                    newInRes[i] = _OSD_720X480I60 ;
                }
                else if (mode[i] == PAL )
                {
                    newInRes[i] = _OSD_720X576I50 ;
                }
                else
                {
                    newInRes[i] = _OSD_No_Signal ;
                }

		 TP_InputMode=newInRes[i] ;
		if(SelectInputMode==1)
			{
		MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_VIDEO_RESET);
		//MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS);
			}

                if (newInRes[i] != gbVXIS_InputMode[i] )
                {

					  
				
                       //VS4210_SetInModeOutMode(i,newInRes[i] ,gbVXIS_OuputModeflg )  ; 20201214-01
                    printf("VS4210_SetInModeOutMode(),i=%d,newInRes=%d,OuputModeflg=%d\r\n",(int)i ,(int)newInRes[i],(int) gbVXIS_OuputModeflg ) ;

//                    tp28xx_byte_write(0xF6,0x00);
//
//                    tmpval = tp28xx_byte_read(0x40);
//                    printf("0x40=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0x4d);
//                    printf("0x4D=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0x4E);
//                    printf("0x4E=0x%02x\r\n",tmpval ) ;
//
//                    tmpval = tp28xx_byte_read(0x50);
//                    printf("0x50=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0x51);
//                    printf("0x51=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0x52);
//                    printf("0x52=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0x53);
//                    printf("0x53=0x%02x\r\n",tmpval ) ;
//
//
//                    tmpval = tp28xx_byte_read(0xF6);
//                    printf("0xF6=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0xF7);
//                    printf("0xF7=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0xF8);
//                    printf("0xF8=0x%02x\r\n",tmpval ) ;
//                    tmpval = tp28xx_byte_read(0xF9);
//                    printf("0xF9=0x%02x\r\n",tmpval ) ;
//
//                    tmpval = tp28xx_byte_read(0xFA);
//                    printf("0xFA=0x%02x\r\n",tmpval ) ;
//
//                    tmpval = tp28xx_byte_read(0xFB);
//                    printf("0xFB=0x%02x\r\n",tmpval ) ;

                }

                if( TVI != std[i] )
                {
                    tp28xx_byte_write(0x07, 0x80|(gain>>2));  // manual mode
                    if (std[i]== HDA )
                    {
                        printf("AHD Mode\r\n");
                    }
                    else
                    {
                        printf("CVI Mode\r\n");
                    }
                }
                else //TVI
                {
                    if( TP2833 == chip || TP2834 == chip)
                    {
                        if( mode[i] & FLAG_MEGA_MODE )
                        {
                            tp28xx_byte_write(0x07, 0x80|(gain/3));  // manual mode
                        }
                    }
                    printf("LINE=%d,TVI Mode\r\n",__LINE__ );
                    //gRxLoopFlg = 0 ;
                }
            }
            else if(EQ_COUNT+1 == count[i])
            {
                if( TVI != std[i] )
                {
                    ManualAGC(i);
                    SetCGain(0x0c);
                }
                else //TVI
                {
                    if( TP2833 == chip || TP2834 == chip)
                    {
                        if( mode[i] & FLAG_MEGA_MODE )
                        {
                            SetCGain(0x16);
                        }
                    }
                }
            }
        }
    }
}



tByte Det_HLock()
{
    tByte Reg01;
    tByte HLOCK;

    Reg01 = tp28xx_byte_read(0x01);
    HLOCK = (Reg01>>5)&0x01;

    if(HLOCK==1)
        return TRUE;
    else
        return FALSE;
}

/*
tByte Set_ResWithAutoDet(tByte ch){
	tByte VidRes;
	tByte cnt=5;
	SetRegPage(ch);
//	printf("\r\nReg40=%02x, Reg01=%02x",(tWord)tp28xx_byte_read(TVII2CAddress,0x40),(tWord)tp28xx_byte_read(TVII2CAddress,0x01));
	if(!Det_VideoLoss()){
		if(Det_HLock()== FALSE){
			Change_720P1080P_Det_mode();
			while(Det_HLock()== FALSE){
				printf(", H/V is not locked");
				if((cnt--)==0)
					return FALSE;
			}
		}
		VidRes = Det_VidRes();
		Set_VidRes(VidRes);

		return TRUE;

	}
	return FALSE;
}
*/


tByte Set_ResWithAutoDet(tByte ch)
{
    tByte VidRes = R1080P30 ;
    tByte cnt=5;
    SetRegPage(ch);
    //	printf("\r\nReg40=%02x, Reg01=%02x",(tWord)tp28xx_byte_read(TVII2CAddress,0x40),(tWord)tp28xx_byte_read(TVII2CAddress,0x01));
    while(cnt--)
    {
        if(!Det_VideoLoss(ch))
        {
            //for(VidRes=R720P60; VidRes<=R720P25; VidRes++)
			//{
                Set_VidRes(ch);
                //printf("\r\nVidRes=%02x, Reg01=%02x, Reg03=%02x",(tWord)VidRes,(tWord)tp28xx_byte_read(TVII2CAddress,0x01),(tWord)tp28xx_byte_read(TVII2CAddress,0x03));
                if((Det_HLock()== TRUE)&&(Det_VidRes()==VidRes))
                    return TRUE;
			//}
        }
    }
    printf("H/V is not locked\r\n");
    return FALSE;
}


tByte Scan_Channels()
{
    tByte ch;
    tByte Det_ch=0;
    tByte Sel_ch=0xff;

    for(ch = CH1; ch<=CH4; ch++)
    {
        if(Set_ResWithAutoDet(ch)&&!Det_ch)
        {
            //Sel_VD_Out(ch);
            Sel_ch = ch;
            Det_ch = 1;
        }
    }

    return Sel_ch;
}

void Set_PLL_StartUp()
{
    tByte PLL_con;
    PLL_con = tp28xx_byte_read(0x44);
    tp28xx_byte_write(0x44,(PLL_con|0x40));
    tp28xx_byte_write(0x44,(PLL_con&0xbf));

}
void ConvertData(unsigned char dat, unsigned char buf_index)
{
    unsigned char i;

    PTZ_pelco=0;

    for(i = 0; i < 8; i++)
    {
        PTZ_pelco <<= 3;

        if(0x80 & dat) PTZ_pelco |= 0x06;
        else PTZ_pelco |= 0x04;

        dat <<= 1;
    }
    PTZ_buf[buf_index]= (PTZ_pelco>>16)&0xff;
    PTZ_buf[buf_index+1]= (PTZ_pelco>>8)&0xff;
    PTZ_buf[buf_index+2]= (PTZ_pelco)&0xff;
}
unsigned char SwapBit(unsigned char dat, unsigned char buf_index)
{
    unsigned char i;
    unsigned char ret=0;
    crc=0;
    for(i = 0; i < 8; i++ )
    {
        ret >>= 1;
        if(0x80 & dat)
        {
            ret |= 0x80;
            crc +=0x80;
        }
        dat <<= 1;
    }
    PTZ_buf[buf_index]= 0x02;
    PTZ_buf[buf_index+1]= ret;
    PTZ_buf[buf_index+2]= 0x7f|crc;
    PTZ_buf[buf_index+3]= 0xff;
    PTZ_buf[buf_index+4]= 0xfc;
    return ret;
}

BYTE AHDBitsReverse(BYTE val)
{
	BYTE i;
	BYTE cnt=7;
	BYTE bitflag=0x01;
	BYTE rval=0x00;
	for(i=0;i<4;i++){
		rval|=((val>>cnt)&bitflag);
		bitflag<<=1;
		cnt-=2;
	}
	cnt=7;
	bitflag=0x80;
	for(i=0;i<4;i++){
		rval|=((val<<cnt)&bitflag);
		bitflag>>=1;
		cnt-=2;
	}
	return rval;
}

void HDC_SetData(tByte reg,tByte dat)
{
    //line1
    //tp28xx_byte_write(reg + 0 , 0x00);
    tp28xx_byte_write(reg + 0 , 0x02);
    tp28xx_byte_write(reg + 1 , SwapBit(dat,1));
    tp28xx_byte_write(reg + 2 , 0x7f|crc);
    tp28xx_byte_write(reg + 3 , 0xff);
    tp28xx_byte_write(reg + 4 , 0xfc);
}

void HDA_SetData(tByte reg,tByte dat)
{
    ConvertData(dat, 0);
    tp28xx_byte_write(reg + 0 , PTZ_buf[0]);
    tp28xx_byte_write(reg + 1 , PTZ_buf[1]);
    tp28xx_byte_write(reg + 2 , PTZ_buf[2]);
}

//20200511 COC  >>
void TVICommandToAHData(void)
{
	AHDcmd3=TVI_speed;
	AHDcmd4=presetNum;
	
	if(TVI_Command==0x17)//KEY_PRESET_CALL//"preset xx"
	{
		AHDcmd1=0x00;
		AHDcmd2=0x07;
		AHDcmd3=0;
	}
	else if(TVI_Command==0x06)//KEY_TILT_UP
	{
		AHDcmd1=0x00;
		AHDcmd2=0x08;
	}
	else if(TVI_Command==0x07)//KEY_TILT_DOWN
	{
		AHDcmd1=0x00;
		AHDcmd2=0x10;
	}
	else if(TVI_Command==0x09)//KEY_PAN_LEFT
	{
		AHDcmd1=0x00;
		AHDcmd2=0x04;
	}
	else if(TVI_Command==0x08)//KEY_PAN_RIGHT
	{
		AHDcmd1=0x00;
		AHDcmd2=0x02;
	}		
	else if(TVI_Command==0x0A)//KEY_TILT_UP + KEY_PAN_LEFT
	{
		AHDcmd1=0x00;
		AHDcmd2=0x0c;
	}
	else if(TVI_Command==0x0C)//KEY_TILT_UP + KEY_PAN_RIGHT
	{
		AHDcmd1=0x00;
		AHDcmd2=0x0a;
	}
	else if(TVI_Command==0x0B)//KEY_TILT_DOWN + KEY_PAN_LEFT
	{
		AHDcmd1=0x00;
		AHDcmd2=0x14;
	}
	else if(TVI_Command==0x0D)//KEY_TILT_DOWN + KEY_PAN_RIGHT
	{
		AHDcmd1=0x00;
		AHDcmd2=0x12;
	}		
	else if(TVI_Command==0x0F)//KEY_IRIS_OPEN / OSD_MENU
	{
		AHDcmd1=0x02;
		AHDcmd2=0x00;
	}
	else if(TVI_Command==0x0E)//KEY_IRIS_CLOSE
	{
		AHDcmd1=0x04;
		AHDcmd2=0x00;
	}
	else if(TVI_Command==0x11)//KEY_FOCUS_FAR
	{
		AHDcmd1=0x00;
		AHDcmd2=0x20;
	}
	else if(TVI_Command==0x10)//KEY_FOCUS_NEAR
	{
		AHDcmd1=0x00;
		AHDcmd2=0x40;
	}
	else if(TVI_Command==0x13)//KEY_ZOOM_TELE
	{
		AHDcmd1=0x00;
		AHDcmd2=0x80;
	}
	else if(TVI_Command==0x12)//KEY_ZOOM_WIDE
	{
		AHDcmd1=0x01;
		AHDcmd2=0x00;
	}
	else if(TVI_Command==0x14)//KEY_MOTOR_STOP
	{
		AHDcmd1=0x00;
		AHDcmd2=0x00;
		AHDcmd3=0x00;
		AHDcmd4=0x00;
	}
}

void Get_2824cIrq(void)//cocrx
{
	tByte addr,pcnt,RegB6;

	if((count[0] > (EQ_COUNT+2) && state[0] == VIDEO_LOCKED) || 
	    (count[1] > (EQ_COUNT+2) && state[1] == VIDEO_LOCKED) || 
	    (count[2] > (EQ_COUNT+2) && state[2] == VIDEO_LOCKED) ||
	    (count[3] > (EQ_COUNT+2) && state[3] == VIDEO_LOCKED))
       {	
				 
		SetRegPage(0x04);
		delay(1);//5ms

			RegB6=tp28xx_byte_read(0xB6);
			tp28xx_byte_read(0xB5);
			tp28xx_byte_read(0xB4);
			
//PTZ RX	//TVI_1080P COC
		if((RegB6&0xF0) != 0)//VnRXIRQ
		{
			//V4RXIRQ---
			if((RegB6&0x80) != 0  && count[3] > (EQ_COUNT+2)/*&&(state[3] == VIDEO_LOCKED)&&std[3]==TVI && mode[3]!=NTSC && mode[3]!=PAL*/ )
			{
				SetRegPage(0x03);
				//delay(1);//5ms
				pcnt=0;
				for(addr=0x8A;addr<=0xA1;addr++)//Line1 & Line2
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}
				tp28xx_byte_write(0xa7, 0x00);
				//delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);
				
				for(pcnt=0;pcnt<5;pcnt++)
					COCRX4[pcnt]=tpRxD[pcnt+1];
				for(pcnt=5;pcnt<10;pcnt++)
					COCRX4[pcnt]=tpRxD[pcnt+2];
		#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
			//GraphicsPrint(CYAN,"(TP_PRES_DOWN=%02x)",(WORD)TP_PRES_DOWN); 		
				printf("\r\nCOC4:");
				for(pcnt=0;pcnt<6;pcnt++)
					printf(" %02X",COCRX4[pcnt]);
				printf(" %02X",COCRX4[9]);
		#endif		
				
				//BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				//COC2: 02 B5 01 61 01 02 9E
				if(COCRX4[3]==0x61 && COCRX4[9]==0x9E )
				{
					QuadBoxId|=8;//1:ch1 / 2:ch2 / 4:ch3 / 8:ch4

				#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
					printf("QuadBoxId %02x\r\n",  QuadBoxId);
				#endif

				if(COCRX4[4]==0x38 && COCRX4[5]==0x02 )
					{
					//Flisheye_CAM[CH4]=TRUE;					
					//WriteEEP(EEP_FlishCAM4,TRUE);
					//Flisheye_VLOSS_Count[CH4]=0;
					QuadBoxId&=~(0x08);
					}
				}
			}
			
			//V3RXIRQ---
			if((RegB6&0x40) != 0  && count[2] > (EQ_COUNT+2)/*&&(state[2] == VIDEO_LOCKED)&&std[2]==TVI && mode[2]!=NTSC && mode[2]!=PAL*/)
			{
				SetRegPage(0x02);
				//delay(1);//5ms
				
				pcnt=0;
				for(addr=0x8A;addr<=0xA1;addr++)
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}
				
				tp28xx_byte_write(0xa7, 0x00);
				//delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);
				
				for(pcnt=0;pcnt<5;pcnt++)
					COCRX3[pcnt]=tpRxD[pcnt+1];
				for(pcnt=5;pcnt<10;pcnt++)
					COCRX3[pcnt]=tpRxD[pcnt+2];

				#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
				printf("\r\nCOC3:");
				for(pcnt=0;pcnt<6;pcnt++)
					printf(" %02X",COCRX3[pcnt]);
				printf(" %02X",COCRX3[9]);
				#endif

				//BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				//COC2: 02 B5 01 61 01 02 9E
				if(COCRX3[3]==0x61 && COCRX3[9]==0x9E)
				{
					QuadBoxId|=4;//1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
					#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
					printf("QuadBoxId %02x\r\n",  QuadBoxId);
					#endif

				if(COCRX3[4]==0x38 && COCRX3[5]==0x02 )
					{
					//Flisheye_CAM[CH3]=TRUE;	
					//WriteEEP(EEP_FlishCAM3,TRUE);					
					//Flisheye_VLOSS_Count[CH3]=0;
					QuadBoxId&=~(0x04);
					}
				}	
			}
			
			//V2RXIRQ---
			if((RegB6&0x20) != 0  && count[1] > (EQ_COUNT+2)/*&&(state[1] == VIDEO_LOCKED) &&std[1]==TVI && mode[1]!=NTSC && mode[1]!=PAL*/)
			{
				SetRegPage(0x01);
				//delay(1);//5ms
				
				pcnt=0;
				for(addr=0x8A;addr<=0xA1;addr++)
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}
				
				tp28xx_byte_write(0xa7, 0x00);
				//delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);
				
				for(pcnt=0;pcnt<5;pcnt++)
					COCRX2[pcnt]=tpRxD[pcnt+1];
				for(pcnt=5;pcnt<10;pcnt++)
					COCRX2[pcnt]=tpRxD[pcnt+2];

				#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
				printf("\r\nCOC2:");
				for(pcnt=0;pcnt<6;pcnt++)
					printf(" %02X",COCRX2[pcnt]);
				printf(" %02X",COCRX2[9]);
				#endif
				
				//BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				//COC2: 02 B5 01 61 01 02 9E
				if(COCRX2[3]==0x61 && COCRX2[9]==0x9E )
				{
					QuadBoxId|=2;//1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
					#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
					printf("QuadBoxId %02x\r\n",  QuadBoxId);
					#endif

				if(COCRX2[4]==0x38 && COCRX2[5]==0x02 )
					{
					//Flisheye_CAM[CH2]=TRUE;					
					//WriteEEP(EEP_FlishCAM2,TRUE);
					//Flisheye_VLOSS_Count[CH2]=0;
					QuadBoxId&=~(0x02);
					}
				}	
			}
			
			//V1RXIRQ---
			if((RegB6&0x10) != 0  && count[0] > (EQ_COUNT+2)/*&&(state[0] == VIDEO_LOCKED) &&std[0]==TVI && mode[0]!=NTSC && mode[0]!=PAL*/)
			{
				SetRegPage(0x00);
				//delay(1);//5ms
				
				pcnt=0;
				for(addr=0x8A;addr<=0xA1;addr++)
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}
				
				tp28xx_byte_write(0xa7, 0x00);
				//delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);
				
				for(pcnt=0;pcnt<5;pcnt++)
					COCRX1[pcnt]=tpRxD[pcnt+1];
				for(pcnt=5;pcnt<10;pcnt++)
					COCRX1[pcnt]=tpRxD[pcnt+2];
				
				#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
				printf("\r\nCOC1:");
				for(pcnt=0;pcnt<6;pcnt++)
					printf(" %02X",COCRX1[pcnt]);
				printf(" %02X",COCRX1[9]);
				#endif
				
				//BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				//COC2: 02 B5 01 61 01 02 9E
				if(COCRX1[3]==0x61 && COCRX1[9]==0x9E )
				{
					QuadBoxId|=1;//1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
					#if(_DEBUG_MESSAGE_Get_2824cIrq==ON)
					printf("QuadBoxId %02x\r\n",  QuadBoxId);
					#endif

				if(COCRX1[4]==0x38 && COCRX1[5]==0x02)
					{
					//Flisheye_CAM[CH1]=TRUE;
					//WriteEEP(EEP_FlishCAM1,TRUE);
					//Flisheye_VLOSS_Count[CH1]=0;
					QuadBoxId&=~(0x01);
					}
				}	
			}		
			//printf("\r\n");
			
		}
//PTZ TX	
		//else
		if(ptztxflag==1)//no any RXIRQ
		{
			if( TVI == std[PtzTxChNum] )
			{
				if(mode[PtzTxChNum]==NTSC || mode[PtzTxChNum]==PAL)
					{
						tByte cmd1=0,cmd2=0,dat1=0,dat2=0;
						
						dat1=(tByte)TVI_speed;
						dat2=(tByte)presetNum;
						
						if(TVI_Command==0x0F)//SET//Iris +
							cmd1=0x02;
						else if(TVI_Command==0x0E)//Iris -
							cmd1=0x04;
						else if(TVI_Command==0x10)//Focus -
							cmd1=0x01;
						else if(TVI_Command==0x11)//Focus +
							cmd2=0x80;
						else if(TVI_Command==0x12)//Zoom -
							cmd2=0x40;
						else if(TVI_Command==0x13)//Zoom +
							cmd2=0x20;
						else if(TVI_Command==0x06)//up
							cmd2=0x08;
						else if(TVI_Command==0x07)//down
							cmd2=0x10;
						else if(TVI_Command==0x09)//left
							cmd2=0x04;
						else if(TVI_Command==0x08)//right
							cmd2=0x02;
						else if(TVI_Command==0x0A)//up + left
							cmd2=0x0c;
						else if(TVI_Command==0x0C)//up + right
							cmd2=0x0a;
						else if(TVI_Command==0x0B)//down + left
							cmd2=0x14;
						else if(TVI_Command==0x0D)//down + right
							cmd2=0x12;
						else if(TVI_Command==0x17)//PRESET x
						{
							cmd2=0x07;
							dat1=0;
							dat2=(tByte)presetNum;
						}
						//set:0x0f / dn:0x06 / up:0x07 / L:0x13 / R:0x12
						cmd1=AHDBitsReverse(cmd1);//20200413
						cmd2=AHDBitsReverse(cmd2);//20200413
						dat1=AHDBitsReverse(dat1);//20200413
						dat2=AHDBitsReverse(dat2);//20200413
						
						SetRegPage(PtzTxChNum);		                    
						tp28xx_byte_write(0x5a, cmd1);//command byte 1
						tp28xx_byte_write(0x59, cmd2);//command byte 2

						tp28xx_byte_write(0x60, dat1);//data byte 1
						tp28xx_byte_write(0x5f, dat2);//data byte 2

						tp28xx_byte_write(0x6f, 0x61);
						delay(10);//50ms
						tp28xx_byte_write(0x6f, 0x60);
						printf("CVBS-Tx %x,%x,%x,%x\n",cmd1,cmd2,dat1,dat2);

					}
					else//1080P
					{
						SetRegPage(PtzTxChNum);		                    
						tp28xx_byte_write(0x59, (tByte)TVI_Command);
						tp28xx_byte_write(0x5a, (tByte)presetNum);
						tp28xx_byte_write(0x5d, (tByte)TVI_speed);//0x5f  check this????
						tp28xx_byte_write(0x60, (tByte)TVI_Chksum);
						
						tp28xx_byte_write(0x6f, 0x41);
						delay(10);//50ms
						tp28xx_byte_write(0x6f, 0x40);
						printf("TVI-Tx %x,%x,%x\n",TVI_Command,presetNum,TVI_speed);
					}
					
					ptztxflag=0;
				}
			if( HDC == std[PtzTxChNum] )//1080P / 720P
				{
					SetRegPage(PtzTxChNum);	

					tp28xx_byte_write(0x6e, 0xA5);
					tp28xx_byte_write(0x6e, 0x02);
					switch(TVI_Command)
					{
						case 0x0F://SET//Iris +
							tp28xx_byte_write(0x6e, 0x50);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0x00);
							break;
							
						case 0x06://up
							tp28xx_byte_write(0x6e, 0x08);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0xFF);
							tp28xx_byte_write(0x6e, 0x00);
							break;
							
						case 0x07://down
							tp28xx_byte_write(0x6e, 0x04);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0xFF);
							tp28xx_byte_write(0x6e, 0x00);
							break;
							
						case 0x09://left
							tp28xx_byte_write(0x6e, 0x02);
							tp28xx_byte_write(0x6e, 0xFF);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0x00);
							break;
							
						case 0x08://right
							tp28xx_byte_write(0x6e, 0x01);
							tp28xx_byte_write(0x6e, 0xFF);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0x00);
							break;
							
						default:	
						case 0x14://STOP
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0x00);
							tp28xx_byte_write(0x6e, 0x00);
							break;
					}
					
					tp28xx_byte_write(0x6f, 0x49);
					delay(5);//50ms
					tp28xx_byte_write(0x6f, 0x48);
					ptztxflag=0;
					printf("CVI-Tx %x\n",TVI_Command);
				}
			if( HDA == std[PtzTxChNum] )//1080P / 720P
			{
					TVICommandToAHData();
				
				  	SetRegPage(PtzTxChNum);		

					//stop command
					tp28xx_byte_write(0x5a, 0x00);
					tp28xx_byte_write(0x60, 0x00);
					tp28xx_byte_write(0x66, 0x00);
					tp28xx_byte_write(0x6c, 0x00);

					tp28xx_byte_write(0x6f, 0x45);
					delay(5);//50ms
					tp28xx_byte_write(0x6f, 0x44);

					//command data
					tp28xx_byte_write(0x5a, AHDBitsReverse(AHDcmd1));
					tp28xx_byte_write(0x60, AHDBitsReverse(AHDcmd2));
					tp28xx_byte_write(0x66, AHDBitsReverse(AHDcmd3));
					tp28xx_byte_write(0x6c, AHDBitsReverse(AHDcmd4));

					tp28xx_byte_write(0x6f, 0x45);
					delay(5);//50ms
					tp28xx_byte_write(0x6f, 0x44);
					ptztxflag=0;
				printf("AHD %x,%x,%x to ",TVI_Command,presetNum,TVI_speed);
				printf("Tx %x,%x,%x,%x\n",AHDcmd1,AHDcmd2,AHDcmd3,AHDcmd4);
				}
				ptztxflag=0;
			}
		}
}

/* //chunyu
void StartTX(void)
{
    unsigned char tmp,i;

    tp28xx_byte_write(0xB6, (0x01<<CurCH)); //clear flag

    if(TP2826 == chip)
    {
        tmp = tp28xx_byte_read(0x6f);
        tmp |= 0x01;
        tp28xx_byte_write(0x6f, tmp); //TX enable
        tmp &= 0xfe;
        tp28xx_byte_write(0x6f, tmp); //TX disable
    }
    else
    {
        tp28xx_byte_write(0xBB, (0x01<<CurCH));
    }

    for(i = 0; i <20; i++) //wait TX end
    {
        tmp = tp28xx_byte_read(0xb6);
        if( (0x01<<CurCH) & tmp) break;
        delay(2);
    }
    if(TP2826 != chip) tp28xx_byte_write(0xBB, 0x00);
}

//ch 0~3 for channel select
//bank 0x00-> 1&2 lines 0x10-> 3&4 lines

void PTZ_SetData(tByte ch,tByte bank)
{

    tp28xx_byte_write(0x40, bank); //data buffer bank1 switch for 282x


    //line1
    tp28xx_byte_write(0x7f + ch*2 , PTZ_buf[0]);
    tp28xx_byte_write(0x56 + ch*10 , PTZ_buf[1]);
    tp28xx_byte_write(0x57 + ch*10 , PTZ_buf[2]);
    tp28xx_byte_write(0x58 + ch*10 , PTZ_buf[3]);
    tp28xx_byte_write(0x59 + ch*10 , PTZ_buf[4]);
    tp28xx_byte_write(0x5A + ch*10 , PTZ_buf[5]);

    //line2
    tp28xx_byte_write(0x80 + ch*2 , PTZ_buf[6]);
    tp28xx_byte_write(0x5B + ch*10 , PTZ_buf[7]);
    tp28xx_byte_write(0x5C + ch*10 , PTZ_buf[8]);
    tp28xx_byte_write(0x5D + ch*10 , PTZ_buf[9]);
    tp28xx_byte_write(0x5E + ch*10 , PTZ_buf[10]);
    tp28xx_byte_write(0x5F + ch*10 , PTZ_buf[11]);
}

void Burst_SetData(tByte *buf)
{
    PTZ_buf[0] = 0x00;
    PTZ_buf[1] = 0x03;
    PTZ_buf[2] = buf[0];
    PTZ_buf[3] = buf[1];
    PTZ_buf[4] = buf[2];
    PTZ_buf[5] = buf[3];
    PTZ_buf[6] = 0x00;
    PTZ_buf[7] = 0x03;
    PTZ_buf[8] = buf[4];
    PTZ_buf[9] = buf[5];
    PTZ_buf[10] = buf[6];
    PTZ_buf[11] = buf[7];
}

//////////////////////////////////////////////////////////////////////////////
//PTZ TX
//////////////////////////////////////////////////////////////////////////////

void MonTX(tByte cmd, tByte data1, tByte data2, tByte data3, tByte data4)
{
    unsigned char sum,tmp,index;
    unsigned char i;
    unsigned char page_bak;

    tcByte PTZ_dat[8][7]=
    {
        {0x0b,0x0c,0x0d,0x0e,0x19,0x78,0x21}, //TVI1.0
        {0x0b,0x0c,0x0d,0x0e,0x33,0xf0,0x21}, //TVI2.0
        //{0x42,0x00,0x00,0x00,0x00,0x0e,0x0f,0x10,0x11,0x24,0xf0,0x57}, //A1080p for 2826
        {0x0e,0x0f,0x10,0x11,0x24,0xf0,0x57}, //ACP2.0 for 2826
        //{0x46,0x1b,0x00,0x00,0x00,0x0e,0x0f,0x10,0x11,0x24,0xf0,0x57}, //A1080p for 2826
        {0x0e,0x0f,0x00,0x00,0x24,0xe0,0xef}, //ACP1.0 for 2826
        {0x0f,0x10,0x00,0x00,0x48,0xf0,0x6f}, //960H for 2826
        {0x10,0x11,0x12,0x13,0x15,0xb8,0xa1}, //HDC 2826
        {0x10,0x11,0x12,0x13,0x95,0xb8,0x21}, //HDC 2833B
        {0x0e,0x0f,0x10,0x11,0x64,0xf0,0x17} //A2.0 for 2833B
    };
    tcByte PTZ_reg[2][7]=
    {
        {0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8},
        {0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0}
    };

    //unsigned char ch = CurCH; //for only single channel display

    page_bak = tp28xx_byte_read(0x40);

    for(i = 0; i < 12; i++)
    {
        PTZ_buf[i]=0x00; //clear buf
    }

    sum = data1 + data2 + data3 + data4;

    switch(cmd)
    {
    case 0xff:
        if( chip < TP2853C) index = 7;
        else 								index = 2;
        break;
    case 0xfe:
        index = 3;
        break;
    case 0xfd:
        index = 4;
        break;
    case 0xfc:
        if( chip < TP2853C) index = 6;
        else 								index = 5;
        sum += 0xa6; //0xa5+0x01
        break;
    default:
        tmp = tp28xx_byte_read(0xf5); //check TVI 1 or 2
        tmp &= SYS_MODE[CurCH];
        if( tmp )          index = 1;
        else               index = 0;

        sum += 0xb5;
        sum += cmd;
        PTZ_buf[1]=0x02;
        PTZ_buf[2]=0xB5;
        PTZ_buf[4]=cmd;
        PTZ_buf[5]=data1;

        PTZ_buf[7]=0x02;
        PTZ_buf[8]=data2;
        PTZ_buf[9]=data3;
        PTZ_buf[10]=data4;
        PTZ_buf[11]=sum;
        break;
    };



    if( TP2826 == chip )
    {
        tp28xx_byte_write(0x40, CurCH);
        printf("CH %x\r\n", (tWord)CurCH);

        tp28xx_byte_write(0x6f, 0x42);
        tp28xx_byte_write(0x71, 0x20);//sayid
        for(i = 0; i < 7; i++)
        {
            tp28xx_byte_write(PTZ_reg[0][i], PTZ_dat[index][i]);
        }
        for(i = 0; i < 24; i++)
        {
            tp28xx_byte_write(0x55+i, 0x00);
        }
        if(0xfc == cmd) //HDC 1080p
        {
            HDC_SetData(0x56, 0xa5);
            HDC_SetData(0x5c, 0x01);
            HDC_SetData(0x62, data1);
            HDC_SetData(0x68, data2);
            StartTX();
            HDC_SetData(0x56, data3);
            HDC_SetData(0x5c, data4);
            HDC_SetData(0x62, sum);
            HDC_SetData(0x68, 0xff);
            //StartTX();
        }
        else if(0xff == cmd) //HDA 1080p
        {
            HDA_SetData(0x58, 0x00);
            HDA_SetData(0x5e, 0x00);
            HDA_SetData(0x64, 0x00);
            HDA_SetData(0x6a, 0x00);
            StartTX();
            HDA_SetData(0x58, data1);
            HDA_SetData(0x5e, data2);
            HDA_SetData(0x64, data3);
            HDA_SetData(0x6a, data4);
            //StartTX();
        }
        else if(0xfe == cmd ||0xfd == cmd) //HDA 720p/960H
        {
            HDA_SetData(0x55, data1);
            HDA_SetData(0x58, data2);
            HDA_SetData(0x5b, data3);
            HDA_SetData(0x5e, data4);
            //StartTX();
        }
        else //TVI
        {
            for(i = 0; i < 12; i++)
            {
                tp28xx_byte_write(0x55+i , PTZ_buf[i]); //line1&2
                tp28xx_byte_write(0x61+i , PTZ_buf[i]); //line3&4
            }
        }
        StartTX();
    }
    else
    {
        tcByte PTZ_bank[4]= {0x00,0x00,0x10,0x10};
        tp28xx_byte_write(0x40, PTZ_bank[CurCH]); //channel 1,2

        for(i = 0; i < 7; i++)
        {
            tp28xx_byte_write(PTZ_reg[CurCH>>1][i], PTZ_dat[index][i]);
        }

        tp28xx_byte_write(0x7e, 0x0f); //TXDx EN

        if(0xfc == cmd) //HDC 1080p
        {
            SwapBit(data1, 1);
            SwapBit(data2, 7);
            PTZ_SetData(CurCH, 0x10); //3&4 lines

            SwapBit(0xa5, 1);
            SwapBit(0x01, 7);
            PTZ_SetData(CurCH, 0x00); //1&2 lines
            StartTX();

            SwapBit(sum, 1);
            SwapBit(0xff, 7);
            PTZ_SetData(CurCH, 0x10); //3&4 lines

            SwapBit(data2, 1);
            SwapBit(data3, 7);
        }
        else if(0xff == cmd) //HDA 1080p
        {
            ConvertData(0x00, 3);
            ConvertData(0x00, 9);
            PTZ_SetData(CurCH, 0x10); //3&4 lines
            ConvertData(0x00, 3);
            ConvertData(0x00, 9);
            PTZ_SetData(CurCH, 0x00); //1&2 lines
            StartTX();

            ConvertData(data3, 3);
            ConvertData(data4, 9);
            PTZ_SetData(CurCH, 0x10); //3&4 lines
            ConvertData(data1, 3);
            ConvertData(data2, 9);
        }
        else if(0xfe == cmd ||0xfd == cmd) //HDA 720p/960H
        {
            ConvertData(data1, 0);
            ConvertData(data2, 3);
            ConvertData(data3, 6);
            ConvertData(data4, 9);
        }
        else //TVI
        {
            //PTZ_SetData(ch,0x10); //line3&4
        }

        PTZ_SetData(CurCH,0x00); //line1&2
        StartTX();

    }
    tp28xx_byte_write(0x40, page_bak);
}

//////////////////////////////////////////////////////////////////////////////
//PTZ RX
//////////////////////////////////////////////////////////////////////////////

void SetRX(tByte mode, tByte ch)
{
    unsigned char tmp,index;
    unsigned char i;

    static unsigned char page_bak;


    tcByte PTZ_dat[2][15]=
    {
        {0x00,0x00,0x07,0x08,0x00,0x00,0x04,0x00,0x00,0x60,0x10,0x06,0xbe,0x39,0x27}, //TVI
        {0x00,0x00,0x06,0x07,0x08,0x09,0x03,0x48,0x34,0x60,0x10,0x04,0xf0,0xd8,0x17} //AHD 1080p
    };

    page_bak = tp28xx_byte_read(0x40);

    tp28xx_byte_write(0x40, ch);

    switch(mode)
    {
    case HDA:
        index = 1;
        break;
    default:
        index = 0;
        break;
    };

    for(i = 0; i < 15; i++)
    {
        tp28xx_byte_write(0xc9+i, PTZ_dat[index][i]);
    }
    tp28xx_byte_write(0xa8, 0x00);
    tp28xx_byte_write(0xa7, 0x03);

    tp28xx_byte_write(0x40, page_bak);


}

void Menu(void)
{
    unsigned char i;
    tcByte menu[][3]=
    {
        {0x17,0x5f,0x00},
        {0xff,0x02,0x00},
        {0xfc,0x89,0x08}
    };

    i = std[CurCH];

    MonTX(menu[i][0], menu[i][1], menu[i][2], 0x00, 0x00);
    //if(HDC == i) MonTX(menu[3][0], menu[3][1], menu[3][2], 0x00, 0x00);
}
void Left(void)
{
    unsigned char i;
    tcByte menu[][3]=
    {
        {0x09,0x00,0x00},
        {0xff,0x00,0x04},
        {0xfc,0x89,0x06}
    };

    i = std[CurCH];
    MonTX(menu[i][0], menu[i][1], menu[i][2], 0x00, 0x00);
}
void Right(void)
{
    unsigned char i;
    tcByte menu[][3]=
    {
        {0x08,0x00,0x00},
        {0xff,0x00,0x02},
        {0xfc,0x89,0x07}
    };

    i = std[CurCH];
    MonTX(menu[i][0], menu[i][1], menu[i][2], 0x00, 0x00);
}
void Up(void)
{
    unsigned char i;
    tcByte menu[][3]=
    {
        {0x06,0x00,0x00},
        {0xff,0x00,0x08},
        {0xfc,0x89,0x04}
    };

    i = std[CurCH];
    MonTX(menu[i][0], menu[i][1], menu[i][2], 0x00, 0x00);
}

void Down(void)
{
    unsigned char i;
    tcByte menu[][3]=
    {
        {0x07,0x00,0x00},
        {0xff,0x00,0x10},
        {0xfc,0x89,0x05}
    };

    i = std[CurCH];
    MonTX(menu[i][0], menu[i][1], menu[i][2], 0x00, 0x00);
}

  //chunyu
void BurstTX(tByte* buf)
{
    tByte i,j,page_bak;
    tByte *p;

    //p = xbuff+3;
    p = buf;

    page_bak = tp28xx_byte_read(0x40);
    if(TP2826 == chip)
    {
        tcByte TX_ADDR[16]= {0x57,0x58,0x59,0x5a,0x5d,0x5e,0x5f,0x60,0x63,0x64,0x65,0x66,0x69,0x6a,0x6b,0x6c};

        tp28xx_byte_write(0x40, CurCH);

        for(i = 0; i < 4; i++  )
        {
            tp28xx_byte_write(0x55+i*6, 0x00);
            tp28xx_byte_write(0x56+i*6, 0x03);
        }
        for(j = 0; j < 8; j++)
        {
            for(i = 0; i < 16; i++  )
            {
                tp28xx_byte_write(TX_ADDR[i], *p);
                //tp28xx_byte_write(TX_ADDR[i], 16*j+i);
                p++;
            }
            StartTX();
            delay(10); //50ms
        }
    }
    else
    {
        for(j = 0; j < 8; j++)
        {
            Burst_SetData(p);
            PTZ_SetData(CurCH, 0x00); //1&2 lines
            p +=8;
            Burst_SetData(p);
            PTZ_SetData(CurCH, 0x10); //3&4 lines
            p +=8;
            StartTX();
            delay(10); //50ms
        }
    }
    tp28xx_byte_write(0x40, page_bak);
}
*/
#if 0
void TP_bt1120_720P_1080P_mode0_w0(void)
{
	
	Printf("\r\nLoading VS4210 AHD table 720P... ");
	
	VXISI2CWrite(0x01,0x00);
	VXISI2CWrite(0x02,0x00);
	VXISI2CWrite(0x03,0x75);
	VXISI2CWrite(0x04,0x18);
	VXISI2CWrite(0x05,0x80);
	VXISI2CWrite(0x06,0x20);
	VXISI2CWrite(0x07,0x08);
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
	VXISI2CWrite(0x12,0x52);
	VXISI2CWrite(0x13,0x4B);
	VXISI2CWrite(0x14,0xCF);
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
	VXISI2CWrite(0x1F,0x62);
	VXISI2CWrite(0x20,0x72);
	VXISI2CWrite(0x21,0xEE);
	VXISI2CWrite(0x22,0x75);
	VXISI2CWrite(0x23,0xBC);
	VXISI2CWrite(0x24,0x00);
	VXISI2CWrite(0x25,0x22);
	VXISI2CWrite(0x26,0xEE);
	VXISI2CWrite(0x27,0xD0);
	VXISI2CWrite(0x28,0xF0);
	VXISI2CWrite(0x29,0x00);
	VXISI2CWrite(0x2A,0x00);
	VXISI2CWrite(0x2B,0x00);
	VXISI2CWrite(0x2C,0x00);
	VXISI2CWrite(0x2D,0x00);
	VXISI2CWrite(0x2E,0x25);
	VXISI2CWrite(0x2F,0x00);
	VXISI2CWrite(0x30,0xD0);
	VXISI2CWrite(0x31,0x80);
	//VXISI2CWrite(0x32,0x80);
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
	VXISI2CWrite(0x40,0x80);
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
	VXISI2CWrite(0x93,0x62);
	VXISI2CWrite(0x94,0x78);
	VXISI2CWrite(0x95,0x00);
	VXISI2CWrite(0x96,0xEC);
	VXISI2CWrite(0x97,0x50);
	VXISI2CWrite(0x98,0x1A);
	VXISI2CWrite(0x99,0x90);
	VXISI2CWrite(0x9A,0x09);
	VXISI2CWrite(0x9B,0x04);
	//VXISI2CWrite(0x9C,0x0A);
	VXISI2CWrite(0x9D,0x00);
	VXISI2CWrite(0x9E,0x04);
	VXISI2CWrite(0x9F,0x65);
	VXISI2CWrite(0xA0,0x55);
	VXISI2CWrite(0xA1,0xD8);
	VXISI2CWrite(0xA2,0x98);
	VXISI2CWrite(0xA3,0x87);
	VXISI2CWrite(0xA4,0x44);
	VXISI2CWrite(0xA5,0x55);
	VXISI2CWrite(0xA6,0x19);
	VXISI2CWrite(0xA7,0x70);
	VXISI2CWrite(0xA8,0x40);
	VXISI2CWrite(0xA9,0x1E);
	VXISI2CWrite(0xAA,0x2E);
	VXISI2CWrite(0xAB,0x00);
	VXISI2CWrite(0xAC,0x88);
	VXISI2CWrite(0xAD,0xEC);
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
	VXISI2CWrite(0xD7,0x10);
	VXISI2CWrite(0xD8,0x00);
	VXISI2CWrite(0xDC,0x77);
	VXISI2CWrite(0xDD,0x01);
	VXISI2CWrite(0xDE,0xA5);
	VXISI2CWrite(0xDF,0xE5);
	VXISI2CWrite(0xE0,0x00);
	VXISI2CWrite(0xE1,0x00);
	VXISI2CWrite(0xE2,0x00);
	VXISI2CWrite(0xE3,0x25);
	VXISI2CWrite(0xE4,0xD2);
	VXISI2CWrite(0xE5,0x00);
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
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x40);
	VXISI2CWrite(0xFF,0x07);
	

}
#endif
#if 0
void TP_bt1120_1080P_1080P_mode0_w0(void)
{
	Printf("\r\nLoading VS4210 AHD table 1080P... ");

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


}
#endif
#if 0
void TP_bt1120_NTSC_1080P_mode0_w0(void)
{
	Printf("\r\nLoading VS4210 NTSC  table... ");

	VXISI2CWrite(0x01,0x00);
	VXISI2CWrite(0x02,0x00);
	VXISI2CWrite(0x03,0x75);
	VXISI2CWrite(0x04,0x18);
	VXISI2CWrite(0x05,0x80);
	VXISI2CWrite(0x06,0x20);
	VXISI2CWrite(0x07,0x08);
	VXISI2CWrite(0x08,0x21);
	VXISI2CWrite(0x09,0x0A);
	VXISI2CWrite(0x0A,0x21);
	VXISI2CWrite(0x0B,0x0A);
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
	VXISI2CWrite(0x16,0xFE);
	VXISI2CWrite(0x17,0x00);
	VXISI2CWrite(0x18,0xC0);
	VXISI2CWrite(0x19,0x01);
	VXISI2CWrite(0x1A,0x30);
	VXISI2CWrite(0x1B,0x04);
	VXISI2CWrite(0x1C,0x00);
	VXISI2CWrite(0x1D,0x00);
	VXISI2CWrite(0x1E,0x20);
	VXISI2CWrite(0x1F,0x31);
	VXISI2CWrite(0x20,0x60);
	VXISI2CWrite(0x21,0x39);
	VXISI2CWrite(0x22,0x97);
	VXISI2CWrite(0x23,0x37);
	VXISI2CWrite(0x24,0xBC);
	VXISI2CWrite(0x25,0x10);
	VXISI2CWrite(0x26,0x07);
	VXISI2CWrite(0x27,0xF0);
	VXISI2CWrite(0x28,0xF0);
	VXISI2CWrite(0x29,0x00);
	VXISI2CWrite(0x2A,0x00);
	VXISI2CWrite(0x2B,0x00);
	VXISI2CWrite(0x2C,0xFF);
	VXISI2CWrite(0x2D,0x00);
	VXISI2CWrite(0x2E,0x07);
	VXISI2CWrite(0x2F,0xBC);
	VXISI2CWrite(0x30,0xF0);
	VXISI2CWrite(0x31,0x80);
	//VXISI2CWrite(0x32,0x80);
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
	VXISI2CWrite(0x40,0x80);
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
	VXISI2CWrite(0x85,0xFC);
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
	VXISI2CWrite(0x93,0x91);
	VXISI2CWrite(0x94,0xDA);
	VXISI2CWrite(0x95,0xBC);
	VXISI2CWrite(0x96,0xEB);
	VXISI2CWrite(0x97,0x70);
	VXISI2CWrite(0x98,0x10);
	VXISI2CWrite(0x99,0x90);
	VXISI2CWrite(0x9A,0x09);
	VXISI2CWrite(0x9B,0x04);
	//VXISI2CWrite(0x9C,0x0A);
	VXISI2CWrite(0x9D,0x00);
	VXISI2CWrite(0x9E,0x04);
	VXISI2CWrite(0x9F,0x65);
	VXISI2CWrite(0xA0,0x02);
	VXISI2CWrite(0xA1,0xD6);
	VXISI2CWrite(0xA2,0x98);
	VXISI2CWrite(0xA3,0x87);
	VXISI2CWrite(0xA4,0xD4);
	VXISI2CWrite(0xA5,0x37);
	VXISI2CWrite(0xA6,0xED);
	VXISI2CWrite(0xA7,0x54);
	VXISI2CWrite(0xA8,0x40);
	VXISI2CWrite(0xA9,0x1E);
	VXISI2CWrite(0xAA,0x2E);
	VXISI2CWrite(0xAB,0x00);
	VXISI2CWrite(0xAC,0x84);
	VXISI2CWrite(0xAD,0xEB);
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
	VXISI2CWrite(0xDF,0xA1);
	VXISI2CWrite(0xE0,0x00);
	VXISI2CWrite(0xE1,0x00);
	VXISI2CWrite(0xE2,0x00);
	VXISI2CWrite(0xE3,0x17);
	VXISI2CWrite(0xE4,0xDB);
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
	VXISI2CWrite(0xF0,0x31);
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

void TP_bt1120_PAL_1080P_mode0_w0(void)
{
	Printf("\r\nLoading VS4210 PAL table... ");
	
	VXISI2CWrite(0x01,0x00);
	VXISI2CWrite(0x02,0x00);
	VXISI2CWrite(0x03,0x75);
	VXISI2CWrite(0x04,0x18);
	VXISI2CWrite(0x05,0x80);
	VXISI2CWrite(0x06,0x20);
	VXISI2CWrite(0x07,0x08);
	VXISI2CWrite(0x08,0x21);
	VXISI2CWrite(0x09,0x0A);
	VXISI2CWrite(0x0A,0x21);
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
	VXISI2CWrite(0x16,0xF8);
	VXISI2CWrite(0x17,0x01);
	VXISI2CWrite(0x18,0xC0);
	VXISI2CWrite(0x19,0x01);
	VXISI2CWrite(0x1A,0x30);
	VXISI2CWrite(0x1B,0x04);
	VXISI2CWrite(0x1C,0x00);
	VXISI2CWrite(0x1D,0x00);
	VXISI2CWrite(0x1E,0x20);
	VXISI2CWrite(0x1F,0x31);
	VXISI2CWrite(0x20,0x60);
	VXISI2CWrite(0x21,0x39);
	VXISI2CWrite(0x22,0x97);
	VXISI2CWrite(0x23,0x38);
	VXISI2CWrite(0x24,0xBC);
	VXISI2CWrite(0x25,0x10);
	VXISI2CWrite(0x26,0x06);
	VXISI2CWrite(0x27,0xF0);
	VXISI2CWrite(0x28,0xF0);
	VXISI2CWrite(0x29,0x00);
	VXISI2CWrite(0x2A,0x00);
	VXISI2CWrite(0x2B,0x00);
	VXISI2CWrite(0x2C,0xFF);
	VXISI2CWrite(0x2D,0x00);
	VXISI2CWrite(0x2E,0x07);
	VXISI2CWrite(0x2F,0xBC);
	VXISI2CWrite(0x30,0xF0);
	VXISI2CWrite(0x31,0x80);
//	VXISI2CWrite(0x32,0x80);
	VXISI2CWrite(0x33,0x80);
	VXISI2CWrite(0x34,0xA0);
	VXISI2CWrite(0x35,0x18);
	VXISI2CWrite(0x36,0x46);
	VXISI2CWrite(0x37,0x64);
	VXISI2CWrite(0x38,0x00);
	VXISI2CWrite(0x39,0xFF);
	VXISI2CWrite(0x3A,0x32);
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
	VXISI2CWrite(0x84,0x51);
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
	VXISI2CWrite(0x93,0x81);
	VXISI2CWrite(0x94,0xC9);
	VXISI2CWrite(0x95,0xBC);
	VXISI2CWrite(0x96,0xF5);
	VXISI2CWrite(0x97,0x70);
	VXISI2CWrite(0x98,0x15);
	VXISI2CWrite(0x99,0x90);
	VXISI2CWrite(0x9A,0x00);
	VXISI2CWrite(0x9B,0x04);
	//VXISI2CWrite(0x9C,0x0A);
	VXISI2CWrite(0x9D,0x00);
	VXISI2CWrite(0x9E,0x04);
	VXISI2CWrite(0x9F,0x65);
	VXISI2CWrite(0xA0,0x03);
	VXISI2CWrite(0xA1,0xD0);
	VXISI2CWrite(0xA2,0x98);
	VXISI2CWrite(0xA3,0x87);
	VXISI2CWrite(0xA4,0x00);
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
	VXISI2CWrite(0xB8,0x00);
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
	VXISI2CWrite(0xDF,0xA2);
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
	VXISI2CWrite(0xF0,0x31);
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
#endif

void SET_HDMI_SYNC_PARA(void)
{
	Printf("\r\nLoading HDMI Embedded Sync para.... ");

	return;
	//DUMP VS4210 Reg..
#if 0
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
VXISI2CWrite(0x10,0x80);
VXISI2CWrite(0x11,0x82);
VXISI2CWrite(0x12,0x74);
VXISI2CWrite(0x13,0x80);
VXISI2CWrite(0x14,0x38);
VXISI2CWrite(0x15,0x78);
VXISI2CWrite(0x16,0xFE);
VXISI2CWrite(0x17,0x00);
VXISI2CWrite(0x18,0xC0);
VXISI2CWrite(0x19,0x00);
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
VXISI2CWrite(0x3A,0x1D);
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
VXISI2CWrite(0x9A,0x80);
VXISI2CWrite(0x9B,0x04);
VXISI2CWrite(0x9C,0x0A);
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
VXISI2CWrite(0xDD,0x00);
VXISI2CWrite(0xDE,0xB0);
VXISI2CWrite(0xDF,0x00);
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

#else
#if 1
/*
	VXISI2CWrite(0x08,0x40);
	
	VXISI2CWrite(0x0A,0x40);
	
	VXISI2CWrite(0x10,0x80);
	
	VXISI2CWrite(0x12,0x74);
	VXISI2CWrite(0x13,0x80);
	VXISI2CWrite(0x14,0x38);
	
	VXISI2CWrite(0x19,0x00);
*/

	VXISI2CWrite(0x1A,0x20);
/*
	VXISI2CWrite(0x1B,0x30);
	
	VXISI2CWrite(0x1F,0xA4);
	VXISI2CWrite(0x20,0x50);
	VXISI2CWrite(0x21,0xE2);
	VXISI2CWrite(0x22,0x87);
	VXISI2CWrite(0x23,0x98);
	VXISI2CWrite(0x24,0x80);
	VXISI2CWrite(0x25,0x44);
	
	VXISI2CWrite(0x27,0x38);
	
	VXISI2CWrite(0x2E,0x47);
	VXISI2CWrite(0x2F,0x80);
	
	VXISI2CWrite(0x3A,0x1D);
	
	VXISI2CWrite(0x45,0x00);

	VXISI2CWrite(0x58,0x00);
	
	VXISI2CWrite(0x69,0x88);
*/	
	VXISI2CWrite(0x74,0x06);

	VXISI2CWrite(0x80,0x01);
	VXISI2CWrite(0x81,0x15);
	
	VXISI2CWrite(0x83,0x2D);
	
	VXISI2CWrite(0x86,0x00);
	VXISI2CWrite(0x87,0x00);
	VXISI2CWrite(0x88,0x00);
	VXISI2CWrite(0x89,0x00);
	
	VXISI2CWrite(0x8D,0x17);
/*
	VXISI2CWrite(0x9A,0x80);
	
	
	VXISI2CWrite(0xB8,0x80);

	VXISI2CWrite(0xBA,0x01);
	
	VXISI2CWrite(0xBE,0x2C);
	
	VXISI2CWrite(0xD7,0x0F);
	
	VXISI2CWrite(0xDE,0xB0);
	
	VXISI2CWrite(0xF0,0x30);
	
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x18);
	VXISI2CWrite(0xFF,0x07);
*/


#else
	VXISI2CWrite(0x08,0x40);
	
	VXISI2CWrite(0x0A,0x40);
	
	VXISI2CWrite(0x10,0x80);
	
	VXISI2CWrite(0x12,0x74);
	VXISI2CWrite(0x13,0x80);
	VXISI2CWrite(0x14,0x38);
	
	VXISI2CWrite(0x19,0x00);
	VXISI2CWrite(0x1A,0x20);
	VXISI2CWrite(0x1B,0x30);
	
	VXISI2CWrite(0x1F,0xA4);
	VXISI2CWrite(0x20,0x50);
	VXISI2CWrite(0x21,0xE2);
	VXISI2CWrite(0x22,0x87);
	VXISI2CWrite(0x23,0x98);
	VXISI2CWrite(0x24,0x80);
	VXISI2CWrite(0x25,0x44);
	
	VXISI2CWrite(0x27,0x38);
	
//	VXISI2CWrite(0x2E,0x47);
//	VXISI2CWrite(0x2F,0x80);
	
//	VXISI2CWrite(0x3A,0x1D);
	
//	VXISI2CWrite(0x45,0x00);
	
//	VXISI2CWrite(0x58,0x00);
	
//	VXISI2CWrite(0x69,0x88);
	
	VXISI2CWrite(0x74,0x06);

	VXISI2CWrite(0x80,0x01);
	VXISI2CWrite(0x81,0x15);
	
	VXISI2CWrite(0x83,0x2D);
	
//	VXISI2CWrite(0x86,0x00);
//	VXISI2CWrite(0x87,0x00);
//	VXISI2CWrite(0x88,0x00);
//	VXISI2CWrite(0x89,0x00);
	
	VXISI2CWrite(0x8D,0x17);

//	VXISI2CWrite(0x9A,0x80);
	
	
//	VXISI2CWrite(0xB8,0x80);
	/*
	VXISI2CWrite(0xBA,0x01);
	
	VXISI2CWrite(0xBE,0x2C);
	
	VXISI2CWrite(0xD7,0x0F);
	
	VXISI2CWrite(0xDE,0xB0);
	
	VXISI2CWrite(0xF0,0x30);
	
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x18);
	VXISI2CWrite(0xFF,0x07);
*/
 #endif
#endif	


}

void SET_AHD_SYNC_PARA(void)
{
	Printf("\r\nLoading AHD Embedded Sync para.... ");
	//DUMP VS4210 Reg..
#if 1
	VXISI2CWrite(0x08,0x40);
	
	VXISI2CWrite(0x0A,0x40);
	
//	VXISI2CWrite(0x10,0x80);
	
	VXISI2CWrite(0x12,0x74);
	VXISI2CWrite(0x13,0x80);
	VXISI2CWrite(0x14,0x38);
	
	VXISI2CWrite(0x19,0x00);
	VXISI2CWrite(0x1A,0x20);
	VXISI2CWrite(0x1B,0x30);
	
	VXISI2CWrite(0x1F,0xA4);
	VXISI2CWrite(0x20,0x50);
	VXISI2CWrite(0x21,0xE2);
	VXISI2CWrite(0x22,0x87);
	VXISI2CWrite(0x23,0x98);
	VXISI2CWrite(0x24,0x80);
	VXISI2CWrite(0x25,0x44);
	
	VXISI2CWrite(0x27,0x38);
	
	VXISI2CWrite(0x2E,0x47);
	VXISI2CWrite(0x2F,0x80);
	
	VXISI2CWrite(0x3A,0x1D);
	
	VXISI2CWrite(0x45,0x00);
	
	VXISI2CWrite(0x58,0x00);
	
	VXISI2CWrite(0x69,0x88);
	
	VXISI2CWrite(0x74,0x06);

	VXISI2CWrite(0x80,0x01);
	VXISI2CWrite(0x81,0x15);
	
	VXISI2CWrite(0x83,0x2D);
	
	VXISI2CWrite(0x86,0x00);
	VXISI2CWrite(0x87,0x00);
	VXISI2CWrite(0x88,0x00);
	VXISI2CWrite(0x89,0x00);
	
	VXISI2CWrite(0x8D,0x17);
	
	VXISI2CWrite(0x9A,0x80);
	
	
	VXISI2CWrite(0xB8,0x80);

	VXISI2CWrite(0xBA,0x01);
	
	VXISI2CWrite(0xBE,0x2C);
	
	VXISI2CWrite(0xD7,0x0F);
	
	VXISI2CWrite(0xDE,0xB0);
	
	VXISI2CWrite(0xF0,0x30);
	
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x18);
	VXISI2CWrite(0xFF,0x07);
#else
	VXISI2CWrite(0x08,0x40);
	
	VXISI2CWrite(0x0A,0x40);
	
//	VXISI2CWrite(0x10,0x80);
	
	VXISI2CWrite(0x12,0x74);
	VXISI2CWrite(0x13,0x80);
	VXISI2CWrite(0x14,0x38);
	
	VXISI2CWrite(0x19,0x00);
	VXISI2CWrite(0x1A,0x20);
	VXISI2CWrite(0x1B,0x30);
	
	VXISI2CWrite(0x1F,0xA4);
	VXISI2CWrite(0x20,0x50);
	VXISI2CWrite(0x21,0xE2);
	VXISI2CWrite(0x22,0x87);
	VXISI2CWrite(0x23,0x98);
	VXISI2CWrite(0x24,0x80);
	VXISI2CWrite(0x25,0x44);
	
	VXISI2CWrite(0x27,0x38);
	
//	VXISI2CWrite(0x2E,0x47);
//	VXISI2CWrite(0x2F,0x80);
	
//	VXISI2CWrite(0x3A,0x1D);
	
//	VXISI2CWrite(0x45,0x00);
	
//	VXISI2CWrite(0x58,0x00);
	
//	VXISI2CWrite(0x69,0x88);
	
	VXISI2CWrite(0x74,0x06);

	VXISI2CWrite(0x80,0x01);
	VXISI2CWrite(0x81,0x15);
	
	VXISI2CWrite(0x83,0x2D);
	
//	VXISI2CWrite(0x86,0x00);
//	VXISI2CWrite(0x87,0x00);
//	VXISI2CWrite(0x88,0x00);
//	VXISI2CWrite(0x89,0x00);
	
	VXISI2CWrite(0x8D,0x17);
	
//	VXISI2CWrite(0x9A,0x80);
	
	
//	VXISI2CWrite(0xB8,0x80);
	/*
	VXISI2CWrite(0xBA,0x01);
	
	VXISI2CWrite(0xBE,0x2C);
	
	VXISI2CWrite(0xD7,0x0F);
	
	VXISI2CWrite(0xDE,0xB0);
	
	VXISI2CWrite(0xF0,0x30);
	
	VXISI2CWrite(0xFD,0x70);
	VXISI2CWrite(0xFE,0x18);
	VXISI2CWrite(0xFF,0x07);
*/

#endif
}

void SET_HDMI_640_480P_PARA(void)
{
	Printf("\r\nLoading HDMI 640X480P Embedded Sync para.... ");
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
	VXISI2CWrite(0x0B,0x03);
	VXISI2CWrite(0x0C,0x00);
	VXISI2CWrite(0x0D,0x20);
	VXISI2CWrite(0x0E,0x10);
	VXISI2CWrite(0x0F,0x00);
	VXISI2CWrite(0x10,0x80);
	VXISI2CWrite(0x11,0x82);
	VXISI2CWrite(0x12,0x42);
	VXISI2CWrite(0x13,0x81);
	VXISI2CWrite(0x14,0xC6);
	VXISI2CWrite(0x15,0x77);
	VXISI2CWrite(0x16,0xFE);
	VXISI2CWrite(0x17,0xFA);
	VXISI2CWrite(0x18,0xC0);
	VXISI2CWrite(0x19,0x00);
	VXISI2CWrite(0x1A,0x20);
	VXISI2CWrite(0x1B,0x30);
	VXISI2CWrite(0x1C,0x00);
	VXISI2CWrite(0x1D,0x00);
	VXISI2CWrite(0x1E,0x20);
	VXISI2CWrite(0x1F,0x52);
	VXISI2CWrite(0x20,0xA0);
	VXISI2CWrite(0x21,0xE5);
	VXISI2CWrite(0x22,0x32);
	VXISI2CWrite(0x23,0x20);
	VXISI2CWrite(0x24,0x80);
	VXISI2CWrite(0x25,0x21);
	VXISI2CWrite(0x26,0x0D);
	VXISI2CWrite(0x27,0xE0);
	VXISI2CWrite(0x28,0xF0);
	VXISI2CWrite(0x29,0x00);
	VXISI2CWrite(0x2A,0x00);
	VXISI2CWrite(0x2B,0x11);
	VXISI2CWrite(0x2C,0x00);
	VXISI2CWrite(0x2D,0x00);
	VXISI2CWrite(0x2E,0x12);
	VXISI2CWrite(0x2F,0x80);
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
	VXISI2CWrite(0x3A,0x2C);
	VXISI2CWrite(0x3B,0x80);
	VXISI2CWrite(0x3C,0x80);
	VXISI2CWrite(0x3D,0x80);
	VXISI2CWrite(0x3E,0x80);
	VXISI2CWrite(0x3F,0x80);
	//VXISI2CWrite(0xVXISI2CWrite(0x80
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
	VXISI2CWrite(0x74,0x04);
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
	VXISI2CWrite(0x84,0x59);
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
	VXISI2CWrite(0x93,0x31);
	VXISI2CWrite(0x94,0x88);
	VXISI2CWrite(0x95,0x80);
	VXISI2CWrite(0x96,0xF2);
	VXISI2CWrite(0x97,0x20);
	VXISI2CWrite(0x98,0x10);
	VXISI2CWrite(0x99,0x90);
	VXISI2CWrite(0x9A,0x80);
	VXISI2CWrite(0x9B,0x04);
	VXISI2CWrite(0x9C,0x0A);
	VXISI2CWrite(0x9D,0x00);
	VXISI2CWrite(0x9E,0x04);
	VXISI2CWrite(0x9F,0x65);
	VXISI2CWrite(0xA0,0x2A);
	VXISI2CWrite(0xA1,0xD8);
	VXISI2CWrite(0xA2,0x98);
	VXISI2CWrite(0xA3,0x87);
	VXISI2CWrite(0xA4,0xD9);
	VXISI2CWrite(0xA5,0x38);
	VXISI2CWrite(0xA6,0xBA);
	VXISI2CWrite(0xA7,0x70);
	VXISI2CWrite(0xA8,0x40);
	VXISI2CWrite(0xA9,0x1E);
	VXISI2CWrite(0xAA,0x2E);
	VXISI2CWrite(0xAB,0x00);
	VXISI2CWrite(0xAC,0x84);
	VXISI2CWrite(0xAD,0xF2);
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
	VXISI2CWrite(0xDF,0xEC);
	VXISI2CWrite(0xE0,0x00);
	VXISI2CWrite(0xE1,0x00);
	VXISI2CWrite(0xE2,0x00);
	VXISI2CWrite(0xE3,0x12);
	VXISI2CWrite(0xE4,0xE2);
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


