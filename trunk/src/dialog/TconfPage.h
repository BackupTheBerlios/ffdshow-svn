#ifndef _TCONFPAGE_H_
#define _TCONFPAGE_H_

struct IffDecoder;
class TffdshowPage;
class TconfPage
{
private:
 char *helpStr;
protected:
 TffdshowPage *parent;
 IffDecoder *deci;
 HINSTANCE hi;
 void createWindow(int IdialogId);
 int  cfgGet(unsigned int i),cfgSet(unsigned int i,int val),cfgInv(unsigned int i);
 void enableWindow(int id,int enable);
 void setCheck(int id,int set);
 bool getCheck(int id);
public:  
 TconfPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual ~TconfPage();
 HWND m_hwnd,hwndParent;
 int dialogId;char dialogName[256];
 virtual void init(void)=0;
 virtual void cfg2dlg(void)=0;
 virtual void interDlg(void) {}
 virtual int  getInter(void) {return -1;}
 virtual int  invInter(void) {return -1;}
 virtual int  getOrder(void) {return -1;}
 virtual void getTip(char *tipS,int len) {tipS[0]='\0';}
 virtual const char* getHelpURL(void) {return NULL;}
 virtual bool reset(bool testOnly) {return false;}
 virtual int  getProcessFull(void) {return -1;}
 virtual void setProcessFull(int full) {} 
 virtual void setOrder(int o) {}
 virtual bool isInPreset(void) {return false;}
 virtual void applySettings(void) {}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)=0;
};

#endif 
