#ifndef _CSHARPENPAGE_H_
#define _CSHARPENPAGE_H_

#include "TconfPage.h"

class TsharpenPage :public TconfPage
{
private:
 int writeSharpenStrength(int x),writeSharpenThreshold(int x);
 void sharpen2dlg(void);
public:
 virtual int getDialogID(void) {return IDD_SHARPEN;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_SHARPEN;}
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
