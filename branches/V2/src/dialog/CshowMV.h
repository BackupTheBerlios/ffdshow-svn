#ifndef _CSHOWMVPAGE_H_
#define _CSHOWMVPAGE_H_

#include "TconfPage.h"

class TshowMVpage :public TconfPage
{
private:
 void showMV2dlg(void);
public:
 virtual int getDialogID(void) {return IDD_SHOWMV;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_SHOWMV;}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
