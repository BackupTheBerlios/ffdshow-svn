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
#include "CconfPage.h"
#include "resource.h"
#include <commctrl.h>
#include "Tconfig.h"
#include <assert.h>
#include "IffDecoder.h"

static INT_PTR CALLBACK dlgWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    {
     TconfPage *cfgDlg=(TconfPage*)lParam;
     SetWindowLong(hwnd,GWL_USERDATA,LONG(cfgDlg));
     cfgDlg->m_hwnd=hwnd;
     cfgDlg->createConfig();
     return TRUE;
    };
   default:
    {
     TconfPage *cfgDlg=(TconfPage*)GetWindowLong(hwnd,GWL_USERDATA);
     if (!cfgDlg) return TRUE;
     return cfgDlg->msgProc(uMsg,wParam,lParam);
    }
  }
}

void TconfPage::createWindow(int dialogId)
{
 HINSTANCE hi=(HINSTANCE)GetWindowLong(hwndParent,GWL_HINSTANCE);
 m_hwnd=CreateDialogParam(hi,MAKEINTRESOURCE(dialogId),hwndParent,dlgWndProc,LPARAM(this));
 assert(m_hwnd);
}

TconfPage::TconfPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci)
{
 assert(Ideci);
 deci=Ideci;
 parent=Iparent;
 hwndParent=IhwndParent;
}

TconfPage::~TconfPage()
{
 DestroyWindow(m_hwnd);
}

int TconfPage::cfgGet(int i)
{
 int val=0;
 deci->get_Param(i,&val);
 return val;
};
int TconfPage::cfgSet(int i,int val)
{
 deci->put_Param(i,val);
 return val;
};

void TconfPage::setCheck(int id,int set)
{
 SendDlgItemMessage(m_hwnd,id,BM_SETCHECK,set?BST_CHECKED:0,0);
}
bool TconfPage::getCheck(int id)
{
 return SendDlgItemMessage(m_hwnd,id,BM_GETCHECK,0,0)==BST_CHECKED?true:false;
}
void TconfPage::enableWindow(int id,int enable)
{
 EnableWindow(GetDlgItem(m_hwnd,id),enable);
}
