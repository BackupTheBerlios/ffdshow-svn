#ifndef _TCONFPAGE_H_
#define _TCONFPAGE_H_

#include "resource.h"

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
 void createWindow(int dialogId);
 int  cfgGet(unsigned int i),cfgSet(unsigned int i,int val),cfgInv(unsigned int i);
 void enableWindow(int id,int enable);
 void setCheck(int id,int set);
 bool getCheck(int id);
 void setInter(bool is);
public:  
 TconfPage(void);
 virtual ~TconfPage();
 HWND m_hwnd,hwndParent;
 HTREEITEM hti;
 virtual int getDialogID(void)=0;
 char dialogName[256];
 TconfPage* create(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
 virtual void init(void)=0;
 virtual void cfg2dlg(void)=0;
 int index;
 void interDlg(void);
 virtual int getInterDlgID(void) {return 0;}
 int getProcessFull(void);
 virtual void getTip(char *tipS,int len) {tipS[0]='\0';}
 virtual const char* getHelpURL(void) {return NULL;}
 virtual bool reset(bool testOnly) {return false;}
 virtual bool isInPreset(void) {return false;}
 virtual void applySettings(void) {}
 virtual HRESULT msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)=0;
};

#endif 
