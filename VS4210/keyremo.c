//
// KeyRemo.h
// Default function for Key Input and Remocon Input
//
//=============================================================================
//		                    Operation Key Mapping
//=============================================================================

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
#include "VX_OSD_Simple.h"

#include "GT911.h"

/*
#include "config.h"
#include "typedefs.h"
#include "keyremo.h"
#include "measure.h"
#include "audio.h"
#include "dispinfo.h"
#include "main.h"
#include "i2c.h"
#include "osdmenu.h"
#include "debug.h"
#include "cpu.h"
#include "printf.h"
#include "regmap.h"
#include "reg.h"
#include "etc_eep.h"
#include "osdbasic.h"
#include "HS_keyremo.h"
#include "HS_DisplayOSD.h"
#include "HS_DVRProtocol.h"
#include "HS_IOdef.h"
#include "audio.h"
*/

bit CAM_PTZ_RUN_flag;
BYTE lock_keycnt; 
BYTE KeyBuffer=0;
BYTE LockKeyState=0; 
BYTE BookMarkState=0;
extern tByte DayNightLevel;
BYTE Disp_Input=0;//ryan@21071019

BYTE TR_JUMP_Flag=0;//ryan@20170912
BYTE TR_JUMP_HOLD=0;//ryan@20170912
BYTE RELEASE_TR_JUMP_Flag=0;//ryan@20170912

BYTE OSD_LOCKF=0;//ryan@21071018
BYTE  updn_reg[4]={0,0,0,0};
BYTE Time5ms=40;
bit ChangeKey;
BYTE TriggerMode;
BYTE	TriggerStatus=0;
BYTE SEQFlag=0;	
extern BYTE SEQIndex;
extern BYTE TimeStep;

 volatile BYTE	MainSetSelectMode[13]={
    	VS4210_LOOP_MODE0_w0,   
    	VS4210_LOOP_MODE0_w1 ,   
    	VS4210_LOOP_MODE0_w2 ,   
    	VS4210_LOOP_MODE0_w3 ,
    	VS4210_LOOP_MODE12_12,
	VS4210_LOOP_MODE12_34,
	VS4210_LOOP_MODE12_14,
      VS4210_LOOP_MODE12_42,
	VS4210_LOOP_MODE12_13,
     VS4210_LOOP_MODE12_32,
VS4210_LOOP_MODE10_412 ,
VS4210_LOOP_MODE8_412 ,
//VS4210_LOOP_MODE1 

/*
    VS4210_LOOP_MODE2 ,///5
    VS4210_LOOP_MODE10_123 ,///6
    VS4210_LOOP_MODE8 ,///7
     VS4210_LOOP_MODE9 ,///8
      VS4210_LOOP_MODE14,///17
    VS4210_LOOP_MODE_NUMBER  //18	
    */
		};
BYTE JumpTable[]={
	VS4210_LOOP_MODE1,//CAM_QUAD,
	 VS4210_LOOP_MODE0_w0, //SEQ,
	VS4210_LOOP_MODE12_12,// CAM_AB,
	VS4210_LOOP_MODE12_34,// CAM_CR,
		VS4210_LOOP_MODE12_14,// CAM_AR,
	 VS4210_LOOP_MODE12_42,// CAM_RB,
	VS4210_LOOP_MODE12_13,// CAM_AC,
	 VS4210_LOOP_MODE12_32,// CAM_CB,
	VS4210_LOOP_MODE10_412,// CAM_RAB_T,
	VS4210_LOOP_MODE8_412 ,// CAM_RAB_H,
	VS4210_LOOP_MODE0_w0, // CAM_A,
  	VS4210_LOOP_MODE0_w1 , // CAM_B,
	VS4210_LOOP_MODE0_w2 ,  // CAM_C,
	VS4210_LOOP_MODE0_w3// CAM_R
	 };

BYTE LeftTable[]={
	VS4210_LOOP_MODE0_w0,  //CAM_A,
	VS4210_LOOP_MODE12_12,//CAM_AB,
	VS4210_LOOP_MODE12_13,//CAM_AC,
	VS4210_LOOP_MODE12_14,///CAM_AR,
	VS4210_LOOP_MODE8_123 ,///CAM_ABC,
	VS4210_LOOP_MODE8_124 ,//CAM_ABR,
	VS4210_LOOP_MODE8_134/// ,CAM_ACR
			    };  

BYTE RightTable[]={
	VS4210_LOOP_MODE0_w1 ,  //CAM_B,
	VS4210_LOOP_MODE12_21,//CAM_BA,
	VS4210_LOOP_MODE12_23,//CAM_BC,
	VS4210_LOOP_MODE12_24,//CAM_BR,
	VS4210_LOOP_MODE8_213,///CAM_BAC,
	VS4210_LOOP_MODE8_214,///CAM_BAR,
	VS4210_LOOP_MODE8_234///CAM_BCR
	};
BYTE TriggerC_Table[]={
			VS4210_LOOP_MODE0_w2,//CAM_C,
			VS4210_LOOP_MODE12_31,//CAM_CA,
			VS4210_LOOP_MODE12_32,//CAM_CB,
			VS4210_LOOP_MODE12_34,///CAM_CR,
			VS4210_LOOP_MODE8_312,//CAM_CAB,
			VS4210_LOOP_MODE8_314,///CAM_CAR,
			VS4210_LOOP_MODE8_324///CAM_CBR
			};


	
BYTE SEQTable[]={
	VS4210_LOOP_MODE0_w0,
  	VS4210_LOOP_MODE0_w1 ,   
    	VS4210_LOOP_MODE0_w2 ,   
    	VS4210_LOOP_MODE0_w3 
    	};	//SEQ  	

BYTE RearTable[]={
	VS4210_LOOP_MODE0_w3 ,//CAM_R,
	VS4210_LOOP_MODE12_14,///CAM_AR,
	  VS4210_LOOP_MODE12_42,//CAM_RB,
	VS4210_LOOP_MODE12_43,//CAM_CR,
	VS4210_LOOP_MODE10_412,//CAM_RAB_T,
	VS4210_LOOP_MODE10_413,//CAM_RAC_T,
	VS4210_LOOP_MODE10_423,//CAM_RBC_T,
	VS4210_LOOP_MODE8_412 ,//CAM_RAB_H,
	VS4210_LOOP_MODE8_413 ,//CAM_RAC_T2,
	VS4210_LOOP_MODE8_423 //CAM_RBC_T2
	};


extern bit SpeedDomeMode;
bit OnChangingValue=0;
extern  BYTE	 Priority;
extern  tByte	OSDStateFlg;
extern  BYTE PowerFlag;
extern  BYTE Monitor_flag;
 volatile BYTE Encoder_flag=ON;

 bit RepeatKey;
extern bit Init_DisplayInput;
#if 0///def SUPPORT_DEBUG
extern  bit Debug_On;
#endif
extern  BYTE  PcMode;
extern  		WORD  OSDLastKeyInTime;		// in ms
//extern   		bit   DisplayInputHold;		//william-981029
extern  DATA  WORD  tic_pc;
		bit KeyReady;
 DATA BYTE Key,TouchP;
extern  BYTE 	keyticSec;
#if 0//def REMO_RC5
extern   BYTE	RemoDataReady;
#endif
extern BYTE RemoSystemCode, RemoDataCode;
//#ifdef Format_SDCard
extern BYTE FormatSDCard;
//#endif
extern	bit	ChannelEdit_Flag;
extern 	bit	ChannelEdit_on;
extern 	bit	GetDVRStatusflag;
BYTE    OSDMenuLevel;
 BYTE	title_item, cursor_item, /*cursor_y,*/  cur_osdwin, /*cur_osdaddr,*/ val_osdwin=0xff, 
						val_index=0;

#if 0//def E_Wintek
extern	bit	FactoryMenu;
#endif

extern BYTE SelectModeType;

extern bit  IsDrawNoVideoSignal;//ryan@20170922

#if 0//def HTRV  //Pinchi 20150825 add model #define
extern BYTE GaugeVStart;

BYTE GaugeSetup =0;
BYTE GaugeInfoOSD =0;


#endif

BYTE TriggerFlag=0;
BYTE NowModeState;

#if 0///def SUPPORT_2835_CLK_TUNE_BACK_DOOR  //Pinchi 20160929 add
DATA bit IsRun2835ClkTune=0;
DATA BYTE b2835ClkTuneVal=0;

DATA bit IsEnter2835ClkTune=0;
DATA DWORD Enter2835TuneClock;
extern DATA DWORD SystemClock;

void HS_Display2835ClkTuneVal(WORD val);
extern CODE BYTE Init_Osd_DisplayInput_B[];
#endif

#if 0///def HS_NEWOSDMENU
void RemoMenu_MenuKey(BYTE isREMOTE)
{
	if(GetDisplayedOSD() & MENU) 	
		OSDHighMenu();       
	else
		{
		/*	//Weylis -@121008 
		//FMONFLG = 3;
		if(GetInputSelection()== soDVDAV || GetInputSelection()== soCOMPONENT)
			{
				if(isREMOTE)
					OpenOSDMenu(1);
				else
					OpenOSDMenu(0);
			}
		else
		*/
			OpenOSDMenu(1);
		}
	
}
#else 
void RemoMenu_MenuKey(void)
{
/*
	if(  GetDisplayedOSD() & MENU  ) 		OSDHighMenu();
	else 		
		OpenOSDMenu();
	*/
}
#endif

void RemoChnUp_UpKey(void)
{
	#if 0///def SUPPORT_DEBUG
	if( Debug_On ) {
		DebugWindowData(UP);
		return;
	}
	else 
	#endif	
	/*
	if( GetDisplayedOSD() & MENU  )	
		#ifdef PAL_TV
		if( ChannelEdit_Flag) {
			if( ChannelEdit_on )					ChannelEdit_Char(UP);
			else {		
				UpDownTVChannel(DN);
				ChannelEdit_display();
			}
		}
		else
		#endif	
		OSDCursorMove(UP);	
	#ifdef SUPPORT_PC
	else if( GetInputSelection()==PC )		AutoAdjust();
	#endif
	else	
		return;
	*/
}

void RemoChnDn_DnKey(void)
{
/*
	#ifdef SUPPORT_DEBUG
	if( Debug_On ) {
		DebugWindowData(DN);
		return;
	}
	else 
	#endif	
	if( GetDisplayedOSD() & MENU  ) {
		OSDCursorMove(DN);	
	}
	else
		#ifdef SUPPORT_LINE_OSDDEMO
		DisplayLineOSD();
		#endif
		return;
		*/
}

void RemoVolUp_RightKey(void)
{
/*
	#ifdef SUPPORT_DEBUG
	if( Debug_On ) {
		DebugWindowCursor(UP);
		return;
	}
	else 
	#endif	
	if(  GetDisplayedOSD() & MENU  ) {
		if( OnChangingValue ) 					OSDValueUpDn(UP );
		else										OSDSelect();			
	}
	else //if(GetDisplayedOSD() & TVVOL)
	{
	#ifdef HTRV  //Pinchi 20150827 add
	    if(GaugeSetup)//william-v1.43-970319
			OSDValueUpDn(UP );	
		else
	#endif
		{
			ChangeVol(1);
			DisplayVol();
		}
	}
	*/
}

void RemoVolDn_LeftKey(void)
{
/*
	#ifdef SUPPORT_DEBUG
	if( Debug_On ) {
		DebugWindowCursor(DN);
		return;
	}
	else 
	#endif
	if(  GetDisplayedOSD() & MENU  ) 
		{
		if( OnChangingValue )						
			OSDValueUpDn(DN );
		else 
			OSDHighMenu();	
		}
	else //if(GetDisplayedOSD() & TVVOL)
	{
	#ifdef HTRV  //Pinchi 20150827 add
	    if(GaugeSetup)//william-v1.43-970319
			OSDValueUpDn(DN);	
		else
	#endif
		{
			ChangeVol(-1);
			DisplayVol();
		}
	}
	*/
}

//=============================================================================
//
//=============================================================================
BYTE ActionRemo(BYTE _RemoDataCode, BYTE AutoKey)
{
/*
	SetOSDLastKeyInTime();
	
	 #if (defined SUPPORT_PC) || (defined SUPPORT_DTV) 	
     #if 0//andy ML084 980930
	  if( (PcMode==EE_PC_NO_SIGNAL) && (GetInputSelection()==PC || GetInputSelection()==DTV) ) {
		 if( PcMode==EE_PC_NO_SIGNAL ) {
			 switch (_RemoDataCode) {
			   case REMO_INPUT:
			 //case REMO_STANDBY:	
			 break;
				
			default:
				tic_pc = 0;
				return 1;
			}
		}
	}
     #endif
	#endif // SUPPORT_PC

	switch(_RemoDataCode) {

	case REMO_STANDBY:				// power
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nSTANDBY Mode, Autokey:%02x", A utoKey);
		#endif
		OSDLastKeyInTime = 0xffff;
		if( AutoKey ) return 1;
		return 0;					// power off

	case REMO_MUTE:
		#if 0
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nMUTE Mode, Autokey:%02x", AutoKey);
		#endif
		if( AutoKey ) return 1;
		ToggleAudioMute();
		if( IsAudioMuteOn() )		DisplayMuteInfo();
		else{						
			ClearMuteInfo();
			//if( DisplayInputHold ) DisplayInput();		//william-981029
		}
		#endif
		break;

	case REMO_INPUT:
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nChange INPUT, Autokey:%02x", AutoKey);
		#endif
		ChangeInput(GetNextInputSelection());
		return 1;

	case REMO_INFO:
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nINFO display, Autokey:%02x", AutoKey);
		#endif
		if( GetDisplayedOSD() & INPUTINFO )	ClearInput();
		else		DisplayInput();
		return 1;

	case REMO_NUM0:
	case REMO_NUM1:
	case REMO_NUM2:
	case REMO_NUM3:
	case REMO_NUM4:
	case REMO_NUM5:
	case REMO_NUM6:
	case REMO_NUM7:
	case REMO_NUM8:
	case REMO_NUM9:
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nNumber %02x Key Pressed, Autokey:%02x", (_RemoDataCode-0x30), AutoKey);
		#endif
		if( AutoKey )								return 1;

		else										return 1;
		break;

	case REMO_CHNUP:
		#ifdef DEBUG_KEYREMO
		dPuts("\r\nChannel UP");
		#endif
		RemoChnUp_UpKey();
		break;
	
	case REMO_CHNDN:
		#ifdef DEBUG_KEYREMO
		dPuts("\r\nChannel DOWN");
		#endif
		RemoChnDn_DnKey();
		break;
	
	case REMO_SELECT:
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nSELECT key pressed, Autokey:%02x", AutoKey);
		#endif
		if( AutoKey ) return 1;
		if(  GetDisplayedOSD() & MENU  )		{
				OSDSelect();
		}
		break;

	case REMO_VOLUP:
		#ifdef DEBUG_KEYREMO
		dPuts("\r\nVolumn UP");
		#endif
		RemoVolUp_RightKey();
		break;
	case REMO_VOLDN:
		#ifdef DEBUG_KEYREMO
		dPuts("\r\nVolumn DOWN");
		#endif
		RemoVolDn_LeftKey();
		break;


	case REMO_MENU:
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nMENU display, Autokey:%02x", AutoKey);
		#endif
		if( AutoKey ) return 1;
	#ifdef HS_NEWOSDMENU	
		RemoMenu_MenuKey(1);
	#else
		RemoMenu_MenuKey();
	#endif
		break;

	case REMO_EXIT:
		#ifdef DEBUG_KEYREMO
		dPuts("\r\n EXIT");
		#endif
		if(  GetDisplayedOSD() & MENU  )		
			DoAction(EXITMENU);
		break;

	#ifdef SUPPORT_PC
	case REMO_AUTO:
		#ifdef DEBUG_KEYREMO
		dPuts("\r\n AUTO mode adjust for PC input");
		#endif
		if( GetInputSelection()==PC )
			AutoAdjust();
		break;
	#endif

	#ifdef WIDE_SCREEN
	case REMO_ASPECT:
		#ifdef DEBUG_KEYREMO
		dPuts("\r\n Screen Aspect Ratio change to WIDE");
		#endif
		SetItemValue(WIDESCREEN,UP);
		break;
	#endif
	}

	#ifdef SUPPORT_DEBUG
	switch(_RemoDataCode) {		
	case REMO_DEBUG:		DebugWindowControl();		break;
	case REMO_READ:			Debug_Read();		break;
	case REMO_WRITE:		Debug_Write();	
							Debug_Read();	// Read back
												break;
	}
	#endif
*/
	return 1;
}

BYTE CheckKeyIn(void)
{
	BYTE ret=1, ikey/*,OSD_LOCK*/;
/*
#ifdef SUPPORT_2835_CLK_TUNE_BACK_DOOR	//Pinchi 20160929 add
	if ((IsEnter2835ClkTune == 1) && (IsRun2835ClkTune == 0))
	{
		if ((SystemClock - Enter2835TuneClock) >= 10)
			IsEnter2835ClkTune = 0;
	}
#endif
	*/	


	if( (ikey = GetKey(1)) ) 
	{

#if (_DEBUG_CheckKeyIn==ON)	
		switch (ikey) {
			case POWERKEY: Printf("\r\nPWR key"); break;
			case MENUKEY: Printf("\r\nMENU key"); 
				/*
				if(VS8812Read(0xf0)==0x85)
				VS8812Write(0xf0,0x00);	
				else
				VS8812Write(0xf0,0x85);					
				*/
			break;
			case INPUTSOURCEKEY: Printf("\r\nJUMP key"); break;
			case UPKEY:	 Printf("\r\nUP key"); break;
			case DOWNKEY: Printf("\r\nDOWN key"); break;
	            	case SELECTKEY: Printf("\r\nSELECT key"); break;//Printf("\r\nSELECTKEY key pressed"); break;
	            	case UNLOCKKEY: Printf("\r\nUNLOCK key"); break;
			default: Printf("\r\nNOT ASSIGNED key pressed"); break;
		}	
		
#endif

	#if 0///(_DEBUG_MESSAGE_Monitor==ON)
	Monitor_flag=_TRUE;
	MCUTimerReactiveTimerEvent(SEC(30), _USER_TIMER_EVENT_Monitor_ON);	
	#endif
	
	   if((TriggerFlag)||((PowerFlag==OFF)&&(ikey!=POWERKEY)))     //�P�_�O�_�i�JTigger Mode or power off mode
	    { 
	      ikey=0;
	      return 1;
 	   }
   		else if(GetItemValue(0xbb)==_TRUE ) 
       	{

	       if(ikey==UNLOCKKEY)
	       	{
	       		MCUTimerDelayXms(2000);
					ikey = GetKey(1);		
						if(ikey==UNLOCKKEY)
							   WriteEEP(EEP_ONOFFChoice,0);	
	       	}

	   	ShowOSDLOCK();	
		
		ikey=0;
		return 1;
		
       	}
	//mask by ryan@20171018	OSD_LOCK = ReadEEP(EEP_ONOFFChoice);
//	OSD_LOCK=OSD_LOCKF;

		
	#if 0///def AWT_NEW_SLIM_MODEL  //Pinchi 20150203 add for AWT new slim model
		if (((~P2 & UnLockKEY) == UnLockKEY) && (OSD_LOCK == 0))   //	��osd lock
	#else
		//if((~P2&0x90)==0x90 && OSD_LOCK==0)   //	��osd lock
	#endif
	#if 0
		{ 
			Wait_ms(2000);
		#if 0///def AWT_NEW_SLIM_MODEL  //Pinchi 20150203 add for AWT new slim model
			if (((~P2 & UnLockKEY) == UnLockKEY) || (LockKeyState == 3))
		#else
			if((~P2&0x90)==0x90||LockKeyState==3)
		#endif
			{
				WriteEEP(EEP_ONOFFChoice,0x01);
				OSD_LOCK=1;//ryan@20170908
				OSD_LOCKF=1;
			//	DisplayInformation(InfoOSDMENUATCIVE);
				ikey=0;
				Wait_ms(2000);
/*
				if(TriggerFlag&0x20)//if rear trigger
				{
				DisplayGaugeOSD();
				}
				else
				{
				ClearGAUGEINFO();
				lock_keycnt=0;
				LockKeyState=0;
				DisplayInput();
			
				}
				*/
				
			
			//#ifdef AWT_NO_SIGNAL_MSG  //Pinchi 20150320 add
			//IsDrawNoVideoSignal = 0;
			//#endif
			
			//	return 1;  //Pinchi 20150825 add  //Pinchi 20150825 disable
			}
		}
	#endif
/*
	    if(TriggerFlag&&(ikey!=INPUTSOURCEKEY))     //�P�_�O�_�i�JTigger Mode
	    { 
	      ikey=0;
	      return 1;
	    }
*/	
	#if 0///ndef AWT_ML072Q  //Pinchi 20140911 add for AWT ML072Q
		if(LockKeyState==3 && OSD_LOCK==0)
		{
			WriteEEP(EEP_ONOFFChoice,0x01);
			OSD_LOCKF=1;
			DisplayInformation(InfoOSDMENUATCIVE);
			Wait_ms(2000);
			//lock_keycnt=0;
			DisplayInput();
			LockKeyState=0;
			PowerLED(ON);
			
			#ifdef AWT_NO_SIGNAL_MSG  //Pinchi 20150320 add
			IsDrawNoVideoSignal = 0;
			#endif
		}
	#endif

	//	if(lock_keycnt==0 && OSD_LOCK==0 && LockKeyState!=0)
	//	{
	//		LockKeyState=0;
	//		PowerLED(ON);
	//	}

 
	#if 0//def AWT_ML072Q  //Pinchi 20140918 add for AWT ML072Q
	  #ifdef AWT_NEW_SLIM_MODEL  //Pinchi 20150203 add for AWT new slim model
		if (ikey && (OSD_LOCK == 0) && ((~P2 & UnLockKEY) != UnLockKEY))
	  #else
		if (ikey && (OSD_LOCK == 0) && ((~P2 & 0x90) != 0x90)&&(TriggerFlag==0))
	  #endif
		{
			ClearOSDInfo();//ryan@20170922
			DisplayInformation(InfoOSDMENULOCKED);
			delay(500);
			ClearGAUGEINFO(); 
			DisplayInput();
			//Printf("\r\OSD_LOCK",0);
								#ifdef AWT_NO_SIGNAL_MSG  //Pinchi 20150320 add
								IsDrawNoVideoSignal = 0;
								#endif
			return 1;
		}
	#endif
		
//		SetOSDLastKeyInTime();
		if(  GetDisplayedOSD() & MENU  ) 
		{
		MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);
		MCUTimerActiveTimerEvent(SEC(15),_USER_TIMER_EVENT_OSD_DISABLE);
		}
	
		switch( ikey ) {
			case  POWERKEY :
				Printf("\r\nPOWER key ");
				if(PowerFlag==ON)
					PowerFlag=OFF;
				else
					PowerFlag=ON;
					
				ret=0;		
				WriteEEP(EEP_Powerflag,PowerFlag);				
		
                break;

			case MENUKEY:
				Printf("\r\nMENUKEY key ");
			//	if(  GetDisplayedOSD() & MENU  ) 
			//	DrawMenulist(0);

				   if(  GetDisplayedOSD() & MENU  ) 
				            {
				            
				                if( OnChangingValue )
				                	{
									//ret = ActionRemo( REMO_MENU, 0 );
								OnChangingValue=0;
								
								DisplayCursor();
									//ret = ActionRemo( REMO_MENU, RepeatKey );
				                	}
								else
								 	OSDSelect();
				            }
				   else
				   	{
				   	
				   	if( GetDisplayedOSD() & TVVOL)
						CloseOSDMenu();

					   MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);
					   MCUTimerActiveTimerEvent(SEC(15),_USER_TIMER_EVENT_OSD_DISABLE);

					   OpenOSDMenu(0);

				   	}
			
			
				break;

			//------------------------------------------------------------------------		
			
			case UPKEY:
				if(  GetDisplayedOSD() & MENU  ) 
					{
						if( OnChangingValue )
							{
							OSDValueUpDn(UP );
							
							}
						else		
						{
							//if(OSDMenuLevel == 1||OSDMenuLevel == 2)
							//   OSDCursorMove( DN);
							//else	
							   OSDCursorMove( UP );
								//Printf("(2)");
						}
							
					}
					else
					{
							ChangeVol(UP);
							DisplayVol();
						
					}
				
				/*
					else	
					{
						#ifdef Hs_debug
						Printf("\r\nREMO_VOLUP",0);
						#endif
	                    ret=ActionRemo(REMO_VOLUP,0);
						return 1;
					}
					*/
				break;
		
			case DOWNKEY:
				if(  GetDisplayedOSD() & MENU  )
					{
						if( OnChangingValue )				
							OSDValueUpDn(DN );
						else		
						{
							//if(OSDMenuLevel == 1||OSDMenuLevel == 2)
							//   OSDCursorMove( UP);
							//else	
							OSDCursorMove( DN);
						}
					}
				else
					{
								ChangeVol(DN);
								DisplayVol();
					}
				/*
					else
					{
						#ifdef Hs_debug
						Printf("\r\nREMO_VOLDN",0);
						#endif
                    	ret=ActionRemo(REMO_VOLDN,0);
						return 1;
					}
					*/
				break;

			case SELECTKEY:
				
			//	ret=1;
			   if(  GetDisplayedOSD() & (MENU |TVVOL) ) 
				CloseOSDMenu();
			   
				SetSelectMode();
				//if(SpeedDomeMode==0)
			//	{
					/*
					if((SelectModeType == AVMode)&&(GetDisplayedOSD() & MENU))
					CloseOSDMenu();
					
					SetSelectMode();
					*/
				//}
				//else
				//{
					/*
					CAM_TxCommand_toRS485(CAM_PTZ_LEFT);
					CAM_PTZ_RUN_flag = 1;
					*/

				//}
				
				break;

			//------------------------------------------------------------------------		

			case INPUTSOURCEKEY:

					if(  GetDisplayedOSD() & (MENU |TVVOL) ) 
					 CloseOSDMenu();

					if(NowModeState==SelectMode)
					NowModeState=JumpMode;
					else
					NowModeState=SelectMode;
		
					SetJumpMode();	
				/*
					if((TriggerFlag)&&(Priority==0))//ryan@20170907
					{
						//not change state... ryan@20170914
						;//Printf("\r\n*INPUTSOURCEKEY* Priority=%02x", (WORD)Priority);
					}
				else if(TriggerFlag&&(TR_JUMP_Flag==0)&&(Priority==1))
					{					
					if(ReadEEP(EEP_ONOFFChoice)==1)//unlock key work
					{
					if((TriggerFlag&0x20))
					 ClearGAUGEINFO();					 

						TR_JUMP_Flag=1;
						
					if(RELEASE_TR_JUMP_Flag==0) TR_JUMP_HOLD=NowModeState;					

					RELEASE_TR_JUMP_Flag=1;
					
					NowModeState=JumpMode;

					SetJumpMode();		
					}
					
					}
				else if(SpeedDomeMode==0)
				{
				
					if(TR_JUMP_Flag)
					{
				    TriggerFlag=0;			
				    TriggerMode=No_TRIG;
					TR_JUMP_Flag=0;
					NowModeState=SelectMode;
				//Printf("\r\n((SelectModeType=%02x    NowModeState=%02x))",(WORD)SelectModeType,(WORD)NowModeState);
					}
					else
					{


	if((SelectModeType == AVMode)&&(GetDisplayedOSD() & MENU))
			CloseOSDMenu();
					
					
						if(NowModeState==SelectMode)
						NowModeState=JumpMode;
						else
						NowModeState=SelectMode;
                				SetJumpMode();
						
					}
				}
				else
				{
					CAM_TxCommand_toRS485(CAM_PTZ_RIGHT);	
					CAM_PTZ_RUN_flag = 1;

				}
				*/
			break;

		#if 0//def QUAD
			case  EVENTKEY:
				Printf("\r\nEVENTKEY key pressed");
		////	Hs_DvrTxCmdPaser(BOOKMARK);				
               	break;
		#endif

		#if 0/// def SUPPORT_SDCARD  //Pinchi 20160929 disable for no DVR function
			case VersionKEY:
				#ifdef E_Wintek
				FactoryMenu=1;
				#endif
				Printf("\r\nVersionKEY key pressed");
			#ifdef SUPPORT_SDCARD	
				if(DVR_Ready)
				{
				#if  1
					if(AudioVol==15)
					{
						Hs_DvrTxCmdPaser(GetDVRVersion);
					}
					#ifndef SUPPORT_SDCARD
					else
						DisplayDVRStatus();
					#endif
				#endif
				}
			#else
				if(AudioVol==15)
				{
					WriteDecoder(0xff, 0x01);
					Wait_ms(5);
					//WriteDecoder(0x82, 0x0c);
					WriteDecoder(0x82, 0x01);
					Wait_ms(5);
					WriteDecoder(0xff, 0x00);
					DisplayFWVersion();
				}
			#endif
				//else
				//	{
				//	CloseOSDMenu();	
				//	DisplayInput();
				//	}
				break;
		#endif

		#if 0///ef SUPPORT_2835_CLK_TUNE_BACK_DOOR  //Pinchi 20160929 add
			case Tune2835ClkKEY :
				if ((AudioVol == 20) && (IsEnter2835ClkTune == 1))
				{
					if (IsRun2835ClkTune == 1)
					{
						IsRun2835ClkTune = 0;
						PowerLED(ON);
						ClearDataCnt(INPUTINFO_ADDR, 2);
						IsEnter2835ClkTune = 0;
					}
					else
					{
						IsRun2835ClkTune = 1;
						PowerLED(2);
						HS_Display2835ClkTuneVal(b2835ClkTuneVal);
					}

					Printf("\r\n[MSG]CheckKeyIn()=> IsRun2835ClkTune=%bx",(BYTE)IsRun2835ClkTune);
				}
				break;

			case Enter2835TuneKEY :
				/*
				if (AudioVol == 23)
				{
					PowerLED(2);
					Wait_ms(100);
					PowerLED(ON);
					Wait_ms(100);
					PowerLED(2);
					Wait_ms(100);
					PowerLED(ON);
					Wait_ms(100);
					PowerLED(2);
					Wait_ms(100);
					PowerLED(ON);
					IsEnter2835ClkTune = 1;
					Enter2835TuneClock = SystemClock;
				}
				*/
				break;
		#endif
		}
	}
	return (ret);

}

void SEQMode(void)
{

		if(SEQIndex<3)
		SEQIndex++;
		else
		SEQIndex=0;
		
		setloopoutmode(SEQTable[SEQIndex]);

}

void SetJumpMode(void)
{
   BYTE val;
 	//Printf("\r\nNowModeState",(WORD)NowModeState);
 	LCDPowerOFF();
	MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
   if(NowModeState==JumpMode)
   {   
	 val=ReadEEP(EEP_JUMPMode);

		if(val==1)
		{
		 SEQFlag=ON;
		SEQIndex=0;
		 MCUTimerCancelTimerEvent( _USER_TIMER_EVENT_SEQ_CHECK);
		 MCUTimerActiveTimerEvent(SEC(TimeStep), _USER_TIMER_EVENT_SEQ_CHECK);
		}
	
	setloopoutmode(JumpTable[val]) ;		 
   	} 
  	 else
   	{
	  SEQFlag=OFF;   
		setloopoutmode(MainSetSelectMode[SelectModeType]) ;
	    }
	 
    WriteEEP(EEP_NowModeState,NowModeState);


}

void SetSelectMode(void)
	{

/*
	CAM_A,         // 0		
	CAM_B,         // 1		
	CAM_C,         // 2		
	CAM_R,         // 3		
	CAM_AB,        // 4		
	CAM_CR,        // 8		
	CAM_AR,        // 9		
	CAM_RB,        // 10		
	CAM_AC,        // 11		
 	CAM_CB,        // 12		
	CAM_RAB_T,     // 13	
	CAM_RAB_H,     // 14	
	CAM_QUAD,      // 15	
*/
	/*
		if(Priority)
			Maxsource=CAM_R;
		else
			Maxsource=CAM_RAB_H;
	*/
		BYTE Maxsource;//VS4210_LOOP_MODE0_w0
	
		if(Priority==_FALSE)
		Maxsource=CAM_R;
		else
		Maxsource=CAM_RAB_H;
		

		NowModeState=SelectMode;

		SEQFlag=0;
	
			if(SelectModeType>=Maxsource)
			{
 	
	
				SelectModeType=CAM_A;
		
		   }
		   else
		   {
	
		
				
				SelectModeType++;
				
			
		   }
		//   volatile 
		//   OutMapLoopModeIndex=SelectModeType;
		//   OutMapLoopModeIndex++ ;
			//		 if (OutMapLoopModeIndex >= VS4210_LOOP_MODE_NUMBER )
				//	 {
				//		 OutMapLoopModeIndex = 0 ;
				//	 }
					LCDPowerOFF();
					MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
				
				   if(  GetDisplayedOSD() & (MENU|TVVOL)  ) CloseOSDMenu();
				   
					 setloopoutmode(MainSetSelectMode[SelectModeType]) ;
					 OSDStateFlg =1 ;

//	volatile 

	//   SetSourceMode(SelectModeType,Attr_A,Attr_B,Attr_C,Attr_R); 
	
		WriteEEP(EEP_SelectMode,SelectModeType);
	
	
	  WriteEEP(EEP_NowModeState,NowModeState);
	   
		//if (ReadEEP(EEP_COSDMode) == 1)
		//	displayhold_flag = 3;
	 Printf("\r\nSelectModeType=%d",(WORD)SelectModeType);
	}


//=============================================================================
//		Get Key 
//=============================================================================
#define CUPKEY 	DOWNKEY
#define CDOWNKEY 	UPKEY

BYTE GetKey(BYTE repeat)
{

	bit ready;
	//BYTE KeyBuffer2;  //Pinchi disable for compiler warning
	ready = KeyReady;
	KeyReady = 0;


//	#if 1//ryan@20170914

		//if((ChangeKey==1)&&(Key==0))
	
	if((ChangeKey==1)&&(Key==0))
	{
	#if 1
	ready=1;


/*
	if((updn_reg[0] == 3) && (updn_reg[1] == 2)&& (updn_reg[2] == 0)&& (updn_reg[3] == 1))  		//Up Key     
			Key=DOWNKEY;  
		else	if((updn_reg[0] ==0) && (updn_reg[1] == 2)&& (updn_reg[2] == 3)&& (updn_reg[3] == 1))  //Down Key    
			Key=UPKEY;  
*/
		#if 1
		if((updn_reg[0] == 1) && (updn_reg[1] ==3)&& (updn_reg[2] == 2)&& (updn_reg[3] == 0))			//Up Key	 
			Key|=DOWNKEY;
		#else
	if((updn_reg[0] == 3) && (updn_reg[1] == 2)&& (updn_reg[2] == 0)&& (updn_reg[3] == 1))			//Up Key	 
		Key|=DOWNKEY;
		#endif	
//	else if((updn_reg[0] == 2) && (updn_reg[1] == 0)&& (updn_reg[2] == 1)&& (updn_reg[3] == 3))  //UP Key
//		Key|=DOWNKEY;
//	else if((updn_reg[0] == 0) && (updn_reg[1] == 1) && (updn_reg[2] == 3)&& (updn_reg[3] == 2))  //UP Key
//		Key|=DOWNKEY;
//	else if((updn_reg[0] == 1) && (updn_reg[1] == 3)&& (updn_reg[2] == 2)&& (updn_reg[3] == 0))  //UP Key
//		Key|=DOWNKEY;
//	else if((updn_reg[0] == 0) && (updn_reg[1] == 2)&& (updn_reg[2] == 3)&& (updn_reg[3] == 1))  //DN Key
//			Key|=UPKEY;
//	else if((updn_reg[0] == 3) && (updn_reg[1] == 1)&& (updn_reg[2] == 0)&& (updn_reg[3] == 2))  //DN Key
//			Key|=UPKEY; 	
	else if((updn_reg[0] == 2) && (updn_reg[1] == 3)&& (updn_reg[2] == 1)&& (updn_reg[3] == 0)) //DN Key
			Key|=UPKEY;
//	else if((updn_reg[0] == 1) && (updn_reg[1] == 0)&& (updn_reg[2] == 2)&& (updn_reg[3] == 3)) //DN Key
//			Key|=UPKEY;



	#else
		ready=1;
		if((updn_reg[0] == 3) && (updn_reg[1] == 2)&& (updn_reg[2] == 0)&& (updn_reg[3] == 1))  		//Up Key     
			Key|=DOWNKEY;
		else if((updn_reg[0] == 2) && (updn_reg[1] == 0)&& (updn_reg[2] == 1)&& (updn_reg[3] == 3))  //UP Key
			Key|=DOWNKEY;
		else if((updn_reg[0] == 0) && (updn_reg[1] == 1) && (updn_reg[2] == 3)&& (updn_reg[3] == 2))  //UP Key
			Key|=DOWNKEY;
		else if((updn_reg[0] == 1) && (updn_reg[1] == 3)&& (updn_reg[2] == 2)&& (updn_reg[3] == 0))  //UP Key
			Key|=DOWNKEY;
		else if((updn_reg[0] == 0) && (updn_reg[1] == 2)&& (updn_reg[2] == 3)&& (updn_reg[3] == 1))  //DN Key
				Key|=UPKEY;
		else if((updn_reg[0] == 3) && (updn_reg[1] == 1)&& (updn_reg[2] == 0)&& (updn_reg[3] == 2))  //DN Key
				Key|=UPKEY;		
		else if((updn_reg[0] == 2) && (updn_reg[1] == 3)&& (updn_reg[2] == 1)&& (updn_reg[3] == 0))	//DN Key
				Key|=UPKEY;
		else if((updn_reg[0] == 1) && (updn_reg[1] == 0)&& (updn_reg[2] == 2)&& (updn_reg[3] == 3))	//DN Key
				Key|=UPKEY;
#endif
/*
if(Encoder_flag==OFF)
{
	Key=0;
		Printf("(Encoder_flag==OFF)");
}
else
{
	Printf("(Encoder_flag==ON)");
	Encoder_flag=OFF;
	//MCUTimerActiveTimerEvent(SEC(0.2), _USER_TIMER_EVENT_Encoder_Loop);
}
*/
		ChangeKey = 0;	

	}   

	
	#if 0//elif 0
	if(ChangeKey==1)
	{

		ready=1;
		
		if((updn_reg[0] == 1) && (updn_reg[1] == 3) && (updn_reg[2] == 2)&& (updn_reg[3] == 0))  //UP Key
			Key|=CUPKEY;
		//else if((updn_reg[0] == 3) && (updn_reg[1] == 2)&& (updn_reg[2] == 0)&& (updn_reg[3] == 1))  //UP Key
		//	Key|=CUPKEY;

		if((updn_reg[0] == 1) && (updn_reg[1] == 0)&& (updn_reg[2] == 2)&& (updn_reg[3] == 3))  //DN Key
			Key|=CDOWNKEY;
		//else if((updn_reg[0] == 3) && (updn_reg[1] == 1)&& (updn_reg[2] == 0)&& (updn_reg[3] == 2))  //DN Key
		//	Key|=CDOWNKEY;
		
		ChangeKey = 0;	

		
	}   
	

	//#else
	if(ChangeKey==1)//Kane @HS 2007 0814 Ver1.31<<<<
	{
		ready=1;

		if((updn_reg[0] == 0) && (updn_reg[1] == 1) && (updn_reg[2] == 3)&& (updn_reg[3] == 2))  //UP Key
			Key|=UPKEY;
		if((updn_reg[0] == 3) && (updn_reg[1] == 2)&& (updn_reg[2] == 0)&& (updn_reg[3] == 1))  //UP Key
			Key|=UPKEY;

		if((updn_reg[0] == 0) && (updn_reg[1] == 2)&& (updn_reg[2] == 3)&& (updn_reg[3] == 1))  //DN Key
			Key|=DOWNKEY;
		if((updn_reg[0] == 3) && (updn_reg[1] == 1)&& (updn_reg[2] == 0)&& (updn_reg[3] == 2))  //DN Key
			Key|=DOWNKEY;
		 
		//if(OnChangingValue==1)
		if ((GetDisplayedOSD() & MENU) == 0) 	//william-v1.42-961203
		{
			if((updn_reg[0] == 1) && (updn_reg[1] == 3)&& (updn_reg[2] == 2)&& (updn_reg[3] == 0))  //UP Key
				Key|=UPKEY;
			if((updn_reg[0] == 2) && (updn_reg[1] == 0)&& (updn_reg[2] == 1)&& (updn_reg[3] == 3))  //UP Key
				Key|=UPKEY;
			if((updn_reg[0] == 2) && (updn_reg[1] == 3)&& (updn_reg[2] == 1)&& (updn_reg[3] == 0))  //DN Key
				Key|=DOWNKEY;
			if((updn_reg[0] == 1) && (updn_reg[1] == 0)&& (updn_reg[2] == 2)&& (updn_reg[3] == 3))  //DN Key
				Key|=DOWNKEY;
		}
		ChangeKey = 0;		//william-v1.42-961130
	}   

	#endif
	
	if( !ready ) return 0;
	if( repeat ) return Key;
	if( !RepeatKey ) return Key;

	return 0;

}




//=============================================================================
//		CheckRemo
//=============================================================================
#if 0//def REMO_RC5

BYTE IsRemoDataReady(BYTE *DataCode, BYTE *AutoKey)
{
	bit togglebit;
	static bit	LastToggle=0;
	static BYTE AutoWait=0;

	BYTE _RemoSystemCode, _RemoDataCode, _RemoDataReady;

	if( !RemoDataReady ) return 0;

	_RemoDataReady = RemoDataReady;
	_RemoDataCode = RemoDataCode;
	_RemoSystemCode = RemoSystemCode;

	*AutoKey = 0;

	EnableRemoconInt();

	#ifdef DEBUG_KEYREMO
	dPrintf("\r\n(CheckRemo)R-S:%02x, R-D:%02x", (WORD)_RemoSystemCode, (WORD)_RemoDataCode);
	#endif

	if( (_RemoSystemCode & 0xc0) != 0xc0 ) return 0;
	if( (_RemoSystemCode & 0x1f) != REMO_CUSTOM ) return 0;

	togglebit = LastToggle;
	if( _RemoSystemCode & 0x20 ) LastToggle = 1;
	else LastToggle = 0;

	#ifdef DEBUG_KEYREMO
	dPrintf("**** %d  %d ", (WORD)LastToggle, (WORD)_RemoDataReady);
	#endif

	if( LastToggle != togglebit ) {			// new key
		AutoWait =  0;
	}
	else if( AutoWait>=4 ) {				// auto key
		AutoWait++;
		if( AutoWait>=6 ) {
			AutoWait = 4;
			*AutoKey = 1;
		}
		else return 0;
	}
	else {									// wait auto key
		AutoWait++;
		return 0;
	}
	
	#ifdef DEBUG_KEYREMO
	if( *AutoKey ) {
		dPuts(" ---> AutoKey ");
	}
	else {
		dPuts(" ---> New Key ");
	}
	#endif

	*DataCode = _RemoDataCode;
	return 1;
}

///#elif defined REMO_NEC
extern DATA		BYTE	 RemoData[4];

BYTE IsRemoDataReady(BYTE *DataCode, BYTE *AutoKey)
{
	static BYTE AutoWait=0;
	static IDATA BYTE LastDataCode=0xff;
	static IDATA BYTE repeatcnt=0;
	BYTE ret =0 ;

	*AutoKey = 0;
	switch ( RemoDataReady ) {

	case 0: return 0;

	case 1:
		#ifdef DEBUG_KEYREMO
		dPrintf("\r\nNEC RemoData = %02x %02x %02x %02x", (WORD)RemoData[0], (WORD)RemoData[1], (WORD)RemoData[2], (WORD)RemoData[3] );
		#endif
		if( (RemoData[0]==REMO_CUSTOM1) && (RemoData[1]==REMO_CUSTOM2 ) && (RemoData[2]+RemoData[3])==0xff ) {
			RemoDataCode = RemoData[2];
			LastDataCode = RemoDataCode;
			AutoWait = 0;
			*AutoKey = 0;
			repeatcnt = 0;
		}
		else {
			RemoDataCode = 0xff;
			LastDataCode = 0xff;

			EnableRemoconInt();		//
			return 0;
		}
		break;

	case 2:
		AutoWait++;
		if( AutoWait>=6 ) {
			RemoDataCode = LastDataCode;
			repeatcnt++;
			if( repeatcnt>=5 ) {
				AutoWait = 5;
			}
			else
				AutoWait = 4;

			*AutoKey = 1;
		}
		else {
			EnableRemoconInt();		//
			return 0;
		}
		break;

	}

	EnableRemoconInt();		////LJY052902
	if( RemoDataCode==0xff )	return 0;

	#ifdef DEBUG_KEYREMO
	dPrintf("(R-D:%02x)  ", (WORD)RemoDataCode);
	#endif

	*DataCode = RemoDataCode;
	return 1;
}

#endif	// REMO_NEC

#if 0//def SUPPORT_2835_CLK_TUNE_BACK_DOOR  //Pinchi 20160929 add
void HS_Display2835ClkTuneVal(WORD val)
{
	BYTE mstr[]={ ' ',' ' };
	//Printf("\r\n[MSG]Display2835ClkTuneVal()=> enter to draw 2835ClkTuneVal",0);  //pinchi test
	mstr[1] =  val % 10 + '0';	 val /= 10;
	mstr[0] = val + '0';

	InitOSDWindow(Init_Osd_DisplayInput_B);
	SetDisplayedOSD(INPUTINFO);
	ClearDataCnt(INPUTINFO_ADDR, 2);
	DrawAttrCnt(INPUTINFO_ADDR , BG_COLOR_CYAN|CH_COLOR_WHITE, 2);
	WriteStringToAddr(INPUTINFO_ADDR , mstr, 2);
	
	ShowOSDWindow(INPUTINFO_OSDWIN-1,TRUE);
	ShowOSDWindowAll(1);  //0x09c  ��4bit : OSD ON, 1: OSD OFF
}
#endif

void SetNowMode(void)
{

	BYTE val;
//    Printf("\r\n[MSG]SetNowMode()=> NowModeState=%02x",(WORD)NowModeState);
//	LCDPowerOFF();
//	MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);

	if(NowModeState==SelectMode)
	{
	 Printf("NowModeState=%02x",(WORD)NowModeState);
		setloopoutmode(MainSetSelectMode[SelectModeType]) ;
	 	SEQFlag=0;
	}
	else  //JUMP mode
	{
		val=ReadEEP(EEP_JUMPMode);

	Printf("JUMPModeState=%02x",(WORD)val);

		if(val==1)
			{
		 SEQFlag=ON;
		SEQIndex=0;
		 MCUTimerCancelTimerEvent( _USER_TIMER_EVENT_SEQ_CHECK);
		 MCUTimerActiveTimerEvent(SEC(TimeStep), _USER_TIMER_EVENT_SEQ_CHECK);
			}
	setloopoutmode(JumpTable[val]) ;
	
	}   

	//MCUTimerActiveTimerEvent(SEC(5),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
}

void SetRearSource(void)
{ 

if(  GetDisplayedOSD() & (MENU|TVVOL)) CloseOSDMenu();

LCDPowerOFF();
MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);

setloopoutmode(RearTable[ ReadEEP(EEP_RearMode)]);

//setloopoutmode(CAM_R) ;
//		OSDStateFlg =1 ;

ShowGaugeOSD();

ShowTRIGOSD();

#if 0
   BYTE rev;
#ifdef QUAD
   rev=ReadEEP(EEP_RearMode);
   SetSourceMode(RearTable[rev],0x02|(Attr_A&0x01),0x02|(Attr_B&0x01),0x02|(Attr_C&0x01),0x0E|(Attr_R&0x01));
#else
	ChangeInput(SOURCE_R); 
#endif

//	Printf("\r\n[MSG]SetRearSource() => ReverseModeFlag=%bx ,RearTable[rev]=%bx",ReverseModeFlag,RearTable[rev]);  //pinchi test

#ifdef AWT_ML072Q
	if(ReverseModeFlag==1 && RearTable[rev]==CAM_R)
#else
  #ifdef HTRV  //Pinchi 20150827 change condition for HTRV
	if (RearTable[rev] == CAM_R)
  #else
	if(ReverseModeFlag==1)
  #endif
#endif
	{
	#ifdef QUAD  //Pinchi 20150827 add
	  #ifdef HTRV_SUPPORT_4_3_SCREEN  //Pinchi 20151209 add
		#ifdef HTRV_REDUCE_SCREEN_CHANGE  //Pinchi 20150828 add
		if (ReadDecoder(0x66) == 0x00)
		#endif
	  	{
			WriteDecoder(0x0a,0x13); 
			WriteDecoder(0x61,0xa6);
			WriteDecoder(0x66,0x60);//<William-960905>  
		//	Printf("\r\n[MSG]SetRearSource() => Reverse trigger to 4:3 & ReadDecoder(0x0a)=%bx , ReadDecoder(0x66)=%bx",ReadDecoder(0x0a),ReadDecoder(0x66));  //pinchi test
		}
	  #endif

	  #ifdef AWT_SUPPORT_4_3_SCREEN//Pinchi 20151209 add
		if (ReadDecoder(0x66) == 0x00)
	  	{
			WriteDecoder(0x0a,0x13); 
			WriteDecoder(0x61,0xa6);
			WriteDecoder(0x66,0x60);//<William-960905>  
		//	Printf("\r\n[MSG]SetRearSource() => Reverse trigger to 4:3 & ReadDecoder(0x0a)=%bx , ReadDecoder(0x66)=%bx",ReadDecoder(0x0a),ReadDecoder(0x66));  //pinchi test
		}
	  #endif



	  
	  Printf("\r\n[MSG]SetRearSource() => Reverse trigger & keep 16:9 ,ReadDecoder(0x66)=%bx",ReadDecoder(0x66));  //pinchi test
	#else
		WriteDecoder(0x0a,0x13); 
		WriteDecoder(0x61,0xac);
		WriteDecoder(0x66,0x60);//<William-960905>  
	#endif
	}


	if(ReverseModeFlag==1)///add by ryan@20180328 AV mode trigger R 4:3/16:9���D
	{///4:3
	WriteDecoder(0x0a,0x13); 
	WriteDecoder(0x61,0xa6);
	WriteDecoder(0x66,0x60);//<William-960905>  
	}
	else
	{///16:9
	WriteDecoder(0x0a,0x16); ///_FULLPANEL_20140313_xx
	WriteDecoder(0x61,0x80);
	WriteDecoder(0x66,0x00);
	}

#if 1  //Pinchi add   
   if( ReadEEP(EEP_GAUGEMode)==0 && ReadEEP(EEP_RearMode)==0)
  	  DisplayGaugeOSD();
#endif 
#endif
}

void SetLeftSource(void)
{
	if(  GetDisplayedOSD() & (MENU|TVVOL)) CloseOSDMenu();


	//setloopoutmode(CAM_A) ;
	//			OSDStateFlg =1 ;
						LCDPowerOFF();
					MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
setloopoutmode(LeftTable[ ReadEEP(EEP_IMAGE_A_Mode)]);

ShowTRIGOSD();

#if 0
#ifdef QUAD	
    BYTE rev;

	#ifdef AWT_ML072Q
	    rev=ReadEEP(EEP_IMAGE_A_Mode);
	#else
	    rev=ReadEEP(EEP_IMAGEMode);
	#endif
	    SetSourceMode(LeftTable[rev],0x06|(Attr_A&0x01),0x02|(Attr_B&0x01),0x02|(Attr_C&0x01),0x02|(Attr_R&0x01));
#else
	ChangeInput(SOURCE_A); 
#endif
#endif
}

void SetTriggerCSource(void)
{
	if(  GetDisplayedOSD() & (MENU|TVVOL)) CloseOSDMenu();

LCDPowerOFF();
MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);

//	setloopoutmode(CAM_C) ;
	setloopoutmode(TriggerC_Table[ ReadEEP(EEP_IMAGE_C_Mode)]);

//			OSDStateFlg =1 ;
ShowTRIGOSD();


}

void SetRightSource(void)
{
	if(  GetDisplayedOSD() & (MENU|TVVOL)) CloseOSDMenu();

	LCDPowerOFF();
	MCUTimerActiveTimerEvent(SEC(1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);

	setloopoutmode(RightTable[ ReadEEP(EEP_IMAGE_B_Mode)]);

	//setloopoutmode(CAM_B) ;
	//		OSDStateFlg =1 ;
	ShowTRIGOSD();

#if 0
#ifdef QUAD
    BYTE rev;
	#ifdef AWT_ML072Q
   		rev=ReadEEP(EEP_IMAGE_B_Mode);
	#else
    	rev=ReadEEP(EEP_IMAGEMode);
	#endif
    	SetSourceMode(RightTable[rev],0x02|(Attr_A&0x01),0x0A|(Attr_B&0x01),0x02|(Attr_C&0x01),0x02|(Attr_R&0x01));
#else
	ChangeInput(SOURCE_B); 
#endif
#endif
}
#if 0
#ifdef AWT_ML072Q  //Pinchi 20150826 add
void SetTriggerCSource(void)
{
    BYTE rev;
	rev=ReadEEP(EEP_IMAGE_C_Mode);
#ifdef AWT_ML072Q

	  Wait_ms(500);///������� ryan@20171019

//  #if 1  //Pinchi 20140925 modify for trigger C setting of AWT ML072Q
  #ifdef AWT_TW2835_DRAW_T_ICON  //Pinchi 20150415 add to draw T icon by TW2835 when CAM C trigger
 //   SetSourceMode(TriggerC_Table[rev],0x02|(Attr_A&0x01),0x0A|(Attr_B&0x01),0x02|(Attr_C&0x01),0x02|(Attr_R&0x01));
    SetSourceMode(TriggerC_Table[rev],0x02|(Attr_A&0x01),0x02|(Attr_B&0x01),0x22|(Attr_C&0x01),0x02|(Attr_R&0x01));
  #else
    SetSourceMode(TriggerC_Table[rev],0x02|(Attr_A&0x01),0x02|(Attr_B&0x01),0x02|(Attr_C&0x01),0x02|(Attr_R&0x01));

	//Pinchi 20140925 add for trigger C display char of AWT ML072Q 
	ClearOSDInfo();
	InitOSDWindow(Init_Osd_DisplayInput_M);

	ClearDataCnt(INPUTINFO_ADDR , 4); // Total 42 Char.
	SetDisplayedOSD(INPUTINFO);
	DrawAttrCnt(INPUTINFO_ADDR , Trigger_COLOR , 4 );  
	WriteStringToAddr(INPUTINFO_ADDR ,"T" , 1); 

    ShowOSDWindow(INPUTINFO_OSDWIN-1 , TRUE);
    ShowOSDWindowAll(1);  //0x09c  ��4bit : OSD ON, 1: OSD OFF
    WriteTW88(TW88_WINBLENDING,0x08);
  #endif
#else
    SetSourceMode(CAM_C,Attr_A,Attr_B,Attr_C,Attr_R);
#endif
}
#endif
#endif

#if (_DEBUG_CheckKeyIn==ON)
#define Trigger_debug 1
#endif

BYTE CheckTrigger(void)
{

	BYTE TriggerFlagtemp;

static BYTE PT=0,POT=0,POTF=0,CPON=0,PPON=0,BT[4]={0,0,0,0};
BYTE x,temp;

//#ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150210 add
//   BYTE j , num;
//#endif



//#ifdef AWT_ML720Q_NEW_TRIGGER_FUN  //Pinchi 20141031 add for AWT request each trigger can work by PRIORITY OFF
//	CheckNewTriggerFunc();
//#endif
	temp=~(GET_PCA9554PW(PCA9554PW_reg0))&0x0F;

TriggerFlagtemp=TriggerFlag;
   	TriggerFlag=0x00;

   //LEFT_T=1;
   if((temp&LEFT_T)==LEFT_T)
   {

      Wait_ms(2);
	  if((temp&LEFT_T)==LEFT_T)
	  	TriggerFlag=TriggerFlag|LEFT_T;   
   }

   //Right_T=1;
   if((temp&Right_T)==Right_T)
   {
      Wait_ms(2);
	  if((temp&Right_T)==Right_T)
	  	TriggerFlag=TriggerFlag|Right_T;  
   }

	  if((temp&PARK_T)==PARK_T)
	  {
		 Wait_ms(2);
		 if((temp&PARK_T)==PARK_T)
		 TriggerFlag=TriggerFlag|PARK_T;	

	  }
 

   if((temp&REVERSE_T)==REVERSE_T)
   {
      Wait_ms(2);
	  if((temp&REVERSE_T)==REVERSE_T)
	  	TriggerFlag=TriggerFlag|REVERSE_T;  
   }





#if 0
   VGA_T=1;
   if(VGA_T==0)
   {
      Wait_ms(10);
	  if(VGA_T==0)
	  	TriggerFlag=TriggerFlag|0x08;  
   }

    CAMD_T=1;
   if(CAMD_T==0)
   {
      Wait_ms(10);
	  if(CAMD_T==0)
	  	TriggerFlag=TriggerFlag|0x04;  
   }
#endif

#if 0

return TriggerFlag;

#else
//#ifndef QUAD
/*
#if 0///ef AWT_ML072Q  //Pinchi 20140902 add to change condition for AWT ML072Q
   CAMC_T=1;
   if(CAMC_T==0)
   {
      Wait_ms(2);
	  if(CAMC_T==0)
	  	TriggerFlag=TriggerFlag|0x08;  
   }
#endif
*/



      #if 0///def AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150206 add for AWT request each trigger can setup trigger delay
	      if (Priority == 0)
	      {
              bLastTrigger = 3;
	      }
       #endif			

	  	if(TriggerFlag!=TriggerFlagtemp)
  	 	{
	
			//if(Priority==0)
			if(Priority==ON)
			{///priority on

		#if 1//def Trigger_debug		
			#if (_DEBUG_CheckKeyIn==ON)	
			Printf("\r\nTriggerFlag=%02x  TriggerFlagtemp=%02x  CPON=%02x",(WORD)TriggerFlag,(WORD)TriggerFlagtemp,(WORD)CPON);
			#endif
		#else
	//	Printf("\r\nTriggerFlag=%02x",(WORD)TriggerFlag);
		#endif				
							////���kC�@�_��trigger 

							if((TriggerFlag!=CPON))//got left trigger
							{
								Wait_ms(50);

									temp=~(GET_PCA9554PW(PCA9554PW_reg0))&0x0F;
								   	TriggerFlag=0x00;

											   //LEFT_T=1;
											   if((temp&LEFT_T)==LEFT_T)
											   {

											      Wait_ms(2);
												  if((temp&LEFT_T)==LEFT_T)
												  	TriggerFlag=TriggerFlag|LEFT_T;   
											   }

											   //Right_T=1;
											   if((temp&Right_T)==Right_T)
											   {
											      Wait_ms(2);
												  if((temp&Right_T)==Right_T)
												  	TriggerFlag=TriggerFlag|Right_T;  
											   }

												  if((temp&PARK_T)==PARK_T)
												  {
													 Wait_ms(2);
													 if((temp&PARK_T)==PARK_T)
													 TriggerFlag=TriggerFlag|PARK_T;	

												  }
											 

											   if((temp&REVERSE_T)==REVERSE_T)
											   {
											      Wait_ms(2);
												  if((temp&REVERSE_T)==REVERSE_T)
												  	TriggerFlag=TriggerFlag|REVERSE_T;  
											   }
								}
								else
									{
									TriggerFlag=PPON;
									return PPON;
									}
								
						CPON=TriggerFlag;

						x=(TriggerFlag&(LEFT_T|Right_T|PARK_T));

						 if(TriggerFlag&REVERSE_T)//rear
						{
							;
						}
						 else if(((x==(LEFT_T|PARK_T))||(x==(Right_T|PARK_T))||(x==(LEFT_T|Right_T|PARK_T))||(x==(LEFT_T|Right_T|PARK_T)))&&((TriggerFlagtemp)==0))
						{
						PPON=0x00;
						TriggerFlag=0x00;
						return 0x00;
						}
					else if((x==(LEFT_T|PARK_T))||(x==(Right_T|PARK_T))||(x==(LEFT_T|Right_T|PARK_T))||(x==(LEFT_T|Right_T)))
					{
						TriggerFlag=TriggerFlagtemp;
						PPON=TriggerFlagtemp;
						
					}
					
	
			}
			else
			{	
			///priority off
		
	PT=TriggerFlag&(~TriggerFlagtemp);
	
					if(PT==0)
					{
					#ifdef Trigger_debug
					Printf("\r\n(PT=0)");
					#endif
					PT=TriggerFlag;		
					}

			if(PT==0)
				{
				BT[0]=0;
				BT[1]=0;
				BT[2]=0;
				BT[3]=0;
				#ifdef Trigger_debug
				Printf("\r\n(CLR)");
				#endif
				}
			else
				{
						 if(BT[3]&PT)
						{
						PT=PT&BT[3];	
						#ifdef Trigger_debug
						Printf("\r\n(D2)");
						#endif
	
						}
						 else if(BT[2]&PT)
						{
						PT=PT&BT[2];	
							if((BT[3]&TriggerFlag))
							{
							BT[2]=BT[3];
							BT[3]=PT;
							}
							else
								{
								BT[3]=0;
								}
						
						#ifdef Trigger_debug
						Printf("\r\n(C2)");
						#endif
						
						}
						else if(BT[1]&PT)
						{
						PT=PT&BT[1];	
							if((BT[3]&TriggerFlag))
							{
							BT[2]=BT[3];
							BT[1]=BT[2];
							BT[3]=PT;
							}
							else if((BT[2]&TriggerFlag))
							{
							BT[3]=0;
							BT[1]=BT[2];
							BT[2]=PT;
							}
							else
								{
							BT[3]=0;
							BT[2]=0;
								}
							
						#ifdef Trigger_debug
						Printf("\r\n(B2)");
						#endif
								
						}
						else if(BT[0]&PT)
							{
							PT=PT&BT[0];	
							if((BT[3]&TriggerFlag))
							{
							BT[2]=BT[3];
							BT[1]=BT[2];							
							BT[0]=BT[1];
							BT[3]=PT;
							}
							else if((BT[2]&TriggerFlag))
							{
							BT[3]=0;
							
							BT[1]=BT[2];
							BT[0]=BT[1];
							BT[2]=PT;
							}
							else if((BT[1]&TriggerFlag))
							{
							BT[3]=0;
							BT[2]=0;
							
							BT[0]=BT[1];
							BT[1]=PT;
							}
							else
								{
							BT[3]=0;
							BT[2]=0;
							BT[1]=0;							
								}

								#ifdef Trigger_debug
								Printf("\r\n(A2)");
								#endif
							}
					 	else if(BT[0]==0)
						{
						BT[0]=PT;
						#ifdef Trigger_debug
						Printf("\r\n(A)");
						#endif
						}
						else if(BT[1]==0)
						{
						BT[1]=PT;
						#ifdef Trigger_debug
						Printf("\r\n(B)");
						#endif
						}
						else if(BT[2]==0)
						{
						BT[2]=PT;	
						#ifdef Trigger_debug
						Printf("\r\n(C)");
						#endif
						}
						else if(BT[3]==0)
						{
						BT[3]=PT;	
						#ifdef Trigger_debug
						Printf("\r\n(D)");
						#endif
						}

							for(x=0;x<4;x++)//clear no trigger
								if((BT[x]&TriggerFlag)==0)
									{
										BT[x]=0;	
										
										if(x==0)
											{
											BT[0]=BT[1];
											BT[1]=BT[2];
											BT[2]=BT[3];
											BT[3]=0;
											}
										else if(x==1)
											{
											BT[1]=BT[2];
											BT[2]=BT[3];
											BT[3]=0;
											}
										else if(x==2)
											{
											BT[2]=BT[3];
											BT[3]=0;
											}
									}
					}
			

if(PowerFlag==OFF)///power off���A
{
						////���kC�@�_��trigger 
							if(TriggerFlag&(LEFT_T|Right_T|PARK_T))//got left trigger
							{
								Wait_ms(50);
								temp=~(GET_PCA9554PW(PCA9554PW_reg0))&0x0F;

																	TriggerFlag=0x00;
								
																			   //LEFT_T=1;
																			   if((temp&LEFT_T)==LEFT_T)
																			   {
								
																				  Wait_ms(2);
																				  if((temp&LEFT_T)==LEFT_T)
																					TriggerFlag=TriggerFlag|LEFT_T;   
																			   }
								
																			   //Right_T=1;
																			   if((temp&Right_T)==Right_T)
																			   {
																				  Wait_ms(2);
																				  if((temp&Right_T)==Right_T)
																					TriggerFlag=TriggerFlag|Right_T;  
																			   }
								
																				  if((temp&PARK_T)==PARK_T)
																				  {
																					 Wait_ms(2);
																					 if((temp&PARK_T)==PARK_T)
																					 TriggerFlag=TriggerFlag|PARK_T;	
								
																				  }
																			 
			
   							//	}

				
							}
				
						x=(TriggerFlag&(LEFT_T|Right_T|PARK_T|REVERSE_T));

	//Printf("\r\n(EE x=%02x POTF=%02x)",(WORD)x,(WORD)POTF);						

						if(x==0)
						POTF=0;
						
						if(TriggerFlag&REVERSE_T)
						 {
						 //	if(x&0xc8)
						//	POTF=1;

							POT=REVERSE_T;
						 }
						else if(POTF==1)
							{
							;
							}
						else if((x==(LEFT_T|PARK_T))||(x==(Right_T|PARK_T))||(x==(LEFT_T|Right_T|PARK_T))||(x==(Right_T|LEFT_T))||(x==0))
						{
						POT=0x00;
	
						}
						else
							{
							POTF=1;
							POT=PT;
							}
					

}



#ifdef Trigger_debug			
Printf("\r\nTriggerFlag=%02x  TriggerFlagtemp=%02x  PT=%02x Priority=%02x",(WORD)TriggerFlag,(WORD)TriggerFlagtemp,(WORD)PT,(WORD)Priority);
#else
//Printf("\r\nTriggerFlag=%02x",(WORD)TriggerFlag);
#endif



}


			
		}


if(Priority==ON)
		{///priority on
		return TriggerFlag;
		}
		else	if((PowerFlag==OFF)&&(POTF==0)) 
			return POT;
		else
			{
			//if(POT==0x20)
			//	{
			//	POT=0;
			//	return 0x20;
			//	}
			//else
			return PT;
			}



		
#endif

 

		
			
}


void TriggerHandler(BYTE T_Flag)
{

BYTE ReverseMask,LeftMask,RightMask,CamCMask;
	BYTE delay;



	ReverseMask=0x00;
	LeftMask=0x00;
	RightMask=0x00;
	CamCMask=0x00;//0xEA;
	//CamDMask=0xE8;//0xEC;
//	VGAMask=0x20;//0xE6;

/*
#if (defined AWT_ML072Q) || (defined AWT_ML072S) || (defined SUPPORT_ML84TQ_TRIGGER_DELAY)  //Pinchi 20160126 add
	BYTE i;
#endif
	BYTE ReverseMask,LeftMask,RightMask,CamCMask,CamDMask,VGAMask;
//#ifdef HTRV  //Pinchi 20150908 change condition for HTRV  //Pinchi 20140918 add for AWT ML072Q
#ifdef  SUPPORT_ML84TQ_TRIGGER_DELAY  //Pinchi 20150825 add for function define
	BYTE delaytrigger,j;
#endif

#ifdef  AWT_ADD_TRIGGER_DELAY_FUN  //ryan@20170907
	BYTE delaytrigger,j;
#endif
*/


/*
#ifdef TW8816

	if(Priority==0)
	{///priority on
		ReverseMask=0x00;
		LeftMask=0x00;
		RightMask=0x00;
		CamCMask=0x00;//0xEA;
		CamDMask=0xE8;//0xEC;
		VGAMask=0x20;//0xE6;
	}
	else
	{
		///priority off
		ReverseMask=0x00;
		LeftMask=0x00;
		RightMask=0x00;
		CamCMask=0x00;//0xCA;
		
		CamDMask=0xC4;//0xCC;
		VGAMask=0xC6;
	}
#else
	if(Priority==0)
	{
		ReverseMask=0x00;
		LeftMask=0x66;
		RightMask=0xA6;
		CamCMask=0xE4;//0xEA;
		CamDMask=0xE8;//0xEC;
		VGAMask=0x20;//0xE6;
	}
	else
	{
		ReverseMask=0xC0;
		LeftMask=0x40;
		RightMask=0x80;
		CamCMask=0xC2;//0xCA;
		CamDMask=0xC4;//0xCC;
		VGAMask=0xC6;
	}
#endif
*/

     if((T_Flag&REVERSE_T)!= 0 && (T_Flag&ReverseMask)==0) //RearMode
  	 {
         if(TriggerMode!=REVERSE_TRIG)
         {
         #if 0//def SUPPORT_TRIGGER_EXIT_MENU  //Pinchi 20150827 add
           #if 0  //Pinchi 20151209 disable //def HTRV  //Pinchi 20150831 add
            if (GaugeSetup || (GetDisplayedOSD() & MENU))
           #else
            if (GetDisplayedOSD() & MENU)
           #endif
            {
                ClearOSDInfo();
                InitOSDMenu();
                Change_OSDColorLookup();
            #if 0  //Pinchi 20151209 disable //def HTRV  //Pinchi 20150831 add
                GaugeSetup = 0;
            #endif
            }
         #endif
		 #if 0///def SUPPORT_AC_ON_REDUCE_TIME  //Pinchi 20150903 add
			if (IsFirstRunfromAC == 1)
			{
				IsFirstRunfromAC = 0;
			//	Printf("\r\n[MSG]TriggerHandler() => AC On 1st time do CAM Trigger"); //pinchi test
			}
		 #endif
		 TriggerStatus=4;
          	Printf("\r\nREVERSE_TRIG",0);
  		    TriggerMode=REVERSE_TRIG;
   if(  GetDisplayedOSD() & (MENU|TVVOL)  ) CloseOSDMenu();			
  	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);	  
	
  		    SetRearSource();
			
 
	MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE);

         }
		
		 return;
     }
      
	 if((T_Flag&LEFT_T)!=0 && (T_Flag&LeftMask)==0)
	 {
         if(TriggerMode!=LEFT_TRIG)
         {
         #if 0///def SUPPORT_TRIGGER_EXIT_MENU  //Pinchi 20150827 add
           #if 0  //Pinchi 20151209 disable //def HTRV  //Pinchi 20150831 add
            if (GaugeSetup || (GetDisplayedOSD() & MENU))
           #else
            if (GetDisplayedOSD() & MENU)
           #endif
            {
                ClearOSDInfo();
                InitOSDMenu();
                Change_OSDColorLookup();
            #if 0  //Pinchi 20151209 disable //def HTRV  //Pinchi 20150831 add
                GaugeSetup = 0;
            #endif
            }
         #endif
		 
		 #if 0///def SUPPORT_AC_ON_REDUCE_TIME  //Pinchi 20150903 add
			if (IsFirstRunfromAC == 1)
			{
				IsFirstRunfromAC = 0;
			//	Printf("\r\n[MSG]TriggerHandler() => AC On 1st time do CAM Trigger"); //pinchi test
			}
		 #endif

            Printf("\r\nLEFT_TRIG",0);
         //   ClearGAUGEINFO();
	        TriggerMode=LEFT_TRIG;   
		 TriggerStatus=1;
		 if(  GetDisplayedOSD() & (MENU|TVVOL)	) CloseOSDMenu();		  
		  MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);		
		 
            SetLeftSource();
			
			MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE);
		 }
		 return;
	 }

	 if((T_Flag&Right_T)!=0 &&(T_Flag&RightMask)==0)
	 {
         if(TriggerMode!=RIGHT_TRIG)
         {
         #if 0///def SUPPORT_TRIGGER_EXIT_MENU  //Pinchi 20150827 add
           #if 0  //Pinchi 20151209 disable //def HTRV  //Pinchi 20150831 add
            if (GaugeSetup || (GetDisplayedOSD() & MENU))
           #else
            if (GetDisplayedOSD() & MENU)
           #endif
            {
                ClearOSDInfo();
                InitOSDMenu();
                Change_OSDColorLookup();
            #if 0  //Pinchi 20151209 disable //def HTRV  //Pinchi 20150831 add
                GaugeSetup = 0;
            #endif
            }
         #endif
		 #if 0///def SUPPORT_AC_ON_REDUCE_TIME  //Pinchi 20150903 add
			if (IsFirstRunfromAC == 1)
			{
				IsFirstRunfromAC = 0;
			//	Printf("\r\n[MSG]TriggerHandler() => AC On 1st time do CAM Trigger"); //pinchi test
			}
		 #endif

            Printf("\r\nRIGHT_TRIG",0);
          //  ClearGAUGEINFO();
          TriggerStatus=2;
            TriggerMode=RIGHT_TRIG;
			if(  GetDisplayedOSD() & (MENU|TVVOL)  ) CloseOSDMenu();		 
			 MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);	   
			
			SetRightSource();
			
			MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE);

		 }
         return;
	 }     

	//#ifdef AWT_ML072Q  //Pinchi 20150826 add
	 if((T_Flag&PARK_T)!=0&& (T_Flag&CamCMask)==0)
	 {
         if(TriggerMode!=CAMC_TRIG)
         {
         #if 0///def SUPPORT_TRIGGER_EXIT_MENU  //Pinchi 20150827 add
            if (GetDisplayedOSD() & MENU)
            {
                ClearOSDInfo();
                InitOSDMenu();
                Change_OSDColorLookup();
            }
         #endif
		 #if 0///def SUPPORT_AC_ON_REDUCE_TIME  //Pinchi 20150903 add
			if (IsFirstRunfromAC == 1)
			{
				IsFirstRunfromAC = 0;
			//	Printf("\r\n[MSG]TriggerHandler() => AC On 1st time do CAM Trigger"); //pinchi test
			}
		 #endif

            Printf("\r\nCAMC_TRIG",0);
           // ClearGAUGEINFO();
           TriggerStatus=3;
            TriggerMode=CAMC_TRIG;
          //  TW2835Cmd("\n\rmode 0\n\r");
		  if(  GetDisplayedOSD() & (MENU|TVVOL)  ) CloseOSDMenu();		   
		   MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE); 	 
          
			SetTriggerCSource();
			//SetSourceMode(CAM_C,Attr_A,Attr_B,Attr_C,Attr_R);
			
			MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE);

		 }
         return;
	 }
	//#endif
	   
	#if 0//def USE_CAMD	  
	   if((T_Flag&0x04)!=0 && (T_Flag&CamDMask)==0)
	   {
           if(TriggerMode!=CAMD_TRIG)
           {
               Printf("\r\nCAMD_TRIG\n",0);
               ClearGAUGEINFO();
               TriggerMode=CAMD_TRIG;
			   SetSourceMode(CAM_D,Attr_A,Attr_B,Attr_C,Attr_R);
		   }
            return;
	   }
	#endif

	#if 0
	if(ReadEEP(EEP_TriggerVGA)==0)
		{
		     if((T_Flag&0x08)!=0 && (T_Flag&VGAMask)==0)
		   	   {
	            if(TriggerMode!=VGA_TRIG)
	              {
	               Printf("\r\nTriggerVGA");
	               ClearGAUGEINFO();
	               TriggerMode=VGA_TRIG;
			 SelectModeType=VGAMode;	   			//WILLIAM-AWT1.2-20110107
				   SetSourceMode(SelectModeType,Attr_A,Attr_B,Attr_C,Attr_R);
			      }
	             return;   
		       }
		}
	#endif

#if 1 //Pinchi 20140904 move from above by compare with ML074Q
	if (TriggerMode != No_TRIG)
	{
		
		Printf("\r\nNo_TRIG",0);
	switch(TriggerStatus)
		{
			case 1:
				delay=ReadEEP(EEP_DELAYCAMA);
				break;
			case 2:
					delay=ReadEEP(EEP_DELAYCAMB);
					break;
			case 3:
					delay=ReadEEP(EEP_DELAYCAMC);
					break;
			case 4:
					delay=ReadEEP(EEP_DELAYCAMR);
					break;

		default:
			delay=0;
			break;
		}

	
	MCUTimerActiveTimerEvent(SEC(delay), _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE);	
		//setloopoutmode(SelectModeType) ;
		//OSDStateFlg =1 ;

		//CloseOSDMenu();

	//	if (PowerFlag)  //Pinchi 20140918 disable for AWT ML072Q
	#if 0
		{
		#if (defined AWT_ML072Q) || (defined AWT_ML072S)  //Pinchi 20150908 add for AWT ML072Q/ML072S  //Pinchi 20140918 add for AWT ML072Q
		//#ifndef  SUPPORT_ML84TQ_TRIGGER_DELAY  //Pinchi 20150825 add for function define	
		  #ifdef SUPPORT_AC_ON_REDUCE_TIME  //Pinchi 20150903 add
			if (IsFirstRunfromAC == 1)
			{
				IsFirstRunfromAC = 0;
			//	Printf("\r\n[MSG]TriggerHandler() => AC On 1st No Trigger & skip delay time"); //pinchi test
			}
			else
		  #endif
		  	{
		  		#ifdef AWT_ADD_TRIGGER_DELAY_FUN //ryan@20170907

						  			switch(TriggerMode)
									{
										case REVERSE_TRIG:	
											delaytrigger=ReadEEP(EEP_CamRDelay);
										break;

										case LEFT_TRIG:	
											delaytrigger=ReadEEP(EEP_CamADelay);
										break;

										case RIGHT_TRIG:	
											delaytrigger=ReadEEP(EEP_CamBDelay);
										break;

									#ifdef AWT_ML072Q  //Pinchi 20150908 add
										case CAMC_TRIG:	
											delaytrigger=ReadEEP(EEP_CamCDelay);
										break;
									#endif
									#ifdef USE_CAMD
										case CAMD_TRIG:	
											delaytrigger=ReadEEP(EEP_DELAYCAMD);
										break;
									#endif	
										
										case No_TRIG:
										case PARK_TRIG:	
										case VGA_TRIG:	
										default:	
											delaytrigger=0;
										break;	
									}

									for(j=0;j<delaytrigger;j++) 
									{
										for(i=0;i<100;i++) 
										{
											CheckTrigger();
											if(TriggerFlag!=0)//andy HTRV 980625
											   return;
											Wait_ms(7);	//william-v1.44a-970923
										}
									}
				#else
				for (i=0 ; i<100 ; i++) 
				{
					CheckTrigger();
					if (TriggerFlag != 0)
						return;

					Wait_ms(14);
				}
				#endif
		  	}
		#else
		  #ifdef  SUPPORT_ML84TQ_TRIGGER_DELAY  //Pinchi 20160126 add
			switch(TriggerMode)
			{
				case REVERSE_TRIG:	
					delaytrigger=ReadEEP(EEP_DELAYCAMR);
				break;

				case LEFT_TRIG:	
					delaytrigger=ReadEEP(EEP_DELAYCAMA);
				break;

				case RIGHT_TRIG:	
					delaytrigger=ReadEEP(EEP_DELAYCAMB);
				break;

			#ifdef AWT_ML072Q  //Pinchi 20150908 add
				case CAMC_TRIG:	
					delaytrigger=ReadEEP(EEP_DELAYCAMC);
				break;
			#endif
			#ifdef USE_CAMD
				case CAMD_TRIG:	
					delaytrigger=ReadEEP(EEP_DELAYCAMD);
				break;
			#endif	
				
				case No_TRIG:
				case PARK_TRIG:	
				case VGA_TRIG:	
				default:	
					delaytrigger=0;
				break;	
			}

			for(j=0;j<delaytrigger;j++) 
			{
				for(i=0;i<100;i++) 
				{
					CheckTrigger();
					if(TriggerFlag!=0)//andy HTRV 980625
					   return;
					Wait_ms(4);	//william-v1.44a-970923
				}
			}
		  #endif
		#endif
     	} 
	#endif
    	TriggerMode=No_TRIG;		 
		/*
    	ClearGAUGEINFO();
		
	#ifdef QUAD  //Pinchi 20150903 change condition for common use when enter Standby mode no action  //AWT_ML072Q  //Pinchi 20140922 add for AWT ML072Q
		if (PowerFlag)
	#endif
		{
        #ifdef HTRV  //Pinchi 20151209 add
            if (GaugeSetup == 1)
            {
                GaugeSetup = 0;
                GaugeInfoOSD = 0;  //Pinchi 20151209 add to clear GaugeSetup
                SelectModeType = ReadEEP(EEP_SelectMode);  //Pinchi 20151209 add to restore original cam
            }
        #endif

			DisplayInput();		//william-20100420
			*/
		#if 0
		  if(SelectModeType==VGAMode)				//william-awt1.2 20110107
		  	SelectModeType=ReadEEP(EEP_SelectMode);
		#endif

	/*
if(RELEASE_TR_JUMP_Flag==1) 
{
	TR_JUMP_Flag=0;
	NowModeState=TR_JUMP_HOLD;
	RELEASE_TR_JUMP_Flag=0;
	Printf("\r\n(SelectModeType=%02x    NowModeState=%02x)",(WORD)SelectModeType,(WORD)NowModeState);
}

			IsDrawNoVideoSignal=0;//ryan@20170921
			SetNowMode();

		#if 1  //Pinchi 20150901 add to setup 2s for Display OFF
			if (ReadEEP(EEP_COSDMode) == 1)
				displayhold_flag = 3;
		#endif
		}
*/

#if 0  //Pinchi 20150827 disable for wrong action
	#ifdef AWT_ML072Q
		if(ReadDecoder(0x0a)==0x13)
		{
			WriteDecoder(0x0a,0x1b);
			WriteDecoder(0x61,0x80);
			WriteDecoder(0x66,0x00);
			Printf("\r\n[MSG]TriggerHandler() => TriggerMode=No_TRIG 4:3 to 16:9 !!");  //pinchi test
		}
	#else
		if(ReadDecoder(0x0a)==0x13)
	    {
			WriteDecoder(0x0a,0x0f); 
			WriteDecoder(0x61,0x88);
			WriteDecoder(0x66,0x00);//<William-960905>  
	    }
	#endif
#endif
	}
#endif




#if 0//ndef AWT_ML072Q			//william @20130813
	if(ParkFlag==0)
	{
		AVFlag=0;
	#ifdef QUAD	
		if(SelectModeType==AVMode)
	#else
		if(SelectModeType==SOURCE_AV)
	#endif
		{
			SelectModeType=ReadEEP(EEP_SelectMode);
			SetNowMode();
		} 
	}
#endif



}



BYTE CheckTouchP(void)
{
BYTE ret=0;
			if(TouchP ) 
			{
		
#if (_DEBUG_MESSAGE_GT911==ON)	
				switch (TouchP) 
				{
					case _TOUCH_COUNT_1: Printf("\r\nTOUCH_COUNT_1"); 
					if(  GetDisplayedOSD() & (MENU |TVVOL) ) 
										 CloseOSDMenu();
									 SetSelectMode();
											break;
					case _TOUCH_COUNT_2: 
											Printf("\r\nTOUCH_COUNT_2"); 
											break;
					case _TOUCH_COUNT_3: Printf("\r\nTOUCH_COUNT_3"); break;
					case _TOUCH_COUNT_4: Printf("\r\nTOUCH_COUNT_4"); break;
					case _TOUCH_COUNT_5: Printf("\r\nTOUCH_COUNT_5"); break;
					default: Printf("\r\n_TOUCH_COUNT_OVER 5"); break;
				}	
				
#endif		
			TouchP=0;	
			}
return ret;
}






