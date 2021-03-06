/*
 * Copyright (c) 2002 Milan Cutka
 * based on config.cpp from XVID DirectShow filter
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
#include "Tconfig.h"
#include "TffdshowPage.h"
#include "resource.h"
#include "IffDecoder.h"
#include "ffdebug.h"
#include "TconfPage.h"
#include "Ccodecs.h"
#include "Cinfo.h"
#include "Ctray.h"
#include "Cpresets.h"
#include "CpostProc.h"
#include "CpictProp.h"
#include "Cnoise.h"
#include "Csharpen.h"
#include "Cblur.h"
#include "Csubtitles.h"
#include "Cfont.h"
#include "Cresize.h"
#include "CaspectNcrop.h"
#include "Cmisc.h"
#include "Cabout.h"
#include "Coffset.h"

using namespace std;

#define WM_FFONCHANGE WM_APP+1

CUnknown * WINAPI TffdshowPage::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
 TffdshowPage * pNewObject=new TffdshowPage(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}

TffdshowPage::TffdshowPage(LPUNKNOWN pUnk, HRESULT * phr):CBasePropertyPage(NAME("TffdshowPage"), pUnk, IDD_FFDSHOW, IDS_FFDSHOW)
{
 ASSERT(phr);
 hil=NULL;
 deci=NULL;
 page=NULL;
 applying=false;
}
TffdshowPage::~TffdshowPage()
{
 ImageList_Destroy(hil);
}
void TffdshowPage::selectPage(TconfPage *Ipage)
{
 if (page) ShowWindow(page->m_hwnd,SW_HIDE);
 page=Ipage;
 RECT rp,rd;
 GetWindowRect(GetDlgItem(m_hwnd,IDC_TV_TREE),&rd);
 GetWindowRect(m_hwnd,&rp);
 OffsetRect(&rd,-rp.left,-rp.top);
 SetWindowPos(page->m_hwnd,GetDlgItem(m_hwnd,IDC_TV_TREE),rd.right+5,rd.top-1,0,0,SWP_NOSIZE);
 page->interDlg();
 ShowWindow(page->m_hwnd,SW_SHOW);
 InvalidateRect(m_hwnd,NULL,TRUE);
}
HTREEITEM TffdshowPage::addTI(TVINSERTSTRUCT &tvis,TconfPage *page,bool push)
{
 tvis.item.lParam=(LPARAM)page;if (push) pages.push_back(page);
 tvis.item.pszText=page->dialogName;;
 HTREEITEM hti=TreeView_InsertItem(htv,&tvis);
 return hti;
}
TconfPage* TffdshowPage::hti2page(HTREEITEM hti)
{
 TVITEM tvi;
 tvi.hItem=hti;
 tvi.mask=TVIF_PARAM;
 TreeView_GetItem(htv,&tvi);
 return (TconfPage*)tvi.lParam;
}
void TffdshowPage::presetChanged(void)
{
 for (unsigned int i=0;i<pages.size();i++)
  pages[i]->cfg2dlg();
 InvalidateRect(htv,NULL,FALSE);
 sortOrder();
 if (IsWindow(dlg))
  {
   char capt[260],presetName[260];
   deci->getActivePresetName(presetName,260);
   sprintf(capt,"%s (%s)",caption,presetName);
   SendMessage(dlg,WM_SETTEXT,0,LPARAM(capt));
  };
}
static int CALLBACK orderCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
{
 int o1=((TconfPage*)lParam1)->getOrder(),o2=((TconfPage*)lParam2)->getOrder();
 //DEBUGS2("sort",o1,o2);
 if (o1==-1 && o2==-1) return 0;
 if (o1==-1) return 1;
 if (o2==-1) return -1;
 return o1-o2;
}
void TffdshowPage::sortOrder(void)
{
 TVSORTCB tvs;
 tvs.hParent=htiPresets;
 tvs.lpfnCompare=orderCompareFunc;
 tvs.lParam=0;
 TreeView_SortChildrenCB(htv,&tvs,0);
}
HWND TffdshowPage::findParentDlg(void)
{
 for (HWND dlg=GetAncestor(m_hwnd,GA_PARENT);dlg;dlg=GetAncestor(dlg,GA_PARENT))
  if (GetWindowLong(dlg,GWL_STYLE)&WS_DLGFRAME)
   return dlg;
 return NULL;
}
HRESULT TffdshowPage::Activate(HWND hwndParent,LPCRECT prect, BOOL fModal)
{
 CBasePropertyPage::Activate(hwndParent,prect,fModal);
 if (!m_hwnd) return ERROR;
 dlg=findParentDlg();
 if (IsWindow(dlg))
  {
   GetWindowText(dlg,caption,255);
   if (deci->getParam2(IDFF_dlgRestorePos))
    {
     int x,y;
     x=deci->getParam2(IDFF_dlgPosX);
     y=deci->getParam2(IDFF_dlgPosY);
     WINDOWPLACEMENT wpl;
     GetWindowPlacement(dlg,&wpl);
     OffsetRect(&wpl.rcNormalPosition,x-wpl.rcNormalPosition.left,y-wpl.rcNormalPosition.top);
     SetWindowPlacement(dlg,&wpl);
    }
  }
 else
  caption[0]='\0';
 htv=GetDlgItem(m_hwnd,IDC_TV_TREE);
 hil=ImageList_Create(16,16,ILC_COLOR|ILC_MASK,2,2);
 HINSTANCE hi=(HMODULE)GetWindowLong(m_hwnd,GWL_HINSTANCE);
 ilClear  =ImageList_Add(hil,LoadBitmap(hi,MAKEINTRESOURCE(IDB_CLEAR  )),LoadBitmap(hi,MAKEINTRESOURCE(IDB_CHB_MASK)));
 ilChecked=ImageList_Add(hil,LoadBitmap(hi,MAKEINTRESOURCE(IDB_CHECKED)),LoadBitmap(hi,MAKEINTRESOURCE(IDB_CHB_MASK))); 
 ilArrowUD=ImageList_Add(hil,LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS )),LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS_MASK_UD)));
 ilArrowU =ImageList_Add(hil,LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS )),LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS_MASK_U)));
 ilArrowD =ImageList_Add(hil,LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS )),LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS_MASK_D)));
 
 TVINSERTSTRUCT tvis;
 tvis.hParent=NULL;
 tvis.hInsertAfter=TVI_LAST;
 tvis.item.mask=TVIF_PARAM|TVIF_TEXT;
 addTI(tvis,new TcodecsPage(this,m_hwnd,deci));
 addTI(tvis,new TinfoPage(this,m_hwnd,deci));
 addTI(tvis,new TdlgMiscPage(this,m_hwnd,deci));
 tvis.item.mask|=TVIF_CHILDREN;
 tvis.item.cChildren=1;
 TconfPage *pagePresets;//presets page must be the last in pages vector
 htiPresets=addTI(tvis,pagePresets=new TpresetsPage(this,m_hwnd,deci),false);
 tvis.hParent=htiPresets;
 tvis.item.cChildren=0;
 addTI(tvis,new TpostProcPage(this,m_hwnd,deci)); 
 addTI(tvis,new TpictPropPage(this,m_hwnd,deci)); 
 addTI(tvis,new TnoisePage(this,m_hwnd,deci));    
 addTI(tvis,new TsharpenPage(this,m_hwnd,deci));  
 addTI(tvis,new TblurPage(this,m_hwnd,deci));     
 addTI(tvis,new ToffsetPage(this,m_hwnd,deci));     
 //addTI(tvis,new TsubtitlesPage(this,m_hwnd,deci));
 //addTI(tvis,new TfontPage(this,m_hwnd,deci));
 tvis.item.cChildren=1;
 HTREEITEM pageSubtitles=addTI(tvis,new TsubtitlesPage(this,m_hwnd,deci));
 tvis.item.cChildren=0;
 tvis.hParent=pageSubtitles;
 addTI(tvis,new TfontPage(this,m_hwnd,deci));
 TreeView_Expand(htv,pageSubtitles,TVE_EXPAND);
 tvis.hParent=htiPresets;
 sortOrder();
 addTI(tvis,new TresizePage(this,m_hwnd,deci));
 addTI(tvis,new TaspectNcropPage(this,m_hwnd,deci));
 addTI(tvis,new TmiscPage(this,m_hwnd,deci));
 tvis.hParent=NULL;
 addTI(tvis,new TaboutPage(this,m_hwnd,deci));
 pages.push_back(pagePresets);
 TreeView_SetIndent(htv,24);
 //TreeView_SetItemHeight(htv,26);
 TreeView_Expand(htv,htiPresets,TVE_EXPAND);
 int lastPage=deci->getParam2(IDFF_lastPage);
 for (HTREEITEM hti=TreeView_GetRoot(htv);hti;hti=TreeView_GetNextVisible(htv,hti))
  {
   TconfPage *page=hti2page(hti);
   if (page->dialogId==lastPage)
    {
     TreeView_SelectItem(htv,hti);
     break;
    }
  }
 deci->putParam(IDFF_cfgDlgHnwd,int(dlg));
 m_bDirty=true;//m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
 deci->setOnChangeMsg(m_hwnd,WM_FFONCHANGE);
 return NOERROR;
}
HRESULT TffdshowPage::OnApplyChanges(void)
{
 applying=true;
 applySettings();
 //deci->put_Param(IDFF_presetShouldBeSaved,1);
// globalPage->savePreset();
 deci->saveGlobalSettings();
 applying=false;
 return CBasePropertyPage::OnApplyChanges();
}
STDMETHODIMP TffdshowPage::Deactivate(void)
{
 //cfg->save();
 //int beSaved=deci->getParam2(IDFF_presetShouldBeSaved);
 //if (beSaved) deci->loadPreset(NULL);
 //loadPreset();
 deci->setOnChangeMsg(NULL,0);
 HRESULT res=CBasePropertyPage::Deactivate();
 for (vector<TconfPage*>::iterator i=pages.begin();i!=pages.end();i++)
  delete *i;
 if (IsWindow(dlg))
  {
   WINDOWPLACEMENT wpl;
   wpl.length=sizeof(wpl);
   GetWindowPlacement(dlg,&wpl);
   deci->putParam(IDFF_dlgPosX,wpl.rcNormalPosition.left);
   deci->putParam(IDFF_dlgPosY,wpl.rcNormalPosition.top);
   SendMessage(dlg,WM_SETTEXT,0,LPARAM(caption));
  };
 deci->saveDialogSettings();
 deci->putParam(IDFF_cfgDlgHnwd,0);
 return res;
}

HRESULT TffdshowPage::OnConnect(IUnknown *pUnk)
{
 DEBUGS("On connect 1");
 ASSERT(deci==NULL); 
 HRESULT hr=pUnk->QueryInterface(IID_IffDecoder,(void **)&deci);
 if (FAILED(hr)) return E_NOINTERFACE;
 ASSERT(deci);
 DEBUGS("On connect 2");
 return S_OK;
}
HRESULT TffdshowPage::OnDisconnect(void)
{
 DEBUGS("On disconnect 1");
 if (deci==NULL) return E_UNEXPECTED;
 deci->loadGlobalSettings();
 deci->Release();
 DEBUGS("On disconnect 2");
 deci=NULL;
 return S_OK;
}

BOOL TffdshowPage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_FFONCHANGE:
    if (!applying)
     {
      DEBUGS("onChangeParam");
      setChange();
     }; 
    return TRUE;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==htv && nmhdr->idFrom==IDC_TV_TREE)
      switch (nmhdr->code)
       {
        case TVN_SELCHANGED:
         {
          NMTREEVIEW *nmtv=LPNMTREEVIEW(lParam);
          TconfPage *page=(TconfPage*)nmtv->itemNew.lParam;
          selectPage(page);
          deci->putParam(IDFF_lastPage,page->dialogId);
          return TRUE;
         }
        case TVN_GETINFOTIP:
         {
          NMTVGETINFOTIP *nmtvit=LPNMTVGETINFOTIP(lParam);
          TconfPage *page=(TconfPage*)nmtvit->lParam;
          char tipS[1024];
          page->getTip(tipS,1023);
          if (tipS[0]=='\0') return FALSE;
          memset(nmtvit->pszText,0,nmtvit->cchTextMax);
          strncpy(nmtvit->pszText,tipS,nmtvit->cchTextMax-1);
          return TRUE;
         }
        case TVN_ITEMEXPANDING:
         {
          NMTREEVIEW *nmtv=LPNMTREEVIEW(lParam);
          if (nmtv->action==TVE_COLLAPSE) SetWindowLong(m_hwnd,DWL_MSGRESULT,TRUE);
          return TRUE;
         }; 
        case NM_CUSTOMDRAW:
         {
          NMTVCUSTOMDRAW *tvcd=LPNMTVCUSTOMDRAW(lParam);
          int rcDy=tvcd->nmcd.rc.bottom-tvcd->nmcd.rc.top;
          if (tvcd->nmcd.dwDrawStage==CDDS_PREPAINT)
           {
            SetWindowLong(m_hwnd,DWL_MSGRESULT,CDRF_NOTIFYITEMDRAW);
            return TRUE;
           } 
          if (tvcd->nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
           {
            TconfPage *page=(TconfPage*)tvcd->nmcd.lItemlParam;
            if (page->getInter()==-1) return FALSE;
            SetWindowLong(m_hwnd,DWL_MSGRESULT,CDRF_NOTIFYPOSTPAINT);
            return TRUE;
           }
          if (tvcd->nmcd.dwDrawStage==CDDS_ITEMPOSTPAINT)
           {
            RECT rr;
            TreeView_GetItemRect(htv,HTREEITEM(tvcd->nmcd.dwItemSpec),&rr,true);
            rr.left-=24;
            //DEBUGS2("tv rect",rr.left,rr.top);
            TconfPage *page=(TconfPage*)tvcd->nmcd.lItemlParam;
            ImageList_Draw(hil,page->getInter()?ilChecked:ilClear,tvcd->nmcd.hdc,tvcd->nmcd.rc.left+8+rr.left,tvcd->nmcd.rc.top+(rcDy-16)/2,ILD_TRANSPARENT);
            if (page->getOrder()!=-1 && (tvcd->nmcd.uItemState&CDIS_SELECTED)) 
             {
              int img;
              if (page->getOrder()==deci->getMinOrder2()) img=ilArrowD;
              else if (page->getOrder()==deci->getMaxOrder2()) img=ilArrowU;
              else img=ilArrowUD;
              ImageList_DrawEx(hil,img,tvcd->nmcd.hdc,tvcd->nmcd.rc.left+2+rr.left,tvcd->nmcd.rc.top+(rcDy-16)/2,5,16,CLR_DEFAULT,CLR_DEFAULT,ILD_TRANSPARENT);
             }
            return TRUE;
           } 
         }
         return FALSE;
        case NM_CLICK:
         {
          POINT ps;
          GetCursorPos(&ps);
          ScreenToClient(htv,&ps);
          TVHITTESTINFO tvhti;
          tvhti.pt=ps;
          HTREEITEM hti=TreeView_HitTest(htv,&tvhti);
          if (!hti) return FALSE;
          RECT rr;
          TreeView_GetItemRect(htv,hti,&rr,TRUE);
          RECT r;
          TreeView_GetItemRect(htv,hti,&r,FALSE);
          ps.x-=rr.left-24;
          //DEBUGS2("tv rect",r.left+(rr.left-24),r.top);
          int iconTop=((r.bottom-r.top)-16)/2;
          ps.y-=r.top;
          if (ps.x>=8 && ps.x<=16+8 && ps.y>=iconTop+2 && ps.y<=iconTop+13) 
           {
            TconfPage *page=hti2page(hti);
            page->invInter();
            InvalidateRect(htv,&r,FALSE);
            SetWindowLong(m_hwnd,DWL_MSGRESULT,TRUE);
            return TRUE;
           }
          else if (ps.x>=2 && ps.x<=7 && TreeView_GetSelection(htv)==tvhti.hItem)
           {
            int center=(r.bottom-r.top)/2;
            if (ps.y>center-6 && ps.y<center-1 && page->getOrder()>deci->getMinOrder2())
             {
              HTREEITEM hti0=TreeView_GetPrevSibling(htv,hti);
              int o1=hti2page(hti0)->getOrder(),o2=hti2page(hti)->getOrder();
              hti2page(hti0)->setOrder(o2);hti2page(hti)->setOrder(o1);
              sortOrder(); //swap(hti,hti0);
              SetWindowLong(m_hwnd,DWL_MSGRESULT,TRUE);
              return TRUE;
             }
            else if (ps.y>center+1 && ps.y<center+6 && page->getOrder()<deci->getMaxOrder2())
             {
              HTREEITEM hti0=TreeView_GetNextSibling(htv,hti);
              int o1=hti2page(hti0)->getOrder(),o2=hti2page(hti)->getOrder();
              hti2page(hti0)->setOrder(o2);hti2page(hti)->setOrder(o1);
              sortOrder();//swap(hti,hti0);
              SetWindowLong(m_hwnd,DWL_MSGRESULT,TRUE);
              return TRUE;
             }
           }
          return FALSE;
         }
       }  
    }
  }
 return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}
void TffdshowPage::applySettings(void)
{
 for (unsigned int i=0;i<pages.size();i++) pages[i]->applySettings();
}
void TffdshowPage::drawInter(void)
{
 InvalidateRect(htv,NULL,FALSE);
}
void TffdshowPage::showHelp(const char *flnm)
{
 char fullflnm[260];
 _makepath(fullflnm,NULL,config.pth,"help\\",NULL);
 strcat(fullflnm,flnm);
 if (GetFileAttributes(fullflnm)!=INVALID_FILE_ATTRIBUTES)
  ShellExecute(m_hwnd,"open",fullflnm,NULL,".",SW_SHOWNORMAL);
}
void TffdshowPage::setChange(void)
{
 m_bDirty=true;m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
}

#ifdef DEBUG
static int refcnt=0;
STDMETHODIMP_(ULONG) TffdshowPage::AddRef()
{
 refcnt++;
 DEBUGS1("TffdshowPage::AddRef",refcnt);
 return CBasePropertyPage::AddRef();
}
STDMETHODIMP_(ULONG) TffdshowPage::Release()
{
 refcnt--;
 DEBUGS1("TffdshowPage::Release",refcnt);
 return CBasePropertyPage::Release();
}
#endif

/* -------------------- configure ---------------------- */
void CALLBACK configure(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 IffDecoder *iff;
 if (CoInitialize(NULL)!=S_OK) return;
 if (CoCreateInstance(CLSID_FFDSHOW,NULL,CLSCTX_INPROC_SERVER,IID_IffDecoder,(void**)&iff)!=S_OK) return;
 iff->putParam(IDFF_inPlayer,0);
 ISpecifyPropertyPages *ispp;
 if (iff->QueryInterface(IID_ISpecifyPropertyPages,(void**)&ispp)==S_OK)
  {
   CAUUID pages;
   if (ispp->GetPages(&pages)==S_OK)
    {
     IUnknown *ifflist[]={ispp};
     OleCreatePropertyFrame(NULL,10,10,L"ffdshow",
                            1,ifflist,
                            pages.cElems,pages.pElems,
                            LOCALE_SYSTEM_DEFAULT,
                            0,0
                           );
    };
   ispp->Release();
  };
 iff->Release();
}
