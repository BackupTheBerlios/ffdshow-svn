#ifndef _CCROPPAGE_H_
#define _CCROPPAGE_H_

#include "TconfPage.h"

class TcropPage :public TconfPage
{
private:
 void crop2dlg(void);
 bool cropOK(HWND hed);
 HBRUSH red;
public:
 TcropPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getOrder(void) {return 0;}
 virtual void getTip(char *tipS,int len);
};

#endif 
