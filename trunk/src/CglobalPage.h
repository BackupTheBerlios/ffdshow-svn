#ifndef _CGLOBALPAGE_H_
#define _CGLOBALPAGE_H_

#include "CconfPage.h"

class TglobalPage :public TconfPage
{
 private:
  void listPresets(void);
  void now2dlg(void);
  void selectPreset(void);
  void removePreset(void);
  void savePresetToFile(void);
  void loadPresetFromFile(void);
 public:
  TglobalPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci,int dialogId);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
  virtual void interDlg(void);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

  void savePreset(void);
  void loadPreset(void);
};

#endif /* _CCONFIGURATION_H_ */
