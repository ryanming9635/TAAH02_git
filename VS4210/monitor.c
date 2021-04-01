/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Monitor.c		                                            										*/
/*                                                                           										*/
/*****************************************************************************/
#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"

//#include "config.h"

//#ifdef SERIAL

//#include "typedefs.h"
#include "main.h"
//#include "i2c.h"
//#include "reg.h"
#include "printf.h"
//#include "etc_eep.h"
#include "monitor.h"
//#include "cpu.h"
//#include "Adc.h"

IDATA BYTE MonAddress = 0x8a;		// initialize value should be placed to IDATA area
BYTE MonIndex;
BYTE MonRdata, MonWdata;
BYTE monstr[30];		// buffer for input string
#if 1
BYTE *argv[7];
//char *argv[7];
#else
volatile  BYTE *argv[7];
//char *argv[7];
#endif
BYTE argc=0;
bit	echo=1;

BYTE Monitor_flag=OFF;
BYTE Monitor_I2C=0x40;//0x88;//0x40;///0x90;//0x40;//0x28;//0x12;//EEPROM_24C02_addr;

		DATA BYTE	RS_buf[BUF_MAX];
        	DATA BYTE   	RS2_buf[BUF_MAX];
		DATA BYTE	DVR_buf[DVR_BUF_MAX];

		
		DATA BYTE   RS_in=0, RS_out=0;
		DATA BYTE   RS2_in=0, RS2_out=0;
		bit	        RS_Xbusy=0;			// bit RS_Xbusy=0;
		bit 		RS2_Xbusy=0; 		// bit RS_Xbusy=0;

static bit indirect=0;
extern BYTE TriggerFlag;

//extern		  bit	AutoDetect;
extern   BYTE InputSelection;
extern	      BYTE	DebugLevel;
//extern  CODE  struct struct_IdName struct_InputSelection[];
extern 		  bit	NoInitAccess;
 		  bit	AccessEnable=1;
extern  BYTE PSW_Flag;
extern BYTE year1,year2,month,day,hour,minute,second;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern tByte	count[4];
extern tByte	state[4];
extern tByte	count[4];
extern tByte	mode[4];
extern tByte	std[4];
extern tByte	egain[4][4];
extern  tByte TP_InputMode;
extern BYTE SelectInputMode;///0=HDMI , 1=AHD
extern tByte bSignal[4] ;
extern BYTE TVI_Command;
extern BYTE presetNum;
extern BYTE TVI_Chksum;
extern BYTE TVI_speed;
extern bit ptztxflag;

//char * asctime(const struct tm * timeptr);
//char *ctime(const time_t *timep);

//=============================================================================
//		Serial RX Check 												   
//=============================================================================
BYTE RS_ready(void)
{
	if( RS_in == RS_out ) return 0;
	else return 1;
}

BYTE RS2_ready(void)
{
	if( RS2_in == RS2_out ) return 0;
	else return 1;
}

//#ifdef Hs_debug
//=============================================================================
//		Serial RX														   
//=============================================================================
BYTE RS_rx(void)
{
	BYTE ret;
		
//	ES = 0;
	ret = RS_buf[RS_out];
//	ret = DVR_buf[RS_out];
	RS_out++;
	if(RS_out >= BUF_MAX) 
		RS_out = 0;
//	ES = 1;

	return ret;
}

BYTE RS2_rx(void)
{
	BYTE ret;
		
//	ES = 0;
	ret = RS2_buf[RS2_out];
//	ret = DVR_buf[RS_out];
	RS2_out++;
	if(RS2_out >= BUF_MAX) 
		RS2_out = 0;
//	ES = 1;

	return ret;
}


//=============================================================================
//		Serial TX														   
//=============================================================================
void RS_tx(BYTE tx_buf)
{


HAL_UART_Transmit(&huart1,&tx_buf,1,0xffff);
//USART1->DR = tx_buf;
//while(0 == (USART1->SR&(1<<6)));

}

void RS2_tx(BYTE tx_buf)
{


HAL_UART_Transmit(&huart2,&tx_buf,1,0xffff);
//USART2->DR = tx_buf;
//while(0 == (USART2->SR&(1<<6)));

}

//void Loader(BYTE);

//=============================================================================
//
//=============================================================================
void Prompt(void)
{
	if(day)
	{
	Printf("\r\nMCU_I2C[%02x][%d:%d:%d:%d]>",(WORD)Monitor_I2C,(WORD)day,(WORD)hour,(WORD)minute,(WORD)second);
	}
	else if(hour)
	{
	Printf("\r\nMCU_I2C[%02x][%d:%d:%d]>",(WORD)Monitor_I2C,(WORD)hour,(WORD)minute,(WORD)second);
	}
	else if(minute)
		{
		Printf("\r\nMCU_I2C[%02x][%d:%d]>",(WORD)Monitor_I2C,(WORD)minute,(WORD)second);
		}
	else
		{
		Printf("\r\nMCU_I2C[%02x][%d]>",(WORD)Monitor_I2C,(WORD)second);
		}

}

void Mon_tx(BYTE ch)
{
	RS_tx(ch);
}


//=============================================================================
//		Convert ASCII to Binery                                             
//=============================================================================
 DWORD a2i(BYTE *str)
{
	 DWORD num=0;
	BYTE i;

	for(i=0; ; i++, str++) {
		if( *str=='\0' || *str==' ' ) break;
		num = num*10 + *str - '0';
	}
	return num;
}

BYTE Asc1Bin(BYTE asc)
{
	if(asc>='0' && asc <='9') return (asc - '0');
	if(asc>='a' && asc <='f') return (asc - 'a' + 0x0a);
	if(asc>='A' && asc <='F') return (asc - 'A' + 0x0a);
	
	return 0;
}

BYTE Asc2Bin(PDATA_P BYTE *s)
{
	WORD bin;

	bin = 0;
	while(*s != '\0' && *s !=' ') {
		bin = bin<<4;
		bin = bin + Asc1Bin(*s);
		s++;
	}
	return (bin);
}


//=============================================================================
BYTE hs_toupper(BYTE ch)
{
	if( ch>='a' && ch<='z' )
		return (ch - 'a' + 'A');
	
	return ch;
}

int stricmp(BYTE *ptr1, BYTE *ptr2)
{
	int		i;
	int		ret;
	
	for(i=0; *ptr1; i++) {
	  ret = hs_toupper(*ptr1++) - hs_toupper(*ptr2++);
		if( ret ) return ret;
	}
	return 0;
}

//=============================================================================
//
//=============================================================================
/*
void SetMonAddress(BYTE addr)
{
	MonAddress = addr;
}
*/
void MonReadI2C(void)
{
/*
	if( argc>=2 ) MonIndex = Asc2Bin( argv[1] );
	else	{
		Printf("   --> Missing parameter !!!");
		return;
	}

	if ( MonAddress == TW88I2CAddress )
		MonRdata = ReadTW88(MonIndex);
	else
		MonRdata = ReadI2C(MonAddress, MonIndex);

	if( echo )
		Printf("\r\nRead %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);	
	
	MonWdata = MonRdata;
	*/
}


void MonWriteI2C(void)
{
/*
	if( argc<3 ) {
		Printf("   --> Missing parameter !!!");
		return;
	}
	
	MonIndex = Asc2Bin( argv[1] );
	MonWdata = Asc2Bin( argv[2] );
	
	if( echo ) {
		Printf("\r\nWrite %2xh:%2xh ", (tWord)MonIndex, (tWord)MonWdata);
		if ( MonAddress == TW88I2CAddress ) {
			WriteTW88(MonIndex, MonWdata);
			MonRdata = ReadTW88(MonIndex);
		}
		else {
			WriteI2C(MonAddress, MonIndex, MonWdata);
			MonRdata = ReadI2C(MonAddress, MonIndex);
		}
   		Printf("==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
	}
	else {
		if ( MonAddress == TW88I2CAddress ) {
			WriteTW88(MonIndex, MonWdata);
		}
		else {
			WriteI2C(MonAddress, MonIndex, MonWdata);
		}
	}
	*/
}


void MonIncDecI2C(BYTE inc)
{

/*
	switch(inc){
	case 0:  MonWdata--;	break;
	case 1:  MonWdata++;	break;
	case 10: MonWdata-=0x10;	break;
	case 11: MonWdata+=0x10;	break;
	}


	if ( MonAddress == TW88I2CAddress ) {
		WriteTW88(MonIndex, MonWdata);
		MonRdata = ReadTW88(MonIndex);
	}
	else {
		WriteI2C(MonAddress, MonIndex, MonWdata);
		MonRdata = ReadI2C(MonAddress, MonIndex);
	}

	if( echo ) {
		Printf("Write %2xh:%2xh ", (tWord)MonIndex, (tWord)MonWdata);
		Printf("==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
	}

*/
	Prompt();

}

void MonDumpI2C(void)
{
/*
	BYTE ToMonIndex;
	int  cnt=8;

	if( argc>=2 ) MonIndex = Asc2Bin(argv[1]);
	if( argc>=3 ) ToMonIndex = Asc2Bin(argv[2]);
	else ToMonIndex = MonIndex+cnt;
	if ( ToMonIndex < MonIndex ) ToMonIndex = 0xFF;
	cnt = ToMonIndex - MonIndex + 1;

	if( echo ) {
		if ( MonAddress == TW88I2CAddress ) {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadTW88(MonIndex);
				Printf("\r\n==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
				MonIndex++;
			}
		}
		else {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadI2C(MonAddress, MonIndex);
				Printf("\r\n==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
				MonIndex++;
			}
		}
	}
	else {
		if ( MonAddress == TW88I2CAddress ) {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadTW88(MonIndex);
				MonIndex++;
			}
		}
		else {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadI2C(MonAddress, MonIndex);
				MonIndex++;
			}
		}
	}
	*/
}

//-----------------------------------------------------------------------------

void MonNewReadI2C(void)
{
/*
	BYTE Slave;

	if( argc>=3 ) MonIndex = Asc2Bin( argv[2] );
	else	{
		Printf("   --> Missing parameter !!!");
		return;
	}
	Slave = Asc2Bin(argv[1]);

	if ( Slave == TW88I2CAddress )
		MonRdata = ReadTW88(MonIndex);
	else
		MonRdata = ReadI2C(Slave, MonIndex);

	if( echo )
		Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)MonIndex, (tWord)MonRdata);
	
	MonWdata = MonRdata;
	*/
}

void MonNewWriteI2C(void)
{
/*
	BYTE Slave;

	if( argc<4 ) {
		Printf("   --> Missing parameter !!!");
		return;
	}
	
	MonIndex = Asc2Bin( argv[2] );
	MonWdata = Asc2Bin( argv[3] );
	Slave = Asc2Bin(argv[1]);
	
	if ( Slave == TW88I2CAddress ) {
		WriteTW88(MonIndex, MonWdata);
		MonRdata = ReadTW88(MonIndex);
	}
	else {
		WriteI2C(Slave, MonIndex, MonWdata);
		MonRdata = ReadI2C(Slave, MonIndex);
   	}

	if( echo )
		Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)MonIndex, (tWord)MonRdata);
*/
}

void MonNewDumpI2C(void)
{
/*
	BYTE 	ToMonIndex, Slave;
	tWord	i;
	
	if( argc>=2 ) MonIndex = Asc2Bin(argv[2]);
	if( argc>=3 ) ToMonIndex = Asc2Bin(argv[3]);
	Slave = Asc2Bin(argv[1]);

	if( echo ) {
		if ( Slave == TW88I2CAddress ) {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadTW88(i);
        		Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)i, (tWord)MonRdata);
			}
		}
		else {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadI2C(Slave, i);
        		Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)i, (tWord)MonRdata);
			}
		}
	}
	else {
		if ( Slave == TW88I2CAddress ) {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadTW88(i);
			}
		}
		else {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadI2C(Slave, i);
			}
		}
	}
	*/
}


void MonWriteBit(void)
{
/*
	BYTE mask, i, FromBit, ToBit,  MonMask, val;
	BYTE Slave;
	// b 88 index startbit|endbit data

	if( argc<5 ) {
		Printf("   --> Missing parameter !!!");
		return;
	}
	Slave = Asc2Bin(argv[1]);

	MonIndex = Asc2Bin( argv[2] );
	FromBit  = (Asc2Bin( argv[3] ) >> 4) & 0x0f;
	ToBit  = Asc2Bin( argv[3] )  & 0x0f;
	MonMask  = Asc2Bin( argv[4] );

	if( FromBit<ToBit || FromBit>7 || ToBit>7) {
		Printf("\r\n   --> Wrong range of bit operation !!!");
		return;
	}
	
	mask = 0xff; 
	val=0x7f;
	for(i=7; i>FromBit; i--) {
		mask &= val;
		val = val>>1;
	}

	val=0xfe;
	for(i=0; i<ToBit; i++) {
		mask &= val;
		val = val<<1;
	}

	if ( Slave == TW88I2CAddress ) {
		MonRdata = ReadTW88(MonIndex);
		MonWdata = (MonRdata & (~mask)) | (MonMask & mask);
				
		WriteTW88(MonIndex, MonWdata);
		MonRdata = ReadTW88(MonIndex);
	}
	else {
		MonRdata = ReadI2C(Slave, MonIndex);
		MonWdata = (MonRdata & (~mask)) | (MonMask & mask);
				
		WriteI2C(Slave, MonIndex, MonWdata);
		MonRdata = ReadI2C(Slave, MonIndex);
	}

	if( echo )
		Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)MonIndex, (tWord)MonRdata);
*/
}

#if  0// defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
void PclkAccess(void)
{
	DtWord pclk;

	if( argc > 2 ) {
		Printf("   --> Too many parameter !!!");
		return;
	}

	if( argc==2 ) {
		pclk = (DtWord)a2i(argv[1]) * 1000000;
		Printf("\r\n --> Set Pclk:  %ld Hz ", pclk);
//		Printf("\r\n Not Suuport ");
		ChangeInternPLL(pclk);
	}
	
	if( argc==1)   GetPPF();
	
}
#endif

void MonTxDump(void)
{
/*
	tWord	count;

	if ( argc < 2 ) {
		Puts("\r\nInsufficient parameters...");
		return;
	}
	count = (tWord)a2i(argv[1]);
	do {
		RS_tx((count&0x1F)+0x20);
		count--;
	} while ( count != 0 );
	*/
}

//=============================================================================
//			Help Message
//=============================================================================
void MonHelp(void)
{
	Printf("\r\n=======================================================");
	Printf("\r\n>>>     Welcome to ML076HQ     Rev 1.00     <<<");
	Printf("\r\n=======================================================");
	Printf("\r\n   R ii             ; Read TP2824 data");
	Printf("\r\n   W ii dd          ; Write TP2824 data");
	Printf("\r\n   VR ii             ; Read VS4210 data");
	Printf("\r\n   VW ii dd          ; Write VS4210 data");
	Printf("\r\n   D [ii] [cc]      ; Dump");
	Printf("\r\n   C aa             ; Change I2C address");
	Printf("\r\n   M ?              ; Current Input");
	Printf("\r\n   M dd             ; Change Input");
	Printf("\r\n   Access [0,1]     ; Program TW88xx Access OFF/ON");
	Printf("\r\n   AutoDetect [0,1] ; PC Automode OFF/ON");
	Printf("\r\n   PCAuto		    ; RUN PC Auto Adjust");
	Printf("\r\n   Debug [0..255]   ; Debuging display level");
	Printf("\r\n   Echo On/Off      ; Terminal Echoing On/Off");
	Printf("\r\n   Delta            ; Init DeltaRGB Panel");
	Printf("\r\n   Delta w ii dh dl; SPI Write to DeltaRGB Panel");
	Printf("\r\n");
	Printf("\r\n=======================================================");
	Printf("\r\n");
}

//=============================================================================
//
//=============================================================================
BYTE MonGetCommand(void)
{
	static BYTE comment=0;
	static BYTE incnt=0, last_argc=0;
	BYTE i, ch;
	BYTE ret=0;

	if( !RS_ready() ) return 0;
	//#ifdef HS_DEBUG
	ch = RS_rx();
	//#endif

	#if (_DEBUG_MESSAGE_Monitor==ON)
	 Monitor_flag=_TRUE;
	#endif
	  MCUTimerReactiveTimerEvent(SEC(30), _USER_TIMER_EVENT_Monitor_ON);  

	//----- if comment, echo back and ignore -----
	if( comment ) {
		if( ch=='\r' || ch==0x1b ) comment = 0;
		else { 
			Mon_tx(ch);
			return 0;
		}
	}
	else if( ch==';' ) {
		comment = 1;
		Mon_tx(ch);
		return 0;
	}

	//=====================================
	switch( ch ) {

	case 0x1b:
		argc = 0;
		incnt = 0;
		comment = 0;
		Prompt();
		return 0;

	//--- end of string
	case '\r':

		if( incnt==0 ) {
			Prompt();
			break;
		}

		monstr[incnt++] = '\0';
		argc=0;

		for(i=0; i<incnt; i++) if( monstr[i]!=' ' ) break;

		if( !monstr[i] ) {
			incnt = 0;
			comment = 0;
			Prompt();
			return 0;
		}
		argv[0] = &monstr[i];
		for(; i<incnt; i++) {
			if( monstr[i]==' ' || monstr[i]=='\0' ) {
				monstr[i]='\0';
     			 //Printf("(%s) ",  argv[argc]);
				i++;
				while( monstr[i]==' ' ) i++;
				argc++;
				if( monstr[i] ){
     			 argv[argc] = &monstr[i];
				}
			}
		}

		ret = 1;
		last_argc = argc;
		incnt = 0;
		
		break;

	//--- repeat command
	case '/':
		argc = last_argc;
		ret = 1;
		break;

	//--- back space
	case 0x08:
		if( incnt ) {
			incnt--;
			Mon_tx(ch);
			Mon_tx(' ');
			Mon_tx(ch);
		}
		break;

	//--- decreamental write
	case ',':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(0);
		break;

	case '<':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(10);
		break;
	//--- increamental write
	case '.':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(1);
		break;

	case '>':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(11);
		break;

	default:
		Mon_tx(ch);
		monstr[incnt++] = ch;
		break;
	}

	if( ret ) {
		comment = 0;
		last_argc = argc;
		return ret;
	}
	else {
		return ret;
	}
}

//*****************************************************************************
//				Monitoring Command
//*****************************************************************************
//#ifdef  HS_DEBUG
#ifdef  HS_DEBUG
void Monitor(void)
{

	 BYTE ret/*,temp*/;
	//WORD addr;		
	if( !MonGetCommand() ) return;

	//--------------------------------------------------
	// Check Indirect command
	/*
	if( !stricmp( argv[0], "I" ) ) 			indirect=1;	
	else if( !stricmp( argv[0], "O" ) ) 	indirect=1;	

	else if( !stricmp( argv[0], "," ) ) 	indirect=0;	
	else if( !stricmp( argv[0], ">" ) ) 	indirect=0;	
	else if( !stricmp( argv[0], "<" ) ) 	indirect=0;	
	else if( !stricmp( argv[0], "." ) ) 	indirect=0;	
	else  indirect=0;	
	*/
	indirect = 0;

	//---------------- Write Register -------------------
	if( !stricmp( (BYTE *)argv[0],(BYTE *)"W" ) ) {

#if 0

VXISI2CWrite( Asc2Bin(argv[1]), Asc2Bin(argv[2]) );
		 ret= VXISI2CRead(Asc2Bin(argv[1]));	
	   Printf("\r\nVS4210 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);

#else
if(Monitor_I2C==0xA0)
	{
	WriteEEP( Asc2Bin(argv[1]), Asc2Bin(argv[2]) );
	   //tp28xx_byte_write(Asc2Bin(argv[1]), Asc2Bin(argv[2])); 	
	 ret= ReadEEP(Asc2Bin(argv[1]));	
	  Printf("\r\nEEPROM Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
	}
else if(Monitor_I2C==0x40)
{
	VXISI2CWrite( Asc2Bin(argv[1]), Asc2Bin(argv[2]) );
	 ret= VXISI2CRead(Asc2Bin(argv[1]));	
	   Printf("\r\nVS4210 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
}
else if(Monitor_I2C==0x12)
{
	VS8812Write( Asc2Bin(argv[1]), Asc2Bin(argv[2]) );
	 ret= VS8812Read(Asc2Bin(argv[1]));	
	   Printf("\r\nVS8812 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
}
/*
else if(Monitor_I2C==0x28)
{
	
	addr=Asc2Bin(argv[1]);
	addr<<=8;
	addr|=Asc2Bin(argv[2]);

	temp=Asc2Bin(argv[3]);
	SET_nGT911( addr,&temp,1 );

Printf("\r\nGT911  Write addr=%02x%02x data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)Asc2Bin(argv[2]),(WORD)temp);
	
	GET_nGT911(addr,&ret,1); 
	
Printf("\r\nGT911  Read addr=%02x%02x data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)Asc2Bin(argv[2]),(WORD)ret);
  
}
*/
else if( Monitor_I2C==0x88)
	{
	tp28xx_byte_write( Asc2Bin(argv[1]), Asc2Bin(argv[2]) );	
	ret= tp28xx_byte_read(Asc2Bin(argv[1]));		  
	 Printf("\r\nTP2824  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
	}

else if( Monitor_I2C==0x90)
	{
	gHDMI_Index=0;	
	HDMIRX_WriteI2C_Byte( Asc2Bin(argv[1]), Asc2Bin(argv[2]) );	
	ret= HDMIRX_ReadI2C_Byte(Asc2Bin(argv[1]));		  
	 Printf("\r\nIT66021  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
	}
else
{
	Printf("\r\nI2C addr. error");   

}

#endif
/*
		if(Monitor_I2C==EEPROM_24C02_addr)
		{
		 SW_I2C1Write( Monitor_I2C , Asc2Bin(argv[1]), Asc2Bin(argv[2]) );
		//tp28xx_byte_write(Asc2Bin(argv[1]), Asc2Bin(argv[2]));	 
		  ret= SW_I2C1Read(Monitor_I2C,Asc2Bin(argv[1]));	 
		Printf("\r\nEEPROM Write addr=%02x	data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
		}
		else
			{
		    	tp28xx_byte_write(Asc2Bin(argv[1]), Asc2Bin(argv[2]));   
	        	//ret= tp28xx_byte_read(0x2a);   
			  ret= tp28xx_byte_read(Asc2Bin(argv[1]));   
			Printf("\r\nTP2824 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
			}
		*/
		
	}
	else if( !stricmp( (BYTE *)argv[0],(BYTE *) ")" ) ) {
		indirect=0;
	//	MonNewWriteI2C();
	}
	/*
	else if( !stricmp( argv[0], "O" ) ) {			// inderect addressing for 8806
		MonIndex = Asc2Bin( argv[1] );
		MonWdata = Asc2Bin( argv[2] );
		WriteI2C(MonAddress, 0xc5, MonIndex);
		WriteI2C(MonAddress, 0xc6, MonWdata);
		MonRdata = ReadI2C(MonAddress, 0xc6);
		Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);
	}
	*/
	//---------------- Read Register --------------------
	else if ( !stricmp((BYTE*) argv[0],(BYTE*) "R" ) ) {
		//MonReadI2C();
	
		#if 0
			   ret= VXISI2CRead(Asc2Bin(argv[1]));			 
			Printf("\r\nVS4210	Read addr=%02x	data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);

		#else
	if(Monitor_I2C==0xA0)
		{
		   ret= ReadEEP(Asc2Bin(argv[1])); 			 
		Printf("\r\nEEPROM  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
		}
	else if(Monitor_I2C==0x40)
		{
		ret= VXISI2CRead(Asc2Bin(argv[1])); 		  
		 Printf("\r\nVS4210  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
		}
	else if(Monitor_I2C==0x12)
		{
		ret= VS8812Read(Asc2Bin(argv[1])); 		  
		 Printf("\r\nVS8812  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
		}
	/*
	else if(Monitor_I2C==0x28)
		{

		addr=Asc2Bin(argv[1]);
		addr<<=8;
		addr|=Asc2Bin(argv[2]);
		
		GET_nGT911(addr,&temp,1); 		  
		 Printf("\r\nGT911  Read addr=%02x%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)Asc2Bin(argv[2]),(WORD)temp);
		}
	*/
	else if( Monitor_I2C==0x88)
		{	
		ret= tp28xx_byte_read(Asc2Bin(argv[1])); 		  
		 Printf("\r\nTP2824  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
		}
	else if( Monitor_I2C==0x90)
			{
			gHDMI_Index=0;
			ret= HDMIRX_ReadI2C_Byte(Asc2Bin(argv[1]));	  
			 Printf("\r\nIT66021  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
			}
	else
		{
		Printf("\r\nI2C addr. error");
		}
		#endif
		/*
		if(Monitor_I2C==EEPROM_24C02_addr)
				{
			   ret= SW_I2C1Read(Monitor_I2C,Asc2Bin(argv[1]));              
			Printf("\r\nEEPROM Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);	
			}
		else
			{
			   ret= tp28xx_byte_read(Asc2Bin(argv[1]));              
			Printf("\r\nTP2824 Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);		
			}
		*/
	}  
	
	else if ( !stricmp( (BYTE *)argv[0],(BYTE *) "VR" ) ) {
			//MonReadI2C();
	//	 ret=VXISI2CRead(Asc2Bin(argv[1])); 
	//	Printf("\r\nVS4210 Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);	
	
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x02));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x03));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x04));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x05));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x06));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x07));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x08));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x09));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x0A));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x0B));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x0C));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x0D));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x10));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x11));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x15));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x16));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x17));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x18));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x19));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x1A));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x1C));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x1D));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x1E));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x28));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x29));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x2A));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x2B));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x2C));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x2D));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x74));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x7E));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x7F));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x80));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x81));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x82));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x83));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x84));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x85));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x86));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x87));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x88));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x89));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x8A));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x8B));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x8C));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x8D));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x8E));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x8F));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x90));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x91));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x92));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x93));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x94));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x95));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x97));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x98));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x99));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x9B));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x9C));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x9D));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x9E));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0x9F));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA0));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA1));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA2));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA3));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA4));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA5));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA6));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA7));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA8));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xA9));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xAA));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xAB));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xAF));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xC0));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xD6));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xD7));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xD8));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xE6));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xE7));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xE8));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xE9));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xEA));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xEB));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xEC));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xED));
			Printf("\r\n40,%02x,%02x",(WORD)VXISI2CRead(0xEE));

		}
	else if ( !stricmp( (BYTE *)argv[0], (BYTE *)"VW" ) ) {
		    VXISI2CWrite(Asc2Bin(argv[1]), Asc2Bin(argv[2]));   
	  ret= VXISI2CRead(Asc2Bin(argv[1]));   
	Printf("\r\nVS4210 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);				
		}
	else if ( !stricmp( argv[0],(BYTE *)"(" ) ) {
		indirect=0;
		//MonNewReadI2C();
	}
	else if ( !stricmp( (BYTE *)argv[0],(BYTE *) "I2C" ) ) {
		 ret=Asc2Bin(argv[1]); 
		Printf("\r\nChange I2C addr. from %02x to %02x",(WORD)Monitor_I2C,(WORD)ret);		
		Monitor_I2C=ret;
		}
	else if ( !stricmp( (BYTE *)argv[0],(BYTE *) "Q" ) ) {
		Monitor_flag=_FALSE;
		}
	else if ( !stricmp( (BYTE *)argv[0],(BYTE *) "DUMP" ) ) {
		WORD s,e;	//20201211-03
		
		 s=Asc2Bin(argv[1]); 
		 e=Asc2Bin(argv[2]); 


		 if(Monitor_I2C==0xA0)
			 {
			 Printf("\r\nDUMP EEPROM Reg..");
			 }
		 else if(Monitor_I2C==0x40)
			 {
			 Printf("\r\nDUMP VS4210 Reg..");
			 }
		 else if(Monitor_I2C==0x12)
			 {
			 Printf("\r\nDUMP VS8812 Reg..");
			 }

for(;s<=e;s++)
{
		 if(Monitor_I2C==0xA0)
			 {
				ret= ReadEEP(s); 		  
			 Printf("\r\nEEPROM  Read addr=%02x  data=%02x",(WORD)s,(WORD)ret);
			 }
		 else if(Monitor_I2C==0x40)
			 {
			 ret= VXISI2CRead(s);		   
			 // Printf("\r\nVXISI2CWrite(0x%02x,0x%02x);",(WORD)s,(WORD)ret);
			  Printf("\r\n40,%02x,%02x",(WORD)s,(WORD)ret);
			 }
		 else if(Monitor_I2C==0x12)
			 {
			 ret= VS8812Read(s); 	   
//			  Printf("\r\nVS8812  Read addr=%02x  data=%02x",(WORD)s,(WORD)ret);
			  Printf("\r\nVS8812,%02x,%02x",(WORD)s,(WORD)ret);
			 }
		  else if(Monitor_I2C==0x88)
					  {
					  ret= tp28xx_byte_read(s);		
					   Printf("\r\ntp28xx,%02x,%02x",(WORD)s,(WORD)ret);
					  } 
		 /*
		 else if(Monitor_I2C==0x28)
		 {
			 addr=Asc2Bin(argv[1]);
			 addr<<=8;
			 addr|=Asc2Bin(argv[2]);
			 
			 GET_nGT911(addr,&temp,1);		   
			  Printf("\r\nGT911  Read addr=%02x%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)Asc2Bin(argv[2]),(WORD)temp);
		 
		 }	
		 */
		 else
			 {
			 Printf("\r\nI2C addr. error");
			 }
}
		}
	#if 0
		else if ( !stricmp( (BYTE *)argv[0],(BYTE *) "DTP" ) ) {
			WORD /*s,e,*/n,i;
			
				 addr=Asc2Bin(argv[1]);
				 addr<<=8;
				 addr|=Asc2Bin(argv[2]);
 					
				 n=Asc2Bin(argv[3]);

			 Printf("\r\nDUMP GT911 Reg..");

	
		for(i=0;i<n;i++)
		{	
   		GET_nGT911(addr+i,&temp,1);		  	
		  Printf("\r\nAddr=%04x Data=%02x",(WORD)addr+i,(WORD)temp);
		}
	}
		#endif
	/*
	else if( !stricmp( argv[0], "i" ) ) {
		MonIndex = Asc2Bin( argv[1] );
		WriteI2C(MonAddress, 0xc5, MonIndex);
		MonRdata = ReadI2C(MonAddress, 0xc6);
		Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);

	}
	*/
	/*
	//---------------- Dump Register --------------------
	else if( !stricmp( argv[0], "D" ) ) {
		Puts("\r\ndump start");
		MonDumpI2C();
	}
	else if( !stricmp( argv[0], "&" ) ) {
		indirect=0;
		MonNewDumpI2C();
	}

	//---------------- Bit Operation --------------------
	else if( !stricmp( argv[0], "B" ) ) {// Write bits - B AA II bb DD
		MonWriteBit();
	}
	//---------------- AD5110 CMD --------------------
	else if( !stricmp( argv[0], "PSW" ) ) {
		PSW_Flag=1;
		Printf("\r\n(PSW_Flag =1)");
	}
	//---------------- AD5110 CMD --------------------
	else if( !stricmp( argv[0], "ADW" ) ) {
		WriteAD5110(Asc2Bin(argv[1]),Asc2Bin(argv[2]));
		Printf("\r\nAD5110 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)Asc2Bin(argv[2]));
	}
	else if( !stricmp( argv[0], "ADR" ) ) {
		ReadAD5110(Asc2Bin(argv[1]));
		Printf("\r\nAD5110 Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ReadAD5110(Asc2Bin(argv[1])));
	}
*/
	/*	no indirect addressing in TW8816
    //---------------------------------------------------
	else if( !stricmp( argv[0], "@" ) ) { // Indirect address bit access

					BYTE Slave, mask, FromBit, ToBit,  MonMask, val,i;
					// @ 8a iaddr idata index  startbit|endbit  data
					// 0  1   2    3     4     5                 6

					if( argc<7 ) {
						Printf("   --> Missing parameter !!!");
						return;
					}

					Slave = Asc2Bin(argv[1]);
					
					FromBit = Asc2Bin( argv[5] );
					FromBit  = ( FromBit >> 4) & 0x0f;
					ToBit = Asc2Bin( argv[5] );
					ToBit  =  ToBit & 0x0f;
					MonMask  = Asc2Bin( argv[6] );

					if( FromBit<ToBit || FromBit>7 || ToBit>7) {
						Printf("\r\n   --> Wrong range of bit operation !!!");
						return;
					}
	
					mask = 0xff; 
					val=0x7f;
					for(i=7; i>FromBit; i--) {
						mask &= val;
						val = val>>1;
					}

					val=0xfe;
					for(i=0; i<ToBit; i++) {
						mask &= val;
						val = val<<1;
					}

					// @ 8a iaddr idata index  startbit|endbit  data
					// 0  1   2    3     4     5                 6
					MonIndex = Asc2Bin(argv[2]);
					MonWdata = Asc2Bin(argv[4]);
					WriteI2C( Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, Asc2Bin( argv[3]));
					MonWdata = (MonRdata & (~mask)) | (MonMask & mask);
					MonIndex = Asc2Bin(argv[3]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, MonIndex);

					Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

	}
	//------------------------------------------------------------------				
	else if( !stricmp( argv[0], "!" ) ) //  Indirect address write access
	{
					BYTE Slave;
					// @ 8a iaddr idata index data
					if( argc<6 ) {
						Printf("   --> Missing parameter !!!");
						return;
					}

					Slave = Asc2Bin(argv[1]);

					MonIndex = Asc2Bin(argv[2]);
					MonWdata = Asc2Bin(argv[4]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonIndex = Asc2Bin(argv[3]);
					MonWdata = Asc2Bin(argv[5]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, MonIndex);

					Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

	}
	*/    

	//---------------- Change I2C -----------------------
	/*
	else if( !stricmp( (BYTE *)argv[0],(BYTE *) "C" ) ) {
		//Monitor_I2C = Asc2Bin( argv[1] );
		SetNowMode(); 
	}
	
	else if( !stricmp( (BYTE *)argv[0],(BYTE *) "PON" ) ) {
			//Monitor_I2C = Asc2Bin( argv[1] );
			SetNowMode(); 
				LCDPowerON();
		}
	*/

	//---------------- Help -----------------------------
	else if( !stricmp( (BYTE *)argv[0],(BYTE *) "H" ) || !stricmp( (BYTE *)argv[0],(BYTE *) "HELP" ) || !stricmp((BYTE *) argv[0], (BYTE *)"?" ) ) {
		MonHelp();
	}
	else if( !stricmp( argv[0], (BYTE *)"PWM" ) ) {

		user_pwm_setvalue(Asc2Bin(argv[1])*15);

			Printf("\r\nPWM=%d",(WORD)(Asc2Bin(argv[1])*15));
	}
	else if( !stricmp( argv[0],(BYTE *) "ALCR" ) ) {

			Printf("\r\nALCR=%04x",(WORD)GET_ALC122(0x02));

		}
	else if( !stricmp( argv[0],(BYTE *) "ALCW" ) ) {
		WORD val=0;
		val=Asc2Bin(argv[1]);
		val<<=8;
		val|=Asc2Bin(argv[1]);
	
		SET_ALC122(0x02,val);

			Printf("\r\nALCR=%04x",(WORD)GET_ALC122(0x02));
	}
	/*
	else if( !stricmp( argv[0], (BYTE *)"tri" ) ) {
	TriggerFlag=Asc2Bin( argv[1]);

			Printf("\r\nTrigger=%02x",(WORD)TriggerFlag);
	}
	*/
else if( !stricmp( argv[0], (BYTE *) "ACCESS" ) ) {
		//if( argc==2 ) {
			AccessEnable = Asc2Bin(argv[1]);
		//}
		Printf("\r\nVS4210 AUTO ACCESS = %d", (WORD)AccessEnable);
	}
else if( !stricmp( argv[0], (BYTE *) "Win" ) ) {	//20201214-02

		BYTE ch;
		ch=Asc2Bin(argv[1]);
		
		HS_Select_Win(ch);
		

		if(ch==1)
		{
		Printf("\r\nTP_InputMode = %d", (WORD)TP_InputMode);
		
			if(TP_InputMode==5)
			TP_bt1120_720P_1080P_mode0_w0();
			else if(TP_InputMode==6)
				TP_bt1120_1080P_1080P_mode0_w0();
			else if(TP_InputMode==0)
				TP_bt1120_NTSC_1080P_mode0_w0();
			else if(TP_InputMode==1)
				TP_bt1120_PAL_1080P_mode0_w0();
							
		}
}	
	else if( !stricmp( argv[0], (BYTE *) "TP0" ) ) { //720P
		TP_bt1120_720P_1080P_mode0_w0();
	}	
	else if( !stricmp( argv[0],(BYTE *)  "TP1" ) ) { //1080P
		TP_bt1120_1080P_1080P_mode0_w0();
	}	
	else if( !stricmp( argv[0], (BYTE *) "TP2" ) ) { //NTSC
		TP_bt1120_NTSC_1080P_mode0_w0();
	}	
	else if( !stricmp( argv[0],(BYTE *) "TP3" ) ) { //PAL
		TP_bt1120_PAL_1080P_mode0_w0();
	}	
	else if( !stricmp( argv[0],(BYTE *) "HDMI" ) ) { //HDMI

		Monitor_flag=_FALSE;
		
		SelectInputMode=0;
		HS_Select_Win(0);
	}
	else if( !stricmp( argv[0],(BYTE *) "AHD" ) ) { //AHD

		Monitor_flag=_FALSE;
	
		SelectInputMode=1;
		HS_Select_Win(1);
		Printf("\r\nTP_InputMode = %d", (WORD)TP_InputMode);
		
			if(TP_InputMode==5)
			TP_bt1120_720P_1080P_mode0_w0();
			else if(TP_InputMode==6)
				TP_bt1120_1080P_1080P_mode0_w0();
			else if(TP_InputMode==0)
				TP_bt1120_NTSC_1080P_mode0_w0();
			else if(TP_InputMode==1)
				TP_bt1120_PAL_1080P_mode0_w0();
			
							
	}	
	else if( !stricmp( argv[0],(BYTE *) "BS" ) ) { //HDMI input
		extern tByte bSignal[4] ;
		Printf("\r\nbSignal[0] = %d", (WORD) bSignal[0]);
		Printf("\r\nbSignal[1] = %d", (WORD) bSignal[1]);
		Printf("\r\nbSignal[2] = %d", (WORD) bSignal[2]);
		Printf("\r\nbSignal[3] = %d", (WORD) bSignal[3]);

		Monitor_flag=_FALSE;
							
	}	
	else if( !stricmp( argv[0],(BYTE *) "SETHDMI" ) ) { //set HDMI embedded sync

		Monitor_flag=_FALSE;
		SET_HDMI_SYNC_PARA();

	}	
	else if( !stricmp( argv[0],(BYTE *) "SETAHD" ) ) { //set AHD embedded sync

		Monitor_flag=_FALSE;
		SET_AHD_SYNC_PARA();

	}	  
	else if( !stricmp( argv[0],(BYTE *) "TPRST" ) ) { //reset TP2824

		Init_TP280x_RegSet();

	}	
	else if( !stricmp( argv[0],(BYTE *) "HDMI6" ) ) { 

		Monitor_flag=_FALSE;
		SET_HDMI_640_480P_PARA();

		}
	else if( !stricmp( argv[0],(BYTE *) "V720" ) ) { 

		Monitor_flag=_FALSE;
		SET_VS4210_720_480_out();
		
	}	
	else if( !stricmp( argv[0],(BYTE *) "om" ) ) {    //COC menu

		Monitor_flag=_FALSE;
		TVI_Command=0x0F;
		presetNum=0;
		TVI_speed=0;
		TVI_Chksum=0xC5;
		ptztxflag=1;
		
	}	
	else if( !stricmp( argv[0],(BYTE *) "ou" ) ) {    //COC up

		Monitor_flag=_FALSE;
		TVI_Command=0x06;
		presetNum=0x1F;
		TVI_speed=0;
		TVI_Chksum=0xDB;
		ptztxflag=1;
		
	}	
	else if( !stricmp( argv[0],(BYTE *) "od" ) ) {    //COC down

		Monitor_flag=_FALSE;
		TVI_Command=0x07;
		presetNum=0x1F;
		TVI_speed=0;
		TVI_Chksum=0xDC;
		ptztxflag=1;

	}	else if( !stricmp( argv[0],(BYTE *) "ol" ) ) {    //COC left

		Monitor_flag=_FALSE;
		TVI_Command=0x09;
		presetNum=0;
		TVI_speed=0x1F;
		TVI_Chksum=0xDE;
		ptztxflag=1;

	}	else if( !stricmp( argv[0],(BYTE *) "or" ) ) {    //COC right

		Monitor_flag=_FALSE;
		TVI_Command=0x08;
		presetNum=0;
		TVI_speed=0x1F;
		TVI_Chksum=0xDD;
		ptztxflag=1;
		
	}

	#if 0
	else if( !stricmp( argv[0], (BYTE *)"sw" ) ) {
		BYTE ch,win;
	ch=Asc2Bin( argv[1]);
	win=Asc2Bin( (BYTE *)argv[2]);
	VS4210_SwitchChannel(ch,win);

			Printf("\r\n(VS4210_SwitchChannel ch=%02x win=%02x)",(WORD)ch,(WORD)win);
	}
	#endif
	#if 0
	else if( !stricmp( (BYTE *)argv[0], (BYTE *)"dpreg" ) ) {	
		BYTE i;
		    for( i = 0; i < 4; i++)
    {/*
        count[i] = 0;
        state[i] = VIDEO_UNPLUG;
        std[i] = TVI;
        mode[i] = NoDet;
		egain[4][4];        
		*/
Printf("\r\n(ch=%02x count=%02x state=%02x std=%02x mode=%02x)",(WORD)i,(WORD)count[i],(WORD)state[i],(WORD) std[i],(WORD)mode[i]);
Printf("(egain[%02x]  [0]=%02x [1]=%02x [2]=%02x [3]=%02x)",(WORD)i,(WORD)egain[i][0],(WORD)egain[i][1],(WORD) egain[i][2],(WORD)egain[i][3]);
    }
	
		}
#endif	
		/*
	//---------------------------------------------------
	else if( !stricmp( argv[0], "*" ) ) {
			
				if( argc==1 ) {
					Printf("\r\n  * 0 : Program default Loader");
					Printf("\r\n  * 1 : Program external Loader");
					Printf("\r\n  * 2 : Execute Loader");
				}
				else { 
					BYTE mode;
					mode = Asc2Bin(argv[1]);
					//Loader(mode);
				}
	}

	//---------------------------------------------------
#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
	else if( !stricmp( argv[0], "PCLK" ) ) {
		PclkAccess();			
	}
#endif	

	//---------------------------------------------------
	else if( !stricmp( argv[0], "POWER" ) ) {
		//LCDPower();
	}

	//---------------------------------------------------
#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
	else if( !stricmp( argv[0], "M" ) ) {
		if( argc==2 ) {
			if( !stricmp( argv[1], "?" ) ) {
				BYTE i;
				Printf("\r\nCurrent Input:%d \r\n", (WORD)InputSelection);
				for(i=1; struct_InputSelection[i].Id!=0 ; i++)
						Printf("%s:%d  ",struct_InputSelection[i].Name,(WORD)struct_InputSelection[i].Id );
				Printf("\r\n");
			}
			else
				ChangeInput(Asc2Bin( argv[1] ));
		}
		else
			ChangeInput(GetNextInputSelection());
	}	
	//---------------------------------------------------
	else if( !stricmp( argv[0], "ACCESS" ) ) {
		if( argc==2 ) {
			AccessEnable = Asc2Bin(argv[1]);
		}
		Printf("\r\nTW88xx AUTO ACCESS = %d", (WORD)AccessEnable);
	}
	#if 0
	//---------------------------------------------------
	else if( !stricmp( argv[0], "AUTODETECT" ) ) {
		if( argc==2 ) {
			AutoDetect = Asc2Bin(argv[1]);
		}
		Printf("\r\nPC Auto Detect = %d", (WORD)AutoDetect);
	}	
	#endif
	//---------------------------------------------------
	else if( !stricmp( argv[0], "PCAUTO" ) ) {
		Puts("\r\nPC Auto Measurement");
		AutoAdjust();
	}	
	//---------------------------------------------------
	else if( !stricmp( argv[0], "PCCOLOR" ) ) {
		Puts("\r\nPC Color Auto Measurement");
		AutoColorAdjust();
	}	
	//---------------------------------------------------
	else if( !stricmp( argv[0], "7" ) ) {
		Puts("\r\nGet HPN / VPN");
		GetHpnVpn(DebugLevel);
	}	
#endif
	//---------------- Debug Level ---------------------
	else if ( !stricmp( argv[0], "DEBUG" ) ) {
		if( argc==2 ) {
			DebugLevel = Asc2Bin(argv[1]);
			SaveDebugLevelEE(DebugLevel);
		}
		Printf("\r\nDebug Level = %2x", (WORD)DebugLevel);
	}
	
	//---------------- Display on/off -------------------
	else if ( !stricmp( argv[0], "echo" ) ) {
		if( !stricmp( argv[1], "off" ) ) {
			echo = 0;
			Printf("\r\necho off");
		}
		else {
			echo = 1;
			Printf("\r\necho on");
		}
	}
	//---------------- RESET/ TEST / POWER_DOWN -------------------
	else if ( !stricmp( argv[0], "reset" ) ) {
		if( !stricmp( argv[1], "0" ) ) {
//			P3_4 = 0;
			Printf("\r\nTW_reset_pin => 0");
		}
		else if( !stricmp( argv[1], "1" ) ) {
	//		P3_4 = 1;
			Printf("\r\nTW_reset_pin => 1");
		}
		else {
			//Printf("\r\nTW_reset_pin = %d", (WORD)P3_4);
		}
	}
	else if ( !stricmp( argv[0], "test" ) ) {
		if( !stricmp( argv[1], "0" ) ) {
			//P0_7 = 0;
			Printf("\r\nTW_test_pin => 0");
		}
		else if( !stricmp( argv[1], "1" ) ) {
			//P0_7 = 1;
			Printf("\r\nTW_test_pin => 1");
		}
		else {
		//	Printf("\r\nTW_test_pin = %d", (WORD)P0_7);
		}
	}
	else if ( !stricmp( argv[0], "pdn" ) ) {
		if( !stricmp( argv[1], "0" ) ) {
		//	P3_7 = 0;
			Printf("\r\nTW_PowerDown_pin => 0");
		}
		else if( !stricmp( argv[1], "1" ) ) {
			//P3_7 = 1;
			Printf("\r\nTW_PowerDown_pin => 1");
		}
		else {
			//Printf("\r\nTW_PowerDown_pin = %d", (WORD)P3_7);
		}
	}
	//---------------- OSD test for parallel -----------------------------------
	else if( !stricmp( argv[0], "OSD" ) ) {
		if( !stricmp( argv[1], "logo" ) ) {
//			DisplayLogo();
		}
		else if( !stricmp( argv[1], "end" ) ) {
		  	Puts("\r\nIf you'd like to exit, press any Key....");
	 		while ( !RS_ready() ){
				P2 = ReadTW88( 0x95 );
			}
		}
		else if( !stricmp( argv[1], "input" ) ) {
//			DisplayInput();
		}
		else if( !stricmp( argv[1], "off" ) ) {
////			ShowOSDWindowAll(OFF);
		}
		//else if( !stricmp( argv[1], "grid" ) ) {
		//	DisplayGrid();
		//}
#if 0		
#ifndef BANKING
		else if( !stricmp( argv[1], "cone" ) ) {
			DisplayCone();
			DisplayJapanese();
		}
#endif		
#endif
		#ifdef BANKING
		else if( !stricmp( argv[1], "agrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayMessage();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}
		else if( !stricmp( argv[1], "cgrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayChinese();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}
		else if( !stricmp( argv[1], "jgrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayJapanese();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}
		else if( !stricmp( argv[1], "kgrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayKorean();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}
		#endif
	}
	//---------------- TELI ML070I Test function -----------------------------------
	else if( !stricmp( argv[0], "PWM0" ) ) {
		SET_PWM(_BL_PWM, Asc2Bin(argv[1]));
		Puts("\r\nSET PWMP1.4");
	}
	else if( !stricmp( argv[0], "PWM1" ) ) {
		SET_PWM(_CHG_CURR, Asc2Bin(argv[1]));
		Puts("\r\nSET PWMP1.5");
	}
	else if( !stricmp( argv[0], "PWM" ) ) {
		if(Asc2Bin(argv[1]))
			{
			SET_PWM(_RUN_PWM, _RUN_PWM);
			Puts("\r\nPOEN PWM");
			}
		else
			{
			SET_PWM(_STOP_PWM, _STOP_PWM);
			Puts("\r\nSTOP PWM");
			}
	}
	//---------------- serial tx test with some number -----------------------------------
	else if( !stricmp( argv[0], "TX" ) ) {
		MonTxDump();
	}
	//---------------- OSD FONTtest for parallel -----------------------------------
	else if( !stricmp( argv[0], "FONT" ) ) {
		if ( !stricmp( argv[1], "RAM" ) ) {
//			DisplayRAMFont( Asc2Bin(argv[2]) );
		}
		else if ( !stricmp( argv[1], "ROM" ) ) {
		//	DisplayROMFont(Asc2Bin(argv[2]));
		}
		else {
			Puts("\r\n Font ROM # or Font RAM # for testing");
		}
	}
	*/
	//---------------- Delta RGB Panel Test -------------------------
	#if 0//def SUPPORT_DELTA_RGB

	else if( !stricmp( argv[0], "delta" ) ) {
		if( argc==1 )	DeltaRGBPanelInit();
		else {
			
			if( !stricmp( argv[1], "w" ) ) {

				WORD val;
				val= Asc2Bin(argv[3])<<8 | Asc2Bin(argv[4]);
				Printf("\r\n SPI Write: Addr:%2x  Data:%4x", (WORD)Asc2Bin(argv[2]), val);
				WriteSPI(Asc2Bin(argv[2]), val );
			}
			else if( !stricmp( argv[1], "r" ) ) {
				WORD val;
				val = ReadSPI(Asc2Bin(argv[2]));
				Printf("\r\n SPI Read: Addr:%2x  Data:%4x", (WORD)Asc2Bin(argv[2]), val);
			}
		}
	}
	#endif
	//----------------------------------------------------
	else {
		Printf("\r\nInvalid command...");
	}


	Prompt();


}
#endif



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


//#endif

//-------------------------------------------------------------------------


//#endif
