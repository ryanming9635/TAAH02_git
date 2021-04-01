
#ifndef __GT911__
#define __GT911__

//I2C write read comm.
#define GT_CMD_WR 		0X28     		//Write command
#define GT_CMD_RD 		0X29			//Read command
  
//GT911 part of the register definition 
#define GT_CTRL_REG 		0X8040   		//GT9147control register
#define GT_CFGS_REG 		0X8047   		//GT9147configuration start address register
#define GT_CHECK_REG 	0X80FF   		//GT9147checksum register
#define GT_PID_REG 		0X8140   		//GT9147product ID register
 
#define GT_GSTID_REG 	0X814E   		//GT9147currently detected touch
#define GT_TP1_REG 		0X8150  		//Data address of the first touch point
#define GT_TP2_REG 		0X8158		//Data address of the second touch point
#define GT_TP3_REG 		0X8160		//Data address of the third touch poin
#define GT_TP4_REG 		0X8168		//The fourth touch point data address
#define GT_TP5_REG 		0X8170		//Data address of the fifth touch point 

#define  GT9147_WR_Reg(x,y,z)	SET_nGT911(x,y,z)
#define  GT9147_RD_Reg(x,y,z) 	GET_nGT911(x,y,z)
#define  delay_ms(x)  			msleep(x) 

  struct TouchInfo
{
   BYTE	sta;
   BYTE	 touchtype;
   BYTE	 touchcount;
   BYTE 	 touchOFF;
   WORD	x[5];
   WORD	y[5];
};

struct PanelInfo
{
   WORD    id;
   WORD	width;
   WORD	height;
};

typedef enum
{
    _TOUCH_COUNT_0=0,
    _TOUCH_COUNT_1,
    _TOUCH_COUNT_2,
    _TOUCH_COUNT_3,
    _TOUCH_COUNT_4,
    _TOUCH_COUNT_5
} EnumTouchPanelConutID;

BYTE GT9147_Send_Cfg(BYTE mode); 
//BYTE GT9147_WR_Reg(u16 reg,u8 *buf,u8 len); 
//void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len);  
BYTE GT9147_Init(void); 
BYTE GT9147_Scan(BYTE mode);  

extern void msleep(unsigned int i );


#endif	// __GT911__

