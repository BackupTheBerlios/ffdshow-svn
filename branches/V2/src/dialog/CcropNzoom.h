#ifndef _CCROPPAGE_H_
#define _CCROPPAGE_H_

#include "TconfPage.h"

class TcropNzoomPage :public TconfPage
{
private:
 void crop2dlg(void);
 bool cropOK(HWND hed);
 HBRUSH red;
public:
 virtual int getDialogID(void) {return IDD_CROPNZOOM;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_CROPNZOOM;}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
