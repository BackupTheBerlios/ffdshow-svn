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

#include "Cffdshow.h"
#include "resource.h"
#include <commctrl.h>
#include "Tconfig.h"
#include <assert.h>
#include "IffDecoder.h"
#include "CglobalPage.h"
#include "CfiltersPage.h"
#include "CresizePage.h"
#include "CsubtitlesPage.h"
#include "CaboutPage.h"

CUnknown * WINAPI TffdshowPage::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
 TffdshowPage * pNewObject=new TffdshowPage(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}

TffdshowPage::TffdshowPage(LPUNKNOWN pUnk, HRESULT * phr) :CBasePropertyPage(NAME("TffdshowPage"), pUnk, IDD_FFDSHOW, IDS_FFDSHOW)
{
 ASSERT(phr);
 deci=NULL;
 page=NULL;
 globalPage=NULL;filtersPage=NULL;resizePage=NULL;subtitlesPage=NULL;aboutPage=NULL;
}

TffdshowPage::~TffdshowPage()
{
}

void TffdshowPage::selectPage(TconfPage *Ipage)
{
 if (page) ShowWindow(page->m_hwnd,SW_HIDE);
 page=Ipage;
 RECT rp,rd;
 GetWindowRect(GetDlgItem(m_hwnd,IDC_DIVIDER),&rd);
 GetWindowRect(m_hwnd,&rp);
 OffsetRect(&rd,-rp.left,-rp.top);
 SetWindowPos(page->m_hwnd,GetDlgItem(m_hwnd,IDC_HEADER),rd.left,rd.top+5,0,0,SWP_NOSIZE);
 page->interDlg();
 ShowWindow(page->m_hwnd,SW_SHOW);
 InvalidateRect(m_hwnd,NULL,TRUE);
}
void TffdshowPage::selectPage(int i)
{
 selectPage(pages[i]);
 TabCtrl_SetCurSel(GetDlgItem(m_hwnd,IDC_HEADER),i);
}

HRESULT TffdshowPage::Activate(HWND hwndParent,LPCRECT prect, BOOL fModal)
{
 CBasePropertyPage::Activate(hwndParent,prect,fModal);
 if (!m_hwnd) return ERROR;
 TCITEM tci;
 tci.mask=TCIF_TEXT;
 pages[0]=globalPage   =new TglobalPage   (m_hwnd,deci,IDD_GLOBAL   );
 pages[1]=filtersPage  =new TfiltersPage  (m_hwnd,deci,IDD_FILTERS  );
 pages[2]=resizePage   =new TresizePage   (m_hwnd,deci,IDD_RESIZE   );
 pages[3]=subtitlesPage=new TsubtitlesPage(m_hwnd,deci,IDD_SUBTITLES);
 pages[4]=aboutPage    =new TaboutPage    (m_hwnd,deci,IDD_ABOUT    );
 RECT r;
 GetWindowRect(GetDlgItem(m_hwnd,IDC_HEADER),&r);
 TabCtrl_SetItemSize(GetDlgItem(m_hwnd,IDC_HEADER),(r.right-r.left)/NUMPAGES-3,20);
 tci.pszText="Generic";
 TabCtrl_InsertItem(GetDlgItem(m_hwnd,IDC_HEADER),0,&tci);
 tci.pszText="Filters";
 TabCtrl_InsertItem(GetDlgItem(m_hwnd,IDC_HEADER),1,&tci);
 tci.pszText="Resize";
 TabCtrl_InsertItem(GetDlgItem(m_hwnd,IDC_HEADER),2,&tci);
 tci.pszText="Subtitles";
 TabCtrl_InsertItem(GetDlgItem(m_hwnd,IDC_HEADER),3,&tci);
 tci.pszText="About";
 TabCtrl_InsertItem(GetDlgItem(m_hwnd,IDC_HEADER),4,&tci);
 deci->put_Param(IDFF_isDlg,1);
 int lastPage;deci->get_Param(IDFF_lastPage,&lastPage);
 selectPage(lastPage);
 m_bDirty=true;
 return NOERROR;
}

STDMETHODIMP TffdshowPage::Deactivate(void)
{
 //cfg->save();
 int lastPage=deci->get_Param2(IDFF_lastPage);
 int beSaved;deci->get_Param(IDFF_presetShouldBeSaved,&beSaved);
 if (beSaved) deci->loadPreset(NULL);
 deci->put_Param(IDFF_lastPage,lastPage);
 //loadPreset();
 HRESULT res=CBasePropertyPage::Deactivate();
 if (globalPage   ) delete globalPage   ;
 if (filtersPage  ) delete filtersPage  ;
 if (resizePage   ) delete resizePage   ;
 if (subtitlesPage) delete subtitlesPage;
 if (aboutPage    ) delete aboutPage    ;
 deci->put_Param(IDFF_isDlg,0);
 return res;
}

HRESULT TffdshowPage::OnApplyChanges(void)
{
 for (int i=0;i<NUMPAGES;i++) pages[i]->applySettings();
 //deci->put_Param(IDFF_presetShouldBeSaved,1);
 globalPage->savePreset();
 return CBasePropertyPage::OnApplyChanges();
}

HRESULT TffdshowPage::OnConnect(IUnknown *pUnk)
{
 ASSERT(deci==NULL); 
 HRESULT hr=pUnk->QueryInterface(IID_IffDecoder,(void **)&deci);
 if (FAILED(hr))
  return E_NOINTERFACE;
 ASSERT(deci);
 return S_OK;
}

HRESULT TffdshowPage::OnDisconnect(void)
{
 if (deci==NULL) 
  return E_UNEXPECTED;
 deci->Release();
 deci=NULL;
 //cfg=NULL;
 return S_OK;
}

BOOL TffdshowPage::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==GetDlgItem(m_hwnd,IDC_HEADER) && nmhdr->idFrom==IDC_HEADER && nmhdr->code==TCN_SELCHANGE)
      {
       int i=TabCtrl_GetCurSel(GetDlgItem(m_hwnd,IDC_HEADER));
       selectPage(pages[i]);
       deci->put_Param(IDFF_lastPage,i);
       return TRUE;
      }  
    }
  }
 return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

/* -------------------- configure ---------------------- */
void CALLBACK configure(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 IffDecoder *iff;
 if (CoInitialize(NULL)!=S_OK) return;
 if (CoCreateInstance(CLSID_FFDSHOW,NULL,CLSCTX_INPROC_SERVER,IID_IffDecoder,(void**)&iff)!=S_OK) return;
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
