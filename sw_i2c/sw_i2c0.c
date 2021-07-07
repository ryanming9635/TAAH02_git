#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#include "VXIS_Extern_Define.h"

#include <sw_i2c1.h>

//#define PIN_SCL0   GPIO_PIN_6
//#define PIN_SDA0   GPIO_PIN_7

//#define PIN_SCL0   GPIO_PIN_7
//#define PIN_SDA0   GPIO_PIN_6

#define PIN_SCL0   GPIO_PIN_9
#define PIN_SDA0   GPIO_PIN_8

extern tByte gHDMI_Index  ;
extern void delay1ms(unsigned int i ) ;

static __inline void TWI_SCL_0(void)
{
    GPIOB->BRR = PIN_SCL0;
}
static __inline void TWI_SCL_1(void)
{
    GPIOB->BSRR = PIN_SCL0;
}
static __inline void TWI_SDA_0(void)
{
    GPIOB->BRR = PIN_SDA0;
}
static __inline void TWI_SDA_1(void)
{
    GPIOB->BSRR = PIN_SDA0;
}
static __inline uint8_t   TWI_SDA_STATE(void)
{
    return (GPIOB->IDR & PIN_SDA0) != 0;
}


static const uint8_t TWI_ACK     =0;
static const uint8_t TWI_READY   =0;
static const uint8_t TWI_NACK    =1;
static const uint8_t TWI_BUS_BUSY    =2;
static const uint8_t TWI_BUS_ERROR   =3;
//static const uint8_t  TWI_RETRY_COUNT = 3;
//extern void Delay_mS(uint32_t n);

static void TWI_NOP(void)
{
    uint8_t j;
    //for (j = 0; j < 12 ; j++)
  for (j = 0; j < 16 ; j++)//ryan@20210623 for GD32F103RET		
    {
    }
}


static void TWI_Initialize(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; 
    GPIO_InitStructure.Pin = PIN_SDA0 ;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; // GPIO_Mode_Out_PP;
    GPIO_InitStructure.Pin = PIN_SCL0 ;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    TWI_SDA_1();
    TWI_SCL_0();
}


static uint8_t TWI_START(void)
{
    TWI_SDA_1();
    TWI_NOP();

    TWI_SCL_1();
    TWI_NOP();

    if(!TWI_SDA_STATE())
    {
        return TWI_BUS_BUSY;
    }
    TWI_SDA_0();
    TWI_NOP();

    TWI_SCL_0();
    TWI_NOP();

    if(TWI_SDA_STATE())
    {
        return TWI_BUS_ERROR;
    }

    return TWI_READY;
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_STOP
 */
/* --------------------------------------------------------------------------*/
static void TWI_STOP(void)
{
    TWI_SDA_0();
    TWI_NOP();

    TWI_SCL_1();
    TWI_NOP();

    TWI_SDA_1();
    TWI_NOP();
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_SendACK
 */
/* --------------------------------------------------------------------------*/


static void TWI_SendACK(void)
{
    TWI_SDA_0();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    TWI_SCL_0();
    TWI_NOP();
    TWI_SDA_1();
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_SendNACK
 */
/* --------------------------------------------------------------------------*/

static void TWI_SendNACK(void)
{
    TWI_SDA_1();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    TWI_SCL_0();
    TWI_NOP();
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_SendByte
 *
 * @Param: Data
 *
 * @Returns:
 */
/* --------------------------------------------------------------------------*/
static uint8_t TWI_SendByte(uint8_t Data)
{
    uint8_t i;
    TWI_SCL_0();
    for(i=0; i<8; i++)
    {
        if(Data&0x80)
        {
            TWI_SDA_1();
        }
        else
        {
            TWI_SDA_0();
        }
        TWI_NOP();
        TWI_SCL_1();
        TWI_NOP();
        TWI_SCL_0();
        Data<<=1;
        //TWI_NOP();
        //---------------------------
    }
    TWI_SDA_1();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    if(TWI_SDA_STATE())
    {
        TWI_SCL_0();
        TWI_SDA_1();

        return TWI_NACK;
    }
    else
    {
        TWI_SCL_0();
        TWI_SDA_1();

        return TWI_ACK;
    }
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_ReceiveByte
 *
 * @Returns:
 */
/* --------------------------------------------------------------------------*/
static uint8_t TWI_ReceiveByte(void)
{
    uint8_t i,Dat;
    TWI_SDA_1();
    TWI_SCL_0();
    Dat=0;
    for(i=0; i<8; i++)
    {
        TWI_SCL_1();
        TWI_NOP();
        Dat<<=1;
        if(TWI_SDA_STATE())
        {
            Dat|=0x01;
        }
        TWI_SCL_0();
        TWI_NOP();
    }
    return Dat;
}


//void SW_I2C0_Init(void)
//{
   // TWI_Initialize();
//}

void I2C0_Write(uint8_t chipID ,uint8_t reg,uint8_t val )
{
    TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    TWI_SendByte(val) ;
    TWI_STOP();
	if (chipID == 0xA0)
	{
		delay1ms(10) ;
	}
		
}

void I2C0_WriteNByte(uint8_t chipID ,uint8_t reg, const uint8_t *ptr ,uint8_t NByte )
{
    uint8_t i ;
    uint8_t val ;
    TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    for (i = 0 ; i< NByte ; i++ )
    {
        val = ptr[i] ;
        TWI_SendByte( val ) ;
    }
    TWI_STOP();
}


///HDMI_ RX TX
//void I2C0_TC9548A_CtrWrite(uint8_t channel)
//{
//    uint8_t ch ;
//    ch = (uint8_t)(1 << channel ) ;
//    TWI_START();
//    TWI_SendByte(0xE0 ) ;
//    TWI_SendByte(ch) ;
//    TWI_STOP();
//    delay1ms(5) ;
//}

uint8_t I2C0_Read(uint8_t chipID ,uint8_t reg )
{
    uint8_t val ;
  TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    //TWI_STOP();  //VXIS
    TWI_NOP();
    //TWI_NOP();
    TWI_START();
    TWI_SendByte(chipID | 0x01) ;
    val = TWI_ReceiveByte() ;

    TWI_SendNACK();

    TWI_STOP();
	
    return val ;
}

void I2C0_nRead(uint8_t chipID ,uint8_t reg ,uint8_t *rBuf , uint8_t n )  
{
    //uint8_t val ;
    uint8_t i ;
    TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    //TWI_STOP();  //VXIS
    TWI_NOP();
    //TWI_NOP();
    TWI_START();
    TWI_SendByte(chipID | 0x01) ;
    
    for (i = 1 ; i < n ; i++)
    {
        rBuf[i-1] = TWI_ReceiveByte() ;
        TWI_SendACK() ;         
        
    }    
    rBuf[n-1] = TWI_ReceiveByte() ;
    TWI_SendNACK();

    TWI_STOP();
    //return val ;
}

void SW_I2C0_Init(void)
{
    TWI_Initialize();
}

void SW_I2C0Write(uint8_t chipID ,uint8_t reg,uint8_t val )
{
    TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    TWI_SendByte(val) ;
    TWI_STOP();
}

void SW_I2C0WriteNByte(uint8_t chipID ,uint8_t reg, const uint8_t *ptr ,uint8_t NByte )
{
    uint8_t i ;
    uint8_t val ;
    TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    for (i = 0 ; i< NByte ; i++ )
    {
        val = ptr[i] ;
        TWI_SendByte( val ) ;
    }
    TWI_STOP();
}



///HDMI_ RX TX
//void I2C0_TC9548A_CtrWrite(uint8_t channel)
//{
//    uint8_t ch ;
//    ch = (uint8_t)(1 << channel ) ;
//    TWI_START();
//    TWI_SendByte(0xE0 ) ;
//    TWI_SendByte(ch) ;
//    TWI_STOP();
//    delay1ms(5) ;
//}

uint8_t SW_I2C0Read(uint8_t chipID ,uint8_t reg )
{
    uint8_t val ;
    TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    TWI_STOP();  //VXIS
    TWI_NOP();
    TWI_NOP();
    TWI_START();
    TWI_SendByte(chipID | 0x01) ;
    val = TWI_ReceiveByte() ;

    TWI_SendNACK();

    TWI_STOP();
    return val ;
}

tByte HDMIRX_ReadI2C_Byte(tByte RegAddr)
{
    tByte uc ;
    if (gHDMI_Index == 0  )
    {
        uc = SW_I2C2Read(HDMIRXADR,RegAddr) ;
    }
    else if (gHDMI_Index == 1  )
    {
        uc = SW_I2C0Read(HDMIRXADR_B,RegAddr) ;
    }
    else if (gHDMI_Index == 2  )
    {
        uc = SW_I2C1Read(HDMIRXADR,RegAddr) ;
    }
    else if (gHDMI_Index == 3  )
    {
        uc = SW_I2C1Read(HDMIRXADR_B,RegAddr) ;
    }

    return uc ;
}
tByte HDMIRX_WriteI2C_Byte(tByte RegAddr,tByte val)
{
    if (gHDMI_Index == 0 )
    {
        SW_I2C2Write(HDMIRXADR,RegAddr,val) ;
    }
    else if (gHDMI_Index == 1 )
    {
        SW_I2C0Write(HDMIRXADR_B,RegAddr,val) ;
    }
    else if (gHDMI_Index == 2 )
    {
        SW_I2C1Write(HDMIRXADR,RegAddr,val) ;
    }
    else if (gHDMI_Index == 3 )
    {
        SW_I2C1Write(HDMIRXADR_B,RegAddr,val) ;
    }

    return 0 ;

}

tByte HDMIRX_WriteI2C_ByteN(tByte RegAddr,tcByte *pData,int N)
{
    //void I2C0_WriteNByte(uint8_t chipID ,uint8_t reg, const uint8_t *ptr ,uint8_t NByte )
    if (gHDMI_Index == 0 )
    {
        SW_I2C2WriteNByte(HDMIRXADR,RegAddr,pData,(tByte) N) ;
    }
    else if (gHDMI_Index == 1 )
    {
        SW_I2C0WriteNByte(HDMIRXADR_B,RegAddr, pData ,(tByte) N ) ;
    }
    else if (gHDMI_Index == 2 )
    {
        SW_I2C1WriteNByte(HDMIRXADR,RegAddr,pData,(tByte) N) ;
    }
    else if (gHDMI_Index == 3 )
    {
        SW_I2C1WriteNByte(HDMIRXADR_B,RegAddr, pData ,(tByte) N ) ;
    }
    return 0  ;
}

//TX
tByte HDMITX_ReadI2C_Byte(tByte RegAddr)
{
    return (SW_I2C0Read(ITE66121_ADDR, RegAddr) ) ;
}

int HDMITX_WriteI2C_Byte(tByte RegAddr, tByte d)
{
    SW_I2C0Write(ITE66121_ADDR, RegAddr, d)  ;
    //ER_SUCCESS = 0,
    return 0 ;
}

//HDCP

//#ifdef SUPPORT_HDCP
int HDMITX_ReadI2C_ByteN(tByte RegAddr,tByte *pData,int N)
{
    int i ;
        for (i = 0 ; i< N ; i++)
        {
            pData[i] = SW_I2C0Read(ITE66121_ADDR, RegAddr) ;
        }
        return 0 ;
}
//#endif


int HDMITX_SetI2C_Byte(tByte Reg,tByte Mask,tByte Value)
{
    tByte Temp;
    if( Mask != 0xFF )
    {
        Temp=HDMITX_ReadI2C_Byte(Reg);
        Temp&=(~Mask);
        Temp|=Value&Mask;
    }
    else
    {
        Temp=Value;
    }
    return HDMITX_WriteI2C_Byte(Reg, Temp);
}

/*
tByte HDMIRX_ReadI2C_Byte(tByte RegAddr)
{
    tByte uc ;
    if (gHDMI_Index % 2 == 0  )
    {
        uc = I2C0_Read(HDMIRXADR,RegAddr) ;
    }
    else
    {
        uc = I2C0_Read(HDMIRXADR_B,RegAddr) ;
    }

    return uc ;
}
tByte HDMIRX_WriteI2C_Byte(tByte RegAddr,tByte val)
{
    if (gHDMI_Index % 2 == 0 )
    {
        I2C0_Write(HDMIRXADR,RegAddr,val) ;
    }
    else
    {
        I2C0_Write(HDMIRXADR_B,RegAddr,val) ;
    }

    return 0 ;

}

tByte HDMIRX_WriteI2C_ByteN(tByte RegAddr,tcByte *pData,int N)
{
    if (gHDMI_Index % 2 == 0 )
    {
        //void I2C0_WriteNByte(uint8_t chipID ,uint8_t reg, const uint8_t *ptr ,uint8_t NByte )
        I2C0_WriteNByte(HDMIRXADR,RegAddr,pData,(tByte) N) ;
    }
    else
    {
        I2C0_WriteNByte(HDMIRXADR_B,RegAddr, pData ,(tByte) N ) ;
    }
    return 0  ;
}

*/

//HDMI_TX
//tByte HDMITX_ReadI2C_Byte(tByte RegAddr)
//{
//	tByte val ; 
//	
//    if (gHDMI_Index % 2 == 0 )
//    {
//		val = I2C0_Read(ITE66121_ADDR, RegAddr)  ; 		
//	}
//	else 
//	{
//		val = I2C0_Read(ITE66121_BADDR, RegAddr)  ; 
//	}
//	return val ;	
//}
//
//int HDMITX_WriteI2C_Byte(tByte RegAddr,tByte d)
//{	
//    if (gHDMI_Index % 2 == 0 )
//    {
//		I2C0_Write(ITE66121_ADDR, RegAddr, d )  ; 
//		
//	}
//	else 
//	{
//		I2C0_Write(ITE66121_BADDR, RegAddr, d)  ; 
//	}
//    return 0 ;
//}
//
////HDCP
//
////#ifdef SUPPORT_HDCP
//int HDMITX_ReadI2C_ByteN(tByte RegAddr,tByte *pData,int N)
//{
//    //int i ;
//	tByte chipid ; 		
//    if (gHDMI_Index % 2 == 0 )
//    {
//		chipid = ITE66121_ADDR ; 
//	}
//	else 
//	{
//		chipid = ITE66121_BADDR ; 
//	}
//    I2C0_nRead(chipid, RegAddr, pData , N )  ; 
//	
////    for (i = 0 ; i< N ; i++)
////    {
////		pData[i] = I2C0_Read(chipid, RegAddr) ;
////    }
//
//	return 0 ;
//}
////#endif 
//
//
//int HDMITX_SetI2C_Byte(tByte Reg,tByte Mask,tByte Value)
//{
//    tByte Temp;
//    if( Mask != 0xFF )
//    {
//        Temp=HDMITX_ReadI2C_Byte(Reg);
//        Temp&=(~Mask);
//        Temp|=Value&Mask;
//    }
//    else
//    {
//        Temp=Value;
//    }
//    //return HDMITX_WriteI2C_Byte(Reg,Temp);
//    HDMITX_WriteI2C_Byte(Reg,Temp);
//    return 0 ;
//}



