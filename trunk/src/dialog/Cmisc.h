#ifndef _CMISCPAGE_H_
#define _CMISCPAGE_H_

#include "TconfPage.h"

class TmiscPage : public TconfPage
{
 public:
  TmiscPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
};

#endif 
