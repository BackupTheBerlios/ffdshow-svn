#ifndef _TIMGFILTERSUBTITLES_H_
#define _TIMGFILTERSUBTITLES_H_

#include "TimgFilter.h"
#include "subtitles\Tfont.h"
#include "subtitles\TfontSettings.h"

class Tsubtitles;
class TimgFilterSubtitles :public TimgFilter
{
private:
 TfontSettings::Tsettings settingsOld;
 Tfont font;
 char oldSubFlnm[260];
 Tsubtitles *subs;
public:
 TimgFilterSubtitles(void);
 virtual ~TimgFilterSubtitles();
 virtual void process(TffPict2 &pict,const Tfilter *cfg0);
};

#endif
