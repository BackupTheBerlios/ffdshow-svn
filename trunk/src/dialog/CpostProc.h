#ifndef _CPOSTPROCPAGE_H_
#define _CPOSTPROCPAGE_H_

#include "TconfPage.h"

class TpostProcPage :public TconfPage
{
private:
 void postProc2dlg(void),setPPchbs(void);
public:
 TpostProcPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void interDlg(void);
 virtual int getInter(void) {return cfgGet(IDFF_isPostproc);};
 virtual int invInter(void) {return cfgInv(IDFF_isPostproc);};
 virtual int getOrder(void) {return cfgGet(IDFF_orderPostproc);};
 virtual void setOrder(int o) {cfgSet(IDFF_orderPostproc,o);};
 virtual void getTip(char *tipS,int len);
};

#endif 
