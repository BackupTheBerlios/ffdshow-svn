#ifndef _CDLGMISCPAGE_H_
#define _CDLGMISCPAGE_H_

#include "TconfPage.h"

class TdlgMiscPage :public TconfPage
{
public:
 virtual int getDialogID(void) {return IDD_DLGMISC;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif 
