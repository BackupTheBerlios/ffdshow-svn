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
#include "CshowMV.h"
#include "resource.h"

void TshowMVpage::init(void)
{
 cfg2dlg();
}

void TshowMVpage::cfg2dlg(void)
{
 showMV2dlg();
}

void TshowMVpage::showMV2dlg(void)
{
}

HRESULT TshowMVpage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_SHOWMV:
       setInter(getCheck(IDC_CHB_SHOWMV));
       parent->drawInter();
       return TRUE;
     }
    break;
  }
 return FALSE;
}

void TshowMVpage::getTip(char *tipS,int len)
{
 sprintf(tipS,"%sshow motion vectors",deci->presetGetFilterIs2(index)?"":"do not ");
}
