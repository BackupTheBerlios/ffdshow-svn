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
#include "Csharpen.h"
#include "resource.h"
#include "TffdshowPage.h"
#include "Tconfig.h"
#include "xvid\xvid.h"

void TsharpenPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(1,127));
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETPAGESIZE,0,16); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETRANGE,TRUE,MAKELPARAM(0,255));
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETPAGESIZE,0,16); 
 if ((config.cpu_flags&XVID_CPU_MMXEXT)==0)
  SendDlgItemMessage(m_hwnd,IDC_RBT_SHARPEN_XSHARPEN,WM_SETTEXT,0,LPARAM("xsharpen (will be disabled because of no MMXEXT support)"));
 cfg2dlg();
}

void TsharpenPage::cfg2dlg(void)
{
 sharpen2dlg();
}

void TsharpenPage::sharpen2dlg(void)
{
 int sharpenMethod=cfgGet(IDFF_sharpenMethod);
 setCheck(IDC_RBT_SHARPEN_XSHARPEN   ,(sharpenMethod==0));
 setCheck(IDC_RBT_SHARPEN_UNSHARPMASK,(sharpenMethod==1));
 if (cfgGet(IDFF_xsharp_strength)==0) cfgSet(IDFF_xsharp_strength,1);
 switch  (sharpenMethod)
  {
   case 0:SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETPOS,TRUE,writeSharpenStrength(cfgGet(IDFF_xsharp_strength)));
          enableWindow(IDC_TBR_XSHARPENTHRESH,true);
          enableWindow(IDC_LBL_XSHARPENTHRESH,true);
          SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETPOS,TRUE,writeSharpenThreshold(cfgGet(IDFF_xsharp_threshold)));
          break;
   case 1:SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETPOS,TRUE,writeSharpenStrength(cfgGet(IDFF_unsharp_strength)));
          enableWindow(IDC_TBR_XSHARPENTHRESH,false);
          enableWindow(IDC_LBL_XSHARPENTHRESH,false);
          SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETPOS,TRUE,writeSharpenThreshold(cfgGet(IDFF_unsharp_threshold)));
          break;
  }
}

int TsharpenPage::writeSharpenStrength(int x)
{
 char s[256];
 sprintf(s,"Strength: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_XSHARPENSTRENGTH,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TsharpenPage::writeSharpenThreshold(int x)
{
 char s[256];
 sprintf(s,"Threshold: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_XSHARPENTHRESH,WM_SETTEXT,0,LPARAM(s));
 return x;
}

HRESULT TsharpenPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_XSHARPENSTRENGTH) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_XSHARPENTHRESH))
     {
      switch (cfgGet(IDFF_sharpenMethod))
       {
        case 0:cfgSet(IDFF_xsharp_strength,writeSharpenStrength(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_GETPOS,0,0)));
               cfgSet(IDFF_xsharp_threshold,writeSharpenThreshold(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_GETPOS,0,0)));
               break;
        case 1:cfgSet(IDFF_unsharp_strength,writeSharpenStrength(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_GETPOS,0,0)));
               cfgSet(IDFF_unsharp_threshold,writeSharpenThreshold(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_GETPOS,0,0)));
               break;
       }
      return TRUE;
     }  
    break; 
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_SHARPEN:
       cfgSet(IDFF_isSharpen,getCheck(IDC_CHB_SHARPEN));
       parent->drawInter();
       return TRUE;
      case IDC_RBT_SHARPEN_XSHARPEN:
      case IDC_RBT_SHARPEN_UNSHARPMASK:
       cfgSet(IDFF_sharpenMethod,getCheck(IDC_RBT_SHARPEN_XSHARPEN)?0:1);
       sharpen2dlg();
       return TRUE; 
     }
    break; 
  }    
 return FALSE;
}

void TsharpenPage::interDlg(void)
{
 setCheck(IDC_CHB_SHARPEN,cfgGet(IDFF_isSharpen));
}
void TsharpenPage::getTip(char *tipS,int len)
{
 char tstr[256];
 sprintf(tstr," threshold: %i",cfgGet(IDFF_xsharp_threshold));
 sprintf(tipS,"method:%s, strength:%i%s",(cfgGet(IDFF_sharpenMethod)==0)?"xsharpen":"unsharp mask",(cfgGet(IDFF_sharpenMethod)==0)?cfgGet(IDFF_xsharp_strength):cfgGet(IDFF_unsharp_strength),(cfgGet(IDFF_sharpenMethod)==0)?tstr:"");
}
TsharpenPage::TsharpenPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_SHARPEN);
}
