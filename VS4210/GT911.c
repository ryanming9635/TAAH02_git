/*
 #include "gt9147.h"
#include "touch.h"
#include "ctiic.h"
#include "usart.h"
#include "delay.h" 
#include "string.h" 
#include "lcd.h" 
*/
#include "VXIS_Extern_Define.h"
#include "GT911.h"
#include "main.h"
#include "VX_OSD_Simple.h"
#include "VX_swi2c.h"

#include "VS4210_param_Mode88.h"
#include "VS4210_param_Modex26.h"

#include "keyremo.h"
#include "tpinc/TP2802.h"
#include "Printf.h"

//IO function	 
#define GT_RST(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET))//GT9147reset pin
#define GT_SET_INT(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET))//GT9147reset pin
#define GT_INT   		 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)  //GT9147 interrupt pin		
 

//GT9147 configuration parameter table
//The first byte is the version number (0X60), you must ensure that the new version number is greater than or equal to GT9147 internal
//The original version number of flash will update the configuration.
const BYTE GT9147_CFG_TBL[]=
{ 
#if 1
0x00,0XE0,0X01,0X20,0X03/*0x00,0x04,0x58,0x02*/,0x0A,0x05/*0x0D*/,0x00,0x01,0x08,
0x28,0x05,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8C,0x2E,0x0E,
0x17,0x15,0x31,0x0D,0x00,0x00,0x01,0xBA,0x04,0x1D,
0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,
0x00,0x0F,0x23,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
0xA1,0x10,0x00,0x8B,0x13,0x00,0x7C,0x16,0x00,0x6A,
0x1B,0x00,0x5E,0x20,0x00,0x5E,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
0x12,0x14,0x16,0x18,0x1A,0x1C,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,
0x10,0x12,0x13,0x14,0x16,0x18,0x1C,0x1D,0x1E,0x1F, 
0x20,0x21,0x22,0x24,0x26,0x28,0x29,0x2A,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00/*,0x2A,0x01*/

/*
0x46,0xE0,0x01,0x56,0x03,0x02,0xF1,0x01,0x02,0x44,\
0x00,0x04,0x46,0x32,0x03,0x00,0x00,0x00,0x00,0x00,\
0x00,0x11,0x04,0x26,0x01,0x74,0x77,0x05,0x00,0x88,\
0x64,0x0F,0xD0,0x07,0x05,0x07,0x00,0xDA,0x01,0x1D,\
0x00,0x01,0x08,0x08,0x33,0x33,0x5D,0xAA,0x00,0x00,\
0x00,0x32,0x96,0x54,0xC5,0x03,0x02,0x00,0x00,0x01,\
0xC8,0x38,0x00,0xA0,0x45,0x00,0x91,0x57,0x00,0x80,\
0x6C,0x00,0x61,0x87,0x00,0x61,0x10,0x0B,0x08,0x00,\
0x51,0x40,0x30,0xFF,0xFF,0x00,0x04,0x00,0x00,0x1E,\
0x0A,0x00,0x06,0x0B,0x09,0x0F,0x08,0x07,0x01,0x03,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x03,0x02,0x05,0x04,0x07,0x06,0x09,\
0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,\
0x00,0x00,0x08,0x09,0x0A,0x0D,0x0E,0xFF,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0xFF,0x0B,0x0C,0xFF,0xFF,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0x6C,0xB2,0xB2,0x6C,0xFF,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,\
0x8C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0xF6,0x01\
*/
#else
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
	#endif
};  


struct TouchInfo tp_dev={0}; 
struct PanelInfo lcddev={0X5510,800,480};
//struct PanelInfo lcddev={0X5510,1024,600};

BYTE TP_PRES_DOWN=0,TP_CATH_PRES=0;
BYTE TP_ACTION=ON;
extern BYTE PowerFlag;
extern  BYTE TouchP;

//Send GT9147 configuration parameters
//mode: 0, the parameter is not saved to flash
//      1, save parameters to flash
BYTE GT9147_Send_Cfg(BYTE mode)
{
	BYTE buf[2];
	BYTE i=0;
	buf[0]=0;
	buf[1]=mode;	//Whether to write to GT9147 FLASH? That is, whether to save after power off

	
	printf("(GT9147_Send_Cfg)");

	
	for(i=0;i<sizeof(GT9147_CFG_TBL);i++)
		buf[0]+=GT9147_CFG_TBL[i];//Calculate checksum
		
    buf[0]=(~buf[0])+1;
	GT9147_WR_Reg(GT_CFGS_REG,(BYTE*)GT9147_CFG_TBL,sizeof(GT9147_CFG_TBL));//Transmit register configuration
	GT9147_WR_Reg(GT_CHECK_REG,buf,2);//Write checksum, and configuration update flag
	return 0;
} 
//Write data to GT9147
//reg: start register address
//buf: data buffer
//len: write data length
//Return value: 0, success; 1, failure.
#if 0
BYTE GT9147_WR_Reg(WORD reg,BYTE *buf,BYTE len)
{
/*
	BYTE i;
	BYTE ret=0;
	CT_IIC_Start();	
 	CT_IIC_Send_Byte(GT_CMD_WR);    //Send write	 
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg>>8);   //Send the upper 8 bits of address
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//Send the lower 8 bits of address
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//Send data
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();				//Generate a stop condition  
    */
	return ret; 
}
//Read data once from GT9147
//reg: start register address
//buf: data buffer
//len: read data length		  
void GT9147_RD_Reg(WORD reg,BYTE *buf,BYTE len)
{
/*
	BYTE i; 
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(GT_CMD_WR);   //Send write command 	 
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg>>8);   	//Send the upper 8 bits of address
	CT_IIC_Wait_Ack(); 	 										  		   
 	CT_IIC_Send_Byte(reg&0XFF);   //Send the lower 8 bits of address
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(GT_CMD_RD);    //Send read command	   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //Send data	  
	} 
    CT_IIC_Stop();//Generate a stop condition   
    */
} 
#endif
//Initialize the GT9147 touch screen
//Return value: 0, initialization successful; 1, initialization failure
BYTE GT9147_Init(void)
{

		#if(_DEBUG_MESSAGE_GT911==ON)
		GraphicsPrint(GREEN,"\r\n(GT911_Init...)");			
		#endif


	BYTE temp[5]; 

    GPIO_InitTypeDef GPIO_Initure;

   GPIO_Initure.Pin =TOUCH_INT;
   GPIO_Initure.Mode = GPIO_MODE_INPUT;
   GPIO_Initure.Pull = GPIO_PULLUP;
   HAL_GPIO_Init(GPIOB, &GPIO_Initure);
   
    //__HAL_RCC_GPIOH_CLK_ENABLE();			 //Turn on the GPIOH clock
  //  __HAL_RCC_GPIOI_CLK_ENABLE();			 //Turn on the GPIOH clock
    /*            
    //PH7
    GPIO_Initure.Pin=TOUCH_INT;            //PH7
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //Input
    GPIO_Initure.Pull=GPIO_PULLUP;          //Pull up
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;      //High speed
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);     //Initialize
*/
        /*    
    //PI8
    GPIO_Initure.Pin=GPIO_PIN_8;            //PI8
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //Push-pull output
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //Initialize
  	CT_IIC_Init();       //Initialize the I2C bus of the capacitive screen
  	
  	  */
	GT_RST(0);				//Reset
	GT_SET_INT(0);
	delay_ms(10);
	GT_SET_INT(1);
	delay_ms(1);
 	GT_RST(1);				 //Release reset		    
	delay_ms(10);

   GPIO_Initure.Pin =TOUCH_INT;
   GPIO_Initure.Mode = GPIO_MODE_INPUT;
   GPIO_Initure.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOB, &GPIO_Initure);	
/*
    GPIO_Initure.Pin=TOUCH_INT;            //PH7
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //Input
    GPIO_Initure.Pull=GPIO_NOPULL;          //Without up and down, floating input
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;      //High speed
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);      //Initialize
*/

	delay_ms(100);  
	GT9147_RD_Reg(GT_PID_REG,temp,4);//Read product ID
	
	temp[4-1]=0;
printf("\r\nTouch Panel ID:%s",temp);	//Print ID

printf("\r\nREG ID=%02x%02x%02x",temp[0],temp[1],temp[2]);	//Print ID
	
//f(strcmp((char*)temp,"9147")==0)//ID==9147

	if(strcmp((char*)temp,"911")==0)//ID==9147
	{
		temp[0]=0X02;			
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);////Soft reset GT9147
		GT9147_RD_Reg(GT_CFGS_REG,temp,1);//Read GT_CFGS_REG register
		#if 1
	  		GT9147_Send_Cfg(1);//Update and save the configuration
		#else
		if(temp[0]<0X60)//The default version is relatively low, you need to update the flash configuration
		{
			printf("  Default Ver:%d\r\n",temp[0]);

			if(lcddev.id==0X5510)
	  		GT9147_Send_Cfg(1);//Update and save the configuration
		} 
		#endif
		delay_ms(10);
		temp[0]=0X00;	 
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);//End reset   
		return 0;
	} 

	return 1;
}
const WORD GT9147_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
//Scan the touch screen (using query method)
//mode:0, normal scanning.
//Return value: current touch screen status.
//0, no touch on the touch screen; 1, touch on the touch screen
BYTE GT9147_Scan(BYTE mode)
{
	BYTE buf[4+2];
	BYTE i=0;
	BYTE res=_TOUCH_COUNT_0;
	BYTE temp;
	BYTE tempsta;
 	static BYTE t=0;//Control the query interval to reduce CPU usage   
	t++;
	if((t%10)==0||t<10)//When idle, the CTP_Scan function is only detected once every 10 times, thus saving CPU usage
	{
		GT9147_RD_Reg(GT_GSTID_REG,&mode,1);	//Read the state of the touch point  
		
 		if((mode&0X80)&&((mode&0XF)<6))
		{
			temp=0;
			GT9147_WR_Reg(GT_GSTID_REG,&temp,1);//Clear the flag	


				TP_PRES_DOWN=(mode&0x0f);
			#if(_DEBUG_MESSAGE_GT911==ON)
			GraphicsPrint(CYAN,"(TP_PRES_DOWN=%02x)",(WORD)TP_PRES_DOWN); 		
			#endif				
		}
		else
			TP_PRES_DOWN=0;

		
		if((mode&0XF)&&((mode&0XF)<6)&&(TP_ACTION==ON))
		{
			#if(_DEBUG_MESSAGE_GT911==ON)
		GraphicsPrint(MAGENTA,"\r\n");			
			#endif
			
			temp=0XFF<<(mode&0XF);		//Convert the number of points to 1 digit, matching the definition of tp_dev.sta
			tempsta=tp_dev.sta;			//Save current tp_dev.sta value
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			tp_dev.x[4]=tp_dev.x[0];	//Save contact 0 data
			tp_dev.y[4]=tp_dev.y[0];

			#if(_DEBUG_MESSAGE_GT911==ON)
			GraphicsPrint(GREEN,"(tp_dev.sta=%02x)",(WORD)tp_dev.sta); 		
			#endif

			tp_dev.touchcount=0;
			
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//Touch valid?
				{
					GT9147_RD_Reg(GT9147_TPX_TBL[i],buf,4+2);	//Read XY coordinate value

				//	if(PowerFlag==OFF)
				//		PowerFlag=ON;
	//	#if(_DEBUG_MESSAGE_GT911==ON)
	//	GraphicsPrint(MAGENTA,"(T=%02x",(WORD)i);			
	//	GraphicsPrint(YELLOW," x=%d y=%d s=%d)",((buf[1]<<8)|buf[0]),((buf[3]<<8)|buf[2]),((buf[5]<<8)|buf[4]));			
	//	#endif


                    if(lcddev.id==0X5510)   //4.3 inch 800*480 MCU screen
                    {
                    tp_dev.touchtype=1;
					
                        if(tp_dev.touchtype&0X01)//Landscape
                        {
                            tp_dev.y[i]=((WORD)buf[1]<<8)+buf[0];
        //                    tp_dev.x[i]=800-(((WORD)buf[3]<<8)+buf[2]);
  				tp_dev.x[i]=(((WORD)buf[3]<<8)+buf[2]);
                        }else
                        {
                            tp_dev.x[i]=((WORD)buf[1]<<8)+buf[0];
                            tp_dev.y[i]=((WORD)buf[3]<<8)+buf[2];
                        }  
                    }else if(lcddev.id==0X4342) //4.3 inch 480*272 RGB screen
                    {
                        if(tp_dev.touchtype&0X01)//Landscape
                        {
                            tp_dev.x[i]=(((WORD)buf[1]<<8)+buf[0]);
                            tp_dev.y[i]=(((WORD)buf[3]<<8)+buf[2]);
                        }else
                        {
                            tp_dev.y[i]=((WORD)buf[1]<<8)+buf[0];
                            tp_dev.x[i]=272-(((WORD)buf[3]<<8)+buf[2]);  
                        }
                    }

		/*
		#if(_DEBUG_MESSAGE_GT911==ON)
		GraphicsPrint(MAGENTA,"(T=%02x",(WORD)i);			
		GraphicsPrint(YELLOW," x=%d y=%d)", tp_dev.x[i],tp_dev.y[i]);			
		#endif	
		*/
		tp_dev.touchcount++;
				}	

				
			}  
			
			if(tp_dev.touchcount>=_TOUCH_COUNT_4)
			tp_dev.touchOFF=ON;
				
			res=0;//tp_dev.touchcount;
			
			if(tp_dev.x[0]>lcddev.width||tp_dev.y[0]>lcddev.height)//Illegal data (coordinates exceeded)
			{ 
				if((mode&0XF)>1)		//If there are other points with data, the data of the second contact will be copied to the first contact.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//Trigger once, it will be monitored at least 10 times continuously, so as to improve the hit rate

					#if(_DEBUG_MESSAGE_GT911==ON)
					GraphicsPrint(RED,"(TP1)");			
					#endif	
				}else				//Illegal data, then ignore this data (restore the original)
				{
					tp_dev.x[0]=tp_dev.x[4];
					tp_dev.y[0]=tp_dev.y[4];
					mode=0X80;		
					tp_dev.sta=tempsta;	 //Restore tp_dev.sta

				#if(_DEBUG_MESSAGE_GT911==ON)
				GraphicsPrint(RED,"(TP2)");			
				#endif	

				}
			}else t=0;	
			//Trigger once, it will be monitored at least 10 times continuously, thus improving the hit rate
		}
		else 	if(((mode&0X8F)==0X80)&&(TP_ACTION==ON))//Press without touch point
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//It was pressed before
		{
			tp_dev.sta&=~(1<<7);	 //Mark button release
				#if(_DEBUG_MESSAGE_GT911==ON)
				GraphicsPrint(RED,"(TP3)");			
				#endif	
		}else					//have not been pressed before
		{ 

			#if(_DEBUG_MESSAGE_GT911==ON)
			GraphicsPrint(GREEN,"\r\n(TouchRelease)"); 		
			GraphicsPrint(RED,"\r\n(tp_dev.sta=%02x)",(WORD)tp_dev.sta); 		
			GraphicsPrint(RED,"\r\n(tp_dev.touchcount=%02x)",(WORD)tp_dev.touchcount); 		
			if(tp_dev.sta!=0)
				{
					for(i=0;i<=(tp_dev.touchcount-1);i++)
					{
					GraphicsPrint(MAGENTA,"(T=%02x",(WORD)i);
					GraphicsPrint(YELLOW," x=%d y=%d)", tp_dev.x[i],tp_dev.y[i]);	
					}
				}
			#endif	

				tp_dev.x[0]=0xffff;
				tp_dev.y[0]=0xffff;
				tp_dev.sta&=0XE0;	 //Clear point valid mark
				TP_ACTION=OFF;
				
					if(tp_dev.touchOFF==ON)
						{
							if(PowerFlag==ON)
							{
							PowerFlag=OFF;
							WriteEEP(0xab,PowerFlag);	
							res=_TOUCH_COUNT_4;
							}
							
						tp_dev.touchOFF=OFF;
						}
					else
						res=tp_dev.touchcount;

						MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_RESET_TOUCH_PANEL);
						MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_RESET_TOUCH_PANEL);

		}	 
	}
			
	}
	#if 0
	if((mode&0X8F)==0X80)//Press without touch point
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//It was pressed before
		{
			tp_dev.sta&=~(1<<7);	 //Mark button release
				#if(_DEBUG_MESSAGE_GT911==ON)
				GraphicsPrint(RED,"(TP3)");			
				#endif	
		}else					//have not been pressed before
		{ 

			#if(_DEBUG_MESSAGE_GT911==ON)
			GraphicsPrint(GREEN,"\r\n(TouchRelease)"); 		
			GraphicsPrint(RED,"\r\n(tp_dev.sta=%02x)",(WORD)tp_dev.sta); 		
			GraphicsPrint(RED,"\r\n(tp_dev.touchcount=%02x)",(WORD)tp_dev.touchcount); 		
				for(i=0;i<=(tp_dev.touchcount-1);i++)
				{
				GraphicsPrint(MAGENTA,"(T=%02x",(WORD)i);
				GraphicsPrint(YELLOW," x=%d y=%d)", tp_dev.x[i],tp_dev.y[i]);	
				}
			#endif	

				tp_dev.x[0]=0xffff;
				tp_dev.y[0]=0xffff;
				tp_dev.sta&=0XE0;	 //Clear point valid mark
				//TP_ACTION=ON;
				MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_RESET_TOUCH_PANEL);

		}	 
	} 	
	#endif
	if(t>240)t=10;//Start counting from 10 again
	
	return res;
}





