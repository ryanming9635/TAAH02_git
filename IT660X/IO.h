///*****************************************
//  Copyright (C) 2009-2020
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IO.h>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2020/05/27
//   @fileversion: ITE_HDMI1.4_RXSAMPLE_1.29
//******************************************/

#ifndef _IO_h_
#define _IO_h_
#include "typedef.h"


#ifdef SUPPORT_UART_CMD
void UartCommand();
#endif


//BYTE HDMI_IIC_Read( BYTE RegAddr);
//BOOL HDMI_IIC_Write( BYTE RegAddr,BYTE DataIn);
//BYTE HDMI_IIC_SET( BYTE offset, BYTE mask, BYTE datain );


BOOL i2c_write_byte( BYTE address,BYTE offset,BYTE byteno,BYTE *p_data,BYTE device );
BOOL i2c_read_byte( BYTE address,BYTE offset,BYTE byteno,BYTE *p_data,BYTE device );

#ifdef Enable_IT6602_CEC
#include "debug.h"
BYTE IT6602_CEC_ReadI2C_Byte(BYTE RegAddr);
SYS_STATUS IT6602_CEC_WriteI2C_Byte(BYTE offset,BYTE buffer );
#endif


//#define hdmirxrd(x) HDMI_IIC_Read(x)
//#define hdmirxwr(x,y) HDMI_IIC_Write(x,y)



void SetEDIDWp(BOOL ENA);


void HotPlug(BYTE Enable);

void init_printf(void);

void SetintActive(BOOL bactive);


#endif
