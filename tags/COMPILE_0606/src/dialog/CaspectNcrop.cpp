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
#include "CaspectNcrop.h"
#include "resource.h"

void TaspectNcropPage::init(void)
{
 red=NULL;
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETRANGE,TRUE,MAKELPARAM(0.1*256,5*256));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETLINESIZE,0,0.1*256);
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETPAGESIZE,0,1.6*256); 

 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMX,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMX,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMX,TBM_SETPAGESIZE,0,8); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMY,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMY,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMY,TBM_SETPAGESIZE,0,8); 

 cfg2dlg();
}

void TaspectNcropPage::cfg2dlg(void)
{
 interDlg();
 aspect2dlg();
 crop2dlg();
}

void TaspectNcropPage::aspect2dlg(void)
{
 int ra=cfgGet(IDFF_resizeAspect);
 setCheck(IDC_RBT_ASPECT_NO  ,ra==0);
 setCheck(IDC_RBT_ASPECT_KEEP,ra==1);
 setCheck(IDC_RBT_ASPECT_USER,ra==2);
 char pomS[256];
 unsigned int dx,dy;
 deci->getAVIdimensions(&dx,&dy);
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

void TaspectNcropPage::crop2dlg(void)
{
 setCheck(IDC_RBT_ZOOM,cfgGet(IDFF_isZoom));
 setCheck(IDC_RBT_CROP,!cfgGet(IDFF_isZoom));
 setCheck(IDC_CHB_MAGNIFICATION_LOCKED,cfgGet(IDFF_magnificationLocked));
 enableWindow(IDC_LBL_ZOOMY,!cfgGet(IDFF_magnificationLocked));
 enableWindow(IDC_TBR_ZOOMY,!cfgGet(IDFF_magnificationLocked));
 int x=cfgGet(IDFF_magnificationX);
 char s[256];
 sprintf(s,"Horizontal magnification:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_ZOOMX,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMX,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_magnificationY);
 sprintf(s,"Vertical magnification:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_ZOOMY,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMY,TBM_SETPOS,TRUE,x);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_LEFT  ,cfgGet(IDFF_cropLeft  ),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_TOP   ,cfgGet(IDFF_cropTop   ),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_RIGHT ,cfgGet(IDFF_cropRight ),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_BOTTOM,cfgGet(IDFF_cropBottom),0);
}

void TaspectNcropPage::interDlg(void)
{
 setCheck(IDC_CHB_CROP,cfgGet(IDFF_isCropNzoom));
}

bool TaspectNcropPage::cropOK(HWND hed)
{
 char pomS[256];
 SendMessage(hed,WM_GETTEXT,255,LPARAM(pomS));
 if (pomS[0]=='\0') return false;
 char *stop=NULL;
 int x=strtoul(pomS,&stop,10);
 if (*stop || x<0 || x>2048) return false;
 return true;
}

HRESULT TaspectNcropPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    if (red) DeleteObject(red);
    return TRUE;
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_ASPECT_USER))
     {
      int a=SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_GETPOS,0,0);
      cfgSet(IDFF_aspectRatio,a*256);
      aspect2dlg();
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_ZOOMX) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_ZOOMY))
     {
      cfgSet(IDFF_magnificationX,SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMX,TBM_GETPOS,0,0));
      cfgSet(IDFF_magnificationY,SendDlgItemMessage(m_hwnd,IDC_TBR_ZOOMY,TBM_GETPOS,0,0));
      crop2dlg();
      return TRUE;
     }
    else 
     break; 
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_CROP:
       cfgSet(IDFF_isCropNzoom,getCheck(IDC_CHB_CROP));
       parent->drawInter();
       return TRUE; 
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
      case IDC_RBT_ZOOM:
      case IDC_RBT_CROP:
       cfgSet(IDFF_isZoom,getCheck(IDC_RBT_ZOOM));
       return TRUE; 
      case IDC_CHB_MAGNIFICATION_LOCKED:
       cfgSet(IDFF_magnificationLocked,getCheck(IDC_CHB_MAGNIFICATION_LOCKED));
       crop2dlg();
       return TRUE;
      case IDC_ED_CROP_LEFT:
      case IDC_ED_CROP_RIGHT:
      case IDC_ED_CROP_TOP:
      case IDC_ED_CROP_BOTTOM:
       if (HIWORD(wParam)==EN_CHANGE) 
        {
         InvalidateRect(GetDlgItem(m_hwnd,LOWORD(wParam)),NULL,TRUE);
         BOOL ok;
         if (cropOK(GetDlgItem(m_hwnd,IDC_ED_CROP_LEFT  ))) cfgSet(IDFF_cropLeft  ,GetDlgItemInt(m_hwnd,IDC_ED_CROP_LEFT  ,&ok,FALSE));
         if (cropOK(GetDlgItem(m_hwnd,IDC_ED_CROP_RIGHT ))) cfgSet(IDFF_cropRight ,GetDlgItemInt(m_hwnd,IDC_ED_CROP_RIGHT ,&ok,FALSE));
         if (cropOK(GetDlgItem(m_hwnd,IDC_ED_CROP_TOP   ))) cfgSet(IDFF_cropTop   ,GetDlgItemInt(m_hwnd,IDC_ED_CROP_TOP   ,&ok,FALSE));
         if (cropOK(GetDlgItem(m_hwnd,IDC_ED_CROP_BOTTOM))) cfgSet(IDFF_cropBottom,GetDlgItemInt(m_hwnd,IDC_ED_CROP_BOTTOM,&ok,FALSE));
         return TRUE;  
        }; 
       break;
     }
    break; 
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     if (hwnd!=GetDlgItem(m_hwnd,IDC_ED_CROP_LEFT) && hwnd!=GetDlgItem(m_hwnd,IDC_ED_CROP_RIGHT) && hwnd!=GetDlgItem(m_hwnd,IDC_ED_CROP_TOP) && hwnd!=GetDlgItem(m_hwnd,IDC_ED_CROP_BOTTOM)) return FALSE;
     if (!red) red=CreateSolidBrush(RGB(255,0,0));
     if (!cropOK(hwnd))
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return HRESULT(red); 
      }
     else return FALSE;  
    }; 
  }   
 return FALSE;
}
void TaspectNcropPage::getTip(char *tipS,int len)
{
 strcpy(tipS,"Crop & zoom (");
 char pomS[256];
 if (cfgGet(IDFF_isZoom))
  if (cfgGet(IDFF_magnificationLocked))
   sprintf(pomS,"zoom: %i%%)",cfgGet(IDFF_magnificationX));
  else
   sprintf(pomS,"horizontal zoom: %i%%, vertical zoom: %i%%)",cfgGet(IDFF_magnificationX),cfgGet(IDFF_magnificationY));
 else
  sprintf(pomS,"crop: left:%i, top:%i, right:%i, bottom:%i)",cfgGet(IDFF_cropLeft),cfgGet(IDFF_cropTop),cfgGet(IDFF_cropRight),cfgGet(IDFF_cropBottom));
 strcat(tipS,pomS);
}
TaspectNcropPage::TaspectNcropPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_ASPECTNCROP);
}
