#ifndef _TIMGFILTERS_H_
#define _TIMGFILTERS_H_

#include <stdlib.h>
#include <vector>

struct TglobalSettings;
struct TpresetSettings;
struct subtitle;
struct TffPict;
class TimgFilter;
struct IffDecoder;
struct TffRect;
class TimgFilters
{
private:
 std::vector<TimgFilter*> filters;
 TimgFilter *postproc,*noise,*luma,*chroma,*blur,*sharpen,*subtitles,*offset,*timesmooth,*showMV;
public:
 TimgFilters(IffDecoder *Ideci);
 ~TimgFilters();
 void process(const TglobalSettings *global,const TpresetSettings *cfg,TffPict *pict,TffRect &rect);
};

#endif
