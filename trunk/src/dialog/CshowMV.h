#ifndef _CSHOWMVPAGE_H_
#define _CSHOWMVPAGE_H_

#include "TconfPage.h"

class TshowMVpage :public TconfPage
{
private:
 void showMV2dlg(void);
public:
 TshowMVpage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void interDlg(void);
 virtual int getInter(void) {return cfgGet(IDFF_isShowMV);}
 virtual int invInter(void) {return cfgInv(IDFF_isShowMV);}
 virtual int getOrder(void) {return cfgGet(IDFF_orderShowMV);}
 virtual void setOrder(int o) {cfgSet(IDFF_orderShowMV,o);}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
