#ifndef _CPRESETSPAGE_H_
#define _CPRESETSPAGE_H_

#include "TconfPage.h"
#include "Tpresets.h"

class TpresetsPage : public TconfPage
{
 private:
  Tpresets localPresets;
  int ncol;
  void addCol(int w,const char *txt,bool right);
  HWND hlv;
 public:
  TpresetsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void init(void);
  virtual void cfg2dlg(void);
  virtual void getTip(char *tipS,int len);
};

#endif 
