#ifndef _CINFOPAGE_H_
#define _CINFOPAGE_H_

#include "TconfPage.h"

class TinfoPage : public TconfPage
{
 public:
  TinfoPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
};

#endif 
