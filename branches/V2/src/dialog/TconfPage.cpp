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
#include "TconfPage.h"
#include "resource.h"
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
     cfgDlg->init();
     return TRUE;
    }
   default:
    {
     TconfPage *cfgDlg=(TconfPage*)GetWindowLong(hwnd,GWL_USERDATA);
     if (!cfgDlg) return TRUE;
     return cfgDlg->msgProc(uMsg,wParam,lParam);
    }
  }
}

TconfPage::TconfPage(void)
{
 helpStr=NULL;
}

void TconfPage::createWindow(int dialogId)
{
 hi=(HINSTANCE)GetWindowLong(hwndParent,GWL_HINSTANCE);
 m_hwnd=CreateDialogParam(hi,MAKEINTRESOURCE(dialogId),hwndParent,dlgWndProc,LPARAM(this));
 dialogName[0]='\0';
 LoadString(hi,dialogId,dialogName,255);
 assert(m_hwnd);
 if (GetDlgItem(m_hwnd,IDC_ED_HELP))
  {
   HMODULE hm=(HMODULE)GetWindowLong(m_hwnd,GWL_HINSTANCE);
   HRSRC rsrc=FindResource(hm,MAKEINTRESOURCE(dialogId),"TEXT");
   if (!rsrc) return;
   HGLOBAL hglb=LoadResource(hm,rsrc);
   int len=SizeofResource(hm,rsrc);
   helpStr=(char*)calloc(len+1,1);
   strncpy(helpStr,(char*)LockResource(hglb),len);
   SendDlgItemMessage(m_hwnd,IDC_ED_HELP,WM_SETTEXT,0,LPARAM(helpStr));
   SendDlgItemMessage(m_hwnd,IDC_ED_HELP,EM_SETSEL,0,-1);
  }
}

TconfPage* TconfPage::create(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci)
{
 index=-1;
 deci=Ideci;
 parent=Iparent;
 hwndParent=IhwndParent;
 createWindow(getDialogID());
 return this;
}

TconfPage::~TconfPage()
{
 DestroyWindow(m_hwnd);
 if (helpStr) free(helpStr);
}

int TconfPage::cfgGet(unsigned int i)
{
 return deci->getParam2(i);
}
int TconfPage::cfgSet(unsigned int i,int val)
{
 deci->putParam(i,val);
 return val;
}
int TconfPage::cfgInv(unsigned int i)
{
 int val=deci->getParam2(i);
 if (val==0) val=1; else val=0;
 deci->putParam(i,val);
 interDlg();
 return val;
}

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
int TconfPage::getProcessFull(void)
{
 if (index==-1) return -1;
 return deci->presetGetFilterFull2(index);
}
void TconfPage::interDlg(void)
{
 int id=getInterDlgID();
 if (id) setCheck(id,deci->presetGetFilterIs2(index));
}
void TconfPage::setInter(bool is)
{
 deci->presetSetFilterIs(index,is);
}
