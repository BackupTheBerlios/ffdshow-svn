#ifndef _COFFSETPAGE_H_
#define _COFFSETPAGE_H_

#include "TconfPage.h"

class ToffsetPage :public TconfPage
{
private:
 void offset2dlg(void);
public:
 virtual int getDialogID(void) {return IDD_OFFSET;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_OFFSET;}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
