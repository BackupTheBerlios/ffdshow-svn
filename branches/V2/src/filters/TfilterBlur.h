#ifndef _TFILTERBLUR_H_
#define _TFILTERBLUR_H_

#include "Tfilter.h"

class TimgFilterBlur;
struct TfilterBlur :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("blurStrength",settings.strength,strengthDef);
   t._REG_OP_N("tempSmooth",settings.tempSmooth,0);
  }
 virtual const int orderDef(void) {return 4;}
 virtual const char *shortName(void) {return "Blur";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_blurStrength:return Tparam(&settings.strength,0,255);
     case IDFF_tempSmooth:return Tparam(&settings.tempSmooth,0,10);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterBlur;}
 virtual int getID(void) {return IDFF_filterBlur;}
 virtual const char *getName(void) {return "Blur";}
 virtual Tfilter* duplicate(void);
 static TimgFilterBlur *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
   int strength;
   int tempSmooth;
  } settings;
 static const int strengthDef;
};

#endif
