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
 virtual void interDlg(void);
 virtual void applySettings(void);
 virtual int getInter(void) {return cfgGet(IDFF_isSubtitles);}
 virtual int invInter(void) {return cfgInv(IDFF_isSubtitles);}
 virtual int getOrder(void) {return cfgGet(IDFF_orderSubtitles);}
 virtual void setOrder(int o) {cfgSet(IDFF_orderSubtitles,o);}
 virtual void getTip(char *tipS,int len);
 virtual int  getProcessFull(void) {return cfgGet(IDFF_fullSubtitles);}
 virtual void setProcessFull(int full) {cfgSet(IDFF_fullSubtitles,full);} 
 virtual bool isInPreset(void) {return true;}
};

#endif 
