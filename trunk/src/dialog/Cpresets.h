#ifndef _CPRESETSPAGE_H_
#define _CPRESETSPAGE_H_

#include "TconfPage.h"
#include "Tpresets.h"

class TpresetsPage :public TconfPage
{
private:
 char oldActivePresetName[260];
 Tpresets localPresets;
 int ncol;
 void addCol(int w,const char *txt,bool right);
 void lvSelectPreset(const char *presetName);
 HWND hlv;
 WNDPROC lvOldWndProc;
 friend static LRESULT CALLBACK lvWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) ;
 char fileDlgFlnm[MAX_PATH];
 HFONT italicFont,boldFont;
public:
 TpresetsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual ~TpresetsPage();
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void applySettings(void);
};

#endif 
