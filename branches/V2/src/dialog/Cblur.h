#ifndef _CBLURPAGE_H_
#define _CBLURPAGE_H_

#include "TconfPage.h"

class TblurPage :public TconfPage
{
private:
 void blur2dlg(void);
public:
 virtual int getDialogID(void) {return IDD_BLUR;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_BLUR;}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
