/** @file VX_OSD_Simple.c
  * @brief OSD subtitle display program
  *
  * Message is displayed on the screen.
  *
  * @author Cunu-Yu
  * @date 2018/06/20
  */

#include <string.h>
#include "VXIS_Extern_Define.h"
#include "VX_OSD.h"
#include "VX_swi2c.h"

#include "main.h"
#include "Printf.h"

tByte MenuOSDonFlg ;
tByte bVloss_Flag=0;

extern  tByte u8gDrawOSDFlg ;
extern  tByte gbVXIS_OuputSize  ;
extern  tByte gbVXIS_OuputModeflg ;
extern  tByte gbVXIS_InputMode[4]  ;
extern  tByte gaHDMI_MapWindowIndex[4] ;
extern  tByte gu8AudioMode  ;
extern BYTE SelectInputMode;///0=HDMI , 1=AHD
extern tByte bSignal[4];

tByte OSDx2Flg ;
#define _TransParentColor  8
#define _Color 0x07
#define _MenuColor 0x17 // _Blue (bg) + _White
#define _MenuSelColor ((_RoyalBlue & 0x07) << 4 ) | _Orange

tcByte *ptcOSDbar ;    
tcByte OSD_720x480I60_param[]=  {10 ,_7,_2,_0,_X,_4,_8,_0,_I,_spc,_spc};
tcByte OSD_720x480P60_param[]=  {10 ,_7,_2,_0,_X,_4,_8,_0,_P,_spc,_spc};
tcByte OSD_720x576I50_param[]=  {10 ,_7,_2,_0,_X,_5,_7,_6,_I,_spc,_spc};
tcByte OSD_720x576P50_param[]=  {10 ,_7,_2,_0,_X,_5,_7,_6,_P,_spc,_spc};
tcByte OSD_1280x720P_param[] =  {10 ,_1,_2,_8,_0,_X,_7,_2,_0,_P  ,_spc};
tcByte OSD_1920x1080P_param[]=  {10 ,_1,_9,_2,_0,_X,_1,_0,_8,_0  ,_P  };
tcByte OSD_1920x1080I_param[]=  {10 ,_1,_9,_2,_0,_X,_1,_0,_8,_0  ,_I  };
//DVI
tcByte OSD_640X480P_param[]=   {10 ,_6,_4,_0,_X,_4,_8,_0,_P,_spc,_spc};
tcByte OSD_800X600P_param[]=  {10 ,_8,_0,_0,_X,_6,_0,_0,_P,_spc,_spc};
tcByte OSD_1024X768P_param[]=  {10 ,_1,_0,_2,_4,_X,_7,_6,_8,_P  ,_spc};
tcByte OSD_1280X768P_param[]=  {10 ,_1,_2,_8,_0,_X,_7,_6,_8,_P  ,_spc};
tcByte OSD_1360X768P_param[]=  {10 ,_1,_3,_6,_0,_X,_7,_6,_8,_P  ,_spc};
tcByte OSD_1366X768P_param[]=  {10 ,_1,_3,_6,_6,_X,_7,_6,_8,_P  ,_spc};
tcByte OSD_1280X800P_param[]=  {10 ,_1,_2,_8,_0,_X,_8,_0,_0,_P  ,_spc};
tcByte OSD_1440X900P_param[]=  {10 ,_1,_4,_4,_0,_X,_9,_0,_0,_P  ,_spc};
tcByte OSD_1280X1024P_param[]= {10 ,_1,_2,_8,_0,_X,_1,_0,_2,_4  ,_P  };
tcByte OSD_1400X1050P_param[]= {10 ,_1,_4,_0,_0,_X,_1,_0,_5,_0  ,_P  };
tcByte OSD_1680X1050P_param[]= {10 ,_1,_6,_8,_0,_X,_1,_0,_5,_0  ,_P  };
tcByte OSD_1600X1200P_param[]= {10 ,_1,_6,_0,_0,_X,_1,_2,_0,_0  ,_P  };
tcByte OSD_1920X1200P_param[]= {10 ,_1,_9,_2,_0,_X,_1,_2,_0,_0  ,_P  };

tcByte OSD_No_Signal_param[]=   {10 ,_N,_o,_spc,_S,_i,_g,_n,_a,_l,_spc};

tcByte OSD_CH0_param[]= {6,_C,_H,_spc,_1,_spc,_spc};
tcByte OSD_CH1_param[]= {6,_C,_H,_spc,_2,_spc,_spc};
tcByte OSD_CH2_param[]= {6,_C,_H,_spc,_3,_spc,_spc};
tcByte OSD_CH3_param[]= {6,_C,_H,_spc,_4,_spc,_spc};

//tByte  OSD_CHx_param[7] ;


tcWord Mode0_1080P_param[]   = {180,61} ;
tcWord Mode1_w0_1080P_param[]= {180,61} ;
tcWord Mode1_w1_1080P_param[]= {1140,61} ;
tcWord Mode1_w2_1080P_param[]= {180,603} ;
tcWord Mode1_w3_1080P_param[]= {1140,603} ;
tcWord Mode2_w0_1080P_param[]= {180,61} ;
tcWord Mode2_w1_1080P_param[]= {664,61} ;
tcWord Mode2_w2_1080P_param[]= {664,603} ;
tcWord Mode2_w3_1080P_param[]= {1624,61} ;
tcWord Mode6_w0_1080P_param[]= {180,61} ;
tcWord Mode6_w1_1080P_param[]= {1460,61} ;
tcWord Mode6_w2_1080P_param[]= {1460,422} ;
tcWord Mode6_w3_1080P_param[]= {1460,783} ;
tcWord Mode12_w0_1080P_param[]= {180,61} ;
tcWord Mode12_w1_1080P_param[]= {1140,61} ;

tcWord Mode0_720P_param[]   = {244,37} ;
tcWord Mode1_w0_720P_param[]= {244,37} ;
tcWord Mode1_w1_720P_param[]= {886,37} ;
tcWord Mode1_w2_720P_param[]= {244,401} ;
tcWord Mode1_w3_720P_param[]= {886,401} ;
tcWord Mode2_w0_720P_param[]= {244,37} ;
tcWord Mode2_w1_720P_param[]= {566,37} ;
tcWord Mode2_w2_720P_param[]= {566,401} ;
tcWord Mode2_w3_720P_param[]= {1206,37} ;

tcWord Mode6_w0_720P_param[]= {244,37} ;
tcWord Mode6_w1_720P_param[]= {1098,37} ;
tcWord Mode6_w2_720P_param[]= {1098,280} ;
tcWord Mode6_w3_720P_param[]= {1098,520} ;

tcWord Mode12_w0_720P_param[]= {244,37} ;
tcWord Mode12_w1_720P_param[]= {886,37} ;


tcWord Mode13_w0_1080P_param[]= {180,61} ;
tcWord Mode13_w1_1080P_param[]= {180,603} ;
tcWord Mode24_w0_1080P_param[]= {180,61} ;
tcWord Mode24_w1_1080P_param[]= {1460,781} ;

tcWord Mode13_w0_720P_param[]= {244,37} ;
tcWord Mode13_w1_720P_param[]= {244,401} ;
tcWord Mode24_w0_720P_param[]= {244,37} ;
tcWord Mode24_w1_720P_param[]= {1098,517} ;


//LINE
tcByte aLineModeReg[] = {0xB8,0xBA,0xBB,0xBC,0xBD,0xBE ,0xB9} ;
tcByte aLineMode1080PMode1Lin1[]= {0x01,0x04,0x25,0x5c,0x04,0x51 ,0x83} ;
tcByte aLineMode1080PMode1Lin2[]= {0x02,0x08,0x94,0x13,0x82,0x41 ,0x83} ;
tcByte aLineMode1080PMode2Lin1[]= {0x01,0x04,0x25,0x5C,0x02,0x72 ,0x83} ;
tcByte aLineMode1080PMode2Lin2[]= {0x02,0x04,0x25,0x5C,0x06,0x32 ,0x83} ;
tcByte aLineMode1080PMode2Lin3[]= {0x03,0x26,0x74,0x33,0x82,0x41 ,0x83} ;
tcByte aLineMode1080PMode6Lin1[]= {0x01,0x04,0x25,0x5C,0x05,0x90 ,0x83} ;
tcByte aLineMode1080PMode6Lin2[]= {0x02,0x58,0x92,0x13,0x81,0x8C ,0x83} ;
tcByte aLineMode1080PMode6Lin3[]= {0x03,0x58,0x92,0x13,0x82,0xF5 ,0x83} ;

tcByte aLineMode720PMode1Lin1[]= {0x01,0x02,0x15,0xE4,0x03,0x5A ,0x83} ;
tcByte aLineMode720PMode1Lin2[]= {0x02,0x05,0xDC,0xDB,0x81,0x7D ,0x83} ;
tcByte aLineMode720PMode2Lin1[]= {0x01,0x02,0x15,0xE4,0x02,0x1A ,0x83} ;
tcByte aLineMode720PMode2Lin2[]= {0x02,0x02,0x15,0xE4,0x04,0x9A ,0x83} ;
tcByte aLineMode720PMode2Lin3[]= {0x03,0x24,0x1B,0x9B,0x81,0x7D ,0x83} ;
tcByte aLineMode720PMode6Lin1[]= {0x01,0x02,0x15,0xE4,0x04,0x2E ,0x83} ;
tcByte aLineMode720PMode6Lin2[]= {0x02,0x45,0x30,0xDB,0x81,0x04 ,0x83} ;
tcByte aLineMode720PMode6Lin3[]= {0x03,0x45,0x30,0xDB,0x81,0xF5 ,0x83} ;

tcByte aLineMode1080PMode12Lin1[]= {0x01,0x04,0x25,0x5C,0x04,0x51 ,0x83} ;
tcByte aLineMode720PMode12Lin1[]= {0x01,0x02,0x15,0xE4,0x03,0x5A ,0x83} ;

///////////////////////////////////////////////////////////////////////////////////////
static void VX_Write_OSD_In_OutString(tByte ch , tByte *Str1,tByte *Str2, int out_mode)
{
    tWord ix,iy ;
    tByte TitleFlg ;
    tByte BarFlg ;

    TitleFlg = (u8gDrawOSDFlg & 0x02 ) >> 1 ;
    BarFlg   = u8gDrawOSDFlg & 0x01 ;

    if (out_mode == VS4210_1080P_MODE0_w0 )
    {
        if(gbVXIS_OuputSize == _OUTPUT_1080P60)
        {
            ix = Mode0_1080P_param[0] ;
            iy = Mode0_1080P_param[1] ;
        }
        else
        {
            ix = Mode0_720P_param[0] ;
            iy = Mode0_720P_param[1] ;
        }
    }
    else if (out_mode == VS4210_1080P_MODE0_w1  )
    {
        if(gbVXIS_OuputSize == _OUTPUT_1080P60)
        {
            ix = Mode0_1080P_param[0] ;
            iy = Mode0_1080P_param[1] ;
        }
        else
        {
            ix = Mode0_720P_param[0] ;
            iy = Mode0_720P_param[1] ;
        }
    }
    else if (out_mode == VS4210_1080P_MODE0_w2  )
    {
        if(gbVXIS_OuputSize == _OUTPUT_1080P60)
        {
            ix = Mode0_1080P_param[0] ;
            iy = Mode0_1080P_param[1] ;
        }
        else
        {
            ix = Mode0_720P_param[0] ;
            iy = Mode0_720P_param[1] ;
        }
    }
    else if (out_mode == VS4210_1080P_MODE0_w3  )
    {
        if(gbVXIS_OuputSize == _OUTPUT_1080P60)
        {
            ix = Mode0_1080P_param[0] ;
            iy = Mode0_1080P_param[1] ;
        }
        else
        {
            ix = Mode0_720P_param[0] ;
            iy = Mode0_720P_param[1] ;
        }
    }
    else if (out_mode == VS4210_1080P_MODE1  )
    {
        if (ch == 0 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode1_w0_1080P_param[0] ;
                iy = Mode1_w0_1080P_param[1] ;
            }
            else
            {
                ix = Mode1_w0_720P_param[0] ;
                iy = Mode1_w0_720P_param[1] ;
            }
        }
        else if (ch == 1 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode1_w1_1080P_param[0] ;
                iy = Mode1_w1_1080P_param[1] ;
            }
            else
            {
                ix = Mode1_w1_720P_param[0] ;
                iy = Mode1_w1_720P_param[1] ;
            }
        }
        else if (ch == 2 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode1_w2_1080P_param[0] ;
                iy = Mode1_w2_1080P_param[1] ;
            }
            else
            {
                ix = Mode1_w2_720P_param[0] ;
                iy = Mode1_w2_720P_param[1] ;
            }
        }
        else //if (ch == 3 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode1_w3_1080P_param[0] ;
                iy = Mode1_w3_1080P_param[1] ;
            }
            else
            {
                ix = Mode1_w3_720P_param[0] ;
                iy = Mode1_w3_720P_param[1] ;
            }
        }
    }
    else if (out_mode == VS4210_1080P_MODE2 )
    {
        if (ch == 0 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode2_w0_1080P_param[0] ;
                iy = Mode2_w0_1080P_param[1] ;
            }
            else
            {
                ix = Mode2_w0_720P_param[0] ;
                iy = Mode2_w0_720P_param[1] ;
            }
        }
        else if (ch == 1 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode2_w1_1080P_param[0] ;
                iy = Mode2_w1_1080P_param[1] ;
            }
            else
            {
                ix = Mode2_w1_720P_param[0] ;
                iy = Mode2_w1_720P_param[1] ;
            }
        }
        else if (ch == 2 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode2_w2_1080P_param[0] ;
                iy = Mode2_w2_1080P_param[1] ;
            }
            else
            {
                ix = Mode2_w2_720P_param[0] ;
                iy = Mode2_w2_720P_param[1] ;
            }
        }
        else //if (ch == 3 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode2_w3_1080P_param[0] ;
                iy = Mode2_w3_1080P_param[1] ;
            }
            else
            {
                ix = Mode2_w3_720P_param[0] ;
                iy = Mode2_w3_720P_param[1] ;
            }
        }
    }
    else if ((out_mode == VS4210_1080P_MODE12 ) )
    {
        if (ch == 0 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode12_w0_1080P_param[0] ;
                iy = Mode12_w0_1080P_param[1] ;
            }
            else
            {
                ix = Mode12_w0_720P_param[0] ;
                iy = Mode12_w0_720P_param[1] ;
            }
        }
        else
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode12_w1_1080P_param[0] ;
                iy = Mode12_w1_1080P_param[1] ;
            }
            else
            {
                ix = Mode12_w1_720P_param[0] ;
                iy = Mode12_w1_720P_param[1] ;
            }
        }
    }
    else if ((out_mode == VS4210_1080P_MODE13 ) )
    {
        if (ch == 0 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode13_w0_1080P_param[0] ;
                iy = Mode13_w0_1080P_param[1] ;
            }
            else
            {
                ix = Mode13_w0_720P_param[0] ;
                iy = Mode13_w0_720P_param[1] ;
            }
        }
        else
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode13_w1_1080P_param[0] ;
                iy = Mode13_w1_1080P_param[1] ;
            }
            else
            {
                ix = Mode13_w1_720P_param[0] ;
                iy = Mode13_w1_720P_param[1] ;
            }
        }
    }
    else if ((out_mode >= VS4210_1080P_MODE6  ) && (out_mode < VS4210_1080P_MODE12))
    {
        if (ch == 0 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode6_w0_1080P_param[0] ;
                iy = Mode6_w0_1080P_param[1] ;
            }
            else
            {
                ix = Mode6_w0_720P_param[0] ;
                iy = Mode6_w0_720P_param[1] ;
            }
        }
        else  if (ch == 1 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode6_w1_1080P_param[0] ;
                iy = Mode6_w1_1080P_param[1] ;
            }
            else
            {
                ix = Mode6_w1_720P_param[0] ;
                iy = Mode6_w1_720P_param[1] ;
            }
        }
        else  if (ch == 2 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode6_w2_1080P_param[0] ;
                iy = Mode6_w2_1080P_param[1] ;
            }
            else
            {
                ix = Mode6_w2_720P_param[0] ;
                iy = Mode6_w2_720P_param[1] ;
            }
        }
        else
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode6_w3_1080P_param[0] ;
                iy = Mode6_w3_1080P_param[1] ;
            }
            else
            {
                ix = Mode6_w3_720P_param[0] ;
                iy = Mode6_w3_720P_param[1] ;
            }
        }
    }
    else if (out_mode == VS4210_1080P_MODE24  )
    {
        if (ch == 0 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode24_w0_1080P_param[0] ;
                iy = Mode24_w0_1080P_param[1] ;
            }
            else
            {
                ix = Mode24_w0_720P_param[0] ;
                iy = Mode24_w0_720P_param[1] ;
            }
        }
        else if (ch == 1 )
        {
            if(gbVXIS_OuputSize == _OUTPUT_1080P60)
            {
                ix = Mode24_w1_1080P_param[0] ;
                iy = Mode24_w1_1080P_param[1] ;
            }
            else
            {
                ix = Mode24_w1_720P_param[0] ;
                iy = Mode24_w1_720P_param[1] ;
            }
        }
    }
    else
    {
        ix = 224 ;
        iy = 64 ;
    }

    if (ch == 0  )
    {
    	#if (DisplayOSDEnable==ON)
        if (TitleFlg == 1 )
        {
            SetOsdSize(_Title0,Str1[0],1);
            SetOsdStartAddress(_Title0,0x00);
            OsdDumpString(0x00,Str1);
            OsdDumpAttribute(0x00,Str1,_Color);
            SetOsdPosition(_Title0,ix,iy);
        }

        if (BarFlg == 1 )
        {
            SetOsdSize(_Bar0,Str2[0],1);
            SetOsdStartAddress(_Bar0,0x40);
            OsdDumpString(0x40,Str2);
            OsdDumpAttribute(0x40,Str2,_Color);
            SetOsdPosition(_Bar0,ix,iy + 20 + OSDx2Flg*20);
        }
		#endif
    }
    else if (ch == 1 )
    {
        if (TitleFlg == 1 )
        {
            SetOsdSize(_Title1,Str1[0],1);
            SetOsdStartAddress(_Title1,0x10);
            OsdDumpString(0x10,Str1);
            OsdDumpAttribute(0x10,Str1,_Color);
            SetOsdPosition(_Title1,ix,iy);
        }

        if (BarFlg == 1 )
        {
            SetOsdSize(_Bar1,Str2[0],1);
            SetOsdStartAddress(_Bar1,0x50);
            OsdDumpString(0x50,Str2);
            OsdDumpAttribute(0x50,Str2,_Color);
            SetOsdPosition(_Bar1,ix,iy + 20 + OSDx2Flg*20 );
        }
    }
    else if (ch == 2 )
    {
        if (TitleFlg == 1 )
        {
            SetOsdSize(_Title2,Str1[0],1);
            SetOsdStartAddress(_Title1,0x20);
            OsdDumpString(0x20,Str1);
            OsdDumpAttribute(0x20,Str1,_Color);
            SetOsdPosition(_Title2,ix,iy);
        }

        if (BarFlg == 1 )
        {
            SetOsdSize(_Bar2,Str2[0],1);
            SetOsdStartAddress(_Bar2,0x60);
            OsdDumpString(0x60,Str2);
            OsdDumpAttribute(0x60,Str2,_Color);
            SetOsdPosition(_Bar2,ix,iy + 20 + OSDx2Flg*20);
        }
    }
    else if (ch == 3 )
    {
        if (TitleFlg == 1 )
        {
            SetOsdSize(_Title3,Str1[0],1);
            SetOsdStartAddress(_Title3,0x30);
            OsdDumpString(0x30,Str1);
            OsdDumpAttribute(0x30,Str1,_Color);
            SetOsdPosition(_Title3,ix,iy);
        }

        if (BarFlg == 1 )
        {
            SetOsdSize(_Bar3,Str2[0],1);
            SetOsdStartAddress(_Bar3,0x70);
            OsdDumpString(0x70,Str2);
            OsdDumpAttribute(0x70,Str2,_Color);
            SetOsdPosition(_Bar3,ix,iy + 20 + OSDx2Flg*20);
        }
    }

    setTransParentIndex(_TransParentColor);

        if (u8gDrawOSDFlg == 3 )
        {
            if ( MenuOSDonFlg == 0 )
                VXIS_fSetOsdOnOff(ch , _TitleOn | _BottomOn );
            else
                VXIS_fSetOsdOnOff(ch , _TitleOn | _BottomOn | _ContentOn );
                //VXIS_fSetOsdOnOff(ch , _ContentOn );
        }
        else if (TitleFlg == 1 )
        {
            if ( MenuOSDonFlg == 0 )
            {
                VXIS_fSetOsdOnOff(ch , _TitleOn  );
            }
            else {
                //VXIS_fSetOsdOnOff(ch , _TitleOn | _ContentOn );
                VXIS_fSetOsdOnOff(ch , _ContentOn );
            }
        }
        else if (BarFlg == 1 )
        {
            if ( MenuOSDonFlg == 0 )
            {
                VXIS_fSetOsdOnOff(ch , _BottomOn  );
            }
            else {
                //VXIS_fSetOsdOnOff(ch , _BottomOn | _ContentOn );
                VXIS_fSetOsdOnOff(ch ,  _ContentOn );
            }
        }
        else
        {
            if ( MenuOSDonFlg == 0 )
            {
                VXIS_fSetOsdOnOff(ch , _TurnoffAll   );
            }
            else {
                VXIS_fSetOsdOnOff(ch ,  _ContentOn );
            }
        }

}

static void GetconvertBarStr( tByte in_mode)
{
    switch(in_mode)
    {
//HDMI
    case _OSD_720X480I60:
        ptcOSDbar = &OSD_720x480I60_param[0] ; 
        break ;
    case _OSD_720X576I50:
        ptcOSDbar = &OSD_720x576I50_param[0] ; 
        break ;
    case _OSD_720X480P60:
        ptcOSDbar = &OSD_720x480P60_param[0] ; 
        break ;
    case _OSD_720X576P50:
        ptcOSDbar = &OSD_720x576P50_param[0] ; 
        break ;
    case _OSD_1920X1080I:
        ptcOSDbar = &OSD_1920x1080I_param[0] ; 
        break;
    case _OSD_1280X720P :
        ptcOSDbar = &OSD_1280x720P_param[0] ; 
        break;
    case _OSD_1920X1080P:
        ptcOSDbar = &OSD_1920x1080P_param[0] ; 
        break;
//DVI
    case _OSD_640X480P  :
        ptcOSDbar = &OSD_640X480P_param[0] ; 
        break;
    case _OSD_800X600P  :
        ptcOSDbar = &OSD_800X600P_param[0] ; 
        break;
    case _OSD_1024X768P :
        ptcOSDbar = &OSD_1024X768P_param[0] ; 
        break;
    case _OSD_1280X768P :
        ptcOSDbar = &OSD_720x480P60_param[0] ; 
        break;
    case _OSD_1360X768P :
        ptcOSDbar = &OSD_1280X768P_param[0] ; 
        break;
    case _OSD_1366X768P :
        ptcOSDbar = &OSD_1366X768P_param[0] ; 
        break;
    case _OSD_1280X800P :
        ptcOSDbar = &OSD_1280X800P_param[0] ; 
        break;
    case _OSD_1440X900P :
        ptcOSDbar = &OSD_1440X900P_param[0] ; 
        break;
    case _OSD_1280X1024P:
        ptcOSDbar = &OSD_1280X1024P_param[0] ; 
        break;
    case _OSD_1400X1050P:
        ptcOSDbar = &OSD_1400X1050P_param[0] ; 
        break;
    case _OSD_1680X1050P:
        ptcOSDbar = &OSD_1680X1050P_param[0] ; 
        break;
    case _OSD_1600X1200P:
        ptcOSDbar = &OSD_1600X1200P_param[0] ; 
        break;
    case _OSD_1920X1200P:
        ptcOSDbar = &OSD_1920X1200P_param[0] ; 
        break;
    case _OSD_No_Signal :
        ptcOSDbar = &OSD_No_Signal_param[0] ; 
        break;

    default :
        ptcOSDbar = &OSD_No_Signal_param[0] ; 
        break;
    }    
}

/**
 * @brief Input resolution
 *
 * Input resolution on the screen display
 *
 * @param in_mode Input resolution
 * @param out_mode Output resolution
 * @return none
 */
void VXIS_fShowInOutMode( tByte ch ,  tByte in_mode,  tByte out_mode  )
{
    tcByte *ptr1 ;    
    tByte  OSD_CHx_param[7] ;
    tByte  pu8aOSDbar[11] ; 
    tByte chFix ;

    chFix = gaHDMI_MapWindowIndex[ch] ;
    ptr1 = &OSD_CH1_param[0] ;

    if (out_mode == VS4210_1080P_MODE0_w1 )
    {
        if (ch == 1 )
        {
            ptr1 = OSD_CH1_param ;
            chFix = 0 ;
        }
    }
    else if (out_mode == VS4210_1080P_MODE0_w2 )
    {
        if (ch == 2 )
        {
            ptr1 = OSD_CH2_param ;
            chFix = 0 ;
        }
    }
    else if (out_mode == VS4210_1080P_MODE0_w3 )
    {
        if (ch == 3 )
        {
            ptr1 = OSD_CH3_param ;
            chFix = 0 ;
        }
    }
    else
    {
        if (chFix == 0 )
        {
            ptr1 = OSD_CH0_param ;
        }
        else  	if (chFix == 1 )
        {
            ptr1 = OSD_CH1_param ;
        }
        else  	if (chFix == 2 )
        {
            ptr1 = OSD_CH2_param ;
        }
        else
        {
            ptr1 = OSD_CH3_param ;
        }
    }

   if (gu8AudioMode == chFix  )
    {
        memcpy(OSD_CHx_param , ptr1 , 6 ) ;
	OSD_CHx_param[6] =	0x6C ;
    }
    else
    {
        memcpy(OSD_CHx_param , ptr1 , 7 ) ;
    }
    
    GetconvertBarStr(in_mode ) ; 
    memcpy(pu8aOSDbar , ptcOSDbar , 11) ; 
    VX_Write_OSD_In_OutString(ch , OSD_CHx_param, pu8aOSDbar ,out_mode) ;    
}



void VXIS_fShowInOutMode0Fast( tByte ch ,  tByte in_mode,  tByte out_mode  )
{
    tcByte *ptr1 ;
    tByte  OSD_CHx_param[7] ;
    tByte  pu8aOSDbar[11] ; 
    tByte  chFix ;

    if (ch == 0 )
    {
        ptr1 = OSD_CH0_param ;
    }
    else if (ch == 1 )
    {
        ptr1 = OSD_CH1_param ;
    }
    else if (ch == 2 )
    {
        ptr1 = OSD_CH2_param ;
    }
    else
    {
        ptr1 = OSD_CH3_param ;
    }

   if(1)//ryan@20201229   if (gu8AudioMode == ch )
    {
        memcpy(OSD_CHx_param , ptr1 , 6 ) ;        
        OSD_CHx_param[6] =  0x6C ;
    }
    else
    {
        memcpy(OSD_CHx_param , ptr1 , 7 ) ;
    }

    chFix = 0  ;    
    GetconvertBarStr(in_mode ) ;     
    memcpy(pu8aOSDbar , ptcOSDbar , 11) ; 
    VX_Write_OSD_In_OutString(chFix , OSD_CHx_param, pu8aOSDbar ,out_mode) ;

}

void OSD_Clear(tByte Win )
{
    tByte temp53;
    tByte temp5E ;
    tByte temp2 ;
    tByte temp ;

    if (Win == 0 )
    {
        SetOsdSize(_Title0,0,1);
        SetOsdSize(_Bar0,0,1);
    }
    else
    {
        temp2 = VXISI2CRead(0x53);
        temp53 = temp2 & 0x3f ;
        temp2 = VXISI2CRead(0x5E);
        temp5E = temp2 & 0x3f ;

        if (Win == 1 )
        {
            VXISI2CWrite(0x53,0x40|temp53 );
            VXISI2CWrite(0x5E,0x40|temp5E );
        }
        else if (Win == 2 )
        {
            VXISI2CWrite(0x53,0x80|temp53 );
            VXISI2CWrite(0x5E,0x80|temp5E );
        }
        else if (Win == 3 )
        {
            VXISI2CWrite(0x53,0xC0|temp53 );
            VXISI2CWrite(0x5E,0xC0|temp5E );
        }

        temp=VXISI2CRead(0x51);
        temp=temp& 0x8 ;

        if (MenuOSDonFlg == 0 )
        {
            VXISI2CWrite(0x51,0x00 | temp );
        }
        else
        {
            VXISI2CWrite(0x51,_ContentOn | temp );
        }
    }
}

void OSD_Clear_4(void )
{
    OSD_Clear(3) ;
}

void OSD_Clear_34(void )
{
    tByte i ;
    for (i = 2 ; i < 4 ;i++)
    {
        OSD_Clear(i) ;
    }
}

void OSD_Clear_234(void )
{
    tByte i ;
    for (i = 1 ; i < 4 ;i++)
    {
        OSD_Clear(i) ;
    }
}

void OSD_Clear_All(void )
{
    tByte i ;
    for (i = 0 ; i < 4 ;i++)
    {
        OSD_Clear(i) ;
    }
}

/*
void ClearAudioOSD(void)
{
    tByte i ;
    for (i = 0  ; i < HDMI_IN_NUMBER ; i++ )
    {
        VXIS_fShowInOutMode(gaHDMI_MapWindowIndex[i]  ,  gbVXIS_InputMode[i] , gbVXIS_OuputModeflg )  ;
    }
}

void Clear1ChAudioOSD(tByte ch )
{
    VXIS_fShowInOutMode(gaHDMI_MapWindowIndex[ch]  ,  gbVXIS_InputMode[ch] , gbVXIS_OuputModeflg )  ;
}
*/

void osd_cont_write(unsigned char cw_dest, unsigned char init_addr, unsigned char num_of_data, const unsigned char *buf_data)
{////Extern OSD
    unsigned char wbuf[255] ;
    wbuf[0] = cw_dest ;
    wbuf[1] = init_addr ;
    memcpy(&wbuf[2],buf_data ,num_of_data )  ;
    I2CMultiWrite(VS4210_ADDR, 0xFD, num_of_data +2, &wbuf[0]) ;
}

//---------------------------------------------------------------------------
/**
 * @brief Initialize the custom font function
 *
 * Initialize the custom font data.
 *
 * @param Size, Number of custom fonts.
 * @param *UserDefineFont, Font data start position.
 * @return none
 */
void VXOSD_UseFont_init(tByte Size, tcByte *UserDefineFontPtr)
{//Extern OSD
    //16*20
    uint16_t FontIndex ;
    uint8_t  bytecommand, Haddr,Laddr ;
    uint16_t i  ;
    uint16_t FontAddr  ;
    uint8_t Size2 ;

    Size2 = Size * 2   ;
    if (Size < 64 )
    {
        for ( i = 0 ; i< Size2 ; i++)   //
        {
            FontAddr = i * 10 ;  //WORD
            FontIndex = i * 20 ; //BYTE
            Haddr = (uint8_t) ((FontAddr & 0x0F00) >> 8)  ;
            Laddr = (uint8_t) (FontAddr & 0x00FF) ;
            bytecommand = 0x20 | Haddr ;
            osd_cont_write(bytecommand, Laddr, 20, &UserDefineFontPtr[FontIndex] );   // user define font
        }
    }
    else {
        //printf("Error ,  Size >64\r\n" ) ;
    }
}

void VXOSD_ShowExContent(tByte *ptrExOsdContent, tByte index )
{
    tByte len ;
    tByte Indexoffset ;
    tByte FontLenx2 ;
    FontLenx2 = 2*EXTERNOSDNUMBER ;
/*
    //SetOsdSize(_Content,FontLenx2 ,1);
    SetOsdSize(_Content, 3 , FontLenx2 );
    SetOsdStartAddress(_Content,0x80);
    OsdDumpString(0x80, ptrExOsdContent );
    len = ptrExOsdContent[0] ;
    OsdDumpAttribute(0x80, &len, _MenuColor );
    if (index > 0 )
    {
        len = 1;
        Indexoffset = index *2 ;
        OsdDumpAttribute(0x80 + Indexoffset + FontLenx2 , &len, _MenuSelColor  );
        //OsdDumpAttribute(0x80 + FontLenx2 , &len, _MenuSelColor  );
    }
*/

    SetOsdSize(_Content,FontLenx2 +1 ,3);
    SetOsdStartAddress(_Content,0x80);
    OsdDumpString(0x80, ptrExOsdContent );
    len = ptrExOsdContent[0] ;
    OsdDumpAttribute(0x80, &len, _MenuColor );
    if (index > 0 )
    {
        len = 1;
        Indexoffset = index *2 ;
        OsdDumpAttribute(0x80 + Indexoffset + FontLenx2 , &len, _MenuSelColor  );
    }

    //OSD_Clear_All();
  //  VXIS_fSetContentOsdOnOff(_ContentOn) ;

    if(gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        //SetOsdPosition(_Content,600,420);
        SetOsdPosition(_Content,600,270);
        VXIS_fSetOsdx2(1) ;
    }
    else
    {
        //SetOsdPosition(_Content,614,300);
        SetOsdPosition(_Content,614,200);
        VXIS_fSetOsdx2(0) ;
    }
}

void VXOSD_SelectExContent(tByte index )
{
    tByte len ;
    tByte Indexoffset ;

    tByte FontLenx2 ;

    FontLenx2 = 2*EXTERNOSDNUMBER ;
    len = FontLenx2 +1  ;
    //len = FontLenx2 ;

//    OsdDumpAttribute(0x80 , &len, _MenuColor );
//    len = 1;
//    Indexoffset = index *2 ;
//    OsdDumpAttribute(0x80 + Indexoffset , &len, _MenuSelColor  );


    OsdDumpAttribute(0x80+ (FontLenx2 +2), &len, _MenuColor );
    len = 1;
    Indexoffset = index *2 ;
    OsdDumpAttribute(0x80 + Indexoffset +FontLenx2 , &len, _MenuSelColor  );


//    if (OSDx2Flg == 0 )
//    {
//        OSD_Clear_All();
//        VXIS_fSetOsdx2(1) ;
//        OSDx2Flg = 1 ;
//    }

//    VXIS_fSetContentOsdOnOff(_ContentOn) ;
}

void Set_Video_Loss_OSD(BYTE ch, BYTE set)	
{
	//VXISI2CWrite(0x32,0x00);

//bVloss_Flag=set;

#if 1
switch(set)
{
	case ON:
			if(ch==_HDMI_Input)
				{
				VXISI2CWrite(0x10,0x00);
				VXISI2CWrite(0x9C,0x0A);	
				}		
			else 
				{
				VXISI2CWrite(0x10,0x01);
				VXISI2CWrite(0x9C,0x0A);	
				}
			break;

	case OFF:
				if(ch==_HDMI_Input)
				{
				VXISI2CWrite(0x10,(VXISI2CRead(0x10)|0x80));
				VXISI2CWrite(0x9C,0x00);
				//SET_HDMI_SYNC_PARA();
				}		
			else
				{
				VXISI2CWrite(0x10,(VXISI2CRead(0x10)|0x81));
				VXISI2CWrite(0x9C,0x00);
				}
	
			break;
	
}
#endif
	SET_VIDEO_VLOSS(set);	
	//MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS);	
	//MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_SET_VIDEO_LOSS);	
	
	#if (_DEBUG_OSD==ON)	
	GraphicsPrint(RED,"(SET_VIDEO_VLOSS ch=%d set=%d)",(WORD)ch,(WORD)set);	
	#endif	

	//MCUTimerActiveTimerEvent( SEC(2),_SYSTEM_TIMER_EVENT_SET_EMBEDDED_SYNC);	

}


