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
 virtual void getTip(char *tipS,int len);
};

#endif 
