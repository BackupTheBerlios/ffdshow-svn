#ifndef _TIMGFILTERRESIZE_H_
#define _TIMGFILTERRESIZE_H_

#include "TimgFilter.h"

struct SwsContext;
class TimgFilterResize :public TimgFilter
{
private:
 SwsContext *swsc;
protected:
 virtual TffRect::Trect* init(TffRect *rect,int full);
public:
 TimgFilterResize(void);
 virtual void done(void);
 virtual void process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg);
};

#endif
