#ifndef _CFILTERSPAGE_H_
#define _CFILTERSPAGE_H_

#include "CconfPage.h"

class TfiltersPage :public TconfPage
{
 private:
  int writeLumGain(int x),writeLumOffset(int x),writeGamma(int x);
  int writeHue(int x),writeSaturation(int x);
  int writeSharpenStrength(int x),writeSharpenThreshold(int x);
  int writeNoiseStrength(int x),writeNoiseStrengthChroma(int x);
  void setPPchbs(void);
  void postProc2dlg(void),noise2dlg(void),pictProp2dlg(void),sharpen2dlg(void);
 public:
  TfiltersPage(HWND IhwndParent,IffDecoder *Ideci,int dialogId);
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void createConfig(void);
  virtual void cfg2dlg(void);
};

#endif 
