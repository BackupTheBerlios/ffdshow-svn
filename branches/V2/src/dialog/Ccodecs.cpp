/*
 * Copyright (c) 2002 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "stdafx.h"
#include "Ccodecs.h"
#include "resource.h"
#include "IffDecoder.h"
#include "Tconfig.h"

void TcodecsPage::init(void)
{
 cfg2dlg();
 enableWindow(IDC_CHB_XVID,config.isLibavcodec || config.isXviD);enableWindow(IDC_CHB_XVID_XVID,config.isXviD);
 enableWindow(IDC_CHB_DIV3,config.isLibavcodec);
 enableWindow(IDC_CHB_DIVX,config.isLibavcodec || config.isXviD);enableWindow(IDC_CHB_DIVX_XVID,config.isXviD);
 enableWindow(IDC_CHB_DX50,config.isLibavcodec || config.isXviD);enableWindow(IDC_CHB_DX50_XVID,config.isXviD);
 enableWindow(IDC_CHB_BLZ0,config.isLibavcodec || config.isXviD);enableWindow(IDC_CHB_BLZ0_XVID,config.isXviD);
 enableWindow(IDC_CHB_MP43,config.isLibavcodec);
 enableWindow(IDC_CHB_MP42,config.isLibavcodec);
 enableWindow(IDC_CHB_MP41,config.isLibavcodec);
 enableWindow(IDC_CHB_H263,config.isLibavcodec);
 enableWindow(IDC_CHB_WMV1,config.isLibavcodec);

}

void TcodecsPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_XVID,cfgGet(IDFF_xvid));setCheck(IDC_CHB_XVID_XVID,cfgGet(IDFF_xvid)==2);if (!config.isLibavcodec && config.isXviD) setCheck(IDC_CHB_XVID_XVID,getCheck(IDC_CHB_XVID));
 setCheck(IDC_CHB_DIV3,cfgGet(IDFF_div3));
 setCheck(IDC_CHB_DIVX,cfgGet(IDFF_divx));setCheck(IDC_CHB_DIVX_XVID,cfgGet(IDFF_divx)==2);if (!config.isLibavcodec && config.isXviD) setCheck(IDC_CHB_DIVX_XVID,getCheck(IDC_CHB_DIVX));
 setCheck(IDC_CHB_DX50,cfgGet(IDFF_dx50));setCheck(IDC_CHB_DX50_XVID,cfgGet(IDFF_dx50)==2);if (!config.isLibavcodec && config.isXviD) setCheck(IDC_CHB_DX50_XVID,getCheck(IDC_CHB_DX50));
 setCheck(IDC_CHB_BLZ0,cfgGet(IDFF_blz0));setCheck(IDC_CHB_BLZ0_XVID,cfgGet(IDFF_blz0)==2);if (!config.isLibavcodec && config.isXviD) setCheck(IDC_CHB_BLZ0_XVID,getCheck(IDC_CHB_BLZ0));
 setCheck(IDC_CHB_MP43,cfgGet(IDFF_mp43));
 setCheck(IDC_CHB_MP42,cfgGet(IDFF_mp42));
 setCheck(IDC_CHB_MP41,cfgGet(IDFF_mp41));
 setCheck(IDC_CHB_H263,cfgGet(IDFF_h263));
 setCheck(IDC_CHB_WMV1,cfgGet(IDFF_wmv1));
 setCheck(IDC_CHB_RAWV,cfgGet(IDFF_rawv));

 setCheck(IDC_CHB_YV12 ,cfgGet(IDFF_outYV12  ));
 setCheck(IDC_CHB_YUY2 ,cfgGet(IDFF_outYUY2  ));
 setCheck(IDC_CHB_YVYU ,cfgGet(IDFF_outYVYU  ));
 setCheck(IDC_CHB_UYVY ,cfgGet(IDFF_outUYVY  ));
 setCheck(IDC_CHB_RGB32,cfgGet(IDFF_outRGB32 ));
 setCheck(IDC_CHB_RGB24,cfgGet(IDFF_outRGB24 ));
 setCheck(IDC_CHB_RGB15,cfgGet(IDFF_outRGB555));
 setCheck(IDC_CHB_RGB16,cfgGet(IDFF_outRGB565));
}

HRESULT TcodecsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_XVID:case IDC_CHB_XVID_XVID:
      case IDC_CHB_DIV3:
      case IDC_CHB_DIVX:case IDC_CHB_DIVX_XVID:
      case IDC_CHB_DX50:case IDC_CHB_DX50_XVID:
      case IDC_CHB_BLZ0:case IDC_CHB_BLZ0_XVID:
      case IDC_CHB_MP43:
      case IDC_CHB_MP42:
      case IDC_CHB_MP41:
      case IDC_CHB_H263:
      case IDC_CHB_WMV1:
      case IDC_CHB_RAWV:
       if (getCheck(IDC_CHB_XVID))
        cfgSet(IDFF_xvid,getCheck(IDC_CHB_XVID_XVID)?2:1);
       else
        cfgSet(IDFF_xvid,0);
       cfgSet(IDFF_div3,getCheck(IDC_CHB_DIV3));
       if (getCheck(IDC_CHB_DIVX))
        cfgSet(IDFF_divx,getCheck(IDC_CHB_DIVX_XVID)?2:1);
       else
        cfgSet(IDFF_divx,0);
       if (getCheck(IDC_CHB_DX50))
        cfgSet(IDFF_dx50,getCheck(IDC_CHB_DX50_XVID)?2:1);
       else
        cfgSet(IDFF_dx50,0);
       if (getCheck(IDC_CHB_BLZ0))
        cfgSet(IDFF_blz0,getCheck(IDC_CHB_BLZ0_XVID)?2:1);
       else
        cfgSet(IDFF_blz0,0);
       cfgSet(IDFF_mp43,getCheck(IDC_CHB_MP43));
       cfgSet(IDFF_mp42,getCheck(IDC_CHB_MP42));
       cfgSet(IDFF_mp41,getCheck(IDC_CHB_MP41));
       cfgSet(IDFF_h263,getCheck(IDC_CHB_H263));
       cfgSet(IDFF_wmv1,getCheck(IDC_CHB_WMV1));
       cfgSet(IDFF_rawv,getCheck(IDC_CHB_RAWV));
       cfg2dlg();
       return TRUE;
      case IDC_CHB_YV12:
      case IDC_CHB_YUY2:
      case IDC_CHB_YVYU:
      case IDC_CHB_UYVY:
      case IDC_CHB_RGB32:
      case IDC_CHB_RGB24:
      case IDC_CHB_RGB15:
      case IDC_CHB_RGB16:
       cfgSet(IDFF_outYV12  ,getCheck(IDC_CHB_YV12 ));
       cfgSet(IDFF_outYUY2  ,getCheck(IDC_CHB_YUY2 ));
       cfgSet(IDFF_outYVYU  ,getCheck(IDC_CHB_YVYU ));
       cfgSet(IDFF_outUYVY  ,getCheck(IDC_CHB_UYVY ));
       cfgSet(IDFF_outRGB32 ,getCheck(IDC_CHB_RGB32));
       cfgSet(IDFF_outRGB24 ,getCheck(IDC_CHB_RGB24));
       cfgSet(IDFF_outRGB555,getCheck(IDC_CHB_RGB15));
       cfgSet(IDFF_outRGB565,getCheck(IDC_CHB_RGB16));
       cfg2dlg();
       return TRUE;
     }
    break; 
  }    
 return FALSE;
}

TcodecsPage::TcodecsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_CODECS);
}
