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
 virtual void getTip(char *tipS,int len);
};

#endif 
