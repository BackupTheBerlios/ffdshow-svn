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

#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#pragma hdrstop
#include "Ccodecs.h"
#include "resource.h"
#include "IffDecoder.h"

void TcodecsPage::init(void)
{
 cfg2dlg();
}

void TcodecsPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_XVID,cfgGet(IDFF_xvid));
 setCheck(IDC_CHB_DIV3,cfgGet(IDFF_div3));
 setCheck(IDC_CHB_DIVX,cfgGet(IDFF_divx));
 setCheck(IDC_CHB_DX50,cfgGet(IDFF_dx50));
 setCheck(IDC_CHB_MP43,cfgGet(IDFF_mp43));
 setCheck(IDC_CHB_MP42,cfgGet(IDFF_mp42));
 setCheck(IDC_CHB_H263,cfgGet(IDFF_h263));
 setCheck(IDC_CHB_RAWV,cfgGet(IDFF_rawv));
}

HRESULT TcodecsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_XVID:
      case IDC_CHB_DIV3:
      case IDC_CHB_DIVX:
      case IDC_CHB_DX50:
      case IDC_CHB_MP43:
      case IDC_CHB_MP42:
      case IDC_CHB_H263:
      case IDC_CHB_RAWV:
       cfgSet(IDFF_xvid,getCheck(IDC_CHB_XVID));
       cfgSet(IDFF_div3,getCheck(IDC_CHB_DIV3));
       cfgSet(IDFF_divx,getCheck(IDC_CHB_DIVX));
       cfgSet(IDFF_dx50,getCheck(IDC_CHB_DX50));
       cfgSet(IDFF_mp43,getCheck(IDC_CHB_MP43));
       cfgSet(IDFF_mp42,getCheck(IDC_CHB_MP42));
       cfgSet(IDFF_h263,getCheck(IDC_CHB_H263));
       cfgSet(IDFF_rawv,getCheck(IDC_CHB_RAWV));
       return TRUE;
     };
    break; 
  }    
 return FALSE;
}

TcodecsPage::TcodecsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_CODECS);
}
