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
#include "IffDecoder.h"
#include "TffdshowPage.h"
#include "Csubtitles.h"
#include "resource.h"

void TsubtitlesPage::init(void)
{
 red=NULL;
 SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,CB_LIMITTEXT,1023,0);
 
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETLINESIZE,0,2);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETPAGESIZE,0,10); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETLINESIZE,0,2);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETPAGESIZE,0,10); 
 
 cfg2dlg();
}

void TsubtitlesPage::cfg2dlg(void)
{
 interDlg();
 sub2dlg();
}

void TsubtitlesPage::getPosHoriz(int x,char *s)
{
 char *posS;
 if (x<40) posS="left";
 else if (x>60) posS="right";
 else posS="center";
 sprintf(s,"Horizontal position: %3i%% (%s)",x,posS);
}
void TsubtitlesPage::getPosVert(int x,char *s)
{
 char *posS;
 if (x<40) posS="top";
 else if (x>60) posS="bottom";
 else posS="center";
 sprintf(s,"Vertical position: %3i%% (%s)",x,posS);
}
void TsubtitlesPage::sub2dlg(void)
{
 char s[256];int x;
 x=cfgGet(IDFF_subPosX);
 getPosHoriz(x,s);
 SendDlgItemMessage(m_hwnd,IDC_LBL_SUB_POSX,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_subPosY);
 getPosVert(x,s);
 SendDlgItemMessage(m_hwnd,IDC_LBL_SUB_POSY,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETPOS,TRUE,x);
 s[0]='\0';
 deci->presetGetParamStr(index,IDFF_subFlnm,s,255);
 SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,WM_SETTEXT,0,LPARAM(s));
 setCheck(IDC_CHB_SUB_AUTOFLNM,cfgGet(IDFF_subAutoFlnm));
 x=cfgGet(IDFF_subDelay);
 SendDlgItemMessage(m_hwnd,IDC_ED_SUB_DELAY,WM_SETTEXT,0,LPARAM(_itoa(x,s,10)));
 x=cfgGet(IDFF_subSpeed);
 SendDlgItemMessage(m_hwnd,IDC_ED_SUB_SPEED,WM_SETTEXT,0,LPARAM(_itoa(x,s,10)));
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
 deci->presetPutParamStr(index,IDFF_subFlnm,flnm);
 sub2dlg(); 
}

HRESULT TsubtitlesPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    if (red) DeleteObject(red);
    return TRUE;
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
       setInter(getCheck(IDC_CHB_SUBTITLES));
       parent->drawInter();
       return TRUE; 
      case IDC_CHB_SUB_AUTOFLNM:
       cfgSet(IDFF_subAutoFlnm,getCheck(IDC_CHB_SUB_AUTOFLNM));
       return TRUE;
      case IDC_BT_SUB_LOADFILE:
       loadSubtitles();
       return TRUE;
      case IDC_ED_SUB_DELAY:
       if (HIWORD(wParam)==EN_CHANGE)
        {
         HWND hed=GetDlgItem(m_hwnd,IDC_ED_SUB_DELAY);
         if (hed!=GetFocus()) return FALSE;
         InvalidateRect(hed,NULL,TRUE);
         BOOL ok;
         int delay=GetDlgItemInt(m_hwnd,IDC_ED_SUB_DELAY,&ok,TRUE);
         if (ok) cfgSet(IDFF_subDelay,delay);
         return TRUE; 
        }
       break; 
      case IDC_ED_SUB_SPEED:
       if (HIWORD(wParam)==EN_CHANGE) 
        {
         HWND hed=GetDlgItem(m_hwnd,IDC_ED_SUB_SPEED);
         if (hed!=GetFocus()) return FALSE;
         InvalidateRect(hed,NULL,TRUE);
         BOOL ok;
         int speed=GetDlgItemInt(m_hwnd,IDC_ED_SUB_SPEED,&ok,TRUE);
         if (ok && speed>0) cfgSet(IDFF_subSpeed,speed);
         return TRUE; 
        }
       break; 
     }
    break;   
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     if (hwnd!=GetDlgItem(m_hwnd,IDC_ED_SUB_DELAY) && hwnd!=GetDlgItem(m_hwnd,IDC_ED_SUB_SPEED)) return FALSE;
     if (!red) red=CreateSolidBrush(RGB(255,0,0));
     char pomS[256];
     SendMessage(hwnd,WM_GETTEXT,255,LPARAM(pomS));
     char *end;
     int val=strtol(pomS,&end,10);
     if (*end!='\0' || (hwnd==GetDlgItem(m_hwnd,IDC_ED_SUB_SPEED) && val<=0))
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return HRESULT(red); 
      }
     else return FALSE;  
    }
  } 
 return FALSE;
}

void TsubtitlesPage::getTip(char *tipS,int len)
{
 char horiz[256],vert[256];getPosHoriz(cfgGet(IDFF_subPosX),horiz);getPosVert(cfgGet(IDFF_subPosY),vert);
 sprintf(tipS,"%s, %s",horiz,vert);
 int delay=cfgGet(IDFF_subDelay);
 if (delay!=cfgGet(IDFF_subDelayDef))
  {
   char pomS[256];
   sprintf(pomS,"\nDelay: %i ms",delay);
   strcat(tipS,pomS);
  }
 int speed=cfgGet(IDFF_subSpeed);
 if (speed!=cfgGet(IDFF_subSpeedDef))
  {
   char pomS[256];
   sprintf(pomS,"\nSpeed: %i/1000",speed);
   strcat(tipS,pomS);
  }
}
