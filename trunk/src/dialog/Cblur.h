#ifndef _CBLURPAGE_H_
#define _CBLURPAGE_H_

#include "TconfPage.h"

class TblurPage :public TconfPage
{
private:
 void blur2dlg(void);
public:
 TblurPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char *tipS,int len);
};

#endif 
