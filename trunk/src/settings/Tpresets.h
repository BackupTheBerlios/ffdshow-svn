#ifndef _TPRESETS_H_
#define _TPRESETS_H_

#include <vector>
#include "TpresetSettings.h"

class Tpresets :public std::vector<TpresetSettings*>
{
private:
 iterator findPreset(const char *presetName);
public:
 void init(void);
 ~Tpresets();
 TpresetSettings* getPreset(const char *presetName);
 void savePreset(TpresetSettings *preset,const char *presetName);
 void storePreset(TpresetSettings *preset);
 void removePreset(const char *presetName);
};

#endif
