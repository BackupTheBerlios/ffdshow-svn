#ifndef _CPOSTPROCPAGE_H_
#define _CPOSTPROCPAGE_H_

#include "TconfPage.h"

class TpostprocPage :public TconfPage
{
private:
 void postproc2dlg(void),setPPchbs(void);
public:
 virtual int getDialogID(void) {return IDD_POSTPROC;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_POSTPROC;}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
