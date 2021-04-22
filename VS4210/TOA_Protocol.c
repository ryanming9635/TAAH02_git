/*****************************************************************************/
/*                                                                           										*/
/*  STM32F103RE   MCU                    													*/
/*                                                                           										*/
/*  TOA_Protocol.c                                             										*/
/*                                                                           										*/
/*****************************************************************************/
/*
#include "Config.h"
#include "typedefs.h"
#include "i2c.h"
#include "main.h"
#include "printf.h"
#include "CPU.h"
#include "HS_DVRProtocol.h"
#include "keyremo.h"
#include "Etc_eep.h"
*/
	
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
#include "TOA_Protocol.h"

bit protocol_escape_flag;
BYTE protocol_state=0;
BYTE protocol_type;
BYTE protocol_len;
BYTE protocol_param;
BYTE protocol_chksum;
BYTE protocol_data_count=0;
//BYTE protocol_data[PROTOCOL_DATA_LENGTH];
WORD protocol_cmd=0;
WORD protocol_data=0;

BYTE CMDCount=0;

extern DATA  BYTE RS2_out;
extern bit ptztxflag;
extern BYTE TVI_Command;
extern BYTE presetNum;
extern BYTE TVI_speed;
extern BYTE TVI_Chksum;
extern BYTE PtzTxChNum;

/*
extern float Encorder1,Encorder2,Encorder3,Encorder4;
extern float Decimal1,Decimal2,Decimal3,Decimal4;
extern StructDVRInfoType g_stDVRInfo;
extern BYTE PowerFlag;
extern BYTE CameraVolt;
extern BYTE PWR_START_flag;
extern StructBatteryInfoType g_stBatteryInfo ;
*/
void TOAReceivePaser(void)
{
	BYTE dat,addr;

if ( RS2_ready())
	{
		BYTE c= RS2_rx();
		
		if ((c == MCU_PROTOCOL_CMD_Sync)&&(protocol_state==PROTOCOL_STATE_NONE))
		{	
			#if(_DEBUG_MESSAGE_UART_Protocol==ON)
			GraphicsPrint(CYAN,"\r\n(GOT)");
			#endif
			//protocol_escape_flag = 0;
			protocol_state = PROTOCOL_STATE_Sync;
			protocol_chksum = 0;
			protocol_data_count=1;
			return;	
		}else if(protocol_data_count>8)//reset command error
			{
					#if(_DEBUG_MESSAGE_UART_Protocol==ON)
					GraphicsPrint(RED,"\r\n(protocol_data_count>8)");
					#endif
					protocol_state=PROTOCOL_STATE_NONE; 

			}
	
		
		/*
		if (c == 0x7D)
		{
			protocol_escape_flag = 1;
			return;
		}
	
		if (protocol_escape_flag)
		{
			protocol_escape_flag = 0;
			c |= 0x70;
		}
		*/

		if ((protocol_state != PROTOCOL_STATE_CHECKSUM)&&(protocol_state != PROTOCOL_STATE_Sync)&&(protocol_state != PROTOCOL_STATE_ADDR))
		{
			//protocol_chksum ^= c;
			protocol_chksum += c;
		}


		switch (protocol_state)
				{
				case PROTOCOL_STATE_Sync:

					#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						//GraphicsPrint(YELLOW,"(1=%02x)",(WORD)c);
						#endif
					if(c==MCU_PROTOCOL_CMD_ADDR)
						protocol_state=PROTOCOL_STATE_COM1;
					else
						protocol_state=PROTOCOL_STATE_NONE;	

					protocol_data_count++;
					break;
					
				case PROTOCOL_STATE_COM1:

                                               #if(_DEBUG_MESSAGE_UART_Protocol==ON)
						//GraphicsPrint(YELLOW,"(2=%02x)",(WORD)c);
	                                        #endif
											
						protocol_cmd=c;
						protocol_state=PROTOCOL_STATE_COM2;
						protocol_data_count++;
					break;

				case PROTOCOL_STATE_COM2:
					
                                               #if(_DEBUG_MESSAGE_UART_Protocol==ON)
						//GraphicsPrint(YELLOW,"(3=%02x)",(WORD)c);
	                                        #endif
				
						protocol_cmd=((protocol_cmd<<8)|c);
						protocol_state=PROTOCOL_STATE_DATA1;				
					protocol_data_count++;
					break;
				case PROTOCOL_STATE_DATA1:	
					#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						//GraphicsPrint(YELLOW,"(4=%02x)",(WORD)c);
						#endif
				
						protocol_data=c;
						protocol_state=PROTOCOL_STATE_DATA2;				
						protocol_data_count++;		
					break;
				case PROTOCOL_STATE_DATA2:	
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						//GraphicsPrint(YELLOW,"(5=%02x)",(WORD)c);
						#endif
						protocol_data=((protocol_data<<8)|c);
						protocol_state=PROTOCOL_STATE_CHECKSUM;	
						protocol_data_count++;
					break;
				case PROTOCOL_STATE_CHECKSUM:		
						if(c==protocol_chksum)
						{
							protocol_data_count++;
							switch(protocol_cmd)
							{
							case MCU_PROTOCOL_CMD_VERSION:

								 if(protocol_data==MCU_PROTOCOL_DATA_VERSION)		
							 	{
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								//GraphicsPrint(YELLOW,"\r\n(Sent MCU VER)");
								#endif
						
								RS2_tx(0xFF);
								RS2_tx(0x00);
								RS2_tx(0x00);
								RS2_tx(0x07);

								//RS2_tx(((MCU_FW_Major0<<4)|(MCU_FW_Major1)));//DATA1
								//RS2_tx(((MCU_FW_Sub0<<4)|(MCU_FW_Sub1)));//DATA2
								//FWVER
								RS2_tx((BYTE)(FWVER>>8));//DATA1
								RS2_tx((BYTE)(FWVER));//DATA2
									
								//Check SUM
								//RS2_tx(0x07+((MCU_FW_Major0<<4)|(MCU_FW_Major1))+((MCU_FW_Sub0<<4)|(MCU_FW_Sub1)));
								RS2_tx(0x07+((BYTE)(FWVER>>8))+((BYTE)(FWVER)));

							 	}
						else	if(((protocol_data>=0x00)&&(protocol_data<=0x40))||((protocol_data>=0x64)&&(protocol_data<=0xC8)))
						{
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)
							GraphicsPrint(MAGENTA,"(CMD:PRESET_LOAD)");
							#endif

							TVI_Command=0x22;
							presetNum=protocol_data;
							TVI_speed=0;
							TVI_Chksum=0xC5;
							ptztxflag=1;	
							CMDCount=3;

						}
					else if(protocol_data==0x42)
						{
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(MAGENTA,"(CMD:AUTO_SCAN)");
								#endif
							TVI_Command=0x23;
							presetNum=protocol_data;
							TVI_speed=0;
							TVI_Chksum=00;
							ptztxflag=1;
							CMDCount=3;
							
						}
					else if(((protocol_data>=0x47)&&(protocol_data<=0x4E)))				
						{
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)
							GraphicsPrint(MAGENTA,"(CMD:GROUP_TOUR)");
							#endif
							TVI_Command=0x24;
							presetNum=protocol_data;
							TVI_speed=0;
							TVI_Chksum=00;
							ptztxflag=1;	
							CMDCount=3;
                            			}				
					else if(protocol_data==0x5F)
						{
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)
							GraphicsPrint(YELLOW,"(CMD:OSD_MENU)");
							#endif
							TVI_Command=0x25;
							presetNum=protocol_data;
							TVI_speed=0;
							TVI_Chksum=00;
							ptztxflag=1;	
							CMDCount=3;
						}
					else if(protocol_data==0x60)
						{
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)
							GraphicsPrint(CYAN,"(CMD:FOCUS_ADJUST)");
							#endif	
							TVI_Command=0x26;
							presetNum=protocol_data;
							TVI_speed=0;
							TVI_Chksum=00;
							ptztxflag=1;	
							CMDCount=3;
						}
					else{

						#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						GraphicsPrint(RED,"(1CMD: ERROR)");
						#endif	
						}				
						break;
case MCU_PROTOCOL_CMD_STOP:		
							if(protocol_data==0x00)
								{
								if(CMDCount>0)
									{
									CMDCount--;
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(YELLOW,"(CMD:STOP)");
								#endif		
								
								TVI_Command=0x14;
								presetNum=0;
								TVI_speed=0;
								TVI_Chksum=0;
								ptztxflag=1;	
								
									}
								else
									{
									#if(_DEBUG_MESSAGE_UART_Protocol==ON)
									GraphicsPrint(YELLOW,"(CMD:STOP no work)");
									#endif		
									CMDCount=0;

									}
								}
                             				 break;
case MCU_PROTOCOL_CMD_UP:
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:UP)");
								#endif
								TVI_Command=0x06;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0xDB;
								ptztxflag=1;
								CMDCount=3;
                            					break;
case MCU_PROTOCOL_CMD_RIGHT:
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:RIGHT)");
								#endif
								TVI_Command=0x08;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0xDD;
								ptztxflag=1;
								CMDCount=3;
                        				break;
case MCU_PROTOCOL_CMD_LEFT:	
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:LEFT)");
								#endif
								TVI_Command=0x09;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0xDE;
								ptztxflag=1;	
								CMDCount=3;
                        				break;
case MCU_PROTOCOL_CMD_DOWN:	
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:DOWN)");
								#endif
								TVI_Command=0x07;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0xDC;
								ptztxflag=1;	
								CMDCount=3;
                          						break;
case MCU_PROTOCOL_CMD_UP_RIGHT:	
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:UP_RIGHT)");
								#endif
								TVI_Command=0x0C;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0x20;
								ptztxflag=1;	
								CMDCount=3;
                          						break;
case MCU_PROTOCOL_CMD_DOWN_RIGHT:
									#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:DOWN_RIGHT)");
								#endif
								TVI_Command=0x0D;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0x21;
								ptztxflag=1;
								CMDCount=3;
                              					break;
case MCU_PROTOCOL_CMD_UP_LEFT:
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(UP_LEFT)");
								#endif
								TVI_Command=0x0A;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0x1E;
								ptztxflag=1;
								CMDCount=3;
	                          				break;
case MCU_PROTOCOL_CMD_DOWN_LEFT:	
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:DOWN_LEFT)");
								#endif
								TVI_Command=0x0B;
								presetNum=protocol_data;
								TVI_speed=(protocol_data>>8);
								TVI_Chksum=0x1F;
								ptztxflag=1;
								CMDCount=3;
                              					break;
case MCU_PROTOCOL_CMD_ZOOM_IN:
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:ZOOM_IN)");
								#endif
								TVI_Command=0x13;
								presetNum=0x00;
								TVI_speed=0x00;
								TVI_Chksum=0xC9;
								ptztxflag=1;
								CMDCount=3;
                          					break;
case MCU_PROTOCOL_CMD_ZOOM_OUT:
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:ZOOM_OUT)");
								#endif
								TVI_Command=0x12;
								presetNum=0x00;
								TVI_speed=0x00;
								TVI_Chksum=0xC8;
								ptztxflag=1;
								CMDCount=3;
                          			  		break;
case MCU_PROTOCOL_CMD_FOCUS_NEAR:	
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:FOCUS_NEAR)");
								#endif
								TVI_Command=0x11;
								presetNum=0x00;
								TVI_speed=0x00;
								TVI_Chksum=0xC7;
								ptztxflag=1;
								CMDCount=3;
                                				break;
case MCU_PROTOCOL_CMD_FOCUS_FAR:
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(GREEN,"(CMD:FOCUS_FAR)");
								#endif
								TVI_Command=0x10;
								presetNum=0x00;
								TVI_speed=0x00;
								TVI_Chksum=0xC6;
								ptztxflag=1;
								CMDCount=3;
                                				break;
case MCU_PROTOCOL_CMD_PRESET_SAVE:	
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)
								GraphicsPrint(MAGENTA,"(CMD:PRESET_SAVE)");
								#endif
							TVI_Command=0x21;
							presetNum=protocol_data;
							TVI_speed=0;
							TVI_Chksum=00;
							ptztxflag=1;	
							CMDCount=3;
                                    				break;	
case MCU_PROTOCOL_CMD_VS4210_W:
	
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)
							GraphicsPrint(BLUE,"\r\n(CMD:VS4210_W)");
							#endif
							addr=(BYTE)(protocol_data>>8); 	
							dat=	(BYTE)protocol_data; 	
							VXISI2CWrite(addr,dat); 
							dat=	VXISI2CRead(addr); 	
							 Printf("\r\nVS4210  Write addr=%02x  data=%02x",(WORD)addr,(WORD)dat);

								break;			
case MCU_PROTOCOL_CMD_VS4210_R:	
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)
							GraphicsPrint(CYAN,"\r\n(CMD:VS4210_R)");
							#endif

							addr=(BYTE)(protocol_data>>8); 	
							dat=VXISI2CRead(addr); 	
							 Printf("\r\nVS4210  Read addr=%02x  data=%02x",(WORD)addr,(WORD)dat);

								break;
							}

						//#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						//GraphicsPrint(RED,"\r\n(CMD: protocol_cmd=%04x)",(WORD)protocol_cmd);
						//#endif
						
						protocol_state=PROTOCOL_STATE_NONE;
						
						
						}
					else
						{
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						GraphicsPrint(RED,"(CHECKUSM ERROR)");
						#endif
						protocol_state=PROTOCOL_STATE_NONE;									
						} 
							break;


	
				//		}
					/*
						protocol_state=PROTOCOL_STATE_NONE;
						
						}
					else
						{
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						GraphicsPrint(RED,"\r\n(CHECKUSM ERROR)");
						#endif
						protocol_state=PROTOCOL_STATE_NONE;									
						}
					break;
				*/
						default:
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)
						GraphicsPrint(RED,"\r\n(2CMD: ERROR)");
						#endif
						protocol_state=PROTOCOL_STATE_NONE;	
							break;
			}


			if(ptztxflag==1)
				{
				Get_2824cIrq();//check tp2824 COC
				#if(_DEBUG_MESSAGE_UART_Protocol==ON)
				//GraphicsPrint(GREEN,"(CMD:ACTION)");
				GraphicsPrint(GREEN,"(A=%d)",(WORD)PtzTxChNum);
				#endif
				}
	}

}



