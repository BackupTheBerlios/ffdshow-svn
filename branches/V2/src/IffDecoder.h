#ifndef _IFFDECODER_H_
#define _IFFDECODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IffConstants.h"

struct TpresetSettings;
class Tpresets;
class TmovieSource;
class Tpostproc;
class TconfPage;
DECLARE_INTERFACE_(IffDecoder, IUnknown)
{
 STDMETHOD (getParam)(unsigned int paramID, int* value) PURE;
 STDMETHOD (getParam2)(unsigned int paramID) PURE;
 STDMETHOD (putParam)(unsigned int paramID, int  value) PURE;
 STDMETHOD (getNumPresets)(unsigned int *value) PURE;
 STDMETHOD (getPresetName)(unsigned int i,char *buf,unsigned int len) PURE;
 STDMETHOD (getActivePresetName)(char *buf,unsigned int len) PURE;
 STDMETHOD (setActivePreset)(const char *name) PURE;
 STDMETHOD (getAVIname)(char *buf,unsigned int len) PURE;
 STDMETHOD (getAVIfourcc)(char *buf,unsigned int len) PURE;
 STDMETHOD (getAVIdimensions)(unsigned int *x,unsigned int *y) PURE;
 STDMETHOD (getAVIfps)(unsigned int *fps1000) PURE;
 STDMETHOD (getAVIfps2)(void) PURE;
 STDMETHOD (saveActivePreset)(const char *name) PURE;
 STDMETHOD (saveActivePresetToFile)(const char *flnm) PURE;
 STDMETHOD (loadActivePresetFromFile)(const char *flnm) PURE;
 STDMETHOD (removePreset)(const char *name) PURE;
 STDMETHOD (notifyParamsChanged)(void) PURE;
 STDMETHOD (getAVcodecVersion)(char *buf,unsigned int len) PURE;
 //STDMETHOD (getFontName)(char *buf,unsigned int len) PURE;
 //STDMETHOD (setFontName)(const char *name) PURE;
 //STDMETHOD (getSubFlnm)(char *buf,unsigned int len) PURE;
 //STDMETHOD (loadSubtitles)(const char *flnm) PURE;
 STDMETHOD (getRealCrop)(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom) PURE;
 STDMETHOD (saveGlobalSettings)(void) PURE;
 STDMETHOD (loadGlobalSettings)(void) PURE;
 STDMETHOD (saveDialogSettings)(void) PURE;
 STDMETHOD (loadDialogSettings)(void) PURE;
 STDMETHOD (getPresets)(Tpresets *presets2) PURE;
 STDMETHOD (setPresets)(const Tpresets *presets2) PURE;
 STDMETHOD (savePresets)(void) PURE;
 STDMETHOD (setPresetPtr)(TpresetSettings *preset) PURE;
 STDMETHOD (renameActivePreset)(const char *newName) PURE;
 STDMETHOD (setOnChangeMsg)(HWND wnd,unsigned int msg) PURE;
 STDMETHOD (setOnInfoMsg)(HWND wnd,unsigned int msg1,unsigned int msg2) PURE;
 STDMETHOD (getDefaultPresetName)(char *buf,unsigned int len) PURE;
 STDMETHOD (setDefaultPresetName)(const char *presetName) PURE;
 STDMETHOD (isDefaultPreset)(const char *presetName) PURE;
 STDMETHOD (showCfgDlg)(HWND owner) PURE;
 STDMETHOD (getXvidVersion)(char *buf,unsigned int len) PURE;
 STDMETHOD (getMovieSource)(TmovieSource* *moviePtr) PURE;
 STDMETHOD (getPostproc)(Tpostproc* *postprocPtr) PURE;
 STDMETHOD (getOutputDimensions)(unsigned int *x,unsigned int *y) PURE;
 STDMETHOD (presetGetNumFilters)(unsigned int *num) PURE;
 STDMETHOD (presetGetNumFilters2)(void) PURE;
 STDMETHOD (presetGetFilterName)(unsigned int i,char *buf,unsigned int len) PURE;
 STDMETHOD (presetGetFilterIs2)(unsigned int i) PURE;
 STDMETHOD (presetSetFilterIs)(unsigned int i,int Iis) PURE;
 STDMETHOD (presetGetFilterFull2)(unsigned int i) PURE;
 STDMETHOD (presetSetFilterFull)(unsigned int i,int Ifull) PURE;
 STDMETHOD (presetGetConfPage)(unsigned int i,TconfPage* *page) PURE; 
 STDMETHOD (presetGetConfSubPage)(unsigned int i,TconfPage* *subPage) PURE; 
 STDMETHOD (presetSetFilterOrder)(unsigned int oldOrder,unsigned int newOrder) PURE;
 STDMETHOD (presetGetPPmode)(unsigned int index,unsigned int *ppmode) PURE;
 STDMETHOD (presetGetParam)(unsigned int index,unsigned int paramID,int *value) PURE;
 STDMETHOD (presetGetParam2)(unsigned int index,unsigned int paramID) PURE;
 STDMETHOD (presetPutParam)(unsigned int index,unsigned int paramID,int value) PURE;
 STDMETHOD (presetGetParamStr)(unsigned int index,unsigned int paramID,char *value,unsigned int len) PURE;
 STDMETHOD (presetPutParamStr)(unsigned int index,unsigned int paramID,char *value) PURE;
 STDMETHOD (getCurrentFrame2)(void) PURE;
 STDMETHOD (getCfcs)(void* *cfcs) PURE;
};

#ifdef __cplusplus
}
#endif

#endif
