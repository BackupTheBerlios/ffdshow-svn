#ifndef _CCODECSPAGE_H_
#define _CCODECSPAGE_H_

#include "TconfPage.h"

class TcodecsPage : public TconfPage
{
 public:
  TcodecsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
};

#endif 
