#ifndef _CTRAYPAGE_H_
#define _CTRAYPAGE_H_

#include "TconfPage.h"

class TtrayPage : public TconfPage
{
 public:
  TtrayPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
};

#endif 
