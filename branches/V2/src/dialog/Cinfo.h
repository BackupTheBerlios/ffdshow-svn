#ifndef _CINFOPAGE_H_
#define _CINFOPAGE_H_

#include "TconfPage.h"

class TinfoPage :public TconfPage
{
private:
 int frameCnt,bytesCnt;
public:
 virtual int getDialogID(void) {return IDD_INFO;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif 
