#ifndef _CRESIZESETTINGSPAGE_H_
#define _CRESIZESETTINGSPAGE_H_

#include "TconfPage.h"

class TresizeSettingsPage :public TconfPage
{
private:
 void resizeSettings2dlg(void);
 static const char *algorithmsNames[];
public:
 TresizeSettingsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char *tipS,int len);
 virtual bool isInPreset(void) {return true;}
};

#endif 
