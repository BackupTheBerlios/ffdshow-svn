#ifndef _CRESIZEPAGE_H_
#define _CRESIZEPAGE_H_

#include "TconfPage.h"

class TresizePage : public TconfPage
{
 private:
  void resize2dlg(void);
  bool applyResizeXY(bool checkOnly);
  HBRUSH red;
  bool sizeOK(HWND hed);
 public:
  TresizePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void init(void);
  virtual void cfg2dlg(void);
};

#endif 
