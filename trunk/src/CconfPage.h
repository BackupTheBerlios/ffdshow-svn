#ifndef _CCONFPAGE_H_
#define _CCONFPAGE_H_

struct IffDecoder;
class TffdshowPage;
class TconfPage
{
 protected:
  TffdshowPage *parent;
  IffDecoder *deci;
  void createWindow(int dialogId);
  int cfgGet(int i),cfgSet(int i,int val);
  void loadPreset(void);
  void enableWindow(int id,int enable);
  void setCheck(int id,int set);
  bool getCheck(int id);
 public:  
  TconfPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
  virtual ~TconfPage();
  
  HWND m_hwnd,hwndParent;
  virtual void createConfig(void)=0;
  virtual void cfg2dlg(void)=0;
  virtual void interDlg(void) {};
  virtual void applySettings(void) {};
  virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)=0;
};

#endif 
