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
#include "Ctray.h"
#include "resource.h"
#include <commctrl.h>
#include <string.h>
#include "IffDecoder.h"

void TtrayPage::createConfig(void)
{
 cfg2dlg();
}

void TtrayPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_TRAYICON,cfgGet(IDFF_trayIcon));
}

HRESULT TtrayPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_TRAYICON:
       cfgSet(IDFF_trayIcon,getCheck(IDC_CHB_TRAYICON));
       return TRUE;
     }
    break; 
  }    
 return FALSE;
}

TtrayPage::TtrayPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_TRAY);
}
