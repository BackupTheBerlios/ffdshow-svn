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
 cfg2dlg();
}

void ToffsetPage::cfg2dlg(void)
{
 offset2dlg();
}

void ToffsetPage::offset2dlg(void)
{
}

HRESULT ToffsetPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
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
 sprintf(tipS,"Offset (Y:[%i,%i] U:[%i,%i] V:[%i,%i])",cfgGet(IDFF_offsetY_X),cfgGet(IDFF_offsetY_Y),cfgGet(IDFF_offsetU_X),cfgGet(IDFF_offsetU_Y),cfgGet(IDFF_offsetV_X),cfgGet(IDFF_offsetV_Y));
}
ToffsetPage::ToffsetPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_OFFSET);
}
