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
 int idffInter,idffFull;int resInter;
 void createWindow(void);
 int  cfgGet(unsigned int i),cfgSet(unsigned int i,int val),cfgInv(unsigned int i);
 void enableWindow(int id,int enable);
 void setCheck(int id,int set);
 bool getCheck(int id);
public:  
 TconfPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual ~TconfPage();
 HWND m_hwnd,hwndParent;
 HTREEITEM hti;
 int dialogId;char dialogName[256];
 virtual void init(void)=0;
 virtual void cfg2dlg(void)=0;
 int inPreset;
 int getInter(void),invInter(void);
 void interDlg(void);
 virtual void setOrder(int o) {}
 virtual int  getOrder(void) {return -1;}
 int  getProcessFull(void);
 void setProcessFull(int full);
 virtual void getTip(char *tipS,int len) {tipS[0]='\0';}
 virtual const char* getHelpURL(void) {return NULL;}
 virtual bool reset(bool testOnly) {return false;}
 virtual void applySettings(void) {}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)=0;
};

#endif 
