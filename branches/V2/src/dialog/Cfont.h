#ifndef _CFONTPAGE_H_
#define _CFONTPAGE_H_

#include "TconfPage.h"

class TfontPage :public TconfPage
{
private:
 void font2dlg(void);
 void fillCharsets(void);
 void getCharset(int i,char **name);
 void selectCharset(int ii);
 COLORREF custColors[16];
 static const char *fontWeights[];
public:
 virtual int getDialogID(void) {return IDD_FONT;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
