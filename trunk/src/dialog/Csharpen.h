#ifndef _CSHARPENPAGE_H_
#define _CSHARPENPAGE_H_

#include "TconfPage.h"

class TsharpenPage : public TconfPage
{
 private:
  int writeSharpenStrength(int x),writeSharpenThreshold(int x);
  void sharpen2dlg(void);
 public:
  TsharpenPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void init(void);
  virtual void cfg2dlg(void);
  virtual void interDlg(void);
  virtual int getInter(void) {return cfgGet(IDFF_isSharpen);};
  virtual int invInter(void) {return cfgInv(IDFF_isSharpen);};
  virtual int getOrder(void) {return cfgGet(IDFF_orderSharpen);};
  virtual void setOrder(int o) {cfgSet(IDFF_orderSharpen,o);};
  virtual void getTip(char *tipS,int len);
};

#endif 
