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
 "Fast bilinear", //0
 "Bilinear",      //1
 "Bicubic",       //2
 "Experimental",  //3
 "Point",         //4
 "Area",          //5
 "Bicublin",      //6
 "Gauss",         //7
 "Sinc",          //8
 "Lanczos",       //9
 "Spline",        //10
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

void TresizeSettingsPage::param2dlg(void)
{
 int max,step1,step2,pos;
 char pomS[256];
 __asm emms;
 float realPos;
 switch (cfgGet(IDFF_resizeMethod))
  {
   //bicubic
   case 2:max=400;step1=10;step2=40;pos=cfgGet(IDFF_resizeBicubicParam);
          realPos=-0.01*pos;
          pos=400-pos;
          break;
   //experimental
   case 3:max=100;step1=5 ;step2=20;pos=cfgGet(IDFF_resizeXparam);
          realPos=0.1*pos;
          break;
   //gauss
   case 7:max=100;step1=5 ;step2=20;pos=cfgGet(IDFF_resizeGaussParam);
          realPos=0.1*pos;
          break;
   //lanczos
   case 9:max=10 ;step1=1 ;step2=2 ;pos=cfgGet(IDFF_resizeLanczosParam);
          realPos=pos;
          break;
   default:enableWindow(IDC_TBR_RESIZE_PARAM,0);
           enableWindow(IDC_LBL_RESIZE_PARAM,0);
           SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_PARAM,TBM_SETPOS,TRUE,0);
           SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_PARAM,WM_SETTEXT,0,LPARAM("Parameter:"));
           return;
  };
 enableWindow(IDC_TBR_RESIZE_PARAM,1);
 enableWindow(IDC_LBL_RESIZE_PARAM,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_PARAM,TBM_SETRANGE,TRUE,MAKELPARAM(0,max));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_PARAM,TBM_SETLINESIZE,0,step1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_PARAM,TBM_SETPAGESIZE,0,step2); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_PARAM,TBM_SETPOS,TRUE,pos);
 if (realPos==0)
  strcpy(pomS,"Parameter: default");
 else
  sprintf(pomS,"Parameter: %3.2f",realPos);
 SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_PARAM,WM_SETTEXT,0,LPARAM(pomS));
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
 param2dlg();
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
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_PARAM))
     {
      int pos=SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_PARAM,TBM_GETPOS,0,0);
      int id;
      switch (cfgGet(IDFF_resizeMethod))
       {
        //bicubic
        case 2:id=IDFF_resizeBicubicParam;pos=400-pos;break;
        //experimental
        case 3:id=IDFF_resizeXparam;break;
        //gauss
        case 7:id=IDFF_resizeGaussParam;break;
        //lanczos
        case 9:id=IDFF_resizeLanczosParam;break;
        default:return TRUE;
       };
      cfgSet(id,pos);
      param2dlg();
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
         param2dlg();
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
 dialogId=IDD_RESIZESETTINGS;
 inPreset=1;
 createWindow();
}
