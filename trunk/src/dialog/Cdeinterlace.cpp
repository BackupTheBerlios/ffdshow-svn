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
#include "Cdeinterlace.h"
#include "resource.h"

const int TdeinterlacePage::deintRbts[]=
{
 IDC_RBT_DEINT_METHOD1,
 IDC_RBT_DEINT_METHOD2,
 IDC_RBT_DEINT_METHOD3,
 IDC_RBT_DEINT_METHOD4,
 IDC_RBT_DEINT_METHOD5,
};

void TdeinterlacePage::init(void)
{
 cfg2dlg();
}

void TdeinterlacePage::cfg2dlg(void)
{
 int method=cfgGet(IDFF_deinterlaceMethod);
 if (method>=5) return;
 setCheck(deintRbts[method],1);
}

HRESULT TdeinterlacePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_DEINTERLACE:
       cfgSet(IDFF_isDeinterlace,getCheck(IDC_CHB_DEINTERLACE));
       parent->drawInter();
       return TRUE;
      default:
       for (int i=0;i<5;i++)
        if (deintRbts[i]==LOWORD(wParam))
         {
          cfgSet(IDFF_deinterlaceMethod,i);
          cfg2dlg();
          return TRUE; 
         } 
       break; 
     }
    break;
  }
 return FALSE;
}

void TdeinterlacePage::getTip(char *tipS,int len)
{
 sprintf(tipS,"method: %i",cfgGet(IDFF_deinterlaceMethod));
}
TdeinterlacePage::TdeinterlacePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 dialogId=IDD_DEINTERLACE;
 idffInter=IDFF_isDeinterlace;resInter=IDC_CHB_DEINTERLACE;
 idffFull=IDFF_fullDeinterlace;
 inPreset=1;
 createWindow();
}
