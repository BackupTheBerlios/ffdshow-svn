#ifndef _TFILTERCROPNZOOM_H_
#define _TFILTERCROPNZOOM_H_

#include "Tfilter.h"

class TimgFilterCropNzoom;
struct TfilterCropNzoom :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("isZoom",settings.isZoom,1);
   t._REG_OP_N("magnificationX"     ,settings.magnificationX,0);
   t._REG_OP_N("magnificationY"     ,settings.magnificationY,0);
   t._REG_OP_N("magnificationLocked",settings.magnificationLocked,1);
   t._REG_OP_N("cropLeft"  ,settings.cropLeft,0);
   t._REG_OP_N("cropRight" ,settings.cropRight ,0);
   t._REG_OP_N("cropTop"   ,settings.cropTop   ,0);
   t._REG_OP_N("cropBottom",settings.cropBottom,0);
   t._REG_OP_N("autocrop"  ,settings.autocrop,0);
  }
 virtual const int orderDef(void) {return 0;}
 virtual const char *shortName(void) {return "CropNzoom";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_isZoom             :return Tparam(&settings.isZoom             );
     case IDFF_magnificationX     :return Tparam(&settings.magnificationX     ,0,100);
     case IDFF_magnificationY     :return Tparam(&settings.magnificationY     ,0,100);
     case IDFF_magnificationLocked:return Tparam(&settings.magnificationLocked);
     case IDFF_cropLeft           :return Tparam(&settings.cropLeft           ,0,2048);
     case IDFF_cropRight          :return Tparam(&settings.cropRight          ,0,2048);
     case IDFF_cropTop            :return Tparam(&settings.cropTop            ,0,2048);
     case IDFF_cropBottom         :return Tparam(&settings.cropBottom         ,0,2048);
     case IDFF_autocrop           :return Tparam(&settings.autocrop           ,0,0);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterCropNzoom;}
 virtual int getID(void) {return IDFF_filterCropNzoom;}
 virtual const char *getName(void) {return "Crop & zoom";}
 virtual Tfilter* duplicate(void);
 static TimgFilterCropNzoom *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
   int isZoom;
   int magnificationX,magnificationY,magnificationLocked;
   int cropTop,cropBottom,cropLeft,cropRight;
   int autocrop;
  } settings;
};

static inline bool operator!=(const TfilterCropNzoom::Tsettings &s1,const TfilterCropNzoom::Tsettings &s2)
{
 return memcmp(&s1,&s2,sizeof(s1))!=0;
}

#endif
