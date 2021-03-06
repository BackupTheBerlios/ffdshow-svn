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
 public:
  TsubtitlesPage(HWND IhwndParent,IffDecoder *Ideci,int dialogId);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
};

#endif 
