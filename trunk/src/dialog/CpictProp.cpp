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
#include "CpictProp.h"
#include "resource.h"

void TpictPropPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETRANGE,TRUE,MAKELPARAM(0,256));
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETPAGESIZE,0,32); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETRANGE,TRUE,MAKELPARAM(0,512));
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETPAGESIZE,0,64); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETRANGE,TRUE,MAKELPARAM(1,400));
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETPAGESIZE,0,40); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETRANGE,TRUE,MAKELPARAM(-180,180));
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETPAGESIZE,0,20); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETRANGE,TRUE,MAKELPARAM(0,128));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETPAGESIZE,0,8); 
 cfg2dlg();
}

void TpictPropPage::cfg2dlg(void)
{
 pictProp2dlg();
}

void TpictPropPage::pictProp2dlg(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETPOS,TRUE,writeLumGain(cfgGet(IDFF_lumGain)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETPOS,TRUE,writeLumOffset(cfgGet(IDFF_lumOffset)+256));
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETPOS,TRUE,writeGamma(cfgGet(IDFF_gammaCorrection)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETPOS,TRUE,writeHue(cfgGet(IDFF_hue)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETPOS,TRUE,writeSaturation(cfgGet(IDFF_saturation)));
}

int TpictPropPage::writeLumGain(int x)
{
 char s[256];
 sprintf(s,"Luminance gain:  %i",x);
 if (x==cfgGet(IDFF_lumGainDef)) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_LUMGAIN,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TpictPropPage::writeLumOffset(int x)
{
 char s[256];
 sprintf(s,"Luminance offset:  %i",x-256);
 if (x-256==cfgGet(IDFF_lumOffsetDef)) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_LUMOFFSET,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TpictPropPage::writeGamma(int x)
{
 char s[256];
 __asm emms;
 sprintf(s,"Gamma correction:  %3.2f",float(x/100.0));
 if (x==cfgGet(IDFF_gammaCorrectionDef)) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_GAMMA,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TpictPropPage::writeHue(int x)
{
 char s[256];
 sprintf(s,"Hue:  %i",x);
 if (x==cfgGet(IDFF_hueDef)) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_HUE,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TpictPropPage::writeSaturation(int x)
{
 char s[256];
 sprintf(s,"Saturation:  %i",x);
 if (x==cfgGet(IDFF_saturationDef)) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_SATURATION,WM_SETTEXT,0,LPARAM(s));
 return x;
}

HRESULT TpictPropPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_LUMGAIN) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_LUMOFFSET) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_GAMMA))
     {
      cfgSet(IDFF_lumGain,writeLumGain(SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_GETPOS,0,0)));
      cfgSet(IDFF_lumOffset,writeLumOffset(SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_GETPOS,0,0))-256);
      cfgSet(IDFF_gammaCorrection,writeGamma(SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_GETPOS,0,0)));
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_HUE) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_SATURATION))
     {
      cfgSet(IDFF_hue,writeHue(SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_GETPOS,0,0)));
      cfgSet(IDFF_saturation,writeSaturation(SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_GETPOS,0,0)));
      return TRUE;
     }   
    break; 
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_PICTPROP:
       cfgSet(IDFF_isPictProp,getCheck(IDC_CHB_PICTPROP));
       parent->drawInter();
       return TRUE;
      case IDC_BT_LUMRESET:
       SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN   ,TBM_SETPOS,TRUE,writeLumGain(cfgSet(IDFF_lumGain,cfgGet(IDFF_lumGainDef))));
       SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET ,TBM_SETPOS,TRUE,writeLumOffset(256+(cfgSet(IDFF_lumOffset,cfgGet(IDFF_lumOffsetDef)))));
       SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA     ,TBM_SETPOS,TRUE,writeGamma(cfgSet(IDFF_gammaCorrection,cfgGet(IDFF_gammaCorrectionDef))));
       SendDlgItemMessage(m_hwnd,IDC_TBR_HUE       ,TBM_SETPOS,TRUE,writeHue(cfgSet(IDFF_hue,cfgGet(IDFF_hueDef))));
       SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETPOS,TRUE,writeSaturation(cfgSet(IDFF_saturation,cfgGet(IDFF_saturationDef))));
       return TRUE;
     }
    break;
  }   
 return FALSE;
}

void TpictPropPage::interDlg(void)
{
 setCheck(IDC_CHB_PICTPROP,cfgGet(IDFF_isPictProp));
}
void TpictPropPage::getTip(char *tipS,int len)
{
 __asm emms;
 sprintf(tipS,"Picture properties (lum gain:%i, lum offset:%i, gamma:%5.2f, hue:%i, saturation:%i)",cfgGet(IDFF_lumGain),cfgGet(IDFF_lumOffset),float(cfgGet(IDFF_gammaCorrection)/100.0),cfgGet(IDFF_hue),cfgGet(IDFF_saturation));
}
TpictPropPage::TpictPropPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_PICTPROP);
}
