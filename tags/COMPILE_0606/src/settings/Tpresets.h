#ifndef _TPRESETS_H_
#define _TPRESETS_H_

#include <vector>
#include <string>
#include "TpresetSettings.h"

class Tpresets :public std::vector<TpresetSettings*>
{
private:
 iterator findPreset(const char *presetName);
 void listRegKeys(std::vector<std::string> &list);
public:
 ~Tpresets();
 void init(void);
 void done(void);
 TpresetSettings* getPreset(const char *presetName);
 TpresetSettings* getAutoPreset(const char *flnm,bool filefirst);
 void savePreset(TpresetSettings *preset,const char *presetName);
 void storePreset(TpresetSettings *preset);
 void removePreset(const char *presetName);
 void saveRegAll(void);
 void nextUniqueName(TpresetSettings *preset),nextUniqueName(char *presetName); 
};

#endif
