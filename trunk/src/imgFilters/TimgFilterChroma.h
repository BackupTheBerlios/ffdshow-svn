#ifndef _TIMGFILTERCHROMA_H_
#define _TIMGFILTERCHROMA_H_

#include "TimgFilter.h"

class TimgFilterChroma :public TimgFilter
{
private:
 int hueSin[360+1],hueCos[360+1];
public:
 TimgFilterChroma(void);
 virtual void process(TtempPictures *pict,const TpresetSettings *cfg);
};

#endif
