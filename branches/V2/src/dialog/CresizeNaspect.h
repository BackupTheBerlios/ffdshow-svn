#ifndef _CRESIZEASPECTPAGE_H_
#define _CRESIZEASPECTPAGE_H_

#include "TconfPage.h"

class TresizeNaspectPage :public TconfPage
{
private:
 void resize2dlg(void),aspect2dlg(void);
 bool applyResizeXY(bool checkOnly);
 HBRUSH red;
 bool sizeOK(HWND hed);
public:
 virtual int getDialogID(void) {return IDD_RESIZENASPECT;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
