#ifndef _CSUBTITLESPAGE_H_
#define _CSUBTITLESPAGE_H_

#include "TconfPage.h"

class TsubtitlesPage :public TconfPage
{
private:
 HBRUSH red;
 void sub2dlg(void);
 void loadSubtitles(void);
 void getPosHoriz(int x,char *s),getPosVert(int x,char *s);
public:
 TsubtitlesPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void applySettings(void);
 virtual void getTip(char *tipS,int len);
};

#endif 
