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
#include "Cnoise.h"
#include "resource.h"

void TnoisePage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(0,255));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETPAGESIZE,0,16); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETRANGE,TRUE,MAKELPARAM(0,127));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETPAGESIZE,0,16); 
 cfg2dlg();
}

void TnoisePage::cfg2dlg(void)
{
 noise2dlg();
}

void TnoisePage::noise2dlg(void)
{
 setCheck(IDC_RBT_NOISE_MOJ    ,cfgGet(IDFF_noiseMethod)==0);
 setCheck(IDC_RBT_NOISE_AVIH   ,cfgGet(IDFF_noiseMethod)==1);
 setCheck(IDC_CHB_NOISE_UNIFORM,cfgGet(IDFF_uniformNoise));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETPOS,TRUE,writeNoiseStrength(cfgGet(IDFF_noiseStrength)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETPOS,TRUE,writeNoiseStrengthChroma(cfgGet(IDFF_noiseStrengthChroma)));
}

int TnoisePage::writeNoiseStrength(int x)
{
 char s[256];
 sprintf(s,"Luminance noise strength: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOISESTRENGTH,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TnoisePage::writeNoiseStrengthChroma(int x)
{
 char s[256];
 sprintf(s,"Chroma noise strength: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOISESTRENGTH_CHROMA,WM_SETTEXT,0,LPARAM(s));
 return x;
}

HRESULT TnoisePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_NOISESTRENGTH))
     {
      cfgSet(IDFF_noiseStrength,writeNoiseStrength(SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_GETPOS,0,0)));
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA))
     {
      cfgSet(IDFF_noiseStrengthChroma,writeNoiseStrengthChroma(SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_GETPOS,0,0)));
      return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_NOISE:
       cfgSet(IDFF_isNoise,getCheck(IDC_CHB_NOISE));
       parent->drawInter();
       return TRUE;
      case IDC_RBT_NOISE_MOJ:
      case IDC_RBT_NOISE_AVIH:
       cfgSet(IDFF_noiseMethod,getCheck(IDC_RBT_NOISE_AVIH)?1:0);
       return TRUE;
      case IDC_CHB_NOISE_UNIFORM:
       cfgSet(IDFF_uniformNoise,getCheck(IDC_CHB_NOISE_UNIFORM));
       return TRUE;
     }
    break;
  }
 return FALSE;
}

void TnoisePage::getTip(char *tipS,int len)
{
 sprintf(tipS,"method:%s, %sluma strength:%i, chroma strength:%i",(cfgGet(IDFF_noiseMethod)==0)?"old":"avih",cfgGet(IDFF_uniformNoise)?"uniform, ":"",cfgGet(IDFF_noiseStrength),cfgGet(IDFF_noiseStrengthChroma));
}

TnoisePage::TnoisePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 dialogId=IDD_NOISE;
 idffInter=IDFF_isNoise;resInter=IDC_CHB_NOISE;
 idffFull=IDFF_fullNoise;
 idffOrder=IDFF_orderNoise;
 inPreset=1;
 createWindow();
}
