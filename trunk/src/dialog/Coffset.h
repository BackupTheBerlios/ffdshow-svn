#ifndef _COFFSETPAGE_H_
#define _COFFSETPAGE_H_

#include "TconfPage.h"

class ToffsetPage :public TconfPage
{
private:
 void offset2dlg(void);
public:
 ToffsetPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void interDlg(void);
 virtual int getInter(void) {return cfgGet(IDFF_isOffset);}
 virtual int invInter(void) {return cfgInv(IDFF_isOffset);}
 virtual int getOrder(void) {return cfgGet(IDFF_orderOffset);}
 virtual void setOrder(int o) {cfgSet(IDFF_orderOffset,o);}
 virtual void getTip(char *tipS,int len);
};

#endif 
