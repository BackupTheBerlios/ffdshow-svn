#ifndef _CDLGMISCPAGE_H_
#define _CDLGMISCPAGE_H_

#include "TconfPage.h"

class TdlgMiscPage : public TconfPage
{
 public:
  TdlgMiscPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void init(void);
  virtual void cfg2dlg(void);
};

#endif 
