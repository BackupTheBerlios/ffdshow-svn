#ifndef _CFONTPAGE_H_
#define _CFONTPAGE_H_

#include "TconfPage.h"

class TfontPage : public TconfPage
{
 private:
  void font2dlg(void);
  void fillCharsets(void);
  void selectCharset(int ii);
  COLORREF custColors[16];
 public:
  TfontPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void init(void);
  virtual void cfg2dlg(void);
};

#endif 
