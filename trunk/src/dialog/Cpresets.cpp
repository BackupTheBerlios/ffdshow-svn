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
#include "Cpresets.h"
#include "resource.h"
#include <commctrl.h>
#include <string.h>
#include "IffDecoder.h"

void TpresetsPage::createConfig(void)
{
 cfg2dlg();
}

void TpresetsPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_AUTOPRESET,cfgGet(IDFF_autoPreset));
 setCheck(IDC_CHB_AUTOPRESET_FILEFIRST,cfgGet(IDFF_autoPresetFileFirst));
 enableWindow(IDC_CHB_AUTOPRESET_FILEFIRST,getCheck(IDC_CHB_AUTOPRESET));
}

HRESULT TpresetsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_AUTOPRESET:
       cfgSet(IDFF_autoPreset,getCheck(IDC_CHB_AUTOPRESET));
       enableWindow(IDC_CHB_AUTOPRESET_FILEFIRST,getCheck(IDC_CHB_AUTOPRESET));
       return TRUE;
      case IDC_CHB_AUTOPRESET_FILEFIRST:
       cfgSet(IDFF_autoPresetFileFirst,getCheck(IDC_CHB_AUTOPRESET_FILEFIRST));
       return TRUE;
     }
   break;
  };
 return FALSE;
}

TpresetsPage::TpresetsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_PRESETS);
}
