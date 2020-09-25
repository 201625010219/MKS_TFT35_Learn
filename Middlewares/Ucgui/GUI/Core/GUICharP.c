/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUICharP.C
Purpose     : Implementation of Proportional fonts
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "GUI_Private.h"
#include "spi_flash.h"  
#include "pic_manager.h"
#include "GUI.h" 
extern const unsigned char asc2_1608[95][16];
extern GUI_CONST_STORAGE U8 acFont8x16[][16];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Fontmks_fontHz14_CharInfo[192];
/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
//字模数据的暂存数组,以单个字模的最大字节数为设定值 
#define BYTES_PER_FONT 32   //最大支持16*16的汉字
static U8 GUI_FontDataBuf[BYTES_PER_FONT];
extern const GUI_FONT GUI_FontHZ16;
/*********************************************************************
*
*       GUIPROP_FindChar
*/
static const GUI_FONT_PROP GUI_UNI_PTR * GUIPROP_FindChar(const GUI_FONT_PROP GUI_UNI_PTR* pProp, U16P c) {
  for (; pProp; pProp = pProp->pNext) {
    if ((c>=pProp->First) && (c<=pProp->Last))
      break;
  }
  return pProp;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIPROP_DispChar
*
* Purpose:
*   This is the routine that displays a character. It is used by all
*   other routines which display characters as a subroutine.
*/
//tan0910
#if 0
void GUIPROP_DispChar(U16P c) {
  int BytesPerLine;
  GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
  if (pProp) {
    GUI_DRAWMODE OldDrawMode;
    const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo+(c-pProp->First);
    BytesPerLine = pCharInfo->BytesPerLine;
    OldDrawMode  = LCD_SetDrawMode(DrawMode);
    LCD_DrawBitmap( GUI_Context.DispPosX, GUI_Context.DispPosY,
                       pCharInfo->XSize,
											 GUI_Context.pAFont->YSize,
                       GUI_Context.pAFont->XMag,
											 GUI_Context.pAFont->YMag,
                       1,     /* Bits per Pixel */
                       BytesPerLine,
                       pCharInfo->pData,
                       &LCD_BKCOLORINDEX
                       );
    /* Fill empty pixel lines */
    if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) {
      int YMag = GUI_Context.pAFont->YMag;
      int YDist = GUI_Context.pAFont->YDist * YMag;
      int YSize = GUI_Context.pAFont->YSize * YMag;
      if (DrawMode != LCD_DRAWMODE_TRANS) {
        LCD_COLOR OldColor = GUI_GetColor();
        GUI_SetColor(GUI_GetBkColor());
        LCD_FillRect(GUI_Context.DispPosX, 
                     GUI_Context.DispPosY + YSize, 
                     GUI_Context.DispPosX + pCharInfo->XSize, 
                     GUI_Context.DispPosY + YDist);
        GUI_SetColor(OldColor);
      }
    }
    LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
    GUI_Context.DispPosX += pCharInfo->XDist * GUI_Context.pAFont->XMag;
  }
}
#endif
void GUIPROP_DispChar(U16P c) {
  
  static const unsigned char *buf_point1,*buf_point2;//tan
    
  unsigned char qh,ql;
  unsigned char i;					  
  unsigned long foffset; 
  unsigned char *mat;
  unsigned char size=16;//字体大小
  unsigned char t;

  U8 BytesPerFont;  //一个字的字节数
  
  int BytesPerLine;
  GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
  
  if (pProp) {
    GUI_DRAWMODE OldDrawMode;
    //const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo+(c-pProp->First);
    const GUI_CHARINFO GUI_UNI_PTR * pCharInfo;
    
    if((GUI_Context.pAFont == &GUI_FontHZ16))
    {
      pCharInfo = pProp->paCharInfo;
      //base = (U32)pProp->paCharInfo->pData; //找出字库对应的首地址
      BytesPerFont = GUI_Context.pAFont->YSize * pProp->paCharInfo->BytesPerLine; //每个字模的数据字节数
      if(BytesPerFont > BYTES_PER_FONT)
      {
        BytesPerFont = BYTES_PER_FONT;
      }
      if (c < 0x80) //英文字符地址偏移算法 
      {
        /*
        memset(GUI_FontDataBuf,0,sizeof(GUI_FontDataBuf));
        
        for(t=0;t<16;t++)
        {
          GUI_FontDataBuf[t] = acFont8x16[c-0x20][t]; //直接取出字模 
        }
        */
        BytesPerLine = pCharInfo->BytesPerLine;//获取一行有多少个字节
        OldDrawMode  = LCD_SetDrawMode(DrawMode);//设置画图模式		

        LCD_DrawBitmap( GUI_Context.DispPosX, 
                        GUI_Context.DispPosY,
                        pCharInfo->XSize,
                        GUI_Context.pAFont->YSize,
                        GUI_Context.pAFont->XMag,
                        GUI_Context.pAFont->YMag,
                        1,     
                        BytesPerLine,
                        GUI_Fontmks_fontHz14_CharInfo[c-0x20].pData,   //加载的字符数据
                        &LCD_BKCOLORINDEX
                        ); 
          
      }
      else
      {
        qh=c/256;
	    ql=c%256;
        if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
        {   		    
            for(i=0;i<(size*2);i++)*mat++=0x00;//填充满格
            return; //结束访问
        }          
        if(ql<0x7f)ql-=0x40;//注意!
        else ql-=0x41;
        qh-=0x81;   
        foffset=((unsigned long)190*qh+ql)*(size*2);//得到字库中的字节偏移量					
        if(size==16)
        {
           memset(GUI_FontDataBuf,0,sizeof(GUI_FontDataBuf));
           SPI_FLASH_BufferRead(GUI_FontDataBuf,foffset+GBK_FLASH_ADDR,32);
        }
        else 
        {
           SPI_FLASH_BufferRead(GUI_FontDataBuf,foffset+GBK_FLASH_ADDR,24);
        }
      //}
      
      BytesPerLine = pCharInfo->BytesPerLine;//获取一行有多少个字节
      OldDrawMode  = LCD_SetDrawMode(DrawMode);//设置画图模式		

      LCD_DrawBitmap( GUI_Context.DispPosX, 
                      GUI_Context.DispPosY,
                      pCharInfo->XSize,
                      GUI_Context.pAFont->YSize,
                      GUI_Context.pAFont->XMag,
                      GUI_Context.pAFont->YMag,
                      1,     
                      BytesPerLine,
                      GUI_FontDataBuf,   //加载的字符数据
                      &LCD_BKCOLORINDEX
                      ); 
      }
    }
    else
    {
      pCharInfo	= pProp->paCharInfo+(c-pProp->First);
      BytesPerLine = pCharInfo->BytesPerLine;
      OldDrawMode  = LCD_SetDrawMode(DrawMode);
      LCD_DrawBitmap( GUI_Context.DispPosX, GUI_Context.DispPosY,
                         pCharInfo->XSize,
                                               GUI_Context.pAFont->YSize,
                         GUI_Context.pAFont->XMag,
                                               GUI_Context.pAFont->YMag,
                         1,     /* Bits per Pixel */
                         BytesPerLine,
                         pCharInfo->pData,
                         &LCD_BKCOLORINDEX
                         );
    }
    /* Fill empty pixel lines */
    if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) {
      int YMag = GUI_Context.pAFont->YMag;
      int YDist = GUI_Context.pAFont->YDist * YMag;
      int YSize = GUI_Context.pAFont->YSize * YMag;
      if (DrawMode != LCD_DRAWMODE_TRANS) {
        LCD_COLOR OldColor = GUI_GetColor();
        GUI_SetColor(GUI_GetBkColor());
        LCD_FillRect(GUI_Context.DispPosX, 
                     GUI_Context.DispPosY + YSize, 
                     GUI_Context.DispPosX + pCharInfo->XSize, 
                     GUI_Context.DispPosY + YDist);
        GUI_SetColor(OldColor);
      }
    }
    LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
    GUI_Context.DispPosX += pCharInfo->XDist * GUI_Context.pAFont->XMag;
  }
}

/*********************************************************************
*
*       GUIPROP_GetCharDistX
*/
#if 0
int GUIPROP_GetCharDistX(U16P c) {
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
  return (pProp) ? (pProp->paCharInfo+(c-pProp->First))->XSize * GUI_Context.pAFont->XMag : 0;
}
#endif
extern const GUI_FONT_PROP GUI_FontHZ16_Prop0020;
int GUIPROP_GetCharDistX(U16P c)
{

  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
  if(pProp)
  {
  	if(GUI_Context.pAFont == &GUI_FontHZ16)
	{
		if(c>0x80)return 16;
		else return 8;
	}	
  }
  return (pProp) ? (pProp->paCharInfo+(c-pProp->First))->XSize * GUI_Context.pAFont->XMag : 0;
}

/*********************************************************************
*
*       GUIPROP_GetFontInfo
*/
void GUIPROP_GetFontInfo(const GUI_FONT GUI_UNI_PTR * pFont, GUI_FONTINFO* pfi) {
  GUI_USE_PARA(pFont);
  pfi->Flags = GUI_FONTINFO_FLAG_PROP;
}

/*********************************************************************
*
*       GUIPROP_IsInFont
*/
char GUIPROP_IsInFont(const GUI_FONT GUI_UNI_PTR * pFont, U16 c) {
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(pFont->p.pProp, c);
  return (pProp==NULL) ? 0 : 1;
}

/*************************** End of file ****************************/
