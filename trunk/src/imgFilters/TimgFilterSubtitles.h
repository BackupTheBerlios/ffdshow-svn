#ifndef _TIMGFILTERSUBTITLES_H_
#define _TIMGFILTERSUBTITLES_H_

#include "TimgFilter.h"
#include "subtitles\Tfont.h"

class TimgFilterSubtitles :public TimgFilter
{
private:
 Tfont font;
public:
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
