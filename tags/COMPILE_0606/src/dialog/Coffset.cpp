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
#include "Coffset.h"
#include "resource.h"

void ToffsetPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_X ,TBM_SETRANGE,TRUE,MAKELPARAM(-32,32));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_X ,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_X ,TBM_SETPAGESIZE,0,4); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_Y ,TBM_SETRANGE,TRUE,MAKELPARAM(-32,32));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_Y ,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_Y ,TBM_SETPAGESIZE,0,4); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_X,TBM_SETRANGE,TRUE,MAKELPARAM(-32,32));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_X,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_X,TBM_SETPAGESIZE,0,4); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_Y,TBM_SETRANGE,TRUE,MAKELPARAM(-32,32));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_Y,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_Y,TBM_SETPAGESIZE,0,4); 
 cfg2dlg();
}

void ToffsetPage::cfg2dlg(void)
{
 offset2dlg();
}

void ToffsetPage::offset2dlg(void)
{
 char s[256];int x;
 x=cfgGet(IDFF_offsetY_X);
 sprintf(s,"Luma offset X: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_OFFSETY_X,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_X,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_offsetY_Y);
 sprintf(s,"Luma offset Y: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_OFFSETY_Y,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_Y,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_offsetU_X);
 sprintf(s,"Chroma offset X: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_OFFSETUV_X,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_X,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_offsetU_Y);
 sprintf(s,"Chroma offset Y: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_OFFSETUV_Y,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_Y,TBM_SETPOS,TRUE,x);

}

HRESULT ToffsetPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_OFFSETY_X) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_OFFSETY_Y) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_OFFSETUV_X) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_OFFSETUV_Y))
     {
      cfgSet(IDFF_offsetY_X,SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_X ,TBM_GETPOS,0,0));
      cfgSet(IDFF_offsetY_Y,SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETY_Y ,TBM_GETPOS,0,0));
      cfgSet(IDFF_offsetU_X,SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_X,TBM_GETPOS,0,0));
      cfgSet(IDFF_offsetU_Y,SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_Y,TBM_GETPOS,0,0));
      cfgSet(IDFF_offsetV_X,SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_X,TBM_GETPOS,0,0));
      cfgSet(IDFF_offsetV_Y,SendDlgItemMessage(m_hwnd,IDC_TBR_OFFSETUV_Y,TBM_GETPOS,0,0));
      offset2dlg();
      return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_OFFSET:
       cfgSet(IDFF_isOffset,getCheck(IDC_CHB_OFFSET));
       parent->drawInter();
       return TRUE;
     }
    break;
  }
 return FALSE;
}

void ToffsetPage::interDlg(void)
{
 setCheck(IDC_CHB_OFFSET,cfgGet(IDFF_isOffset));
}
void ToffsetPage::getTip(char *tipS,int len)
{
 sprintf(tipS,"Offset (luma:[%i,%i] chroma:[%i,%i])",cfgGet(IDFF_offsetY_X),cfgGet(IDFF_offsetY_Y),cfgGet(IDFF_offsetU_X),cfgGet(IDFF_offsetU_Y),cfgGet(IDFF_offsetV_X),cfgGet(IDFF_offsetV_Y));
}
ToffsetPage::ToffsetPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_OFFSET);
}
