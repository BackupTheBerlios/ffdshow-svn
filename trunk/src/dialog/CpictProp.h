#ifndef _CPICTPROPPAGE_H_
#define _CPICTPROPPAGE_H_

#include "TconfPage.h"

class TpictPropPage :public TconfPage
{
private:
 int writeLumGain(int x),writeLumOffset(int x),writeGamma(int x),writeHue(int x),writeSaturation(int x);
 void pictProp2dlg(void);
public:
 TpictPropPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void interDlg(void);
 virtual int getInter(void) {return cfgGet(IDFF_isPictProp);}
 virtual int invInter(void) {return cfgInv(IDFF_isPictProp);}
 virtual int getOrder(void) {return cfgGet(IDFF_orderPictProp);}
 virtual void setOrder(int o) {cfgSet(IDFF_orderPictProp,o);}
 virtual bool reset(bool testOnly);
 virtual int  getProcessFull(void) {return cfgGet(IDFF_fullPictProp);}
 virtual void setProcessFull(int full) {cfgSet(IDFF_fullPictProp,full);} 
 virtual void getTip(char *tipS,int len);
};

#endif 
