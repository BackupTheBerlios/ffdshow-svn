#ifndef _TFILTEROFFSET_H_
#define _TFILTEROFFSET_H_

#include "Tfilter.h"

class TimgFilterOffset;
struct TfilterOffset :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &t)
  {
   t._REG_OP_N("offsetY_X",settings.Y_X,0);
   t._REG_OP_N("offsetY_Y",settings.Y_Y,0);
   t._REG_OP_N("offsetU_X",settings.U_X,0);
   t._REG_OP_N("offsetU_Y",settings.U_Y,0);
   t._REG_OP_N("offsetV_X",settings.V_X,0);
   t._REG_OP_N("offsetV_Y",settings.V_Y,0);
  }
 virtual const int orderDef(void) {return 3;}
 virtual const char *shortName(void) {return "Offset";}
 virtual Tparam getTparam(unsigned int id)
  {
   switch (id)
    {
     case IDFF_offsetY_X:return Tparam(&settings.Y_X,-32,32);
     case IDFF_offsetY_Y:return Tparam(&settings.Y_Y,-32,32);
     case IDFF_offsetU_X:return Tparam(&settings.U_X,-32,32);
     case IDFF_offsetU_Y:return Tparam(&settings.U_Y,-32,32);
     case IDFF_offsetV_X:return Tparam(&settings.V_X,-32,32);
     case IDFF_offsetV_Y:return Tparam(&settings.V_Y,-32,32);
     default:return Tparam();
    }
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterOffset;}
 virtual int getID(void) {return IDFF_filterOffset;}
 virtual const char *getName(void) {return "Offset";}
 virtual Tfilter* duplicate(void);
 static TimgFilterOffset *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
   int Y_X,Y_Y,U_X,U_Y,V_X,V_Y;
  } settings;
};

#endif
