#ifndef _CABOUTPAGE_H_
#define _CABOUTPAGE_H_

#include "TconfPage.h"

class TaboutPage :public TconfPage
{
public:
 TaboutPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif 
