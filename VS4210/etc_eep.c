/*****************************************************************************/
/*                                                                           										*/
/*  VS4210   MCU                    													*/
/*                                                                           										*/
/*  Etc_eep.c                                                            									*/
/*                                                                           										*/
/*****************************************************************************/

//eeprom.c
#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"

//#include "Config.h"
//#include "typedefs.h"
#include "main.h"
//#include "i2c.h"
#include "printf.h"
#include "etc_eep.h"
#include "keyremo.h"


extern tByte	DayNightLevel;
extern BYTE	AudioVol ;
extern BYTE	Priority;
extern BYTE OSD_LOCKF;
extern BYTE SEQFlag;	
extern BYTE NowModeState;
extern BYTE	SelectModeType;


#if 0
void WriteEEP(WORD index, BYTE dat)
{
//	BYTE addr;

//	addr = (EEPROMI2CAddress | (index>>7)) & 0xfe;
/*
	I2CStart();
	I2CWriteData(addr);
	I2CWriteData((BYTE)index);
	I2CWriteData(dat);
	I2CStop();
*/
//	#ifdef DEBUG_EEP
//		dPrintf("Write %02xh to [Adrs(%02xh)Index(%02xh)\r\n", (WORD)dat, (WORD)addr, (WORD)index );
//	#endif

	delay(2);
}

BYTE ReadEEP(WORD index)
{
	BYTE val=_FALSE;
//	BYTE addr;

//	addr = (EEPROMI2CAddress | (index>>7)) & 0xfe;
/*
	val = ReadI2C(addr,(BYTE)index);
*/
//	#ifdef DEBUG_EEP
//		dPrintf("Read [Adrs:%02xh,Index:%02xh] %02xh\r\n", (WORD)addr, (WORD)index, (WORD)val);
//	#endif
	delay(2);
	return val;
}
#endif
WORD GetFWRevEE()
{
	WORD rev=0;

	if( ReadEEP(0) != 'T' ) return 0;
	if( ReadEEP(1) != 'O' ) return 0;
	if( ReadEEP(2) != 'A' ) return 0;
	if( ReadEEP(3) != '0' ) return 0;
	
	rev = (ReadEEP(EEP_FWREV_MAJOR) )<< 8;
	rev |= ReadEEP(EEP_FWREV_MINOR);

	return rev;
	//return 0x0001;
}

void SaveFWRevEE(WORD rev)
{
	WriteEEP(0, 'T');
	WriteEEP(1, 'O');
	WriteEEP(2, 'A');
	WriteEEP(3, '0');
	WriteEEP(EEP_FWREV_MAJOR, (BYTE)(rev>>8) );
	WriteEEP(EEP_FWREV_MINOR, (BYTE)rev );

}

#if 0/// (_DEBUG_MESSAGE_Monitor==ON)

void SaveDebugLevelEE(BYTE dl)
{
	WriteEEP(EEP_DEBUGLEVEL, dl);
}
#endif

BYTE GetDebugLevelEE(void)
{
	return ReadEEP(EEP_DEBUGLEVEL);
}

//===================================================================
void ResetOSDValue(void)
{
Printf("\r\nReset OSD Data..");
#if 0
	   WriteEEP(EEP_AutoMode , 0x00);	  // Auto Day&Night...... OFF

	WriteEEP(EEP_CONTRAST,50);	  
	WriteEEP(EEP_BRIGHTNESS,50);	  
	WriteEEP(EEP_SATURATION_U,50);	  
	WriteEEP(EEP_SHARPNESS,50);	  
	WriteEEP(EEP_HUE,50);	  

	//WriteEEP(EEP_PRIORITY,0);	  
	   DayNightLevel=ReadEEP(EEP_AutoMode);
	   WriteEEP(EEP_IMAGEMode,0x00);	 // IMAGEMode..... SINGLE
	   WriteEEP(EEP_RearMode,0x00); 	 // RearMode...... R  
	   WriteEEP(EEP_TriggerVGA,0x00); 	 // trigger vga...... on  
	   WriteEEP(EEP_GAUGEMode,0x01);	 // ­Ë¨®OSD....... ON	
	   WriteEEP(EEP_Powerflag,0x01);	 // POWER FLAG....... ON  
	   WriteEEP(EEP_COSDMode,0x01); 	 // Display OSD... ON 
	   WriteEEP(EEP_DVRDisplay,0x00); 	 //DVR Display OSD... ON
	   WriteEEP(EEP_CAMAMode,0x00); 	 // MirrorA....... OFF
	   WriteEEP(EEP_CAMBMode,0x00); 	 // MirrorB....... OFF
	   WriteEEP(EEP_CAMCMode,0x00); 	 // MirrorC....... OFF
	   WriteEEP(EEP_CAMRMode,0x01); 	 // MirrorR....... ON
	   WriteEEP(EEP_AUDIOPVOL,50);       // AudioVol

	   WriteEEP(EEP_ONOFFChoice,0x00); 	 // osd lock oFF

	   AudioVol=50;
	 //  ChangeVol(0);
	   WriteEEP(EEP_PCAUDIOPVOL,50);
	   WriteEEP(EEP_AudioRecord,0x03); 	 // dvr audio channel
	
	   WriteEEP(EEP_IMAGE_A_Mode,0x00);
	   WriteEEP(EEP_IMAGE_B_Mode,0x00);
       WriteEEP(EEP_IMAGE_C_Mode,0x00);

	   WriteEEP(EEP_DELAYCAMA,0x00); 	 // DELAY CAM A....... 0
	   WriteEEP(EEP_DELAYCAMB,0x00); 	 // DELAY CAM B....... 0
	   WriteEEP(EEP_DELAYCAMC,0x00); 	 // DELAY CAM C....... 0.
	   WriteEEP(EEP_DELAYCAMR,0x00); 	 // DELAY CAM R....... 0

	   if(Priority==ON)//on
	   {
		WriteEEP(EEP_JUMPMode,0);	  // JUMPMode...... QUAL
		WriteEEP(EEP_ONOFFChoice,0x00); 	// OSD	lock....... off
		OSD_LOCKF=0;
	   }
		else
	   {
		WriteEEP(EEP_JUMPMode,13);		// JUMPMode...... CAM R
		WriteEEP(EEP_ONOFFChoice,0x01); 	// OSD	lock....... ON
		OSD_LOCKF=1;
	   }
		
		//TP2824 video data reset	
		WriteEEP(EEP_TP2824_STD0,0xFF);	  
		WriteEEP(EEP_TP2824_MODE0,0xFF);	  
		WriteEEP(EEP_TP2824_STD1,0xFF);	  
		WriteEEP(EEP_TP2824_MODE1,0xFF);	  
		WriteEEP(EEP_TP2824_STD2,0xFF);	  
		WriteEEP(EEP_TP2824_MODE2,0xFF);	  
		WriteEEP(EEP_TP2824_STD3,0xFF);	  
		WriteEEP(EEP_TP2824_MODE3,0xFF);	  

	//WriteEEP(EEP_JUMPMode,0);

	   SEQFlag=0;
//#ifdef AWT_ML072S
//	   WriteEEP(EEP_TimeStep,0x03); 	 // TimerStep.....02
//#else
	   WriteEEP(EEP_TimeStep,0x02); 	 // TimerStep.....02
//#endif	   
	   WriteEEP(EEP_CAMOutMode,0x00);	 // CAMOut........ QUAL 
//	   CAMOutDSel( 0x00 );	///V1.1 02/27/07 simonsung 
//	   TW2835Cmd("\n\rmode 0\n\r");

//	   ClearDataCnt(INPUTINFO_ADDR, 51);   //william-980416

	   LoadEEPROM();

	  NowModeState=SelectMode;
	   SelectModeType=CAM_A;

	   WriteEEP(EEP_NowModeState,SelectMode);
			
		WriteEEP(EEP_SelectMode,SelectModeType);
		 setloopoutmode(SelectModeType) ;

LoadVideoData();
#endif



}

void ClearBasicEE(void)
{

	Printf("\r\nClearBasicEE...");

	WriteEEP(EEP_PRIORITY,0x01);	//PRIORITY.........OFF	Andy-980602
	Priority=1;

//	SaveVInputStdDetectModeEE(0);	// AUTO
//	WriteEEP(EEP_CCD, 0);	// CC = OFF


	ResetOSDValue();  //Pinchi 20150212 enable

//	ResetVideoValue();

	



/*
	WriteEEP(EEP_Powerflag,ON);

	WriteEEP(EEP_CameraVolt, 14);  //7.6V

	WriteEEP(EEP_DEBUGLEVEL,OFF); 

	WriteEEP(EEP_DC12_PWR_START, OFF); 	
*/	
}

//=========  Audio =================================================
BYTE GetAudioVolEE(void)
{
	return ReadEEP( EEP_AUDIOPVOL );
}

void SetAudioVolEE( BYTE vol )
{
	WriteEEP( EEP_AUDIOPVOL, vol );
}

BYTE GetPCAudioVolEE(void)
{
	return ReadEEP( EEP_PCAUDIOPVOL );
}
void SetPCAudioVolEE( BYTE vol )
{
	WriteEEP( EEP_PCAUDIOPVOL, vol );
}





