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
#include "CresizePage.h"
#include "resource.h"
#include <commctrl.h>
#include <string.h>
#include <assert.h>
#include "IffDecoder.h"
#include "Tpostproc.h"

void TresizePage::aspect2dlg(void)
{
 int ra=cfgGet(IDFF_resizeAspect);
 setCheck(IDC_RBT_ASPECT_NO  ,ra==0);
 setCheck(IDC_RBT_ASPECT_KEEP,ra==1);
 setCheck(IDC_RBT_ASPECT_USER,ra==2);
 char pomS[256];
 int dx,dy;
 deci->get_AVIdimensions(&dx,&dy);
 __asm emms;
 if (dx!=0 && dy!=0)
  {
   sprintf(pomS,"Keep original aspect ratio (%3.2f:1)",float(dx)/dy);
   SendDlgItemMessage(m_hwnd,IDC_RBT_ASPECT_KEEP,WM_SETTEXT,0,LPARAM(pomS));
  };
 int aspectI=cfgGet(IDFF_aspectRatio);
 sprintf(pomS,"%3.2f:1",float(aspectI/65536.0));
 SendDlgItemMessage(m_hwnd,IDC_LBL_ASPECT_USER,WM_SETTEXT,0,LPARAM(pomS));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETPOS,TRUE,aspectI/256);
}
void TresizePage::resize2dlg(void)
{
 setCheck(IDC_CHB_RESIZE,cfgGet(IDFF_isResize));
 char pomS[256];
 SendDlgItemMessage(m_hwnd,IDC_ED_RESIZEDX,WM_SETTEXT,0,LPARAM(_itoa(cfgGet(IDFF_resizeDx),pomS,10)));
 SendDlgItemMessage(m_hwnd,IDC_ED_RESIZEDY,WM_SETTEXT,0,LPARAM(_itoa(cfgGet(IDFF_resizeDy),pomS,10)));
 __asm emms;
 sprintf(pomS,"Luma gaussian blur:  %3.2f"  ,float(cfgGet(IDFF_resizeGblurLum    )/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_GBLUR_LUM    ,WM_SETTEXT,0,LPARAM(pomS));
 sprintf(pomS,"Chroma gaussian blur:  %3.2f",float(cfgGet(IDFF_resizeGblurChrom  )/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_GBLUR_CHROM  ,WM_SETTEXT,0,LPARAM(pomS));
 sprintf(pomS,"Luma sharpen:  %3.2f"        ,float(cfgGet(IDFF_resizeSharpenLum  )/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_SHARPEN_LUM  ,WM_SETTEXT,0,LPARAM(pomS));
 sprintf(pomS,"Chroma sharpen:  %3.2f"      ,float(cfgGet(IDFF_resizeSharpenChrom)/100.0));SendDlgItemMessage(m_hwnd,IDC_LBL_RESIZE_SHARPEN_CHROM,WM_SETTEXT,0,LPARAM(pomS));

 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM    ,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeGblurLum    ));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM  ,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeGblurChrom  ));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM  ,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeSharpenLum  ));
 SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM,TBM_SETPOS,TRUE,cfgGet(IDFF_resizeSharpenChrom));

 setCheck(IDC_CHB_RESIZE_FIRST,cfgGet(IDFF_resizeFirst));
 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_SETCURSEL,cfgGet(IDFF_resizeMethod),0);
}
void TresizePage::crop2dlg(void)
{
 setCheck(IDC_CHB_CROP,cfgGet(IDFF_isCrop));
}
void TresizePage::cfg2dlg(void)
{
 resize2dlg();
 aspect2dlg();
 crop2dlg();
}

bool TresizePage::applyResizeXY(bool checkOnly)
{
 BOOL ok;
 int x=GetDlgItemInt(m_hwnd,IDC_ED_RESIZEDX,&ok,FALSE);
 if (!ok || x<8 || x>2048 || (x&15)) return false;
 int y=GetDlgItemInt(m_hwnd,IDC_ED_RESIZEDY,&ok,FALSE);
 if (!ok || y<8 || y>2048 || (y&15)) return false;
 if (!checkOnly)
  {
   cfgSet(IDFF_resizeDx,x);
   cfgSet(IDFF_resizeDy,y);
  };
 return true;
}

void TresizePage::createConfig(void)
{
 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_ADDSTRING,0,LPARAM("FAST_BILINEAR"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_ADDSTRING,0,LPARAM("BILINEAR"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_ADDSTRING,0,LPARAM("BICUBIC"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_ADDSTRING,0,LPARAM("EXPERIMENTAL"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_ADDSTRING,0,LPARAM("POINT"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_ADDSTRING,0,LPARAM("AREA"));
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

 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETRANGE,TRUE,MAKELPARAM(0.1*256,5*256));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETLINESIZE,0,0.1*256);
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETPAGESIZE,0,1.6*256); 

 cfg2dlg();
}

HRESULT TresizePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
  //case WM_SHOWWINDOW:
  // if (wParam) cfg2dlg();
  // break;
   
  case WM_HSCROLL:
   if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_ASPECT_USER))
    {
     int a=SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_GETPOS,0,0);
     cfgSet(IDFF_aspectRatio,a*256);
     aspect2dlg();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM))
    {
     cfgSet(IDFF_resizeGblurLum,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_LUM,TBM_GETPOS,0,0));
     resize2dlg();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM))
    {
     cfgSet(IDFF_resizeGblurChrom,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_GBLUR_CHROM,TBM_GETPOS,0,0));
     resize2dlg();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM))
    {
     cfgSet(IDFF_resizeSharpenLum,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_LUM,TBM_GETPOS,0,0));
     resize2dlg();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM))
    {
     cfgSet(IDFF_resizeSharpenChrom,SendDlgItemMessage(m_hwnd,IDC_TBR_RESIZE_SHARPEN_CHROM,TBM_GETPOS,0,0));
     resize2dlg();
     return TRUE;
    }
   else break;
   
  case WM_COMMAND:
   switch (LOWORD(wParam))  
    {
    case IDC_CHB_RESIZE:
     cfgSet(IDFF_isResize,getCheck(IDC_CHB_RESIZE));
     return TRUE; 
    case IDC_CHB_RESIZE_FIRST:
     cfgSet(IDFF_resizeFirst,getCheck(IDC_CHB_RESIZE_FIRST));
     return TRUE; 
    case IDC_CBX_RESIZE_METHOD:
     if (HIWORD(wParam)==CBN_SELCHANGE)
      {
       int i=SendDlgItemMessage(m_hwnd,IDC_CBX_RESIZE_METHOD,CB_GETCURSEL,0,0);
       cfgSet(IDFF_resizeMethod,i);
       return TRUE;
      }
     break;
    case IDC_RBT_ASPECT_NO:
     cfgSet(IDFF_resizeAspect,0);
     aspect2dlg();
     return TRUE;
    case IDC_RBT_ASPECT_KEEP:
     cfgSet(IDFF_resizeAspect,1);
     aspect2dlg();
     return TRUE;
    case IDC_RBT_ASPECT_USER:
     cfgSet(IDFF_resizeAspect,2);
     aspect2dlg();
     return TRUE;
     
    case IDC_ED_RESIZEDX:
    case IDC_ED_RESIZEDY:
     if (HIWORD(wParam)==EN_CHANGE) 
      {
       enableWindow(IDC_BT_RESIZE_SET,applyResizeXY(true));
       return TRUE;  
      };
     break;
    case IDC_BT_RESIZE_SET:
     applyResizeXY(false);
     return TRUE;
    case IDC_CHB_CROP:
     cfgSet(IDFF_isCrop,getCheck(IDC_CHB_CROP));
     return TRUE; 
   };
   break;
  };
 return FALSE;
};

void TresizePage::applySettings(void)
{
 applyResizeXY(false);
}

TresizePage::TresizePage(HWND IhwndParent,IffDecoder *Ideci,int dialogId) :TconfPage(IhwndParent,Ideci)
{
 createWindow(dialogId);
}
