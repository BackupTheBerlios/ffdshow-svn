#ifndef _TFILTERPICTPROP_H_
#define _TFILTERPICTPROP_H_

#include "Tfilter.h"

class TimgFilterPictProp;
struct TfilterPictProp :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("lumGain",settings.lumGain,lumGainDef);
   t._REG_OP_N("lumOffset",settings.lumOffset,lumOffsetDef);
   t._REG_OP_N("gammaCorrection",settings.gammaCorrection,gammaCorrectionDef);
   t._REG_OP_N("hue",settings.hue,hueDef);
   t._REG_OP_N("saturation",settings.saturation,saturationDef);
  }
 virtual const int orderDef(void) {return 2;}
 virtual const char *shortName(void) {return "PictProp";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_lumGain           :return Tparam(&settings.lumGain        ,0,256);
     case IDFF_lumOffset         :return Tparam(&settings.lumOffset      ,-256,256);
     case IDFF_gammaCorrection   :return Tparam(&settings.gammaCorrection,1,400);
     case IDFF_hue               :return Tparam(&settings.hue            ,-180,180);
     case IDFF_saturation        :return Tparam(&settings.saturation     ,0,128);
     case IDFF_lumGainDef        :return Tparam(&lumGainDef        );
     case IDFF_lumOffsetDef      :return Tparam(&lumOffsetDef      );
     case IDFF_gammaCorrectionDef:return Tparam(&gammaCorrectionDef);
     case IDFF_hueDef            :return Tparam(&hueDef            );
     case IDFF_saturationDef     :return Tparam(&saturationDef     );
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterPictProp;}
 virtual int getID(void) {return IDFF_filterPictProp;}
 virtual const char *getName(void) {return "Picture properties";}
 virtual Tfilter* duplicate(void);
 static TimgFilterPictProp *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
   int lumGain,lumOffset,gammaCorrection;
   int hue,saturation;
  } settings;
 static const int lumGainDef,lumOffsetDef,gammaCorrectionDef;
 static const int hueDef,saturationDef;
};

#endif
