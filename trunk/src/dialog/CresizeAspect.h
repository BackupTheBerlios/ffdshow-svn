#ifndef _CRESIZEASPECTPAGE_H_
#define _CRESIZEASPECTPAGE_H_

#include "TconfPage.h"

class TresizeAspectPage : public TconfPage
{
 private:
  void resize2dlg(void),aspect2dlg(void);
  bool applyResizeXY(bool checkOnly);
  HBRUSH red;
  bool sizeOK(HWND hed);
 public:
  TresizeAspectPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void init(void);
  virtual void cfg2dlg(void);
};

#endif 
