#ifndef _TFILTERNOISE_H_
#define _TFILTERNOISE_H_

#include "Tfilter.h"

class TimgFilterNoise;
struct TfilterNoise :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("noiseMethod",settings.method,0);
   t._REG_OP_N("noiseStrength",settings.strength,strengthDef);
   t._REG_OP_N("noiseStrengthChroma",settings.strengthChroma,strengthChromaDef);
   t._REG_OP_N("uniformNoise",settings.uniform,0);
  }
 virtual const int orderDef(void) {return 6;}
 virtual const char *shortName(void) {return "Noise";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_noiseMethod        :return Tparam(&settings.method        ,0,1);
     case IDFF_uniformNoise       :return Tparam(&settings.uniform       );
     case IDFF_noiseStrength      :return Tparam(&settings.strength      ,0,255);
     case IDFF_noiseStrengthChroma:return Tparam(&settings.strengthChroma,0,127);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterNoise;}
 virtual int getID(void) {return IDFF_filterNoise;}
 virtual const char *getName(void) {return "Noise";}
 virtual Tfilter* duplicate(void);
 static TimgFilterNoise *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
   int method,uniform;
   int strength,strengthChroma;
  } settings;
 static const int strengthDef,strengthChromaDef;
};

#endif
