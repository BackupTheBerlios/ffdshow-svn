#ifndef _CNOISEPAGE_H_
#define _CNOISEPAGE_H_

#include "TconfPage.h"

class TnoisePage :public TconfPage
{
private:
 int writeNoiseStrength(int x),writeNoiseStrengthChroma(int x);
 void noise2dlg(void);
public:
 virtual int getDialogID(void) {return IDD_NOISE;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_NOISE;}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
