#ifndef _CCODECSPAGE_H_
#define _CCODECSPAGE_H_

#include "TconfPage.h"

class TcodecsPage :public TconfPage
{
public:
 virtual int getDialogID(void) {return IDD_CODECS;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual const char* getHelpURL(void) {return "in_out.html";}
};

#endif 
