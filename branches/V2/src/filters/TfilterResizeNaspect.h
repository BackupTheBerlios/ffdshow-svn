#ifndef _TFILTERRESIZENASPECT_H_
#define _TFILTERRESIZENASPECT_H_

#include "Tfilter.h"

struct SwsContext;
class TimgFilterResizeNaspect;
struct TfilterResizeNaspect :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("resizeDx",settings.dx,640);
   t._REG_OP_N("resizeDy",settings.dy,480);       
   t._REG_OP_N("resizeAscpect",settings.isAspect,1);
   t._REG_OP_N("resizeMethod",settings.method,2);
   t._REG_OP_N("aspectRatio",settings.aspectRatio,1.33*65536);
   t._REG_OP_N("resizeGblurLum",settings.GblurLum,0);
   t._REG_OP_N("resizeGblurChrom",settings.GblurChrom,0);
   t._REG_OP_N("resizeSharpenLum",settings.sharpenLum,0);
   t._REG_OP_N("resizeSharpenChrom",settings.sharpenChrom,0);
  }
 virtual const int orderDef(void) {return 8;}
 virtual const char *shortName(void) {return "Resize";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_resizeDx          :return Tparam(&settings.dx          ,16,2048);
     case IDFF_resizeDy          :return Tparam(&settings.dy          ,16,2048);
     case IDFF_isAspect          :return Tparam(&settings.isAspect);
     case IDFF_aspectRatio       :return Tparam(&settings.aspectRatio);
     case IDFF_resizeMethod      :return Tparam(&settings.method      ,0,methodNone);
     case IDFF_resizeGblurLum    :return Tparam(&settings.GblurLum    ,0,200);
     case IDFF_resizeGblurChrom  :return Tparam(&settings.GblurChrom  ,0,200);
     case IDFF_resizeSharpenLum  :return Tparam(&settings.sharpenLum  ,0,200);
     case IDFF_resizeSharpenChrom:return Tparam(&settings.sharpenChrom,0,200);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterResizeNaspect;}
 virtual int getID(void) {return IDFF_filterResizeNaspect;}
 virtual const char *getName(void) {return "Resize & aspect";}
 virtual Tfilter* duplicate(void);
 static TimgFilterResizeNaspect *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 virtual TconfPage *getConfSubPage(void);
 struct Tsettings
  {
   int dx,dy;
   int isAspect;
   int aspectRatio;
   int method;
   int GblurLum,GblurChrom;
   int sharpenLum,sharpenChrom;
  } settings;
 static const int methodNone;
};

static inline bool operator!=(const TfilterResizeNaspect::Tsettings &s1,const TfilterResizeNaspect::Tsettings &s2)
{
 return memcmp(&s1,&s2,sizeof(s1))!=0;
}

#endif
