#ifndef _TIMGFILTERSUBTITLES_H_
#define _TIMGFILTERSUBTITLES_H_

#include "TimgFilter.h"
#include "..\subtitles\Tfont.h"

class TimgFilterSubtitles :public TimgFilter
{
private:
 Tfont font;
public:
 virtual void process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      Tconfig *cfg);
};

#endif
