#ifndef _TIMGFILTEROSD_H_
#define _TIMGFILTEROSD_H_

#include "TimgFilter.h"
#include "subtitles\Tfont.h"
#include "subtitles\TfontSettings.h"

class TimgFilterOSD :public TimgFilter
{
private:
 Tfont font;
 TfontSettings oldSettings;
public:
 TimgFilterOSD(void);
 virtual void process(TffPict2 &pict,const TpresetSettings *cfg);
};

#endif
