#ifndef _TFILTERSHOWMV_H_
#define _TFILTERSHOWMV_H_

#include "Tfilter.h"

class TimgFilterShowMV;
struct TfilterShowMV :public Tfilter
{
//--------- settings ----------
protected:
 virtual void reg_op_settings(TregOp &)
  {
  }
 virtual const int orderDef(void) {return 9;}
 virtual const char *shortName(void) {return "ShowMV";}
 virtual Tparam getTparam(unsigned int)
  {
   return Tparam();
  }
public:
 static void init(TcreateFcVector *cfcs,IffDecoder *deci);
 static Tfilter* create(void) {return new TfilterShowMV;}
 virtual int getID(void) {return IDFF_filterShowMV;}
 virtual const char *getName(void) {return "Show motion vectors";}
 virtual Tfilter* duplicate(void);
 static TimgFilterShowMV *imgFilter;
 virtual TimgFilter *getImgFilter(void);
 virtual TconfPage *getConfPage(void);
 struct Tsettings
  {
  } settings;
};

#endif
