#ifndef _CFFDSHOW_H_
#define _CFFDSHOW_H_

#include <streams.h>
#include <vector>

struct IffDecoder;

// {9A98ADCC-C6A4-449e-A8B1-0363673D9F8A}
DEFINE_GUID(CLSID_TFFDSHOWPAGE, 0x9a98adcc, 0xc6a4, 0x449e, 0xa8, 0xb1, 0x3, 0x63, 0x67, 0x3d, 0x9f, 0x8a);

class TconfPage;
class TffdshowPage : public CBasePropertyPage
{
 private:
  HIMAGELIST hil;
  HWND htv;
  HTREEITEM addTI(TVINSERTSTRUCT &tvis,TconfPage *page);
  IffDecoder *deci;
  TconfPage  *page;
  void selectPage(int i),selectPage(TconfPage *Ipage);
  std::vector<TconfPage*> *pages;
  TconfPage* hti2page(HTREEITEM hti);
 public: 
  static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT * phr);
  TffdshowPage(LPUNKNOWN pUnk, HRESULT * phr);
  ~TffdshowPage();
  void applySettings(void);
  void drawInter(void);
  
  STDMETHODIMP Activate(HWND hwndParent,LPCRECT prect, BOOL fModal);
  STDMETHODIMP Deactivate(void);
  virtual HRESULT OnApplyChanges(void);
  virtual HRESULT OnConnect(IUnknown *pUnk);
  virtual HRESULT OnDisconnect(void);
  virtual INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  #ifdef DEBUG
  STDMETHODIMP_(ULONG) AddRef();                              
  STDMETHODIMP_(ULONG) Release();
  #endif
};

#endif 
