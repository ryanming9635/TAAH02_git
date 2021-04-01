/** @file VX8812.c
  * @brief VX8812 setup program
  *
  * Processing CCIR656/VGA/YPbPr, output rgb / yuv 24bits to HDMI.
  *
  * @author Cunu-Yu
  * @date 2011/12/19
  */
//#include "VXIS_Extern_Define.h"
//#include "./isp/VX8812_param.h"

#include "VXIS_Extern_Define.h"
#include "VS4210.h"
#include "main.h"
#include "VX_OSD_Simple.h"
#include "VX_swi2c.h"

#include "VS4210_param_Mode88.h"
#include "VS4210_param_Modex26.h"

#include "keyremo.h"
#include "tpinc/TP2802.h"
#include "Printf.h"


extern tiByte 	gbVXIS_OuputModeflg ;
extern bit I2CWrite(unsigned char maddr,unsigned char saddr,unsigned char value) ; 
////////////////////////////////////////////////////////////////
void VX8812_BlueScreen( void )
{
	#if 0
	I2CWrite(VX8812_ADDR ,0x45,0x09) ; //padj
	I2CWrite(VX8812_ADDR ,0x3B,0x00) ; //R
	I2CWrite(VX8812_ADDR ,0x3C,0x00) ; //G
	#endif
}

#if 0
static void VX8812_init( tByte *Addrn, tByte *Valn , tByte Len)
{
/*
tByte i ; 
tByte addr ;
tByte val ;
*/
#if 0
	I2CWrite(VX8812_ADDR, 0x01, 0x5A); 

	for (i=0 ; i< Len ; i++)
	{	
		addr = Addrn[i] ;
		val  = Valn[i];
	 	I2CWrite(VX8812_ADDR, addr  ,val);
	}								  

	I2CWrite(VX8812_ADDR, 0x01 ,0xA5) ;
	I2CWrite(VX8812_ADDR ,0xF0,0x00) ;


	if (Color_PIN == 1 )
	{
		I2CWrite(VX8812_ADDR, 0x31 ,0x80) ;
		I2CWrite(VX8812_ADDR, 0x32 ,0x80) ;
		I2CWrite(VX8812_ADDR, 0x33 ,0x80) ;
	}
	else
	{
		I2CWrite(VX8812_ADDR, 0x31 ,0x8C) ;
		I2CWrite(VX8812_ADDR, 0x32 ,0x8C) ;
		I2CWrite(VX8812_ADDR, 0x33 ,0xC8) ;
	}


	I2CWrite(VX8812_ADDR ,0x45,0x00) ; //padj
	I2CWrite(VX8812_ADDR ,0x3B,0x80) ; //R
	I2CWrite(VX8812_ADDR ,0x3C,0x80) ; //G
#endif
}
#endif

void VXIS_fVX8812_Set_HDMI_in_OutCCIR656_NTSC(tByte tbMode)
{

#if 0
	switch(tbMode)
	{
	case _OSD_1280X720P60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_720P_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 	
	break ;

	case _OSD_1280X720P50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_720P_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 	
	break ;
	
	case _OSD_1920X1080P60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P60_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080P50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P50_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 
	break ;
	
	case _OSD_1920X1080P24 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P24_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080P25 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P25_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080P30 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P30_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 
	break ;


	case _OSD_1920X1080I50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080i_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080I60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080i_to_NTSC_param,VX8812_CCIR656_NUMBER) ; 
	break ;
	
	case _OSD_720X480I60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_480i60_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	
	case _OSD_720X480P60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_480P60_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	
	case _OSD_720X576I50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_576i50_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;

	case _OSD_720X576P50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_576P50_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	
	case _OSD_VESA_800x600:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_800x600_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1366x768:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1366x768_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1024x768:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1024x768_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1440x900:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1440x900_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1280x960:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1280x960_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1280x1024:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1280x1024_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1680x1050:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1680x1050_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1400x1050:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1400x1050_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1920x1200:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1920x1200_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1600x1200:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1600x1200_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	
	case _OSD_VESA_640x480:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_640x480_to_NTSC_param,VX8812_CCIR656_NUMBER) ;
	break ;
	
	case 100 :
		VX8812_BlueScreen();
	break ;
	}
	#endif
}

void VXIS_fVX8812_Set_HDMI_in_OutCCIR656_PAL(tByte tbMode)
{
#if 0
	switch(tbMode)
	{
	case _OSD_1280X720P60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_720P_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1280X720P50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_720P_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	
	case _OSD_1920X1080P60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P60_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080P50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P50_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080P24 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P24_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080P25 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P25_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080P30 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080P30_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	
	case _OSD_1920X1080I50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080i_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	case _OSD_1920X1080I60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1080i_to_PAL_param,VX8812_CCIR656_NUMBER) ; 
	break ;

	
	case _OSD_720X480I60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_480i60_to_PAL_param,VX8812_CCIR656_NUMBER) ; 					
	break ;
	
	case _OSD_720X480P60 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_480P60_to_PAL_param,VX8812_CCIR656_NUMBER) ; 							
	break ;
	
	case _OSD_720X576I50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_576i50_to_PAL_param,VX8812_CCIR656_NUMBER) ; 									
	break ;

	case _OSD_720X576P50 :
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_576P50_to_PAL_param,VX8812_CCIR656_NUMBER) ; 												
	break ;
	
	case _OSD_VESA_800x600:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_800x600_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1366x768:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1366x768_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1024x768:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1024x768_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1440x900:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1440x900_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1280x960:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1280x960_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1280x1024:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1280x1024_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1680x1050:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1680x1050_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1400x1050:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1400x1050_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1920x1200:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1920x1200_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_1600x1200:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_1600x1200_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	case _OSD_VESA_640x480:
		VX8812_init(VX8812_Addr_param, VX8812_ITU656_640x480_to_PAL_param,VX8812_CCIR656_NUMBER) ;
	break ;
	
	case 100 :
		VX8812_BlueScreen();
	break ;
	}
	#endif
}

void VXIS_fVX8812_Set_HDMI_in_SD_Video_Out(tByte tbMode)
{		
#if 0
	if (gbVXIS_OuputModeflg == _NTSC_FORMAT  ) 
	{
		VXIS_fVX8812_Set_HDMI_in_OutCCIR656_NTSC( tbMode) ;
	}
	else if (gbVXIS_OuputModeflg == _PAL_FORMAT  ) 
	{
		VXIS_fVX8812_Set_HDMI_in_OutCCIR656_PAL( tbMode) ;
	}
	#endif
}


void SET_VX8812_SD_Video_Out(tByte tbMode)
{		
	switch(tbMode)
		{	
		case _NTSC:
			#if(_DEBUG_MESSAGE_VX8812==ON)
			GraphicsPrint(MAGENTA,"\r\n(SET_VX8812_NTSC_OUT)");			
			#endif
		VS8812Write(0x01,0x00);
		VS8812Write(0x02,0x07);
		VS8812Write(0x03,0x1C);
		VS8812Write(0x04,0x01);
		VS8812Write(0x05,0x18);
		VS8812Write(0x06,0x00);
		VS8812Write(0x07,0x05);
		VS8812Write(0x08,0x00);
		VS8812Write(0x09,0x00);
		VS8812Write(0x0B,0x00);
		VS8812Write(0x0C,0x00);
		VS8812Write(0x0D,0x83);
		VS8812Write(0x0E,0x30);
		VS8812Write(0x0F,0x07);
		VS8812Write(0x10,0x80);
		VS8812Write(0x11,0x28);
		VS8812Write(0x12,0x52);
		VS8812Write(0x13,0x28);
		VS8812Write(0x14,0xDB);
		VS8812Write(0x15,0x88);
		VS8812Write(0x16,0x06);
		VS8812Write(0x17,0x08);
		VS8812Write(0x18,0x00);
		VS8812Write(0x19,0x00);
		VS8812Write(0x1A,0x20);
		VS8812Write(0x1B,0x32);
		VS8812Write(0x1C,0x00);
		VS8812Write(0x1D,0x00);
		VS8812Write(0x1E,0x00);
		VS8812Write(0x1F,0x00);
		VS8812Write(0x20,0x03);
		VS8812Write(0x21,0x00);
		VS8812Write(0x22,0x10);
		VS8812Write(0x23,0x80);
		VS8812Write(0x24,0x00);
		VS8812Write(0x25,0xA0);
		VS8812Write(0x26,0x03);
		VS8812Write(0x27,0x1C);
		VS8812Write(0x28,0x00);
		VS8812Write(0x29,0xFF);
		VS8812Write(0x31,0x80);
		VS8812Write(0x32,0x80);
		VS8812Write(0x33,0x80);
		VS8812Write(0x34,0xA0);
		VS8812Write(0x35,0x18);
		VS8812Write(0x36,0x46);
		VS8812Write(0x37,0x64);
		VS8812Write(0x38,0x00);
		VS8812Write(0x39,0xFF);
		VS8812Write(0x3A,0x00);
		VS8812Write(0x3B,0x80);
		VS8812Write(0x3C,0x80);
		VS8812Write(0x3D,0x80);
		VS8812Write(0x3E,0x80);
		VS8812Write(0x3F,0x80);
		VS8812Write(0x40,0x80);
		VS8812Write(0x41,0x60);
		VS8812Write(0x42,0x5F);
		VS8812Write(0x43,0x37);
		VS8812Write(0x44,0x14);
		VS8812Write(0x45,0x00);
		VS8812Write(0x46,0x00);
		VS8812Write(0x47,0x00);
		VS8812Write(0x48,0x00);
		VS8812Write(0x49,0x01);
		VS8812Write(0x4A,0x20);
		VS8812Write(0x4B,0x20);
		VS8812Write(0x4C,0x00);
		VS8812Write(0x4D,0x00);
		VS8812Write(0x4E,0x00);
		VS8812Write(0x4F,0x7F);
		VS8812Write(0x50,0x20);
		VS8812Write(0x51,0x20);
		VS8812Write(0x52,0x00);
		VS8812Write(0x53,0x0C);
		VS8812Write(0x54,0x00);
		VS8812Write(0x55,0xA0);
		VS8812Write(0x56,0x28);
		VS8812Write(0x57,0x04);
		VS8812Write(0x58,0x10);
		VS8812Write(0x59,0x08);
		VS8812Write(0x5A,0x10);
		VS8812Write(0x5B,0x00);
		VS8812Write(0x5C,0x68);
		VS8812Write(0x5D,0x25);
		VS8812Write(0x5E,0x04);
		VS8812Write(0x5F,0x00);
		VS8812Write(0x60,0xA0);
		VS8812Write(0x61,0x41);
		VS8812Write(0x62,0x33);
		VS8812Write(0x63,0x01);
		VS8812Write(0x64,0xFF);
		VS8812Write(0x65,0x01);
		VS8812Write(0x66,0xFF);
		VS8812Write(0x67,0x89);
		VS8812Write(0x68,0xAB);
		VS8812Write(0x69,0x28);
		VS8812Write(0x6A,0x6E);
		VS8812Write(0x6B,0x6E);
		VS8812Write(0x6C,0x6E);
		VS8812Write(0x6D,0x00);
		VS8812Write(0x6E,0x00);
		VS8812Write(0x6F,0x00);
		VS8812Write(0x70,0x00);
		VS8812Write(0x71,0x00);
		VS8812Write(0x72,0x00);
		VS8812Write(0x73,0x00);
		VS8812Write(0x74,0x10);
		VS8812Write(0x75,0xFF);
		VS8812Write(0x76,0xFF);
		VS8812Write(0x77,0xFF);
		VS8812Write(0x78,0xFF);
		VS8812Write(0x79,0xFF);
		VS8812Write(0x7A,0xFF);
		VS8812Write(0x7B,0xFF);
		VS8812Write(0x7C,0xFF);
		VS8812Write(0x7D,0xF0);
		VS8812Write(0x7E,0x70);
		VS8812Write(0x7F,0x00);
		VS8812Write(0x80,0x00);
		VS8812Write(0x81,0x00);
		VS8812Write(0x82,0x60);
		VS8812Write(0x83,0x0F);
		VS8812Write(0x84,0xFF);
		VS8812Write(0x85,0xFF);
		VS8812Write(0x86,0x00);
		VS8812Write(0x87,0x00);
		VS8812Write(0x88,0x00);
		VS8812Write(0x89,0x00);
		VS8812Write(0x8A,0x00);
		VS8812Write(0x8B,0x7E);
		VS8812Write(0x8C,0x00);
		VS8812Write(0x8D,0x1F);
		VS8812Write(0x8E,0x6C);
		VS8812Write(0x8F,0x03);
		VS8812Write(0x90,0x08);
		VS8812Write(0x91,0x01);
		VS8812Write(0x92,0xC0);
		VS8812Write(0x93,0x32);
		VS8812Write(0x94,0x5A);
		VS8812Write(0x95,0xD0);
		VS8812Write(0x96,0x21);
		VS8812Write(0x97,0x0D);
		VS8812Write(0x98,0xE0);
		VS8812Write(0x99,0x02);
		VS8812Write(0x9A,0x00);
		VS8812Write(0x9B,0x90);
		VS8812Write(0x9C,0x22);
		VS8812Write(0x9D,0x00);
		VS8812Write(0x9E,0x00);
		VS8812Write(0x9F,0x00);
		VS8812Write(0xA0,0x80);
		VS8812Write(0xA1,0x00);
		VS8812Write(0xA2,0x7F);
		VS8812Write(0xA3,0x00);
		VS8812Write(0xA4,0x47);
		VS8812Write(0xA5,0x80);
		VS8812Write(0xA6,0x0C);
		VS8812Write(0xA7,0x02);
		VS8812Write(0xA8,0x20);
		VS8812Write(0xA9,0x10);
		VS8812Write(0xAA,0x10);
		VS8812Write(0xAB,0x00);
		VS8812Write(0xAC,0x42);
		VS8812Write(0xAD,0x02);
		VS8812Write(0xAE,0x06);
		VS8812Write(0xAF,0x04);
		VS8812Write(0xB0,0x00);
		VS8812Write(0xB1,0x00);
		VS8812Write(0xB2,0x00);
		VS8812Write(0xB3,0x00);
		VS8812Write(0xB4,0x00);
		VS8812Write(0xB5,0x00);
		VS8812Write(0xB6,0x00);
		VS8812Write(0xC0,0x88);
		VS8812Write(0xC1,0x7E);
		VS8812Write(0xC2,0xDC);
		VS8812Write(0xC3,0x20);
		VS8812Write(0xC4,0x20);
		VS8812Write(0xC5,0x34);
		VS8812Write(0xC6,0x18);
		VS8812Write(0xC7,0x04);
		VS8812Write(0xC8,0x02);
		VS8812Write(0xD0,0x02);
		VS8812Write(0xD1,0x40);
		VS8812Write(0xD2,0x40);
		VS8812Write(0xD3,0x40);
		VS8812Write(0xD4,0x00);
		VS8812Write(0xD5,0x00);
		VS8812Write(0xD6,0x00);
		VS8812Write(0xD7,0x00);
		VS8812Write(0xD8,0x00);
		VS8812Write(0xD9,0x00);
		VS8812Write(0xE0,0x33);
		VS8812Write(0xE1,0xA7);
		VS8812Write(0xE2,0x7F);
		VS8812Write(0xE3,0x22);
		VS8812Write(0xE4,0x0D);
		VS8812Write(0xE5,0x0A);
		VS8812Write(0xE6,0x00);
		VS8812Write(0xE7,0x99);
		VS8812Write(0xE8,0x67);
		VS8812Write(0xE9,0x02);
		VS8812Write(0xF0,0x00);
		VS8812Write(0xF1,0x43);
		VS8812Write(0xF2,0x1F);
		VS8812Write(0xF3,0x1F);
		VS8812Write(0xF4,0x22);
		VS8812Write(0xF5,0x73);
		VS8812Write(0xF6,0x53);
		VS8812Write(0xF7,0x80);
		VS8812Write(0xF8,0x01);
		VS8812Write(0xF9,0x01);
		VS8812Write(0xFA,0x00);
		VS8812Write(0xFB,0x00);
		VS8812Write(0xFC,0x00);
		VS8812Write(0xFD,0x70);
		VS8812Write(0xFE,0x25);
		VS8812Write(0xFF,0x17);              			
		break;
		case _PAL:
				#if(_DEBUG_MESSAGE_VX8812==ON)
				GraphicsPrint(MAGENTA,"\r\n(SET_VX8812_PAL_OUT)");			
				#endif
		VS8812Write(0x01,0x00);
		VS8812Write(0x02,0x07);
		VS8812Write(0x03,0x1C);
		VS8812Write(0x04,0x01);
		VS8812Write(0x05,0x18);
		VS8812Write(0x06,0x00);
		VS8812Write(0x07,0x04);
		VS8812Write(0x08,0x00);
		VS8812Write(0x09,0x00);
		VS8812Write(0x0B,0x00);
		VS8812Write(0x0C,0x00);
		VS8812Write(0x0D,0x83);
		VS8812Write(0x0E,0x30);
		VS8812Write(0x0F,0x07);
		VS8812Write(0x10,0x80);
		VS8812Write(0x11,0x28);
		VS8812Write(0x12,0x00);
		VS8812Write(0x13,0x00);
		VS8812Write(0x14,0x00);
		VS8812Write(0x15,0x88);
		VS8812Write(0x16,0x04);
		VS8812Write(0x17,0x00);
		VS8812Write(0x18,0x00);
		VS8812Write(0x19,0x00);
		VS8812Write(0x1A,0x26);
		VS8812Write(0x1B,0x32);
		VS8812Write(0x1C,0x00);
		VS8812Write(0x1D,0x00);
		VS8812Write(0x1E,0x00);
		VS8812Write(0x1F,0x00);
		VS8812Write(0x20,0x04);
		VS8812Write(0x21,0x00);
		VS8812Write(0x22,0x10);
		VS8812Write(0x23,0x80);
		VS8812Write(0x24,0x00);
		VS8812Write(0x25,0x20);
		VS8812Write(0x26,0x03);
		VS8812Write(0x27,0x1C);
		VS8812Write(0x28,0x00);
		VS8812Write(0x29,0xFD);
		VS8812Write(0x31,0x80);
		VS8812Write(0x32,0x80);
		VS8812Write(0x33,0x80);
		VS8812Write(0x34,0xA0);
		VS8812Write(0x35,0x18);
		VS8812Write(0x36,0x46);
		VS8812Write(0x37,0x64);
		VS8812Write(0x38,0x00);
		VS8812Write(0x39,0xFF);
		VS8812Write(0x3A,0x00);
		VS8812Write(0x3B,0x80);
		VS8812Write(0x3C,0x80);
		VS8812Write(0x3D,0x80);
		VS8812Write(0x3E,0x80);
		VS8812Write(0x3F,0x80);
		VS8812Write(0x40,0x80);
		VS8812Write(0x41,0x60);
		VS8812Write(0x42,0x5F);
		VS8812Write(0x43,0x37);
		VS8812Write(0x44,0x14);
		VS8812Write(0x45,0x00);
		VS8812Write(0x46,0x00);
		VS8812Write(0x47,0x00);
		VS8812Write(0x48,0x00);
		VS8812Write(0x49,0x00);
		VS8812Write(0x4A,0x20);
		VS8812Write(0x4B,0x20);
		VS8812Write(0x4C,0x00);
		VS8812Write(0x4D,0x00);
		VS8812Write(0x4E,0x00);
		VS8812Write(0x4F,0x7F);
		VS8812Write(0x50,0x20);
		VS8812Write(0x51,0x20);
		VS8812Write(0x52,0x00);
		VS8812Write(0x53,0x0C);
		VS8812Write(0x54,0x00);
		VS8812Write(0x55,0xA0);
		VS8812Write(0x56,0x28);
		VS8812Write(0x57,0x04);
		VS8812Write(0x58,0x10);
		VS8812Write(0x59,0x08);
		VS8812Write(0x5A,0x10);
		VS8812Write(0x5B,0x00);
		VS8812Write(0x5C,0x68);
		VS8812Write(0x5D,0x25);
		VS8812Write(0x5E,0x04);
		VS8812Write(0x5F,0x00);
		VS8812Write(0x60,0xA0);
		VS8812Write(0x61,0x41);
		VS8812Write(0x62,0x33);
		VS8812Write(0x63,0x01);
		VS8812Write(0x64,0xFF);
		VS8812Write(0x65,0x01);
		VS8812Write(0x66,0xFF);
		VS8812Write(0x67,0x89);
		VS8812Write(0x68,0xAB);
		VS8812Write(0x69,0x28);
		VS8812Write(0x6A,0x6E);
		VS8812Write(0x6B,0x6E);
		VS8812Write(0x6C,0x6E);
		VS8812Write(0x6D,0x00);
		VS8812Write(0x6E,0x00);
		VS8812Write(0x6F,0x00);
		VS8812Write(0x70,0x00);
		VS8812Write(0x71,0x00);
		VS8812Write(0x72,0x00);
		VS8812Write(0x73,0x00);
		VS8812Write(0x74,0x50);
		VS8812Write(0x75,0xFF);
		VS8812Write(0x76,0xFF);
		VS8812Write(0x77,0xFF);
		VS8812Write(0x78,0xFF);
		VS8812Write(0x79,0xFF);
		VS8812Write(0x7A,0xFF);
		VS8812Write(0x7B,0xFF);
		VS8812Write(0x7C,0xFF);
		VS8812Write(0x7D,0xF0);
		VS8812Write(0x7E,0x70);
		VS8812Write(0x7F,0x00);
		VS8812Write(0x80,0x00);
		VS8812Write(0x81,0x00);
		VS8812Write(0x82,0x60);
		VS8812Write(0x83,0x06);
		VS8812Write(0x84,0x00);
		VS8812Write(0x85,0xFB);
		VS8812Write(0x86,0x00);
		VS8812Write(0x87,0x00);
		VS8812Write(0x88,0x00);
		VS8812Write(0x89,0x00);
		VS8812Write(0x8A,0x00);
		VS8812Write(0x8B,0x7E);
		VS8812Write(0x8C,0x00);
		VS8812Write(0x8D,0x0A);
		VS8812Write(0x8E,0x8C);
		VS8812Write(0x8F,0x00);
		VS8812Write(0x90,0x08);
		VS8812Write(0x91,0x01);
		VS8812Write(0x92,0xC6);
		VS8812Write(0x93,0x32);
		VS8812Write(0x94,0x60);
		VS8812Write(0x95,0xD0);
		VS8812Write(0x96,0x22);
		VS8812Write(0x97,0x71);
		VS8812Write(0x98,0x40);
		VS8812Write(0x99,0x02);
		VS8812Write(0x9A,0x00);
		VS8812Write(0x9B,0xA0);
		VS8812Write(0x9C,0x21);
		VS8812Write(0x9D,0x00);
		VS8812Write(0x9E,0x00);
		VS8812Write(0x9F,0x00);
		VS8812Write(0xA0,0x6B);
		VS8812Write(0xA1,0x00);
		VS8812Write(0xA2,0x80);
		VS8812Write(0xA3,0x80);
		VS8812Write(0xA4,0x47);
		VS8812Write(0xA5,0x80);
		VS8812Write(0xA6,0x0C);
		VS8812Write(0xA7,0x02);
		VS8812Write(0xA8,0x20);
		VS8812Write(0xA9,0x10);
		VS8812Write(0xAA,0x10);
		VS8812Write(0xAB,0x00);
		VS8812Write(0xAC,0x42);
		VS8812Write(0xAD,0x02);
		VS8812Write(0xAE,0x06);
		VS8812Write(0xAF,0x04);
		VS8812Write(0xB0,0x00);
		VS8812Write(0xB1,0x00);
		VS8812Write(0xB2,0x00);
		VS8812Write(0xB3,0x00);
		VS8812Write(0xB4,0x00);
		VS8812Write(0xB5,0x00);
		VS8812Write(0xB6,0x00);
		VS8812Write(0xC0,0x8C);
		VS8812Write(0xC1,0x7E);
		VS8812Write(0xC2,0xDC);
		VS8812Write(0xC3,0x20);
		VS8812Write(0xC4,0x20);
		VS8812Write(0xC5,0x34);
		VS8812Write(0xC6,0x18);
		VS8812Write(0xC7,0x84);
		VS8812Write(0xC8,0x02);
		VS8812Write(0xD0,0x02);
		VS8812Write(0xD1,0x40);
		VS8812Write(0xD2,0x40);
		VS8812Write(0xD3,0x40);
		VS8812Write(0xD4,0x00);
		VS8812Write(0xD5,0x00);
		VS8812Write(0xD6,0x00);
		VS8812Write(0xD7,0x00);
		VS8812Write(0xD8,0x00);
		VS8812Write(0xD9,0x00);
		VS8812Write(0xE0,0x33);
		VS8812Write(0xE1,0xA7);
		VS8812Write(0xE2,0x7F);
		VS8812Write(0xE3,0x22);
		VS8812Write(0xE4,0x0D);
		VS8812Write(0xE5,0x0A);
		VS8812Write(0xE6,0x00);
		VS8812Write(0xE7,0x99);
		VS8812Write(0xE8,0x67);
		VS8812Write(0xE9,0x02);
		VS8812Write(0xF0,0x00);
		VS8812Write(0xF1,0x43);
		VS8812Write(0xF2,0x1F);
		VS8812Write(0xF3,0x1F);
		VS8812Write(0xF4,0x22);
		VS8812Write(0xF5,0x73);
		VS8812Write(0xF6,0x53);
		VS8812Write(0xF7,0x80);
		VS8812Write(0xF8,0x01);
		VS8812Write(0xF9,0x01);
		VS8812Write(0xFA,0x00);
		VS8812Write(0xFB,0x00);
		VS8812Write(0xFC,0x00);
		VS8812Write(0xFD,0x70);
		VS8812Write(0xFE,0x25);
		VS8812Write(0xFF,0x17);				
		break;
		}
}


