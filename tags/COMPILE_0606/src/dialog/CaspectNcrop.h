#ifndef _CASPECTNCROPPAGE_H_
#define _CASPECTNCROPPAGE_H_

#include "TconfPage.h"

class TaspectNcropPage : public TconfPage
{
 private:
  void aspect2dlg(void),crop2dlg(void);
  bool cropOK(HWND hed);
  HBRUSH red;
 public:
  TaspectNcropPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void init(void);
  virtual void cfg2dlg(void);
  virtual void interDlg(void);
  virtual int getInter(void) {return cfgGet(IDFF_isCropNzoom);};
  virtual int invInter(void) {return cfgInv(IDFF_isCropNzoom);};
  virtual void getTip(char *tipS,int len);
};

#endif 
