#ifndef _CDEINTERLACEPAGE_H_
#define _CDEINTERLACEPAGE_H_

#include "TconfPage.h"

class TdeinterlacePage :public TconfPage
{
private:
 static const int deintRbts[];
public:
 TdeinterlacePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInter(void) {return cfgGet(IDFF_isDeinterlace);}
 virtual int invInter(void) {return cfgInv(IDFF_isDeinterlace);}
 virtual int getOrder(void) {return 0;}
 virtual void getTip(char *tipS,int len);
};

#endif 
