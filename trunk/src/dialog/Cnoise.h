#ifndef _CNOISEPAGE_H_
#define _CNOISEPAGE_H_

#include "TconfPage.h"

class TnoisePage : public TconfPage
{
 private:
  int writeNoiseStrength(int x),writeNoiseStrengthChroma(int x);
  void noise2dlg(void);
 public:
  TnoisePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
  virtual void interDlg(void);
  virtual int getInter(void) {return cfgGet(IDFF_isNoise);};
  virtual int invInter(void) {return cfgInv(IDFF_isNoise);};
};

#endif 
