#ifndef _TTRAYICON_H_
#define _TTRAYICON_H_

struct IffDecoder;
class TtrayIcon
{
private:
 HWND h;
 HINSTANCE hi;
 NOTIFYICONDATA nid;
 bool visible;
 void insertSeparator(HMENU hm,int &ord),insertSubmenu(HMENU hm,int &ord,char *caption,HMENU subMenu);
 void insertMenuItem(HMENU hm,int &ord,int id,char *caption,int cfgId,int negate=0);
 void insertMenuItem(HMENU hm,int &ord,int id,char *caption,bool checked);
 HMENU createPostProcMenu(void);
public:
 TtrayIcon(IffDecoder *Ideci,HINSTANCE Ihi);
 ~TtrayIcon();
 IffDecoder *deci;
 void show(void),hide(void);
 int cfgGet(int id),cfgSet(int id,int val),negate_Param(int id);
 HMENU createMenu(void);
};

#endif
