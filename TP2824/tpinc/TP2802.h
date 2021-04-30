// *****************************************************
// Company : Techpoint Inc
// $Id: TP2802.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2014-03-10 $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef __TP2802_H__
#define __TP2802_H__

enum {
	CH1 = 0,
	CH2,
	CH3,
	CH4,
	AutoSel
};

enum{
	VIN1_Y = 0,
	VIN2_Y,
	VIN3_Y,
	VIN4_Y,
	VIN1_C,
	VIN2_C,
	VIN3_C,
	VIN4_C
};

enum{
	R720P60=0x00,
	R720P50=0x01,
	R1080P30=0x02,
	R1080P25=0x03,
	R720P30=0x04,
	R720P25=0x05,
	NTPAL=0x06,
	NoDet=0x07,
	R720P30V2=0x0C,
	R720P25V2=0x0D,
	NTSC=0x08,
	PAL=0x09,
	R3M18=0x20,
	R5M12=0x21,	
	R4M15=0x22,
	R3M20=0x23,
	R4M12=0x24,
	R6M10=0x25,
	R4M30=0x26	
};
enum{
	RES1280X720=0x00,
	RES1920X1080=0x01,
	RES1920X480=0x02,
	RES1920X576=0x03,
	RES2048X1536=0x04,
	RES2056X1944=0x05
};
enum{
	TVI,
	HDA,
	HDC
};
enum{
    VIDEO_UNPLUG,
    VIDEO_IN,
    VIDEO_LOCKED,
    VIDEO_UNLOCK
};

#define FLAG_LOSS       0x80
#define TVI_FLAG_LOCKED     0x60
#define CVBS_FLAG_LOCKED	0x60
#define FLAG_HDC_MODE    0x80
#define FLAG_HALF_MODE    0x40
#define FLAG_MEGA_MODE    0x20
#define FLAG_HDA_MODE    0x10

#define Bits16out	0
#define Bits8out	1

//#define OUTPUT Bits8out 
#define OUTPUT Bits16out 		//20201211-05
#define EQ_COUNT 20
#define EQ_DIFF 2

#define NoVid	0
#define IsVid	1



void Init_TP280x_RegSet(void);
unsigned char  Set_ResWithAutoDet(unsigned char ch);
void Sel_VD_Out(unsigned char  ch);
void Init_TP280x_CHx_RegSet(BYTE index);

//void Menu(void);
//void Left(void);
//void Right(void);
//void Up(void);
//void Down(void);

void EQ_Detect(void);
//void MonTX(BYTE,BYTE,BYTE,BYTE,BYTE);
//void BurstTX(BYTE *buf);
//void TP_bt1120_720P_1080P_mode0_w0(void);
//void TP_bt1120_1080P_1080P_mode0_w0(void);
//void TP_bt1120_NTSC_1080P_mode0_w0(void);
//void TP_bt1120_PAL_1080P_mode0_w0(void);
void SET_HDMI_640_480P_PARA(void);

void SET_HDMI_SYNC_PARA(void);
void SET_AHD_SYNC_PARA(void);

void Set_PLL_StartUp(void);
void Set_VidRes(tByte ch);
void Get_2824cIrq(void);//cocrx

void TP2824_PTZ_init(void);
void TP2824_PTZ_mode(unsigned char ch, unsigned char mod);
void TP2824C_PTZ_mode(unsigned char chip, unsigned char ch, unsigned char mod);
void TP2824C_RX_init(unsigned char chip, unsigned char mod);

void HDA_SetACPV2Data(unsigned char chip, unsigned char reg,unsigned char dat);
void TP2824C_StartTX(unsigned char chip, unsigned char ch);


void COC_test(void);
void Init_COC_PTZOUT1(void);
void Set_Mirror_COC(void);

tByte newInRes[4];	//20201214-01

extern void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID);
extern void Set_Video_Loss_OSD(BYTE ch, BYTE set);	
extern void HS_Select_Win(unsigned char WinNum);	//20201214-02


#endif

