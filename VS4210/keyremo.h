//
// KeyRemo.h
// Default function for Key Input and Remocon Input
//
//=============================================================================
//		                    Operation Key Mapping
//=============================================================================



#define Wait_ms(a)   msleep(a) //MCUTimerDelayXms(a)

#define ResetKey()		{ P2 = 0xff;}
//#define ReadKey()		(~P1 & 0xfc);

#if 1//def QUAD
#define EVENTKEY		0x02
#endif
//#define POWERKEY		0x04 //andy AWT 980907
//#define MENUKEY			0x08
#define POWERKEY		0x10 //andy AWT 980907
#define MENUKEY			0x20
#define UNLOCKKEY (INPUTSOURCEKEY|SELECTKEY)
#if 0//def AWT_NEW_SLIM_MODEL  //Pinchi 20150203 add for AWT new slim model
#define UPKEY 			0x10
#define DOWNKEY 		0x20
#define SELECTKEY 		0x40
#define INPUTSOURCEKEY	0x80
#define VersionKEY 		0xC0
#define UnLockKEY 		0xC0
#else
//#define INPUTSOURCEKEY	0x10
#define INPUTSOURCEKEY	0x80
//#define UPKEY		    0x20
//#define DOWNKEY		    0x40
#define UPKEY		    0x01
#define DOWNKEY	0x02	   

//#define SELECTKEY	    0x80
#define SELECTKEY	    0x40


  #if 1  //Pinchi 20160929 add
  #define Tune2835ClkKEY 0x90
  #define Enter2835TuneKEY 0x0C
  #else
  #define VersionKEY	    0x90
  #endif
#endif

#define LEFT_T 		0x01
#define Right_T 		0x02
#define PARK_T 		0x04
#define REVERSE_T 	0x08

#define  SelectMode 0
#define  JumpMode	1


#define REMO_CUSTOM		0

#define REMO_NUM0		0
#define REMO_NUM1		1
#define REMO_NUM2		2
#define REMO_NUM3		3
#define REMO_NUM4		4
#define REMO_NUM5		5
#define REMO_NUM6		6
#define REMO_NUM7		7
#define REMO_NUM8		8
#define REMO_NUM9		9

#define REMO_STANDBY	12
#define REMO_CHNUP		32
#define REMO_CHNDN		33
#define REMO_VOLUP		16
#define REMO_VOLDN		17
#define REMO_SELECT		21
#define REMO_MENU		63
#define REMO_MUTE		13
#define REMO_INPUT		56

#define REMO_AUTO		10
#define REMO_INFO		58
#define REMO_AUDIO		38
#define REMO_ASPECT		62
#define REMO_CC			15
#define REMO_EXIT		0x16

#define REMO_TTXRED			0x37
#define REMO_TTXGREEN		0x36
#define REMO_TTXYELLOW		0x32
#define REMO_TTXCYAN		0x34



//=============================================================================
//                                 RC5 type
//=============================================================================
#ifdef REMO_RC5

#ifdef TECHWELL_REMOCON
#define REMO_CUSTOM		0

#define REMO_NUM0		0
#define REMO_NUM1		1
#define REMO_NUM2		2
#define REMO_NUM3		3
#define REMO_NUM4		4
#define REMO_NUM5		5
#define REMO_NUM6		6
#define REMO_NUM7		7
#define REMO_NUM8		8
#define REMO_NUM9		9

#define REMO_STANDBY	12
#define REMO_CHNUP		32
#define REMO_CHNDN		33
#define REMO_VOLUP		16
#define REMO_VOLDN		17
#define REMO_SELECT		21
#define REMO_MENU		63
//#define REMO_TV			63
//#define REMO_VCR		60
//#define REMO_SVIDEO		56
#define REMO_MUTE		13
#define REMO_INPUT		56

#define REMO_AUTO		10
#define REMO_INFO		58
#define REMO_AUDIO		38
#define REMO_ASPECT		62
#define REMO_CC			15
#define REMO_EXIT		0x16

#define REMO_TTXRED			0x37
#define REMO_TTXGREEN		0x36
#define REMO_TTXYELLOW		0x32
#define REMO_TTXCYAN		0x34

#ifdef SUPPORT_DEBUG
#define REMO_DEBUG		0x2b
#define REMO_READ		0x29
#define REMO_WRITE		0x2a
#define REMO_RESERVED	0x30
#endif

#else 
#define REMO_CUSTOM		0

#define REMO_NUM0		0
#define REMO_NUM1		1
#define REMO_NUM2		2
#define REMO_NUM3		3
#define REMO_NUM4		4
#define REMO_NUM5		5
#define REMO_NUM6		6
#define REMO_NUM7		7
#define REMO_NUM8		8
#define REMO_NUM9		9

#define REMO_STANDBY	12
#define REMO_CHNUP		32
#define REMO_CHNDN		33
#define REMO_VOLUP		16
#define REMO_VOLDN		17
#define REMO_SELECT		18
#define REMO_MENU		48
#define REMO_TV			63
#define REMO_VCR		60
#define REMO_SVIDEO		56
#define REMO_MUTE		13
#define REMO_INPUT		0xff

#define REMO_INFO		0x0a //??
#endif

//=============================================================================
//                                 NEC type
//=============================================================================
#elif defined REMO_NEC
/*
// Techwell Origin
#define REMO_CUSTOM1	0x20
#define REMO_CUSTOM2	0xdf


#define REMO_NUM0		0x08
#define REMO_NUM1		0x88
#define REMO_NUM2		0x48
#define REMO_NUM3		0xc8
#define REMO_NUM4		0x28
#define REMO_NUM5		0xa8
#define REMO_NUM6		0x68
#define REMO_NUM7		0xe8
#define REMO_NUM8		0x18
#define REMO_NUM9		0x98

#define REMO_STANDBY	0x10
#define REMO_CHNUP		0x00
#define REMO_CHNDN		0x80
#define REMO_VOLUP		0x40
#define REMO_VOLDN		0xc0
#define REMO_SELECT		0x70
#define REMO_MENU		0x9c
#define REMO_MUTE		0x90
#define REMO_INPUT		0xd0
*/

// Techwell New Remocon after 4/7/2005
#ifdef PHILIPS_REMOCON
#define REMO_CUSTOM1	0x02
#define REMO_CUSTOM2	0xfd

#define REMO_NUM0		0x00
#define REMO_NUM1		0x80
#define REMO_NUM2		0x40
#define REMO_NUM3		0xc0
#define REMO_NUM4		0x20
#define REMO_NUM5		0xa0
#define REMO_NUM6		0x60
#define REMO_NUM7		0xe0
#define REMO_NUM8		0x10
#define REMO_NUM9		0x90

#define REMO_STANDBY	0x48
#define REMO_CHNUP		0xd8
#define REMO_CHNDN		0xf8
#define REMO_VOLUP		0x58
#define REMO_VOLDN		0x78
#define REMO_SELECT		0xe8
#define REMO_MENU		0x01
#define REMO_MUTE		0x08
#define REMO_INPUT		0xf0

#define REMO_INFO		0x38



#define REMO_TTXRED			0x4E
#define REMO_TTXGREEN		0x8E
#define REMO_TTXYELLOW		0xC6
#define REMO_TTXCYAN		0x86

#else
// RYU For Test
#define REMO_CUSTOM1		0x04
#define REMO_CUSTOM2		0xfb

#define REMO_NUM0		0xf8
#define REMO_NUM1		0x48
#define REMO_NUM2		0xc8
#define REMO_NUM3		0x28
#define REMO_NUM4		0xa8
#define REMO_NUM5		0x18
#define REMO_NUM6		0x98
#define REMO_NUM7		0x58
#define REMO_NUM8		0xd8
#define REMO_NUM9		0x78

#define REMO_STANDBY	0x00
#define REMO_CHNUP		0x40	//
#define REMO_CHNDN		0x70	//
#define REMO_VOLUP		0x90	//
#define REMO_VOLDN		0xe0	//
#define REMO_SELECT		0x11//??
#define REMO_MENU		0x10

#define REMO_MUTE		0x60
#define REMO_INPUT		0xc0
#define REMO_AUTO		0x30
#define REMO_AUTOSCAN	0x80
#define REMO_TV			0x20
#define REMO_EXIT		0xa0
#define REMO_INFO		0x08
#define REMO_ASPECT		0x62

#define REMO_TTXRED		0xb8
#define REMO_TTXGREEN	0x02
#define REMO_TTXYELLOW	0x82
#define REMO_TTXCYAN	0x42

#ifdef SUPPORT_DEBUG
#define REMO_DEBUG		0xb8
#define REMO_READ		0x02
#define REMO_WRITE		0x82
#endif

#endif //PHILIPS_REMOCON

#endif // REMO_NEC



BYTE ActionRemo(BYTE, BYTE);
BYTE CheckKeyIn(void);
BYTE GetKey(BYTE repeat);
BYTE IsRemoDataReady(BYTE *, BYTE *);
BYTE CheckTrigger(void);
void TriggerHandler(BYTE T_Flag);
void SetJumpMode(void);
void SetSelectMode(void);
void SetNowMode(void);
void SEQMode(void);
BYTE CheckTouchP(void);

extern void setloopoutmode(int index);
extern void OSDCursorMove(BYTE flag );
extern void ShowGaugeOSD(void);
extern void DisplayVol(void);
extern void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID);
extern void MCUTimerCancelTimerEvent(BYTE ucEventID);
extern void MCUTimerDelayXms(WORD usNum);
extern void ShowOSDLOCK(void);

//extern OSDValueUpDn(BYTE flag);
//extern OpenOSDMenu(BYTE OSD_ID);
//extern OSDSelect(void);
//extern DisplayCursor(void);
//extern BYTE  GetDisplayedOSD(void);

typedef enum   ///add enum  by ryan@20170907
{
	NORMALL_MODE,
	REVERSE_TRIG,
	LEFT_TRIG,
	RIGHT_TRIG,
#ifdef USE_CAMD	
	CAMD_TRIG,
#endif	
	CAMC_TRIG,
	VGA_TRIG,
	PARK_TRIG,
	No_TRIG,
	TRIGGER_NUM
}TriggerModeType;

enum SourceModeTypeEnum
{	
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
	
	CAM_ABR_T3,    // 16		0x0e
	CAM_ABR_T4,    // 17		0x0f
	CAM_RAC_T,			//  0x10
	CAM_RBC_T,			//  0x11
	CAM_RAC_T2,			//  0x12
	CAM_RBC_T2,			//  0x13
	CAM_BA,				//  0x14
	CAM_BC,				//  0x15
	CAM_BR,				//  0x16
	CAM_CA,				//  0x17
	CAM_ABC,			//  0x18
	CAM_ABR,			//  0x19
	CAM_ACR,			//  0x1a
	CAM_BAC,			//  0x1b
	CAM_BAR,			//  0x1c
	CAM_BCR,			//  0x1d
	CAM_CAB,			//  0x1e
	CAM_CAR,			//  0x1f
	CAM_CBR,			//  0x20
	SEQ,           // 18			0x10
	CAM_NUM
};

typedef unsigned char SourceModeType;




