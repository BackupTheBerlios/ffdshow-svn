#ifndef _TFFDECODER_H_
#define _TFFDECODER_H_

#include "IffDecoder.h"
#include <assert.h>
#include "Tlibavcodec.h"
#include "TglobalSettings.h"
#include "TdialogSettings.h"
#include "TpresetSettings.h"
#include "Tpresets.h"
#include "TimgFilters.h"

class TtrayIcon;
class TresizeCtx;
class Tsubtitles;
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
   };                                                          
  #else
  DECLARE_IUNKNOWN;
  #endif

  TffDecoder(LPUNKNOWN punk, HRESULT *phr);
  virtual ~TffDecoder();

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
  STDMETHODIMP getActivePresetName(char *buf,unsigned int len);
  STDMETHODIMP getAVIname(char *buf,unsigned int len);
  STDMETHODIMP getAVIfourcc(char *buf,unsigned int len);
  STDMETHODIMP getAVIdimensions(unsigned int *x,unsigned int *y);
  STDMETHODIMP getAVIfps(unsigned int *fps);
  STDMETHODIMP savePreset(const char *name);
  STDMETHODIMP savePresetToFile(const char *flnm);
  STDMETHODIMP loadPreset(const char *name);
  STDMETHODIMP loadPresetFromFile(const char *flnm);
  STDMETHODIMP removePreset(const char *name);
  STDMETHODIMP notifyParamsChanged(void);
  STDMETHODIMP getAVcodecVersion(char *buf,unsigned int len);
  STDMETHODIMP getPPmode(unsigned int *ppmode);
  STDMETHODIMP getPostProcDescription(char *buf,unsigned int len);
  STDMETHODIMP getPictPropDescription(char *buf,unsigned int len);
  STDMETHODIMP getBlurDescription(char *buf,unsigned int len);
  STDMETHODIMP getNoiseDescription(char *buf,unsigned int len);
  STDMETHODIMP getSharpenDescription(char *buf,unsigned int len);
  STDMETHODIMP getCropDescription(char *buf,unsigned int len);
  STDMETHODIMP getFontName(char *buf,unsigned int len);
  STDMETHODIMP setFontName(const char *name);
  STDMETHODIMP getSubFlnm(char *buf,unsigned int len);
  STDMETHODIMP loadSubtitles(const char *flnm);
  STDMETHODIMP getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom);
  STDMETHODIMP getMinOrder2(void);
  STDMETHODIMP getMaxOrder2(void);
  STDMETHODIMP saveGlobalSettings(void);
  STDMETHODIMP loadGlobalSettings(void);
  STDMETHODIMP saveDialogSettings(void);
  STDMETHODIMP loadDialogSettings(void);
  STDMETHODIMP getPreset(unsigned int i,void *buf);
 
 private:
  bool firstFrame;
  TtrayIcon *tray;
  TpresetSettings presetSettings;
  TglobalSettings globalSettings;
  TdialogSettings dialogSettings;
  Tpresets presets;
  int inPlayer,isDlg;
  char AVIname[1024],AVIfourcc[10];
  void loadAVInameAndPreset(void);
  int* getIDFFvar(int paramID);
  struct Tparam
  {
   Tparam(void):val(NULL) {};
   Tparam(int *Ival,int Imin,int Imax,void (TffDecoder::*IonNotify)(void)=NULL)
    {
     val=Ival;
     min=Imin;max=Imax;
     onNotify=IonNotify;
    }
   Tparam(const int *Ival)
    {
     val=(int*)Ival;
     min=max=-1;
     onNotify=NULL;
    }
   int *val;
   int min,max;
   void (TffDecoder::*onNotify)(void);
  };                          
  #define MAX_PARAMID 1100
  Tparam params[MAX_PARAMID];
  void fillParams(void);
  void subsChanged(void),resizeChanged(void),trayIconChanged(void);
  int idctOld;
  Tlibavcodec libavcodec;
  HRESULT ChangeColorspace(GUID subtype, GUID formattype, void * format);
  
  int AVIdx,AVIdy;double AVIfps;
  struct
   {
    int colorspace;
    int bpp;
    int stride;
    int length;
    int outLenght;
    void *bitstream;
    void *image;
   } m_frame;
   
  AVCodecContext *avctx;
  TresizeCtx*resizeCtx;
  int cropLeft,cropTop,cropDx,cropDy;
  void calcCrop(void);
  TimgFilters *imgFilters;
  Tsubtitles *subs;
  char *codecName;
};

#endif 
