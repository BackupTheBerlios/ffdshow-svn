#ifndef _CABOUTPAGE_H_
#define _CABOUTPAGE_H_

#include "CconfPage.h"

class TaboutPage : public TconfPage
{
 public:
  TaboutPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci,int dialogId);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
};

#endif 
