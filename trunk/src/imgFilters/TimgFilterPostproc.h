#ifndef _TIMGFILTERPOSTPROC_H_
#define _TIMGFILTERPOSTPROC_H_

#include "TimgFilter.h"

class TmovieSource;
class Tpostproc;
class TimgFilterPostproc :public TimgFilter
{
public:
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
