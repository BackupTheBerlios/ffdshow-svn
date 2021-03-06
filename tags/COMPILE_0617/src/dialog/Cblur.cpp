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
#include "IffDecoder.h"
#include "TffdshowPage.h"
#include "Cblur.h"
#include "resource.h"

void TblurPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(0,255));
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETPAGESIZE,0,16); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_TEMPSMOOTH,TBM_SETRANGE,TRUE,MAKELPARAM(0,10));
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_TEMPSMOOTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_TEMPSMOOTH,TBM_SETPAGESIZE,0,1); 
 cfg2dlg();
}

void TblurPage::cfg2dlg(void)
{
 blur2dlg();
}

void TblurPage::blur2dlg(void)
{
 int x;char s[256];
 x=cfgGet(IDFF_blurStrength);
 sprintf(s,"Soften: %i",x);
 if (x==0) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_BLUR_STRENGTH,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETPOS,TRUE,x);
 
 x=cfgGet(IDFF_tempSmooth);
 sprintf(s,"Temporal smooth: %i",x);
 if (x==0) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_BLUR_TEMPSMOOTH,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_TEMPSMOOTH,TBM_SETPOS,TRUE,x);
}

HRESULT TblurPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_BLUR_STRENGTH) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_BLUR_TEMPSMOOTH))
     {
      cfgSet(IDFF_blurStrength,SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH  ,TBM_GETPOS,0,0));
      cfgSet(IDFF_tempSmooth  ,SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_TEMPSMOOTH,TBM_GETPOS,0,0));
      blur2dlg();
      return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_BLUR:
       cfgSet(IDFF_isBlur,getCheck(IDC_CHB_BLUR));
       parent->drawInter();
       return TRUE;
     }
    break;
  }
 return FALSE;
}

void TblurPage::interDlg(void)
{
 setCheck(IDC_CHB_BLUR,cfgGet(IDFF_isBlur));
}
void TblurPage::getTip(char *tipS,int len)
{
 sprintf(tipS,"blur: %i, temporal smooth: %i",cfgGet(IDFF_blurStrength),cfgGet(IDFF_tempSmooth));
}
TblurPage::TblurPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_BLUR);
}
