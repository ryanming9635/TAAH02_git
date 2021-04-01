#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#include "VXIS_Extern_Define.h"

//#define PIN_SCL2   GPIO_PIN_10
//#define PIN_SDA2   GPIO_PIN_11

//#define PIN_SCL2   GPIO_PIN_11
//#define PIN_SDA2   GPIO_PIN_10

#define PIN_SCL2   GPIO_PIN_10
#define PIN_SDA2   GPIO_PIN_11

#define dd()  I2Cdelay2()

//extern tByte g998xIndex  ;
extern void delay1ms(unsigned int i ) ;

static __inline void TWI_SCL_0(void)
{
    GPIOB->BRR = PIN_SCL2;

}
static __inline void TWI_SCL_1(void)
{
    GPIOB->BSRR = PIN_SCL2;

}
static __inline void TWI_SDA_0(void)
{
    GPIOB->BRR = PIN_SDA2;
}
static __inline void TWI_SDA_1(void)
{
    GPIOB->BSRR = PIN_SDA2;
}
static __inline uint8_t   TWI_SDA_STATE(void)
{
    return (GPIOB->IDR & PIN_SDA2) != 0;

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
    for (j = 0; j < 1 ; j++)
    {
    }
}


static void TWI_Initialize(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; 
    GPIO_InitStructure.Pin = PIN_SDA2 ;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; // GPIO_Mode_Out_PP;
    GPIO_InitStructure.Pin = PIN_SCL2 ;
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


void SW_I2C2_Init(void)
{
    TWI_Initialize();
}

void SW_I2C2Write(uint8_t chipID ,uint8_t reg,uint8_t val )
{
    TWI_START();
    TWI_SendByte(chipID) ;
    TWI_SendByte(reg) ;
    TWI_SendByte(val) ;
    TWI_STOP();
}

void SW_I2C2WriteNByte(uint8_t chipID ,uint8_t reg,const uint8_t *ptr ,uint8_t NByte )
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

uint8_t SW_I2C2Read(uint8_t chipID ,uint8_t reg )
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

void SW_I2C2nRead(uint8_t chipID ,uint8_t reg ,uint8_t *rBuf , uint8_t n )  
{
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
}

//==========================================================
//==========================================================
//==========================================================

void I2Cdelay(void)
 {
	int i;
	for (i=0;i<10;i++) {
		};
}


void I2CStart(void)
{
	TWI_SCL_1();		I2Cdelay();
	TWI_SDA_1();		I2Cdelay();
	TWI_SDA_0();		I2Cdelay();

}

void I2CReStart(void)
{
	TWI_SDA_1();	I2Cdelay();
	TWI_SCL_1();	I2Cdelay();
	TWI_SDA_0();	I2Cdelay();

}

void I2CStop(void)
{
	TWI_SDA_0();	I2Cdelay();
	TWI_SCL_0();	I2Cdelay();
	TWI_SCL_1();	I2Cdelay();
	TWI_SDA_1();	I2Cdelay();
//	I2C_SCL = 1;	I2Cdelay();
//	I2C_SDA = 1;

}


void I2CWriteData(BYTE value)
{
	int i;

	for(i=0; i<8; i++){
		TWI_SCL_0();
		I2Cdelay();
		if((value & 0x80) != 0) 
			TWI_SDA_1();
		else 			 
			TWI_SDA_0();
		I2Cdelay();
		TWI_SCL_1();
		I2Cdelay();//vhuang
		TWI_SCL_0();
		I2Cdelay();
		value<<=1;
	}
	TWI_SCL_0();	I2Cdelay();
	TWI_SCL_1();	I2Cdelay();
	TWI_SCL_0();	I2Cdelay();
}

BYTE I2CReadData(void)
{
	int i;
	BYTE value=0;

	
	for(i=0; i<8; i++) {
		TWI_SCL_0();
		I2Cdelay();
		TWI_SCL_1();
		value <<= 1;
		if(TWI_SDA_STATE()) value |= 1;
		I2Cdelay();
		TWI_SCL_0();
		I2Cdelay();
	}

	TWI_SCL_0();	I2Cdelay();
	TWI_SCL_1();	I2Cdelay();
	TWI_SCL_0();	I2Cdelay();

	return value;

}

void I2CWriteData3(BYTE value)
{
	int i;

	for(i=0; i<8; i++){
		TWI_SCL_0();
		I2Cdelay();
		if((value & 0x80) != 0) 
			TWI_SDA_1();
		else 			 
			TWI_SDA_0();
		I2Cdelay();
		TWI_SCL_1();
		I2Cdelay();//vhuang
		TWI_SCL_0();
		I2Cdelay();
		value<<=1;
	}
	TWI_SCL_0();	I2Cdelay();
	TWI_SCL_1();	I2Cdelay();
	TWI_SCL_0();	I2Cdelay();
}

void I2CWriteData4(BYTE value)
{
	int i;

	for(i=0; i<8; i++){
		TWI_SCL_0();
		I2Cdelay();
		if((value & 0x80) != 0) 
			TWI_SDA_1();
		else 			 
			TWI_SDA_0();
		I2Cdelay();
		TWI_SCL_1();
		I2Cdelay();//vhuang
		TWI_SCL_0();
		I2Cdelay();
		value<<=1;
	}
	//TWI_SCL_0();	I2Cdelay();
	//TWI_SCL_1();	I2Cdelay();
	//TWI_SCL_0();	I2Cdelay();
}

BYTE I2CReadData3(void)
{
	int i;
	BYTE value=0;

	
	for(i=0; i<8; i++) {
		TWI_SCL_0();
		I2Cdelay();
		TWI_SCL_1();
		value <<= 1;
		if(TWI_SDA_STATE()) value |= 1;
		I2Cdelay();
		TWI_SCL_0();
		I2Cdelay();
	}

//	TWI_SCL_0();	I2Cdelay();
//	TWI_SCL_1();	I2Cdelay();
//	TWI_SCL_0();	I2Cdelay();

	return value;

}

BYTE ReadI2C(BYTE addr, BYTE index)
{
	BYTE val;

	I2CStart();
	I2CWriteData(addr);
	I2CWriteData(index);
	I2CReStart();
	I2CWriteData(addr | 0x01);
	val = I2CReadData();
	I2CStop();

	return val;
}

void WriteI2C(BYTE addr, BYTE index, BYTE val)
{
	I2CStart();
	I2CWriteData(addr);
	I2CWriteData(index);
	I2CWriteData(val);
	I2CStop();
}


static void I2CACK(void)
{
    TWI_SDA_0();
   I2Cdelay();/// TWI_NOP();
    TWI_SCL_1();
   I2Cdelay();/// TWI_NOP();
    TWI_SCL_0();
   I2Cdelay();/// TWI_NOP();
    TWI_SDA_1();
	  I2Cdelay();/// TWI_NOP();
}

static void I2CNACK(void)
{
    TWI_SDA_1();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    TWI_SCL_0();
    TWI_NOP();
    TWI_SDA_1();
}

WORD ReadI2C3(BYTE addr, BYTE index)
{
	WORD val;

	I2CStart();
	I2CWriteData3(addr);
//	I2CACK();
	I2CWriteData3(index);
//	I2CACK();
	I2CReStart();
	I2CWriteData3(addr | 0x01);	
//	I2CACK();
	val = I2CReadData3();
	I2CACK();
	val<<=8;
	val |= I2CReadData3();
	I2CNACK();
	I2CStop();

	return val;
}

void WriteI2C3(BYTE addr, BYTE index, WORD val)
{

	I2CStart();
	I2CWriteData3(addr);
	//I2CACK();
	I2CWriteData3(index);
	//I2CACK();
	I2CWriteData3((val>>8));
//	I2CACK();
	I2CWriteData3((val));
	//I2CACK();
	I2CStop();
}

//==========================================================================
/*
void I2C2_Write(uint8_t chipID ,uint8_t reg,uint8_t val )
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

uint8_t I2C2_Read(uint8_t chipID ,uint8_t reg )
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
*/

//==========================================================================
//==========================================================================
//==========================================================================
#if 1
void I2Cdelay2(void)
 {
	int i;
	for (i=0;i<10;i++) {
		};
}


void I2CStart2(void)
{
	TWI_SCL_1();		I2Cdelay2();
	TWI_SDA_1();		I2Cdelay2();
	TWI_SDA_0();		I2Cdelay2();

}

void I2CReStart2(void)
{
	TWI_SDA_1();	I2Cdelay2();
	TWI_SCL_1();	I2Cdelay2();
	TWI_SDA_0();	I2Cdelay2();

}

void I2CStop2(void)
{
	TWI_SDA_0();	I2Cdelay2();
	TWI_SCL_0();	I2Cdelay2();
	TWI_SCL_1();	I2Cdelay2();
	TWI_SDA_1();	I2Cdelay2();
//	I2C_SCL = 1;	I2Cdelay();
//	I2C_SDA = 1;

}

void I2CWriteData2(BYTE value)
{
/*
	int i;

	for(i=0; i<8; i++, value<<=1) {
		I2C_SCL = 0;
		I2Cdelay();	// HHY 2.04 in special case, it was unstable
		if(value & 0x80) I2C_SDA = 1;
		else 			 I2C_SDA = 0;
		I2Cdelay();
		I2C_SCL = 1;
	}
*/

		
#if 1

	int i;

	for(i=0; i<8; i++){
		TWI_SCL_0();
		I2Cdelay2();
		if((value & 0x80) != 0) 
			TWI_SDA_1();
		else 			 
			TWI_SDA_0();
		I2Cdelay2();
		TWI_SCL_1();
		I2Cdelay2();//vhuang
		TWI_SCL_0();
		I2Cdelay2();
		value<<=1;
	}
	TWI_SCL_0();	I2Cdelay2();
	TWI_SCL_1();	I2Cdelay2();
	TWI_SCL_0();	I2Cdelay2();
//}

#else
	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x80)? 1:0;	TWI_SCL_1();	dd();
	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x40)? 1:0;	TWI_SCL_1();	dd();
	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x20)? 1:0;	TWI_SCL_1();	dd();
	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x10)? 1:0;	TWI_SCL_1();	dd();

	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x08)? 1:0;	TWI_SCL_1();	dd();
	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x04)? 1:0;	TWI_SCL_1();	dd();
	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x02)? 1:0;	TWI_SCL_1();	dd();
	TWI_SCL_0();	TWI_SDA_STATE()=(value & 0x01)? 1:0;	TWI_SCL_1();	dd();


	TWI_SCL_0();
	TWI_SCL_0();
	TWI_SCL_0();
	TWI_SDA_1();

	TWI_SCL_1();
	TWI_SCL_1();
	TWI_SCL_1();
	TWI_SCL_0();
#endif
}


BYTE I2CReadData2()
{
	int i;
	BYTE value=0;

	TWI_SDA_1();	dd();
	TWI_SCL_0();
	
	for(i=0; i<8; i++) {
		TWI_SCL_1();
		value <<= 1;
		if(TWI_SDA_STATE()) value |= 1;
		TWI_SCL_0();
	}
	dd();

	TWI_SCL_1();
	TWI_SCL_1();
	TWI_SCL_1();
	TWI_SCL_0();

	return value;

}

BYTE ReadI2C2(BYTE addr, BYTE index)
{
	BYTE val;

	I2CStart2();
	I2CWriteData2(addr);
	I2CWriteData2(index);
	I2CStart2();
	I2CWriteData2(addr | 0x01);
	val = I2CReadData2();
	I2CStop2();

	return val;
}

void EEPWrite(WORD index, BYTE dat)
{
	BYTE addr;

	addr = (0xA0 | (index>>7)) & 0xfe;

	I2CStart2();
	I2CWriteData2(addr);
	I2CWriteData2((BYTE)index);
	I2CWriteData2(dat);
	I2CStop2();

	#ifdef DEBUG_EEP
		dPrintf("Write %02xh to [Adrs(%02xh)Index(%02xh)\r\n", (WORD)dat, (WORD)addr, (WORD)index );
	#endif

	//delay(2);
	msleep(20-10) ;

}

BYTE EEPRead(WORD index)
{
	BYTE val;
	BYTE addr;

	addr = (0xA0 | (index>>7)) & 0xfe;

	val = ReadI2C2(addr,(BYTE)index);

	#ifdef DEBUG_OSD
		dPrintf("Read [Adrs:%02xh,Index:%02xh] %02xh\r\n", (WORD)addr, (WORD)index, (WORD)val);
	#endif
//	delay(2);
	msleep(20-10) ;
	return val;
}
#endif


