#ifndef _CMISCPAGE_H_
#define _CMISCPAGE_H_

#include "TconfPage.h"

class TmiscPage :public TconfPage
{
private:
 static const char *idctNames[];
public:
 virtual int getDialogID(void) {return IDD_MISC;}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
