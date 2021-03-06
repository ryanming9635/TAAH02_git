// *****************************************************
// Company : Techpoint Inc
// $Id:  $
// $Author:  $
// $Date:  $
// $Revision:  $
// $Log:  $
// ******************************************************

//
//Registers.h	: Register declaration
//

#ifndef __REGISTERS__
#define __REGISTERS__

#ifdef SDCC
/* BYTE addressable registers */
sfr at 0x80 P0          ;
sfr at 0x81 SP          ;
sfr at 0x82 DPL         ;
sfr at 0x83 DPH         ;
sfr at 0x87 PCON        ;
sfr at 0x88 TCON        ;
sfr at 0x89 TMOD        ;
sfr at 0x8A TL0         ;
sfr at 0x8B TL1         ;
sfr at 0x8C TH0         ;
sfr at 0x8D TH1         ;
sfr at 0x90 P1          ;
sfr at 0x98 SCON        ;
sfr at 0x99 SBUF        ;
sfr at 0xA0 P2          ;
sfr at 0xA8 IE          ;
sfr at 0xB0 P3          ;
sfr at 0xB8 IP          ;
sfr at 0xD0 PSW         ;
sfr at 0xE0 ACC         ;
sfr at 0xE0 A           ;
sfr at 0xF0 B           ;
sfr at 0xd8 P4			;

/* BIT addressable registers */
/* P0 */
sbit at 0x80 P0_0       ;
sbit at 0x81 P0_1       ;
sbit at 0x82 P0_2       ;
sbit at 0x83 P0_3       ;
sbit at 0x84 P0_4       ;
sbit at 0x85 P0_5       ;
sbit at 0x86 P0_6       ;
sbit at 0x87 P0_7       ;

/* TCON */
sbit at 0x88 IT0        ;
sbit at 0x89 IE0        ;
sbit at 0x8A IT1        ;
sbit at 0x8B IE1        ;
sbit at 0x8C TR0        ;
sbit at 0x8D TF0        ;
sbit at 0x8E TR1        ;
sbit at 0x8F TF1        ;

/* P1 */
sbit at 0x90 P1_0       ;
sbit at 0x91 P1_1       ;
sbit at 0x92 P1_2       ;
sbit at 0x93 P1_3       ;
sbit at 0x94 P1_4       ;
sbit at 0x95 P1_5       ;
sbit at 0x96 P1_6       ;
sbit at 0x97 P1_7       ;

/* SCON */
sbit at 0x98 RI         ;
sbit at 0x99 TI         ;
sbit at 0x9A RB8        ;
sbit at 0x9B TB8        ;
sbit at 0x9C REN        ;
sbit at 0x9D SM2        ;
sbit at 0x9E SM1        ;
sbit at 0x9F SM0        ;

/* P2 */
sbit at 0xA0 P2_0       ;
sbit at 0xA1 P2_1       ;
sbit at 0xA2 P2_2       ;
sbit at 0xA3 P2_3       ;
sbit at 0xA4 P2_4       ;
sbit at 0xA5 P2_5       ;
sbit at 0xA6 P2_6       ;
sbit at 0xA7 P2_7       ;

/* IE */
sbit at 0xA8 EX0        ;
sbit at 0xA9 ET0        ;
sbit at 0xAA EX1        ;
sbit at 0xAB ET1        ;
sbit at 0xAC ES         ;
sbit at 0xAF EA         ;

/* P3 */
sbit at 0xB0 P3_0       ;
sbit at 0xB1 P3_1       ;
sbit at 0xB2 P3_2       ;
sbit at 0xB3 P3_3       ;
sbit at 0xB4 P3_4       ;
sbit at 0xB5 P3_5       ;
sbit at 0xB6 P3_6       ;
sbit at 0xB7 P3_7       ;

/* P4 */
sbit at 0xd9 P4_1       ;

sbit at 0xB0 RXD        ;
sbit at 0xB1 TXD        ;
sbit at 0xB2 INT0       ;
sbit at 0xB3 INT1       ;
sbit at 0xB4 T0         ;
sbit at 0xB5 T1         ;
sbit at 0xB6 WR         ;
sbit at 0xB7 RD         ;

/* IP */ 
sbit at 0xB8 PX0        ;
sbit at 0xB9 PT0        ;
sbit at 0xBA PX1        ;
sbit at 0xBB PT1        ;
sbit at 0xBC PS         ;

/* PSW */
sbit at 0xD0 P          ;
sbit at 0xD1 FL         ;
sbit at 0xD2 OV         ;
sbit at 0xD3 RS0        ;
sbit at 0xD4 RS1        ;
sbit at 0xD5 F0         ;
sbit at 0xD6 AC         ;
sbit at 0xD7 CY         ;

#elif defined KEILC

/*  BYTE Registers  */
sfr P0    = 0x80;
sfr P1    = 0x90;
sfr P2    = 0xA0;
sfr P3    = 0xB0;
sfr PSW   = 0xD0;
sfr ACC   = 0xE0;
sfr B     = 0xF0;
sfr SP    = 0x81;
sfr DPL   = 0x82;
sfr DPH   = 0x83;
sfr P0UPR = 0x86;
sfr PCON  = 0x87;
sfr TCON  = 0x88;
sfr TMOD  = 0x89;
sfr TL0   = 0x8A;
sfr TL1   = 0x8B;
sfr TH0   = 0x8C;
sfr TH1   = 0x8D;
sfr IE    = 0xA8;
sfr IP    = 0xB8;
sfr SCON  = 0x98;
sfr SBUF  = 0x99;
sfr P4    = 0xd8;

/*  8052 Extensions  */
sfr T2CON = 0xC8;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2   = 0xCC;
sfr TH2   = 0xCD;


/*  BIT Registers  */
/*  PSW  */
sbit CY    = 0xD7;
sbit AC    = 0xD6;
sbit F0    = 0xD5;
sbit RS1   = 0xD4;
sbit RS0   = 0xD3;
sbit OV    = 0xD2;
sbit P     = 0xD0;

/*  TCON  */
sbit TF1   = 0x8F;
sbit TR1   = 0x8E;
sbit TF0   = 0x8D;
sbit TR0   = 0x8C;
sbit IE1   = 0x8B;
sbit IT1   = 0x8A;
sbit IE0   = 0x89;
sbit IT0   = 0x88;

/*  IE  */
sbit EA    = 0xAF;
sbit ES    = 0xAC;
sbit ET1   = 0xAB;
sbit EX1   = 0xAA;
sbit ET0   = 0xA9;
sbit EX0   = 0xA8;

/*  IP  */
sbit PS    = 0xBC;
sbit PT1   = 0xBB;
sbit PX1   = 0xBA;
sbit PT0   = 0xB9;
sbit PX0   = 0xB8;

/*  P3  */
sbit RD    = 0xB7;
sbit WR    = 0xB6;
sbit T1    = 0xB5;
sbit T0    = 0xB4;
sbit INT1  = 0xB3;
sbit INT0  = 0xB2;
sbit TXD   = 0xB1;
sbit RXD   = 0xB0;

/*  SCON  */
//sbit SM0   = 0x9F;
//sbit SM1   = 0x9E;
//sbit SM2   = 0x9D;
//sbit REN   = 0x9C;
//sbit TB8   = 0x9B;
//sbit RB8   = 0x9A;
sbit TI    = 0x99;
sbit RI    = 0x98;

/*  8052 Extensions  */
/*  IE  */
sbit ET2   = 0xAD;

/*  IP  */
sbit PT2   = 0xBD;

/*  P1  */
sbit T2EX  = 0x91;
sbit T2    = 0x90;
             
/*  T2CON  */
sbit TF2   = 0xCF;
sbit EXF2  = 0xCE;
sbit RCLK  = 0xCD;
sbit TCLK  = 0xCC;
sbit EXEN2 = 0xCB;
sbit TR2   = 0xCA;
sbit C_T2  = 0xC9;
sbit CP_RL2= 0xC8;

//sfr	CHPENR = 0xF6;
//sfr	CHPCON = 0xBF;
//sfr	SFRAL  = 0xC4;
//sfr	SFRAH  = 0xC5;
//sfr	SFRFD  = 0xC6;
//sfr	SFRCN  = 0xC7;
sfr	TA  = 0xC7;
sfr	CHPCON = 0x9F;

sbit P0_0  = 0x80;
sbit P0_1  = 0x81;
sbit P0_2  = 0x82;
sbit P0_3  = 0x83;
sbit P0_4  = 0x84;
sbit P0_5  = 0x85;
sbit P0_6  = 0x86;
sbit P0_7  = 0x87;

sbit P1_0  = 0x90;
sbit P1_1  = 0x91;
sbit P1_2  = 0x92;
sbit P1_3  = 0x93;
sbit P1_4  = 0x94;
sbit P1_5  = 0x95;
sbit P1_6  = 0x96;
sbit P1_7  = 0x97;

sbit P2_0  = 0xa0;
sbit P2_1  = 0xa1;
sbit P2_2  = 0xa2;
sbit P2_3  = 0xa3;
sbit P2_4  = 0xa4;
sbit P2_5  = 0xa5;
sbit P2_6  = 0xa6;
sbit P2_7  = 0xa7;

sbit P3_0  = 0xb0;
sbit P3_1  = 0xb1;
sbit P3_2  = 0xb2;
sbit P3_3  = 0xb3;
sbit P3_4  = 0xb4;
sbit P3_5  = 0xb5;
sbit P3_6  = 0xb6;
sbit P3_7  = 0xb7;

sbit P4_1  = 0xd9;

#elif defined TASKINGC

_sfrbyte	P0		_at( 0x80 );	_sfrbit	TR1		_at( 0x8E );
_sfrbyte	SP		_at( 0x81 );	_sfrbit	TF1		_at( 0x8F );
_sfrbyte	DPL		_at( 0x82 );	_sfrbit	P1_0	_at( 0x90 );
_sfrbyte	DPH		_at( 0x83 );	_sfrbit	P1_1	_at( 0x91 );
_sfrbyte	PCON	_at( 0x87 );	_sfrbit	P1_2	_at( 0x92 );
_sfrbyte	TCON	_at( 0x88 );	_sfrbit	P1_3	_at( 0x93 );
_sfrbyte	TMOD	_at( 0x89 );	_sfrbit	P1_4	_at( 0x94 );
_sfrbyte	TL0		_at( 0x8A );	_sfrbit	P1_5	_at( 0x95 );
_sfrbyte	TL1		_at( 0x8B );	_sfrbit	P1_6	_at( 0x96 );
_sfrbyte	TH0		_at( 0x8C );	_sfrbit	P1_7	_at( 0x97 );
_sfrbyte	TH1		_at( 0x8D );	_sfrbit	RI		_at( 0x98 );
_sfrbyte	P1		_at( 0x90 );	_sfrbit	TI		_at( 0x99 );
_sfrbyte	SCON	_at( 0x98 );	_sfrbit	RB8		_at( 0x9A );
_sfrbyte	SBUF	_at( 0x99 );	_sfrbit	TB8		_at( 0x9B );
_sfrbyte	P2		_at( 0xA0 );	_sfrbit	REN		_at( 0x9C );
_sfrbyte	IE		_at( 0xA8 );	_sfrbit	SM2		_at( 0x9D );
_sfrbyte	P3		_at( 0xB0 );	_sfrbit	SM1		_at( 0x9E );
_sfrbyte	IP		_at( 0xB8 );	_sfrbit	SM0		_at( 0x9F );
_sfrbyte	PSW		_at( 0xD0 );	_sfrbit	P2_0	_at( 0xA0 );
_sfrbyte	ACC		_at( 0xE0 );	_sfrbit	P2_1	_at( 0xA1 );
_sfrbyte	B		_at( 0xF0 );	_sfrbit	P2_2	_at( 0xA2 );
_sfrbit		P0_0	_at( 0x80 );	_sfrbit	P2_3	_at( 0xA3 );
_sfrbit		P0_1	_at( 0x81 );	_sfrbit	P2_4	_at( 0xA4 );
_sfrbit		P0_2	_at( 0x82 );	_sfrbit	P2_5	_at( 0xA5 );
_sfrbit		P0_3	_at( 0x83 );	_sfrbit	P2_6	_at( 0xA6 );
_sfrbit		P0_4	_at( 0x84 );	_sfrbit	P2_7	_at( 0xA7 );
_sfrbit		P0_5	_at( 0x85 );	_sfrbit	EX0		_at( 0xA8 );
_sfrbit		P0_6	_at( 0x86 );	_sfrbit	ET0		_at( 0xA9 );
_sfrbit		P0_7	_at( 0x87 );	_sfrbit	EX1		_at( 0xAA );
_sfrbit		IT0		_at( 0x88 );	_sfrbit	ET1		_at( 0xAB );
_sfrbit		IE0		_at( 0x89 );	_sfrbit	ES		_at( 0xAC );
_sfrbit		IT1		_at( 0x8A );	_sfrbit	EA		_at( 0xAF );
_sfrbit		IE1		_at( 0x8B );	_sfrbit	P3_0	_at( 0xB0 );
_sfrbit		TR0		_at( 0x8C );	_sfrbit	RXD		_at( 0xB0 );
_sfrbit		TF0		_at( 0x8D );	_sfrbit	P3_1	_at( 0xB1 );
_sfrbit		TXD		_at( 0xB1 );	_sfrbit	RS1		_at( 0xD4 );
_sfrbit		INT0	_at( 0xB2 );	_sfrbit	F0		_at( 0xD5 );
_sfrbit		P3_2	_at( 0xB2 );	_sfrbit	AC		_at( 0xD6 );
_sfrbit		INT1	_at( 0xB3 );	_sfrbit	CY		_at( 0xD7 );
_sfrbit		P3_3	_at( 0xB3 );	_sfrbit	ACC_0	_at( 0xE0 );
_sfrbit		P3_4	_at( 0xB4 );	_sfrbit	ACC_1	_at( 0xE1 );
_sfrbit		T0		_at( 0xB4 );	_sfrbit	ACC_2	_at( 0xE2 );
_sfrbit		P3_5	_at( 0xB5 );	_sfrbit	ACC_3	_at( 0xE3 );
_sfrbit		T1		_at( 0xB5 );	_sfrbit	ACC_4	_at( 0xE4 );
_sfrbit		P3_6	_at( 0xB6 );	_sfrbit	ACC_5	_at( 0xE5 );
_sfrbit		WR		_at( 0xB6 );	_sfrbit	ACC_6	_at( 0xE6 );
_sfrbit		P3_7	_at( 0xB7 );	_sfrbit	ACC_7	_at( 0xE7 );
_sfrbit		RD		_at( 0xB7 );	_sfrbit	B_0		_at( 0xF0 );
_sfrbit		PX0		_at( 0xB8 );	_sfrbit	B_1		_at( 0xF1 );
_sfrbit		PT0		_at( 0xB9 );	_sfrbit	B_2		_at( 0xF2 );
_sfrbit		PX1		_at( 0xBA );	_sfrbit	B_3		_at( 0xF3 );
_sfrbit		PT1		_at( 0xBB );	_sfrbit	B_4		_at( 0xF4 );
_sfrbit		PS		_at( 0xBC );	_sfrbit	B_5		_at( 0xF5 );
_sfrbit		P		_at( 0xD0 );	_sfrbit	B_6		_at( 0xF6 );
_sfrbit		OV		_at( 0xD2 );	_sfrbit	B_7		_at( 0xF7 );
_sfrbit		RS0		_at( 0xD3 );

_sfrbyte	P4		_at( 0xd8 );	_sfrbit	P4_1	_at( 0xd9 );
_sfrbyte	CHPENR	_at( 0xF6 );
_sfrbyte	CHPCON	_at( 0xBF );
_sfrbyte	SFRAL	_at( 0xC4 );
_sfrbyte	SFRAH	_at( 0xC5 );
_sfrbyte	SFRFD	_at( 0xC6 );
_sfrbyte	SFRCN	_at( 0xC7 );


#endif
#endif
