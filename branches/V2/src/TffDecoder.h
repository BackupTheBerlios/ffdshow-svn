#ifndef _TFFDECOEDR_H_
#define _TFFDECODER_H_

#include "IffDecoder.h"
#include "TglobalSettings.h"
#include "TdialogSettings.h"
#include "Tpresets.h"
#include "Tpostproc.h"

class TtrayIcon;
struct TpresetSettings;
struct TfilterResizeNaspect;
class TffDecoder : public CVideoTransformFilter, public IffDecoder, public ISpecifyPropertyPages
{
public:
 static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
 #ifdef DEBUG
 STDMETHODIMP_(ULONG) AddRef();
 STDMETHODIMP_(ULONG) Release();
 STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
  {
   return GetOwner()->QueryInterface(riid,ppv);
  }
 #else
 DECLARE_IUNKNOWN
 #endif

 TffDecoder(LPUNKNOWN punk, HRESULT *phr);
 virtual ~TffDecoder();

 STDMETHODIMP JoinFilterGraph(IFilterGraph * pGraph,LPCWSTR pName);

 HRESULT CheckInputType(const CMediaType * mtIn);
 HRESULT GetMediaType(int iPos, CMediaType * pmt);
 HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);

 HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
 HRESULT DecideBufferSize(IMemAllocator * pima, ALLOCATOR_PROPERTIES * pProperties);

 HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);

 STDMETHODIMP GetPages(CAUUID * pPages);
 STDMETHODIMP FreePages(CAUUID * pPages);

 STDMETHODIMP getParam(unsigned int paramID, int* value);
 STDMETHODIMP getParam2(unsigned int paramID);
 STDMETHODIMP putParam(unsigned int paramID, int  value);
 STDMETHODIMP getPresetName(unsigned int i,char *buf,unsigned int len);
 STDMETHODIMP getNumPresets(unsigned int *value);
 STDMETHODIMP setActivePreset(const char *name);
 STDMETHODIMP getActivePresetName(char *buf,unsigned int len);
 STDMETHODIMP getAVIname(char *buf,unsigned int len);
 STDMETHODIMP getAVIfourcc(char *buf,unsigned int len);
 STDMETHODIMP getAVIdimensions(unsigned int *x,unsigned int *y);
 STDMETHODIMP getAVIfps(unsigned int *fps1000);
 STDMETHODIMP getAVIfps2(void);
 STDMETHODIMP saveActivePreset(const char *name);
 STDMETHODIMP saveActivePresetToFile(const char *flnm);
 STDMETHODIMP loadActivePresetFromFile(const char *flnm);
 STDMETHODIMP removePreset(const char *name);
 STDMETHODIMP notifyParamsChanged(void);
 STDMETHODIMP getAVcodecVersion(char *buf,unsigned int len);
 STDMETHODIMP getXvidVersion(char *buf,unsigned int len);
 //STDMETHODIMP getFontName(char *buf,unsigned int len);
 //STDMETHODIMP setFontName(const char *name);
 //STDMETHODIMP getSubFlnm(char *buf,unsigned int len);
 //STDMETHODIMP loadSubtitles(const char *flnm);
 STDMETHODIMP getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom);
 STDMETHODIMP saveGlobalSettings(void);
 STDMETHODIMP loadGlobalSettings(void);
 STDMETHODIMP saveDialogSettings(void);
 STDMETHODIMP loadDialogSettings(void);
 STDMETHODIMP getPresets(Tpresets *presets2);
 STDMETHODIMP setPresets(const Tpresets *presets2);
 STDMETHODIMP savePresets(void);
 STDMETHODIMP setPresetPtr(TpresetSettings *preset);
 STDMETHODIMP renameActivePreset(const char *newName);
 STDMETHODIMP setOnChangeMsg(HWND Iwnd,unsigned int Imsg);
 STDMETHODIMP setOnInfoMsg(HWND Iwnd,unsigned int msg1,unsigned int msg2);
 STDMETHODIMP getDefaultPresetName(char *buf,unsigned int len);
 STDMETHODIMP setDefaultPresetName(const char *presetName);
 STDMETHODIMP isDefaultPreset(const char *presetName);
 STDMETHODIMP showCfgDlg(HWND owner);
 STDMETHODIMP getMovieSource(TmovieSource* *moviePtr);
 STDMETHODIMP getPostproc(Tpostproc* *postprocPtr);
 STDMETHODIMP getOutputDimensions(unsigned int *x,unsigned int *y);
 STDMETHODIMP presetGetNumFilters(unsigned int *num);
 STDMETHODIMP presetGetNumFilters2(void);
 STDMETHODIMP presetGetFilterName(unsigned int i,char *buf,unsigned int len);
 STDMETHODIMP presetGetFilterIs2(unsigned int i);
 STDMETHODIMP presetSetFilterIs(unsigned int i,int Iis);
 STDMETHODIMP presetGetFilterFull2(unsigned int i);
 STDMETHODIMP presetSetFilterFull(unsigned int i,int Ifull);
 STDMETHODIMP presetGetConfPage(unsigned int i,TconfPage* *page); 
 STDMETHODIMP presetGetConfSubPage(unsigned int i,TconfPage* *subPage); 
 STDMETHODIMP presetSetFilterOrder(unsigned int oldOrder,unsigned int newOrder);
 STDMETHODIMP presetGetPPmode(unsigned int index,unsigned int *ppmode);
 STDMETHODIMP presetGetParam(unsigned int index,unsigned int paramID,int *value);
 STDMETHODIMP presetGetParam2(unsigned int index,unsigned int paramID);
 STDMETHODIMP presetPutParam(unsigned int index,unsigned int paramID,int value);
 STDMETHODIMP presetGetParamStr(unsigned int index,unsigned int paramID,char *value,unsigned int len);
 STDMETHODIMP presetPutParamStr(unsigned int index,unsigned int paramID,char *value);
 STDMETHODIMP getCurrentFrame2(void);
 STDMETHODIMP getCfcs(void* *cfcs);

private:
 TtrayIcon *tray;
 Tfilter::TcreateFcVector cfcs;
 TpresetSettings *presetSettings;
 TglobalSettings globalSettings;
 TdialogSettings dialogSettings;
 Tpresets presets;
 int inPlayer;
 int currentq;
 int cfgDlgHnwd;
 char AVIname[1024],AVIfourcc[10];
 int loadAVInameAndPreset(void);
 void onTrayIconChanged(void);
 HRESULT ChangeColorspace(GUID subtype,GUID formattype,void * format);
 HWND onChangeWnd;unsigned int onChangeMsg;
 HWND onInfoWnd;unsigned int onInfoMsg1,onInfoMsg2;
 void sendOnChange(void);

 unsigned int AVIdx,AVIdy;double AVIfps;LONGLONG t1;
 bool isResize;unsigned int outDx,outDy;
 struct
  {
   int colorspace;
   unsigned int bpp;
   unsigned int stride;
   unsigned int length;
   unsigned int outLength;
   void *bitstream;
   void *image;
  } m_frame;

 TmovieSource *movie;
 TfilterResizeNaspect *filterResizeNaspect;
 Tpostproc postproc;
 int codecId;
 clock_t lastTime;
};

#endif
