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
 virtual void process(TtempPictures *pict,const TpresetSettings *cfg);
};

#endif
