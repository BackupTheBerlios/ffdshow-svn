#ifndef _TGLOBALSETTINGS_H_
#define _TGLOBALSETTINGS_H_

struct TglobalSettings
{
 int trayIcon;
 int autoPreset,autoPresetFileFirst;
 int xvid,div3,divx,dx50,mp43,mp42,mp41,h263,rawv;
 int TglobalSettings::codecSupported(DWORD fourCC,char *AVIfourCC);
 char defaultPreset[260];
 int showMV;
 void load(void),save(void);
};

#endif
