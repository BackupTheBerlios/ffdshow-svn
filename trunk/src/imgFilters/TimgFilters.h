#ifndef _TIMGFILTERS_H_
#define _TIMGFILTERS_H_

#include <stdlib.h>
#include <vector>

struct TglobalSettings;
struct TpresetSettings;
struct subtitle;
struct TffPict2;
class TimgFilter;
struct IffDecoder;
class TimgFilters
{
private:
 std::vector<TimgFilter*> filters;
 TimgFilter *crop,*postproc,*noise,*luma,*chroma,*blur,*sharpen,*subtitles,*offset,*timesmooth,*showMV,*resize;
public:
 TimgFilters(IffDecoder *Ideci);
 ~TimgFilters();
 void process(const TglobalSettings *global,const TpresetSettings *cfg,TffPict2 &pict);
};

#endif
