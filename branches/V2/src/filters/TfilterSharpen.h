#ifndef _TFILTERSHARPEN_H_
#define _TFILTERSHARPEN_H_

#include "Tfilter.h"

class TimgFilterSharpen;
struct TfilterSharpen :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("sharpenMethod",settings.method,0);
   t._REG_OP_N("xsharp_strenght",settings.xsharpStrength,xsharpStrengthDef);
   t._REG_OP_N("xsharp_threshold",settings.xsharpThreshold,xsharpThresholdDef);
   t._REG_OP_N("unsharp_strength",settings.unsharpStrength,unsharpStrengthDef);
   t._REG_OP_N("unsharp_threshold",settings.unsharpThreshold,unsharpThresholdDef);
  }
 virtual const int orderDef(void) {return 5;}
 virtual const char *shortName(void) {return "Sharpen";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_sharpenMethod    :return Tparam(&settings.method          ,0,1);
     case IDFF_xsharp_strength  :return Tparam(&settings.xsharpStrength  ,1,127);
     case IDFF_xsharp_threshold :return Tparam(&settings.xsharpThreshold ,0,255);
     case IDFF_unsharp_strength :return Tparam(&settings.unsharpStrength ,1,127);
     case IDFF_unsharp_threshold:return Tparam(&settings.unsharpThreshold,0,255);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterSharpen;}
 virtual int getID(void) {return IDFF_filterSharpen;}
 virtual const char *getName(void) {return "Sharpen";}
 virtual Tfilter* duplicate(void);
 static TimgFilterSharpen *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
   int method;
   int xsharpStrength,xsharpThreshold;
   int unsharpStrength,unsharpThreshold;
  } settings;
 static const int xsharpStrengthDef,xsharpThresholdDef;
 static const int unsharpStrengthDef,unsharpThresholdDef;
};

#endif
