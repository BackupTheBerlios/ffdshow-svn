#ifndef _TFILTERPOSTPROC_H_
#define _TFILTERPOSTPROC_H_

#include "Tfilter.h"

class TimgFilterPostproc;
struct TfilterPostproc :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("ppIsCustom",settings.isCustom,0);
   t._REG_OP_N("ppqual",settings.qual,0);
   t._REG_OP_N("autoq",settings.autoq,0);
   t._REG_OP_N("ppcustom",settings.custom,0);
   t._REG_OP_N("deblockStrength",settings.deblockStrength,deblockStrengthDef);
   t._REG_OP_N("levelFixLum",settings.levelFixLum,0);
   t._REG_OP_N("levelFixChrom",settings.levelFixChrom,0);settings.levelFixChrom=0;
  }
 virtual const int orderDef(void) {return 1;}
 virtual const char *shortName(void) {return "Postproc";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_ppqual         :return Tparam(&settings.qual,0,6);
     case IDFF_autoq          :return Tparam(&settings.autoq);
     case IDFF_ppIsCustom     :return Tparam(&settings.isCustom);
     case IDFF_ppcustom       :return Tparam(&settings.custom);
     case IDFF_deblockStrength:return Tparam(&settings.deblockStrength,0,512);
     case IDFF_levelFixLum    :return Tparam(&settings.levelFixLum);
     case IDFF_levelFixChrom  :return Tparam(&settings.levelFixChrom);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterPostproc;}
 virtual int getID(void) {return IDFF_filterPostproc;}
 virtual const char *getName(void) {return "Postprocessing";}
 virtual Tfilter* duplicate(void);
 static TimgFilterPostproc *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
   int qual;
   int autoq;
   int isCustom,custom;
   int deblockStrength;
   int levelFixLum,levelFixChrom;
  } settings;
 static const int deblockStrengthDef;
};

#endif
