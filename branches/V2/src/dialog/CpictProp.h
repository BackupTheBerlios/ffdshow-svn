#ifndef _CPICTPROPPAGE_H_
#define _CPICTPROPPAGE_H_

#include "TconfPage.h"

class TpictPropPage :public TconfPage
{
private:
 int writeLumGain(int x),writeLumOffset(int x),writeGamma(int x),writeHue(int x),writeSaturation(int x);
 void pictProp2dlg(void);
public:
 virtual int getDialogID(void) {return IDD_PICTPROP;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual int getInterDlgID(void) {return IDC_CHB_PICTPROP;}
 virtual bool reset(bool testOnly);
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
