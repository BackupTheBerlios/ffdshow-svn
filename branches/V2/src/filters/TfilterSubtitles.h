#ifndef _TFILTERSUBTITLES_H_
#define _TFILTERSUBTITLES_H_

#include "Tfilter.h"
#include "subtitles\TfontSettings.h"

class TimgFilterSubtitles;
struct TfilterSubtitles :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("subPosX",settings.posX,50);
   t._REG_OP_N("subPosY",settings.posY,90);
   t._REG_OP_N("subDelay",settings.delay,delayDef);
   t._REG_OP_N("subSpeed",settings.speed,speedDef);
   t._REG_OP_S("subFlnm",settings.flnm,"");
   t._REG_OP_N("subAutoFlnm",settings.autoFlnm,1);
   fontSettings.reg_op(t);
  }
 virtual const int orderDef(void) {return 7;}
 virtual const char *shortName(void) {return "Subtitles";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_fontCharset       :return Tparam(&fontSettings.settings.charset);
     case IDFF_fontSize          :return Tparam(&fontSettings.settings.size          ,2,255);
     case IDFF_fontWeight        :return Tparam(&fontSettings.settings.weight        ,0,900);
     case IDFF_fontColor         :return Tparam(&fontSettings.settings.color);
     case IDFF_fontShadowStrength:return Tparam(&fontSettings.settings.shadowStrength,0,100);
     case IDFF_fontShadowRadius  :return Tparam(&fontSettings.settings.shadowRadius  ,1,100);
     case IDFF_fontSpacing       :return Tparam(&fontSettings.settings.spacing       ,-10,10);
     case IDFF_fontName          :return Tparam(fontSettings.settings.name);
     case IDFF_subAutoFlnm       :return Tparam(&settings.autoFlnm);
     case IDFF_subPosX           :return Tparam(&settings.posX,0,100);
     case IDFF_subPosY           :return Tparam(&settings.posY,0,100);
     case IDFF_subDelay          :return Tparam(&settings.delay);
     case IDFF_subSpeed          :return Tparam(&settings.speed);
     case IDFF_subDelayDef       :return Tparam(&delayDef);
     case IDFF_subSpeedDef       :return Tparam(&speedDef);
     case IDFF_subFlnm           :return Tparam(settings.flnm);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterSubtitles;}
 virtual int getID(void) {return IDFF_filterSubtitles;}
 virtual const char *getName(void) {return "Subtitles";}
 virtual Tfilter* duplicate(void);
 static TimgFilterSubtitles *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 virtual TconfPage *getConfSubPage(void);
 struct Tsettings
  {
   Tsettings(void) {memset(flnm,0,260);}
   char flnm[260];
   int autoFlnm;
   int posX,posY;
   int delay,speed;
  } settings;
 static const int delayDef,speedDef;
 TfontSettings fontSettings; 
};

#endif
