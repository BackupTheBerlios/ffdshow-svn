#ifndef _CDEINTERLACEPAGE_H_
#define _CDEINTERLACEPAGE_H_

#include "TconfPage.h"

class TdeinterlacePage :public TconfPage
{
private:
 static const int deintRbts[];
 static const char* deintNames[];
public:
 TdeinterlacePage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char *tipS,int len);
};

#endif 
