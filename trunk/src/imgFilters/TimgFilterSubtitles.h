#ifndef _TIMGFILTERSUBTITLES_H_
#define _TIMGFILTERSUBTITLES_H_

#include "TimgFilter.h"
#include "subtitles\Tfont.h"
#include "subtitles\TfontSettings.h"

struct TpresetSettings;
class TimgFilterSubtitles :public TimgFilter
{
private:
 Tfont font;
 TfontSettings oldSettings;
public:
 TimgFilterSubtitles(void);
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
