#ifndef DEF_HDMI_RX_lib_h_
#define DEF_HDMI_RX_lib_h_
#include "VXIS_Extern_Define.h"

extern bit CheckHDMIRX();
extern unsigned char HDMIRX_WriteI2C_Byte(unsigned char RegAddr,unsigned char val) ; 
extern void InitHDMIRX(bit bFullInit) ; 
extern unsigned char HDMIRX_ReadI2C_Byte(unsigned char RegAddr);
extern bit getHDMIRXAudioInfo(unsigned char *pbAudioSampleFreq, unsigned char *pbValidCh) ; 
extern unsigned char GetHDMI_InputMode(unsigned char ch ) ; 
extern void GetHDMIstate(void) ; 
extern void HDMI_VideoModeChangeDetect(void) ; 

#endif 
