#ifndef _CBLURPAGE_H_
#define _CBLURPAGE_H_

#include "TconfPage.h"

class TblurPage : public TconfPage
{
 private:
  void blur2dlg(void);
 public:
  TblurPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
  virtual void interDlg(void);
  virtual int getInter(void) {return cfgGet(IDFF_isBlur);};
  virtual int invInter(void) {return cfgInv(IDFF_isBlur);};
  virtual int getOrder(void) {return cfgGet(IDFF_orderBlur);};
  virtual void setOrder(int o) {cfgSet(IDFF_orderBlur,o);};
  virtual void getTip(char *tipS,int len) {deci->getBlurDescription(tipS,len);};
};

#endif 
