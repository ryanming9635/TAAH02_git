#ifndef _VX_OSD_Simple_H
#define _VX_OSD_Simple_H
#include "VXIS_Extern_Define.h"

//-----------------------------------------------------------------------------
// extern Function Prototypes
//-----------------------------------------------------------------------------
extern void VXIS_fSetOsdx2(bit OnOff);
extern void VXIS_fSetOsdOnOff(tByte ch, tByte block) ;
extern void VXIS_fShowInOutMode(tByte ch , tByte in_mode, tByte out_mode) ;
extern void VXIS_fShowInOutMode0Fast(tByte ch , tByte in_mode, tByte out_mode ) ;
extern void OSD_Clear(tByte Win ) ; 
extern void OSD_Clear_34(void ) ;
extern void OSD_Clear_234(void) ;
extern void OSD_Clear_4(void ) ; 
extern void OSD_Clear_All(void ); 
//extern void ClearAudioOSD(void) ; 
//extern void Clear1ChAudioOSD(tByte ch ) ;

extern void VXOSD_UseFont_init(tByte Size, tcByte *UserDefineFontPtr) ;
extern void VXOSD_ShowExContent(tByte *ptrExOsdContent, tByte index ) ;
extern void VXOSD_SelectExContent(tByte index ) ;
void Set_Video_Loss_OSD(BYTE ch, BYTE set);	

#ifdef OSD_CHANNEL_ALWAYS_ON
    extern void OSD_ShowTitleOnly(void ) ; 
#endif 


#endif  
