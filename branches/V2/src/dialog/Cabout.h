#ifndef _CABOUTPAGE_H_
#define _CABOUTPAGE_H_

#include "TconfPage.h"

class TaboutPage :public TconfPage
{
public:
 virtual int getDialogID(void) {return IDD_ABOUT;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif 
