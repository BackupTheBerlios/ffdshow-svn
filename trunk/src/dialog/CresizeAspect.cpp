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
#include "CresizeAspect.h"
#include "resource.h"
#include "IffDecoder.h"
#include "TffdshowPage.h"

void TresizeAspectPage::init(void)
{
 red=NULL;

 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETRANGE,TRUE,MAKELPARAM(0.1*256,5*256));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETLINESIZE,0,0.1*256);
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETPAGESIZE,0,1.6*256); 

 cfg2dlg();
}

void TresizeAspectPage::cfg2dlg(void)
{
 resize2dlg();
 aspect2dlg();
}

void TresizeAspectPage::resize2dlg(void)
{
 setCheck(IDC_CHB_RESIZE,cfgGet(IDFF_isResize));
 SetDlgItemInt(m_hwnd,IDC_ED_RESIZEDX,cfgGet(IDFF_resizeDx),0);
 SetDlgItemInt(m_hwnd,IDC_ED_RESIZEDY,cfgGet(IDFF_resizeDy),0);
}

void TresizeAspectPage::aspect2dlg(void)
{
 int ra=cfgGet(IDFF_isAspect);
 setCheck(IDC_RBT_ASPECT_NO  ,ra==0);
 setCheck(IDC_RBT_ASPECT_KEEP,ra==1);
 setCheck(IDC_RBT_ASPECT_USER,ra==2);
 char pomS[256];
 unsigned int dx,dy;
 deci->getAVIdimensions(&dx,&dy);
 if (dx!=0 && dy!=0)
  {
   sprintf(pomS,"Keep original aspect ratio (%3.2f:1)",float(dx)/dy);
   SendDlgItemMessage(m_hwnd,IDC_RBT_ASPECT_KEEP,WM_SETTEXT,0,LPARAM(pomS));
  }
 int aspectI=cfgGet(IDFF_aspectRatio);
 sprintf(pomS,"%3.2f:1",float(aspectI/65536.0));
 SendDlgItemMessage(m_hwnd,IDC_LBL_ASPECT_USER,WM_SETTEXT,0,LPARAM(pomS));
 SendDlgItemMessage(m_hwnd,IDC_TBR_ASPECT_USER,TBM_SETPOS,TRUE,aspectI/256);
}

bool TresizeAspectPage::applyResizeXY(bool checkOnly)
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
  }
 parent->setChange(); 
 return true;
}
bool TresizeAspectPage::sizeOK(HWND hed)
{
 char pomS[256];
 SendMessage(hed,WM_GETTEXT,255,LPARAM(pomS));
 char *stop=NULL;
 int x=strtoul(pomS,&stop,10);
 if (*stop || x<8 || x>2048 || (x&15)) return false;
 return true;
}

HRESULT TresizeAspectPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    else 
     break;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_RESIZE:
       cfgSet(IDFF_isResize,getCheck(IDC_CHB_RESIZE));
       return TRUE; 
      case IDC_ED_RESIZEDX:
      case IDC_ED_RESIZEDY:
       if (HIWORD(wParam)==EN_CHANGE) 
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         InvalidateRect(hed,NULL,TRUE);
         applyResizeXY(false);
         return TRUE;  
        }
       break;
      case IDC_RBT_ASPECT_NO:
       cfgSet(IDFF_isAspect,0);
       aspect2dlg();
       return TRUE;
      case IDC_RBT_ASPECT_KEEP:
       cfgSet(IDFF_isAspect,1);
       aspect2dlg();
       return TRUE;
      case IDC_RBT_ASPECT_USER:
       cfgSet(IDFF_isAspect,2);
       aspect2dlg();
       return TRUE;
     }   
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     if (hwnd!=GetDlgItem(m_hwnd,IDC_ED_RESIZEDX) && hwnd!=GetDlgItem(m_hwnd,IDC_ED_RESIZEDY)) return FALSE;
     if (!red) red=CreateSolidBrush(RGB(255,0,0));
     if (!sizeOK(hwnd))
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return HRESULT(red); 
      }
     else return FALSE;  
    }
  }
 return FALSE;
}

void TresizeAspectPage::getTip(char *tipS,int len)
{
 strcpy(tipS,"Aspect ratio: ");
 switch (cfgGet(IDFF_isAspect))
  { 
   case 0:strcat(tipS,"no change");break;
   case 1:strcat(tipS,"keeping original aspect ratio");break;
   case 2:
    {
     int aspectI=cfgGet(IDFF_aspectRatio);
     char pomS[256];sprintf(pomS,"set to %3.2f:1",float(aspectI/65536.0));
     strcat(tipS,pomS);
     break;
    }
  }
}

TresizeAspectPage::TresizeAspectPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_RESIZEASPECT);
}

