#ifndef _TIMGFILTERSUBTITLES_H_
#define _TIMGFILTERSUBTITLES_H_

#include "TimgFilter.h"
#include "subtitles\Tfont.h"

class TimgFilterSubtitles :public TimgFilter
{
private:
 Tfont font;
public:
 subtitle *sub;
 TimgFilterSubtitles(void);
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg);
};

#endif
