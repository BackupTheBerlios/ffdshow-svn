#ifndef _CRESIZEASPECTPAGE_H_
#define _CRESIZEASPECTPAGE_H_

#include "TconfPage.h"

class TresizeAspectPage :public TconfPage
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
 virtual int getOrder(void) {return cfgGet(IDFF_orderResize);}
 virtual void setOrder(int o) {cfgSet(IDFF_orderResize,o);}
 virtual void getTip(char *tipS,int len);
 virtual int  getProcessFull(void) {return cfgGet(IDFF_fullResize);}
 virtual void setProcessFull(int full) {cfgSet(IDFF_fullResize,full);} 
 virtual bool isInPreset(void) {return true;}
};

#endif 
