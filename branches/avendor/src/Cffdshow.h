#ifndef _CFFDSHOW_H_
#define _CFFDSHOW_H_

#include <streams.h>

struct IffDecoder;

// {9A98ADCC-C6A4-449e-A8B1-0363673D9F8A}
DEFINE_GUID(CLSID_TFFDSHOWPAGE, 0x9a98adcc, 0xc6a4, 0x449e, 0xa8, 0xb1, 0x3, 0x63, 0x67, 0x3d, 0x9f, 0x8a);

class TconfPage;
class TglobalPage;
class TfiltersPage;
class TresizePage;
class TsubtitlesPage;
class TaboutPage;
class TffdshowPage : public CBasePropertyPage
{
 private:
  IffDecoder *deci;
  TglobalPage    *globalPage;
  TfiltersPage   *filtersPage;
  TresizePage    *resizePage;
  TsubtitlesPage *subtitlesPage;
  TaboutPage     *aboutPage;
  #define NUMPAGES 5
  TconfPage    *page,*pages[NUMPAGES];
  void selectPage(int i),selectPage(TconfPage *Ipage);
 public: 
  static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT * phr);
  TffdshowPage(LPUNKNOWN pUnk, HRESULT * phr);
  ~TffdshowPage();
  
  STDMETHODIMP Activate(HWND hwndParent,LPCRECT prect, BOOL fModal);
  HRESULT OnApplyChanges(void);
  STDMETHODIMP Deactivate(void);
  HRESULT OnConnect(IUnknown *pUnk);
  HRESULT OnDisconnect(void);
  BOOL OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif 
