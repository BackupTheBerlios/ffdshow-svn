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
#include "Cresize.h"
#include "resource.h"
#include "IffDecoder.h"
#include "TffdshowPage.h"

void TresizePage::init(void)
{
 red=NULL;
 cfg2dlg();
}

void TresizePage::cfg2dlg(void)
{
 resize2dlg();
}

void TresizePage::resize2dlg(void)
{
 setCheck(IDC_CHB_RESIZE,cfgGet(IDFF_isResize));
 SetDlgItemInt(m_hwnd,IDC_ED_RESIZEDX,cfgGet(IDFF_resizeDx),0);
 SetDlgItemInt(m_hwnd,IDC_ED_RESIZEDY,cfgGet(IDFF_resizeDy),0);
}

bool TresizePage::applyResizeXY(bool checkOnly)
{
 BOOL ok;
 int x=GetDlgItemInt(m_hwnd,IDC_ED_RESIZEDX,&ok,FALSE);
 if (!ok || x<8 || x>2048 || (x&15)) return false;
 int y=GetDlgItemInt(m_hwnd,IDC_ED_RESIZEDY,&ok,FALSE);
 if (!ok || y<8 || y>2048 || (y&15)) return false;
 if (!checkOnly)
  {
   cfgSet(IDFF_resizeDx,x);
   cfgSet(IDFF_resizeDy,y);
  };
 parent->setChange(); 
 return true;
}
bool TresizePage::sizeOK(HWND hed)
{
 char pomS[256];
 SendMessage(hed,WM_GETTEXT,255,LPARAM(pomS));
 char *stop=NULL;
 int x=strtoul(pomS,&stop,10);
 if (*stop || x<8 || x>2048 || (x&15)) return false;
 return true;
}

HRESULT TresizePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    if (red) DeleteObject(red);
    return TRUE;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_RESIZE:
       cfgSet(IDFF_isResize,getCheck(IDC_CHB_RESIZE));
       return TRUE; 
      case IDC_ED_RESIZEDX:
      case IDC_ED_RESIZEDY:
       if (HIWORD(wParam)==EN_CHANGE) 
        {
         InvalidateRect(GetDlgItem(m_hwnd,LOWORD(wParam)),NULL,TRUE);
         applyResizeXY(false);
         return TRUE;  
        };
       break;
     }   
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     if (hwnd!=GetDlgItem(m_hwnd,IDC_ED_RESIZEDX) && hwnd!=GetDlgItem(m_hwnd,IDC_ED_RESIZEDY)) return FALSE;
     if (!red) red=CreateSolidBrush(RGB(255,0,0));
     if (!sizeOK(hwnd))
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return HRESULT(red); 
      }
     else return FALSE;  
    }; 
  }; 
 return FALSE;
}

TresizePage::TresizePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_RESIZE);
}
