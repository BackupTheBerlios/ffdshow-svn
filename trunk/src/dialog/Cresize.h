#ifndef _CRESIZEPAGE_H_
#define _CRESIZEPAGE_H_

#include "TconfPage.h"

class TresizePage : public TconfPage
{
 private:
  void resize2dlg(void);
  bool applyResizeXY(bool checkOnly);
 public:
  TresizePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
  virtual void applySettings(void);
};

#endif 
