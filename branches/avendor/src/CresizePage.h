#ifndef _CRESIZEPAGE_H_
#define _CRESIZEPAGE_H_

#include "CconfPage.h"

class TresizePage :public TconfPage
{
 private:
  void resize2dlg(void),aspect2dlg(void);
  bool applyResizeXY(bool checkOnly);
 public:
  TresizePage(HWND IhwndParent,IffDecoder *Ideci,int dialogId);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
  virtual void applySettings(void);
};

#endif 
