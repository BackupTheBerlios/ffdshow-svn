#ifndef _TFONTSETTINGS_H_
#define _TFONTSETTINGS_H_

#include "reg.h"

struct TfontSettings
{
public:
 struct Tsettings
  {
   Tsettings(void) {memset(name,0,sizeof(name));}
   char name[256];
   int charset,size,spacing,weight,color,shadowStrength,shadowRadius;
  } settings;
 void reg_op(TregOp &t)
  {
   t._REG_OP_S("fontName",settings.name,"Arial");     
   t._REG_OP_N("fontCharset",settings.charset,DEFAULT_CHARSET);     
   t._REG_OP_N("fontSize",settings.size,26);     
   t._REG_OP_N("fontWeight",settings.weight,FW_NORMAL);if (settings.weight>900) settings.weight=900;
   t._REG_OP_N("fontColor",settings.color,RGB(255,255,255));     
   t._REG_OP_N("fontShadowStrength",settings.shadowStrength,90);     
   t._REG_OP_N("fontShadowRadius"  ,settings.shadowRadius  ,50);     
   t._REG_OP_N("fontSpacing",settings.spacing,0);
  }
};

static inline bool operator!=(const TfontSettings::Tsettings &s1,const TfontSettings::Tsettings &s2)
{
 return memcmp(&s1,&s2,sizeof(s1))!=0;
}

#endif
