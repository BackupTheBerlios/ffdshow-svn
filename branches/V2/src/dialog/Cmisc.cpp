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
#include "Cmisc.h"
#include "resource.h"
#include "IffDecoder.h"

const char *TmiscPage::idctNames[]=
{
 "simple (16383)",
 "normal",
 "reference",
 "simple (16384) - deprecated: mapped to simple 16383)",
 "XviD"
};

void TmiscPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_RESETCONTENT,0,0);
 for (int i=0;i<sizeof(idctNames)/sizeof(char*);i++)
  SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_ADDSTRING,0,LPARAM(idctNames[i]));
 cfg2dlg();
}

void TmiscPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_FLIP,cfgGet(IDFF_flip));
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_SETCURSEL,cfgGet(IDFF_idct),0);
}

HRESULT TmiscPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_FLIP:
       cfgSet(IDFF_flip,getCheck(IDC_CHB_FLIP));
       return TRUE;
      case IDC_CBX_IDCT:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         int i=SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_GETCURSEL,0,0);
         cfgSet(IDFF_idct,i);
         return TRUE;
        }
       else
        break;
     }
    break;
  }
 return FALSE;
}

void TmiscPage::getTip(char *tipS,int len)
{
 if (cfgGet(IDFF_flip)) strcpy(tipS,"Flipping video\n");
 else tipS[0]='\0';
 strcat(tipS,"IDCT: ");strcat(tipS,idctNames[cfgGet(IDFF_idct)]);
}

TmiscPage::TmiscPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_MISC);
}
