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
#include "CresizeSettings.h"
#include "resource.h"
#include "IffDecoder.h"
#include "TffdshowPage.h"

const char *TresizeSettingsPage::algorithmsNames[]=
{
 "Fast bilinear",
 "Bilinear",
 "Bicubic",
 "Experimental",
 "Point",
 "Area",
 "Bicublin",
 "Gauss",
 "Sinc",
 "Lanczos",
 "Spline",
 "None"
};

void TresizeSettingsPage::init(void)
{
 for (int i=0;i<sizeof(algorithmsNames)/sizeof(char*);i++)
  SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_ADDSTRING,0,LPARAM(algorithmsNames[i]));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM    ,TBM_SETRANGE,TRUE,MAKELPARAM(0,200));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM    ,TBM_SETLINESIZE,0,5);
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM    ,TBM_SETPAGESIZE,0,20); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM  ,TBM_SETRANGE,TRUE,MAKELPARAM(0,200));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM  ,TBM_SETLINESIZE,0,5);
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM  ,TBM_SETPAGESIZE,0,20); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM  ,TBM_SETRANGE,TRUE,MAKELPARAM(0,200));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM  ,TBM_SETLINESIZE,0,5);
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM  ,TBM_SETPAGESIZE,0,20); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM,TBM_SETRANGE,TRUE,MAKELPARAM(0,200));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM,TBM_SETLINESIZE,0,5);
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM,TBM_SETPAGESIZE,0,20); 

 cfg2dlg();
}

void TresizeSettingsPage::cfg2dlg(void)
{
 resizeSettings2dlg();
}

void TresizeSettingsPage::resizeSettings2dlg(void)
{
 char pomS[256];
 sprintf(pomS,"Luma gaussian blur: %3.2f"  ,float(cfgGet(IDFF_resizeGblurLum    )/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_GBLUR_LUM    ,WM_SETTEXT,0,LPARAM(pomS));
 sprintf(pomS,"Chroma gaussian blur: %3.2f",float(cfgGet(IDFF_resizeGblurChrom  )/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_GBLUR_CHROM  ,WM_SETTEXT,0,LPARAM(pomS));
 sprintf(pomS,"Luma sharpen: %3.2f"        ,float(cfgGet(IDFF_resizeSharpenLum  )/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_SHARPEN_LUM  ,WM_SETTEXT,0,LPARAM(pomS));
 sprintf(pomS,"Chroma sharpen: %3.2f"      ,float(cfgGet(IDFF_resizeSharpenChrom)/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_SHARPEN_CHROM,WM_SETTEXT,0,LPARAM(pomS));

 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM    ,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeGblurLum    ));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM  ,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeGblurChrom  ));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM  ,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeSharpenLum  ));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeSharpenChrom));

 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_SETCURSEL,cfgGet(IDFF_resizeMethod),0);
}

HRESULT TresizeSettingsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM))
     {
      cfgSet(IDFF_resizeGblurLum,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM,TBM_GETPOS,0,0));
      resizeSettings2dlg();
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM))
     {
      cfgSet(IDFF_resizeGblurChrom,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM,TBM_GETPOS,0,0));
      resizeSettings2dlg();
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM))
     {
      cfgSet(IDFF_resizeSharpenLum,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM,TBM_GETPOS,0,0));
      resizeSettings2dlg();
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM))
     {
      cfgSet(IDFF_resizeSharpenChrom,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM,TBM_GETPOS,0,0));
      resizeSettings2dlg();
      return TRUE;
     }    
    else 
     break; 
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_RESIZE:
       cfgSet(IDFF_isResize,getCheck(IDC_CHB_RESIZE));
       return TRUE; 
      case IDC_CBX_RESIZE_METHOD:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         int i=SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_GETCURSEL,0,0);
         cfgSet(IDFF_resizeMethod,i);
         return TRUE;
        }
       break;
     }   
    break;
  }
 return FALSE;
}

void TresizeSettingsPage::getTip(char *tipS,int len)
{
 sprintf(tipS,"Algorithm: %s\nLuma gaussian blur:%3.2f, luma sharpen:%3.2f\nChroma gaussian blur:%3.2f, chroma sharpen:%3.2f",algorithmsNames[cfgGet(IDFF_resizeMethod)],float(cfgGet(IDFF_resizeGblurLum)/100.0),float(cfgGet(IDFF_resizeSharpenLum)/100.0),float(cfgGet(IDFF_resizeGblurChrom)/100.0),float(cfgGet(IDFF_resizeSharpenChrom)/100.0));
}

TresizeSettingsPage::TresizeSettingsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_RESIZESETTINGS);
}
