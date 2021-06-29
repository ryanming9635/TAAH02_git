/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f103xb.h"
#include "VXIS_Extern_Define.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/*
#define	MCU_FW_Major0			0	
#define	MCU_FW_Major1			0	
#define	MCU_FW_Sub0			0		
#define	MCU_FW_Sub1			6	
*/
#define	FWVER				0x100B

#define	_NTSC			1
#define	_PAL			2

#define MS_LOOP                 100

///#define _SUPPORT_EDID_RAM_		TRUE

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void main_init (void);
void uComOnChipInitial(void);
void LCDPowerON(void);
void LCDPowerOFF(void);
void user_pwm_setvalue(uint16_t value);
void main_loop(void);

extern BYTE GET_PCA9554PW(BYTE reg);
extern void  SET_PCA9554PW(BYTE addr, BYTE val);
void LoadEEPROM (void);
void VS4210_FULL88init2( tcByte *Valn );

BYTE RS_ready(void);
BYTE RS2_ready(void);

extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

extern void VXOSD_UseFont_init(uint8_t Size,const uint8_t *UserDefineFontPtr); //20200110-01
extern void PowerLED(EnumSystemTimerEventID flag);
extern void MCUTimerDelayXms(WORD usNum);
extern BYTE CheckKeyIn(void);
extern void TriggerHandler(BYTE T_Flag);
extern BYTE CheckTrigger(void);
extern BYTE GetAudioVolEE(void);
extern void LoadVideoData(void);
extern void VS4210_SetCamMirror(tByte ch, tByte win );
extern void AUDIO_SELECT(BYTE sel);
extern void MCUTimerCancelTimerEvent(BYTE ucEventID);
extern void SET_VX8812_SD_Video_Out(tByte tbMode);
extern void SW_I2C3_Init(void);
extern BYTE GT9147_Scan(BYTE mode);  
extern BYTE GT9147_Init(void);
extern void IT6602_fsm_init(void);
extern void IT6602_fsm(void);

extern  void VXIS_fSetOsdx2(bit OnOff);
extern void TOAReceivePaser(void);
extern void SETIT66021EDID(void);
extern void Get_2824cIrq(void);//cocrx

void HS_Select_Win(unsigned char WinNum); //20201214-01
void SET_Audio_Mute(unsigned char mode );

void TP_bt1120_720P_1080P_mode0_w0(void);
void TP_bt1120_1080P_1080P_mode0_w0(void);
void TP_bt1120_NTSC_1080P_mode0_w0(void);
void TP_bt1120_PAL_1080P_mode0_w0(void);
void SET_VS4210_720_480_out(void);
void SET_VS4210_1280_720_out(void);
void SET_AHD_MDOE(void);
BYTE CHECK_HDMI720P50(void);

//extern uint8_t ReadEEP( uint16_t reg);
//extern void WriteEEP (uint16_t reg, uint8_t val);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define _spc     0x6d
/*
#define VS4210_RST_Pin GPIO_PIN_2
#define VS4210_RST_GPIO_Port GPIOA
*/
#define IR_In_Pin GPIO_PIN_4
#define IR_In_GPIO_Port GPIOA
//#define IR_In_EXTI_IRQn 	EXTI4_15_IRQn
#define A_AUDIO_Pin GPIO_PIN_5
#define A_AUDIO_GPIO_Port GPIOA
#define B_AUDIO_Pin GPIO_PIN_6
#define B_AUDIO_GPIO_Port GPIOA
/*
#define SW3_Pin GPIO_PIN_12
#define SW3_GPIO_Port GPIOB
*/
#define SW2_Pin GPIO_PIN_13
#define SW2_GPIO_Port GPIOB
/*
#define SW1_Pin GPIO_PIN_14
#define SW1_GPIO_Port GPIOB
*/
/*
#define TX_RST_Pin GPIO_PIN_11
#define TX_RST_GPIO_Port GPIOA
*/

#define H4_AUDIO_Pin GPIO_PIN_15
#define H4_AUDIO_GPIO_Port GPIOA
#define H3_AUDIO_Pin GPIO_PIN_3
#define H3_AUDIO_GPIO_Port GPIOB
#define H2_AUDIO_Pin GPIO_PIN_4
#define H2_AUDIO_GPIO_Port GPIOB
#define H1_AUDIO_Pin GPIO_PIN_5
#define H1_AUDIO_GPIO_Port GPIOB




#define TX_RST_Pin 			GPIO_PIN_0
#define TX_RST_GPIO_Port 		GPIOA

#define TP2824_RST_Pin 		GPIO_PIN_1
#define TP2824_RST_GPIO_Port GPIOA
/*
#define IR_IN_Pin 				GPIO_PIN_2
#define IR_IN_GPIO_Port 		GPIOA
#define IR_IN_EXTI_IRQn 		EXTI2_IRQn
*/
#define VS_RST_Pin 			GPIO_PIN_12
#define VS_RST_GPIO_Port 	GPIOB

#define STM_SCL_Pin 			GPIO_PIN_10
#define STM_SCL_GPIO_Port 	GPIOB

#define STM_SDA_Pin 			GPIO_PIN_11
#define STM_SDA_GPIO_Port 	GPIOB

#define RX1_Pin 				GPIO_PIN_0
#define RX1_GPIO_Port 		GPIOB

#define SW1_Pin 				GPIO_PIN_12
#define SW1_GPIO_Port 		GPIOB

#define SW2_Pin 				GPIO_PIN_13
#define SW2_GPIO_Port 		GPIOB

#define SW3_Pin 				GPIO_PIN_14
#define SW3_GPIO_Port 		GPIOB

#define SW4_Pin 				GPIO_PIN_15
#define SW4_GPIO_Port 		GPIOB

#define SW5_Pin 				GPIO_PIN_3
#define SW5_GPIO_Port 		GPIOB

#define SW6_Pin 				GPIO_PIN_4
#define SW6_GPIO_Port 		GPIOB

#define SW7_Pin 				GPIO_PIN_5
#define SW7_GPIO_Port 		GPIOB

//LED
#define GREEN_LED_Pin 		GPIO_PIN_9
#define GREEN_LED_GPIO_Port 	GPIOC

#define RED_LED_Pin			GPIO_PIN_8
#define RED_LED_GPIO_Port	GPIOC


//KEY
#define SOURCE_KEY_Pin			GPIO_PIN_7
#define SOURCE_KEY_GPIO_Port	GPIOC

#define JUMP_KEY_Pin			GPIO_PIN_6
#define JUMP_KEY_GPIO_Port	GPIOC

#define DOWN_KEY_Pin			GPIO_PIN_15
#define DOWN_KEY_GPIO_Port		GPIOB

#define UP_KEY_Pin			GPIO_PIN_14
#define UP_KEY_GPIO_Port		GPIOB

#define MENU_KEY_Pin			GPIO_PIN_13
#define MENU_KEY_GPIO_Port	GPIOB

#define POWER_KEY_Pin			GPIO_PIN_12
#define POWER_KEY_GPIO_Port		GPIOB

//Audio sel
#define AUD_SEL1_Pin			GPIO_PIN_2
#define AUD_SEL1_GPIO_Port		GPIOD

#define AUD_SEL2_Pin			GPIO_PIN_12
#define AUD_SEL2_GPIO_Port		GPIOC

#define AUD_I2C_MUTE_Pin			GPIO_PIN_3
#define AUD_I2C_MUTE_Port		GPIOC

//Panel
#define BL_ENABLE_Pin		GPIO_PIN_8
#define BL_ENABLE_GPIO_Port	GPIOA

#define PA7_Pin			GPIO_PIN_7
#define PA7_GPIO_Port	GPIOA

#define PANEL_EN_Pin			GPIO_PIN_11
#define PANEL_EN_GPIO_Port	GPIOB

#define BL_PWM_Pin			GPIO_PIN_10
#define BL_PWM_GPIO_Port	GPIOB

#define LCD_PWR_Pin			GPIO_PIN_2
#define LCD_PWR_GPIO_Port	GPIOC

#define TTLI1_RES_Pin			GPIO_PIN_13
#define TTLI1_RES_GPIO_Port	GPIOC

//Power 
#define PCON3V3_TP_Pin			GPIO_PIN_6
#define PCON3V3_TP_GPIO_Port	GPIOA

#define PCON1V2_TP_Pin			GPIO_PIN_5
#define PCON1V2_TP_GPIO_Port	GPIOA

//VS4210
#define VS4210_RST_Pin			GPIO_PIN_4
#define VS4210_RST_GPIO_Port	GPIOA

//light sensor
#define DIMMER_Pin			GPIO_PIN_3
#define DIMMER_GPIO_Port		GPIOC

//#define EXTERNOSDNUMBER (4+23+1)

//STM i2c
/*
#define STM_SDA_Pin			GPIO_PIN_10//GPIO_PIN_10
#define STM_SDA_Port			GPIOC

#define STM_SCL_Pin			GPIO_PIN_11
#define STM_SCL_Port			GPIOC
*/
//touch panel
#define TOUCH_INT			GPIO_PIN_1
#define TOUCH_INT_Port		GPIOB
#define TOUCH_RESET			GPIO_PIN_0
#define TOUCH_RESET_Port		GPIOB



#if 1
//Panel
#define BL_ENABLE(x)      	HAL_GPIO_WritePin(GPIOA, BL_ENABLE_Pin , (x))
#define PANEL_EN(x)      	HAL_GPIO_WritePin(GPIOB, PANEL_EN_Pin , (x^1))
#define LCD_PWR(x)      	HAL_GPIO_WritePin(GPIOC, LCD_PWR_Pin , (x^1))
#define TTLI1_RES(x)      	HAL_GPIO_WritePin(GPIOC, TTLI1_RES_Pin , (x^1))
#define BL_PWM(x)      		HAL_GPIO_WritePin(BL_PWM_GPIO_Port, BL_PWM_Pin , (x))

//power
#define PCON3V3_TP(x)      	HAL_GPIO_WritePin(GPIOA, PCON3V3_TP_Pin , (x^1))
#define PCON1V2_TP(x)      	HAL_GPIO_WritePin(GPIOA, PCON1V2_TP_Pin , (x^1))

//VS4210
#define VS4210_RST(x)      	HAL_GPIO_WritePin(GPIOA, VS4210_RST_Pin , (x^1))

//Audio sel
#define AUD_SEL1(x)      	HAL_GPIO_WritePin(GPIOD, AUD_SEL1_Pin , (x))
#define AUD_SEL2(x)      	HAL_GPIO_WritePin(GPIOC, AUD_SEL2_Pin , (x))
#define AUDIO_MUTE(x)      	HAL_GPIO_WritePin(GPIOC, AUD_SEL2_Pin , (x))
#define AUD_SET_MUTE(x)      	HAL_GPIO_WritePin(AUD_I2C_MUTE_Port, AUD_I2C_MUTE_Pin , (x))

//#define _ReadKey()  		( ((~P4>>2)& 0x01) | (~P1 & 0xfe) )
#define _ReadKey()  		( ((~GPIOC->IDR)& (GPIO_PIN_7|GPIO_PIN_6)) | (((~GPIOB->IDR) & (GPIO_PIN_13|GPIO_PIN_12))>>8) )

//touch panel configure
#define SET_TOUCH_RESET(x)     	HAL_GPIO_WritePin(GPIOB, TOUCH_RESET , (x))


//pwm max
#define _PWMLevelMax 	4000
#endif

//IT66021
#define STM_SCL(x)    	 	HAL_GPIO_WritePin(GPIOB, STM_SCL_Pin , (x))
#define STM_SDA(x)      	HAL_GPIO_WritePin(GPIOB, STM_SDA_Pin , (x))
#define STM_GET_SDA()	HAL_GPIO_ReadPin(GPIOB, STM_SDA_Pin ) 

//TOA Pin Assignment
#define I2S_MUTE					GPIO_PIN_3
#define I2S_MUTE_PORT			GPIOC

#define VIDEO_LOSS				GPIO_PIN_5
#define VIDEO_LOSS_PORT			GPIOA

#define HDMI_SELECT				GPIO_PIN_4
#define HDMI_SELECT_PORT		GPIOA

#define TP2824_IRQ				GPIO_PIN_0
#define TP2824_IRQ_PORT			GPIOC
#define TP2824_REST				GPIO_PIN_1
#define TP2824_REST_PORT		GPIOC

#define MCU_WAKEUP				GPIO_PIN_6
#define MCU_WAKEUP_PORT		GPIOC

#define HDMI_MUTE(x)      			HAL_GPIO_WritePin(I2S_MUTE_PORT, I2S_MUTE , (x))
#define SET_VIDEO_VLOSS(x)    		HAL_GPIO_WritePin(VIDEO_LOSS_PORT, VIDEO_LOSS , (x))
#define TP2824_GET_IRQ()      		HAL_GPIO_ReadPin(TP2824_IRQ_PORT, TP2824_IRQ )
#define TP2824_SET_REST(x)      	HAL_GPIO_WritePin(TP2824_REST_PORT, TP2824_REST , (x))

#define HDMI_SELECT_GET()      		HAL_GPIO_ReadPin(HDMI_SELECT_PORT, HDMI_SELECT )
#define MCU_GET_WAKEUP()		HAL_GPIO_ReadPin(MCU_WAKEUP_PORT, MCU_WAKEUP ) 


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
