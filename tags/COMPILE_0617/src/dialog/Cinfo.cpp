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
#include "Cinfo.h"
#include "resource.h"
#include "IffDecoder.h"

#define WM_FFONINFO WM_APP+2

void TinfoPage::init(void)
{
 cfg2dlg();
 deci->setOnInfoMsg(m_hwnd,WM_FFONINFO);
}

void TinfoPage::cfg2dlg(void)
{
 char pomS[1024];
 char AVIname[1024];
 deci->getAVIname(AVIname,1023);
 sprintf(pomS,"Now playing: %s",AVIname);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_PLAYING,WM_SETTEXT,0,LPARAM(pomS));
 char AVIfourcc[20];
 deci->getAVIfourcc(AVIfourcc,19); 
 sprintf(pomS,"Codec: %s",AVIfourcc);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_CODEC,WM_SETTEXT,0,LPARAM(pomS));
 unsigned int x,y;
 if (deci->getAVIdimensions(&x,&y)!=S_OK)
  SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_DIMENSIONS,WM_SETTEXT,0,LPARAM("Dimensions:"));
 else
  {
   sprintf(pomS,"Dimensions: %i x %i",x,y);
   SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_DIMENSIONS,WM_SETTEXT,0,LPARAM(pomS));
  } 
 if (deci->getAVIfps(&x)!=S_OK)
  SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_FPS,WM_SETTEXT,0,LPARAM("FPS:"));
 else
  {
   __asm emms;
   sprintf(pomS,"FPS: %6.2f",float(x/1000.0));
   SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_FPS,WM_SETTEXT,0,LPARAM(pomS));
  } 
}

HRESULT TinfoPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 if (uMsg==WM_FFONINFO)
  {
   char pomS[256];
   sprintf(pomS,"Decoder FPS: %i",wParam/1000);
   SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_DECODERFPS,WM_SETTEXT,0,LPARAM(pomS));
   sprintf(pomS,"Current frame: %i",lParam);
   SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_FRAME     ,WM_SETTEXT,0,LPARAM(pomS));
  }
 else if (uMsg==WM_DESTROY)
  deci->setOnInfoMsg(NULL,0);
 return FALSE;
}

TinfoPage::TinfoPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_INFO);
}
