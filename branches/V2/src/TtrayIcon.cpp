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
#include <shellapi.h>
#include "TtrayIcon.h"
#include "resource.h"
#include "IffDecoder.h"

#define TRAYICON 2999
#define MSG_TRAYICON (WM_APP+9)
#define IDC_FIRST_FILTER 2999
#define IDC_FIRST_PRESET 3999

void TtrayIcon::insertSeparator(HMENU hm,int &ord)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_TYPE;
 mii.fType=MFT_SEPARATOR;
 InsertMenuItem(hm,ord++,TRUE,&mii);
}

void TtrayIcon::insertSubmenu(HMENU hm,int &ord,char *caption,HMENU subMenu)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_TYPE|MIIM_SUBMENU;
 mii.fType=MFT_STRING;
 mii.hSubMenu=subMenu;
 mii.dwTypeData=caption;
 mii.cch=strlen(mii.dwTypeData);
 InsertMenuItem(hm,ord++,TRUE,&mii);
}

void TtrayIcon::insertMenuItem(HMENU hm,int &ord,int id,char *caption,int cfgId,int negate)
{
 bool checked;
 checked=cfgGet(cfgId)?true:false;
 if (negate==-1)
  checked=!checked;
 insertMenuItem(hm,ord,id,caption,checked);
}

void TtrayIcon::insertMenuItem(HMENU hm,int &ord,int id,char *caption,bool checked)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_ID|MIIM_STATE|MIIM_TYPE;
 mii.fType=MFT_STRING;
 mii.wID=id;
 mii.fState=(checked)?MFS_CHECKED:0;
 mii.dwTypeData=caption;
 mii.cch=strlen(mii.dwTypeData);
 InsertMenuItem(hm,ord++,TRUE,&mii);
}

HMENU TtrayIcon::createPostProcMenu(void)
{
 HMENU hm=CreatePopupMenu();
 int ord=0;
 insertMenuItem(hm,ord,IDC_RBT_PPPRESETS,"Presets",IDFF_ppIsCustom,-1);
 insertMenuItem(hm,ord,IDC_CHB_AUTOQ,"Automatic quality control",IDFF_autoq);
 insertSeparator(hm,ord);
 insertMenuItem(hm,ord,IDC_RBT_PPCUSTOM,"Custom",IDFF_ppIsCustom);
 return hm;
}

HMENU TtrayIcon::createMenu(void)
{
 HMENU hm=CreatePopupMenu();
 int ord=0;
 unsigned int len;
 deci->getNumPresets(&len);
 char actPreset[1024];
 deci->getActivePresetName(actPreset,1023);
 unsigned int i;
 for (i=0;i<len;i++)
  {
   char preset[1024];
   deci->getPresetName(i,preset,1023);
   insertMenuItem(hm,ord,IDC_FIRST_PRESET+i,preset,_stricmp(preset,actPreset)==0);
  }
 insertSeparator(hm,ord);
 unsigned int cnt;
 deci->presetGetNumFilters(&cnt);
 for (i=0;i<cnt;i++)
  {
   char filterName[260];
   deci->presetGetFilterName(i,filterName,260);
   insertMenuItem(hm,ord,IDC_FIRST_FILTER+i,filterName,deci->presetGetFilterIs2(i)==1);   
  }
 insertSeparator(hm,ord);
 insertMenuItem(hm,ord,IDC_CHB_FLIP,"Flip",IDFF_flip);

 return hm;
}

static LRESULT CALLBACK trayWndProc(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm)
{
 if (msg==MSG_TRAYICON)
  {
   TtrayIcon *ti=(TtrayIcon*)GetWindowLong(hwnd,GWL_USERDATA);
   if (ti)
    switch (lprm)
     {
      case WM_LBUTTONDBLCLK:
       ti->deci->showCfgDlg(hwnd);
       break;
      case WM_RBUTTONUP:
      case WM_CONTEXTMENU:
       {
        if (ti->cfgGet(IDFF_cfgDlgHnwd)) break;
        POINT p;
        GetCursorPos(&p);
        SetForegroundWindow(hwnd);
        HMENU hm=ti->createMenu();
        int cmd=TrackPopupMenu(hm,TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_RETURNCMD,p.x,p.y,0,hwnd,NULL);
        PostMessage(hwnd, WM_NULL, 0, 0);
        if (cmd>=IDC_FIRST_PRESET)
         {
          int i=cmd-IDC_FIRST_PRESET;
          MENUITEMINFO mii;
          mii.cbSize=sizeof(mii);
          char preset[1024];
          mii.fMask=MIIM_TYPE;
          mii.dwTypeData=preset;
          GetMenuItemInfo(hm,i,TRUE,&mii);
          ti->deci->setActivePreset(preset);
         }
        else if (cmd>=IDC_FIRST_FILTER)
         {
          int i=cmd-IDC_FIRST_FILTER;
          ti->deci->presetSetFilterIs(i,1-ti->deci->presetGetFilterIs2(i));
         }
        else  
         switch (cmd)
          {
           case IDC_CHB_FLIP:
            ti->negateParam(IDFF_flip);
            break;
          }
        DestroyMenu(hm);
       }
     break;
    }
   return 0;
  }
 return DefWindowProc(hwnd,msg,wprm,lprm);
}

int TtrayIcon::cfgGet(int i)
{
 return deci->getParam2(i);
}
int TtrayIcon::cfgSet(int i,int val)
{
 deci->putParam(i,val);
 return val;
}

TtrayIcon::TtrayIcon(IffDecoder *Ideci,HINSTANCE Ihi):deci(Ideci),hi(Ihi)
{
 WNDCLASS wndclass ;
 wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
 wndclass.lpfnWndProc   = trayWndProc ;
 wndclass.cbClsExtra    = 0 ;
 wndclass.cbWndExtra    = 0 ;
 wndclass.hInstance     = hi;
 wndclass.hIcon         = LoadIcon(hi,MAKEINTRESOURCE(IDI_FFDSHOW));
 wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW) ;
 wndclass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
 wndclass.lpszMenuName  = NULL ;
 wndclass.lpszClassName = "ffdshow";
 RegisterClass(&wndclass);

 h=CreateWindow(
                "ffdshow",                   // window class name
                TEXT("ffdshow"),            // window caption
                WS_OVERLAPPED|WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX,
                CW_USEDEFAULT,              // initial x position
                CW_USEDEFAULT,              // initial y position
                1,                          // initial x size
                1,                          // initial y size
                NULL,                       // parent window handle
                NULL,                       // window menu handle
                hi,                         // program instance handle
                NULL
               );
 SetWindowLong(h,GWL_USERDATA,LONG(this));
 nid.cbSize=sizeof(nid);
 nid.hWnd=h;
 nid.uID=TRAYICON;
 nid.uFlags=NIF_ICON|NIF_TIP|NIF_MESSAGE;
 nid.hIcon=LoadIcon(hi,MAKEINTRESOURCE(IDI_FFDSHOW));
 nid.uCallbackMessage=MSG_TRAYICON;
 strcpy(nid.szTip,"ffdshow");

 visible=false;
}
TtrayIcon::~TtrayIcon()
{
 Shell_NotifyIcon(NIM_DELETE,&nid);
 DestroyWindow(h);
 UnregisterClass("ffdshow",hi);
}
void TtrayIcon::show(void)
{
 if (!visible)
  {
   Shell_NotifyIcon(NIM_ADD,&nid);
   visible=true;
  }
}
void TtrayIcon::hide(void)
{
 if (visible)
  {
   Shell_NotifyIcon(NIM_DELETE,&nid);
   visible=false;
  }
}

int TtrayIcon::negateParam(int id)
{
 int oldVal=deci->getParam2(id);
 oldVal=1-oldVal;
 deci->putParam(id,oldVal);
 deci->saveGlobalSettings();
 deci->saveActivePreset(NULL);
 return oldVal;
}
