#ifndef _TGLOBALSETTINGS_H_
#define _TGLOBALSETTINGS_H_

#include <vector>

struct TregOp;
struct TglobalSettings
{
private:
 void reg_op_HKLM(TregOp &t),reg_op_HKCU(TregOp &t);
public:
 int trayIcon;
 int autoPreset,autoPresetFileFirst;
 int xvid,div3,divx,dx50,mp43,mp42,mp41,h263,blz0,wmv1,rawv;
 int dontRegisterUnused;
 int outYV12,outYUY2,outYVYU,outUYVY,outRGB32,outRGB24,outRGB555,outRGB565;
 int TglobalSettings::codecSupported(DWORD fourCC,char *AVIfourCC);
 char defaultPreset[260];
 int isShowMV;
 void load(void),save(void);
 struct ToutputColorspace
  {
   ToutputColorspace(DWORD IbiCompression,const GUID *Ig,WORD IbiBitCount):biCompression(IbiCompression),g(Ig),biBitCount(IbiBitCount) {}
   DWORD biCompression;
   const GUID *g;
   WORD biBitCount; 
  };
 std::vector<ToutputColorspace> outputColorspaces;
};

#endif
