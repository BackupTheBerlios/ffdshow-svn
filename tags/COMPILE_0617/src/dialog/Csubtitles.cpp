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
#include "Csubtitles.h"
#include "resource.h"

void TsubtitlesPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,CB_LIMITTEXT,1023,0);
 
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETPAGESIZE,0,10); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETPAGESIZE,0,10); 
 
 cfg2dlg();
}

void TsubtitlesPage::cfg2dlg(void)
{
 interDlg();
 sub2dlg();
}

void TsubtitlesPage::interDlg(void)
{
 setCheck(IDC_CHB_SUBTITLES,cfgGet(IDFF_isSubtitles));
}

void TsubtitlesPage::sub2dlg(void)
{
 char s[256];int x;
 x=cfgGet(IDFF_subPosX);
 char *posS;
 if (x<40) posS="left";
 else if (x>60) posS="right";
 else posS="center";
 sprintf(s,"Horizontal position:  %3i%% (%s)",x,posS);
 SendDlgItemMessage(m_hwnd,IDC_LBL_SUB_POSX,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_subPosY);
 if (x<40) posS="top";
 else if (x>60) posS="bottom";
 else posS="center";
 sprintf(s,"Vertical position:  %3i%% (%s)",x,posS);
 SendDlgItemMessage(m_hwnd,IDC_LBL_SUB_POSY,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETPOS,TRUE,x);
 s[0]='\0';
 deci->getSubFlnm(s,255);
 SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,WM_SETTEXT,0,LPARAM(s));
 setCheck(IDC_CHB_SUB_AUTOFLNM,cfgGet(IDFF_subAutoFlnm));
}

void TsubtitlesPage::applySettings(void)
{
 if (cfgGet(IDFF_inPlayer))
  loadSubtitles();
}

void TsubtitlesPage::loadSubtitles(void)
{
 char flnm[1024];
 SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,WM_GETTEXT,1023,LPARAM(flnm));
 deci->loadSubtitles(flnm);
 sub2dlg(); 
}

HRESULT TsubtitlesPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_SUB_POSX) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_SUB_POSY))
     {
      cfgSet(IDFF_subPosX,SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_GETPOS,0,0));
      cfgSet(IDFF_subPosY,SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_GETPOS,0,0));
      sub2dlg();
      return TRUE;
     }
    else
     break;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_SUBTITLES:
       cfgSet(IDFF_isSubtitles,getCheck(IDC_CHB_SUBTITLES));
       parent->drawInter();
       return TRUE; 
      case IDC_CHB_SUB_AUTOFLNM:
       cfgSet(IDFF_subAutoFlnm,getCheck(IDC_CHB_SUB_AUTOFLNM));
       return TRUE;
      case IDC_BT_SUB_LOADFILE:
       loadSubtitles();
       return TRUE;
     }
    break;   
  } 
 return FALSE;
}

TsubtitlesPage::TsubtitlesPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_SUBTITLES);
}
