#ifndef _CSUBTITLESPAGE_H_
#define _CSUBTITLESPAGE_H_

#include "CconfPage.h"

class TsubtitlesPage :public TconfPage
{
 private:
  void font2dlg(void),sub2dlg(void);
  void fillCharsets(void);
  void selectCharset(int ii);
  COLORREF custColors[16];
  void loadSubtitles(void);
 public:
  TsubtitlesPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci,int dialogId);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
  virtual void interDlg(void);
  virtual void applySettings(void);
};

#endif 
