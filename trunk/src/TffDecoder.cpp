/*
 * Copyright (c) 2002 Milan Cutka
 * based on CXvidDecoder.cpp from XVID DirectShow filter
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
    this requires the directx sdk
    place these paths at the top of the Tools|Options|Directories list
 
    headers:
    C:\DXVCSDK\include
    C:\DXVCSDK\samples\Multimedia\DirectShow\BaseClasses
    
    libraries (optional):
    C:\DXVCSDK\samples\Multimedia\DirectShow\BaseClasses\Release
*/

//#define FF__MPEG

#include <windows.h>
#include <streams.h>
#include <initguid.h>
#include <olectl.h>
#if (1100 > _MSC_VER)
#include <olectlid.h>
#endif
#include <dvdmedia.h>   // VIDEOINFOHEADER2
#include <assert.h>
#include <commctrl.h>

#include "ffdebug.h"
#define FF_POSTPROCESS
extern "C"
{
 #include "ffmpeg\libavcodec\avcodec.h"
 #include "mplayer\libmpcodecs\img_format.h"
}
extern "C" 
{
 #include "xvid\image\image.h"
}
#include "xvid\xvid.h"

#include "dialog\TffdshowPage.h"
#include "TffDecoder.h"
#include "TresizeCtx.h"
#include "trayIcon.h"
#include "subtitles\Tsubtitles.h"

// --- fourcc --- 
#define FOURCC_XVID mmioFOURCC('X','V','I','D') 
#define FOURCC_xvid mmioFOURCC('x','v','i','d')
#define FOURCC_DIVX mmioFOURCC('D','I','V','X') 
#define FOURCC_divx mmioFOURCC('d','i','v','x')
#define FOURCC_DIV3 mmioFOURCC('D','I','V','3') 
#define FOURCC_div3 mmioFOURCC('d','i','v','3')
#define FOURCC_DX50 mmioFOURCC('D','X','5','0') 
#define FOURCC_dx50 mmioFOURCC('d','x','5','0')
#define FOURCC_MP41 mmioFOURCC('M','P','4','1') 
#define FOURCC_mp41 mmioFOURCC('m','p','4','1')
#define FOURCC_MP42 mmioFOURCC('M','P','4','2') 
#define FOURCC_mp42 mmioFOURCC('m','p','4','2')
#define FOURCC_MP43 mmioFOURCC('M','P','4','3') 
#define FOURCC_mp43 mmioFOURCC('m','p','4','3')
#define FOURCC_H263 mmioFOURCC('H','2','6','3') 
#define FOURCC_h263 mmioFOURCC('h','2','6','3')
#define FOURCC_WMV1 mmioFOURCC('W','M','V','1') 
#define FOURCC_wmv1 mmioFOURCC('w','m','v','1')

// --- media uids --- 
DEFINE_GUID(CLSID_xvid, 0x64697678, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_XVID, 0x44495658, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_divx, 0x78766964, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DIVX, 0x58564944, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_div3, 0x33766964, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DIV3, 0x33564944, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_dx50, 0x30357864, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DX50, 0x30355844, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_mp43, 0x3334706D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_MP43, 0x3334504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_mp42, 0x3234706D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_MP42, 0x3234504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_mp41, 0x3134706D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_MP41, 0x3134504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_h263, 0x33363268, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_H263, 0x33363248, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_wmv1, 0x31766d77, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_WMV1, 0x31564d57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

DEFINE_GUID(CLSID_MPEG1Packet,
                           0xe436eb80, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
DEFINE_GUID(CLSID_MPEG1Payload,
                           0xe436eb81, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 

const AMOVIESETUP_MEDIATYPE sudInputPinTypes[] =
 {
  { &MEDIATYPE_Video, &CLSID_xvid },
  { &MEDIATYPE_Video, &CLSID_XVID },
  { &MEDIATYPE_Video, &CLSID_divx },
  { &MEDIATYPE_Video, &CLSID_DIVX },
  { &MEDIATYPE_Video, &CLSID_div3 },
  { &MEDIATYPE_Video, &CLSID_DIV3 },
  { &MEDIATYPE_Video, &CLSID_dx50 },
  { &MEDIATYPE_Video, &CLSID_DX50 },
  { &MEDIATYPE_Video, &CLSID_mp43 },
  { &MEDIATYPE_Video, &CLSID_MP43 },
  { &MEDIATYPE_Video, &CLSID_mp42 },
  { &MEDIATYPE_Video, &CLSID_MP42 },
  { &MEDIATYPE_Video, &CLSID_mp41 },
  { &MEDIATYPE_Video, &CLSID_MP41 },
  { &MEDIATYPE_Video, &CLSID_h263 },
  { &MEDIATYPE_Video, &CLSID_H263 },
#ifdef FF__WMV1  
  { &MEDIATYPE_Video, &CLSID_wmv1 },
  { &MEDIATYPE_Video, &CLSID_WMV1 },
#endif  
#ifdef FF__MPEG
  { &MEDIATYPE_Video, &CLSID_MPEG1Packet },  
  { &MEDIATYPE_Video, &CLSID_MPEG1Payload }
#endif  
 };

const AMOVIESETUP_MEDIATYPE sudOutputPinTypes[] =
 {
  {
   &MEDIATYPE_Video, &MEDIASUBTYPE_NULL
  }
 };

const AMOVIESETUP_PIN psudPins[] =
 {
  {
   L"Input",           // String pin name
   FALSE,              // Is it rendered
   FALSE,              // Is it an output
   FALSE,              // Allowed none
   FALSE,              // Allowed many
   &CLSID_NULL,        // Connects to filter
   L"Output",          // Connects to pin
   sizeof(sudInputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
   &sudInputPinTypes[0]    // The pin details
  },
  {
   L"Output",          // String pin name
   FALSE,              // Is it rendered
   TRUE,               // Is it an output
   FALSE,              // Allowed none
   FALSE,              // Allowed many
   &CLSID_NULL,        // Connects to filter
   L"Input",           // Connects to pin
   sizeof(sudOutputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
   sudOutputPinTypes   // The pin details
  }
 };

const AMOVIESETUP_FILTER sudXvidDecoder =
 {
  &CLSID_FFDSHOW,            // Filter CLSID
  FFDSHOW_NAME_L,            // Filter name
  #ifdef  FF__MPEG
   0x40000002,
  #else
   MERIT_PREFERRED+1+1,    // Its merit - stupid hack, but Windows Media filters use it too
  #endif 
  sizeof(psudPins) / sizeof(AMOVIESETUP_PIN), // Number of pins
  psudPins                // Pin details
 };

// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance
CFactoryTemplate g_Templates[] =
 {
  {
   FFDSHOW_NAME_L,
   &CLSID_FFDSHOW,
   TffDecoder::CreateInstance,
   NULL,
   &sudXvidDecoder
  },
  {
   FFDSHOW_NAME_L L"ffdshow",
   &CLSID_TFFDSHOWPAGE,
   TffdshowPage::CreateInstance
  }
 };

int g_cTemplates = sizeof(g_Templates) / sizeof(CFactoryTemplate);

STDAPI DllRegisterServer()
{
 return AMovieDllRegisterServer2( TRUE );
}

STDAPI DllUnregisterServer()
{
 return AMovieDllRegisterServer2( FALSE );
}

// create instance 
CUnknown * WINAPI TffDecoder::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
#ifdef DEBUG
 DbgSetModuleLevel(0xffffffff,0);
#endif 
 TffDecoder * pNewObject = new TffDecoder(punk, phr);
 if (pNewObject == NULL)
  {
   *phr = E_OUTOFMEMORY;
  }
 return pNewObject;
}
void TffDecoder::fillParams(void)
{
 params[IDFF_autoPreset         ]=Tparam(&cfg.autoPreset         ,0,0);
 params[IDFF_lastPage           ]=Tparam(&cfg.lastPage           ,0,0);
 params[IDFF_trayIcon           ]=Tparam(&cfg.trayIcon           ,0,0);
 params[IDFF_isDlg              ]=Tparam(&cfg.isDlg              ,0,0);
 params[IDFF_autoPresetFileFirst]=Tparam(&cfg.autoPresetFileFirst,0,0);
 params[IDFF_presetShouldBeSaved]=Tparam(&cfg.presetShouldBeSaved,0,0);
 params[IDFF_inPlayer           ]=Tparam(&cfg.inPlayer           ,0,0);
 params[IDFF_autoloadedfromreg  ]=Tparam(&cfg.autoloadedfromreg  ,0,0);

 params[IDFF_isPostproc         ]=Tparam(&cfg.isPostproc         ,0,0);
 params[IDFF_orderPostproc      ]=Tparam(&cfg.orderPostproc      ,MIN_ORDER,MAX_ORDER);
 params[IDFF_ppqual             ]=Tparam(&cfg.ppqual             ,0,6);
 params[IDFF_autoq              ]=Tparam(&cfg.autoq              ,0,0);
 params[IDFF_ppIsCustom         ]=Tparam(&cfg.ppIsCustom         ,0,0);
 params[IDFF_ppcustom           ]=Tparam(&cfg.ppcustom           ,0,0);
 params[IDFF_currentq           ]=Tparam(&cfg.currentq           ,0,6);
 params[IDFF_levelFixLum        ]=Tparam(&cfg.levelFixLum        ,0,0);
 params[IDFF_levelFixChrom      ]=Tparam(&cfg.levelFixChrom      ,0,0);

 params[IDFF_isPictProp         ]=Tparam(&cfg.isPictProp         ,0,0);
 params[IDFF_orderPictProp      ]=Tparam(&cfg.orderPictProp      ,MIN_ORDER,MAX_ORDER);
 params[IDFF_lumGain            ]=Tparam(&cfg.lumGain            ,0,256);
 params[IDFF_lumOffset          ]=Tparam(&cfg.lumOffset          ,-256,256);
 params[IDFF_gammaCorrection    ]=Tparam(&cfg.gammaCorrection    ,1,400);
 params[IDFF_hue                ]=Tparam(&cfg.hue                ,-180,180);
 params[IDFF_saturation         ]=Tparam(&cfg.saturation         ,0,128);
                                                         
 params[IDFF_flip               ]=Tparam(&cfg.flip               ,0,0);

 params[IDFF_isBlur             ]=Tparam(&cfg.isBlur             ,0,0);
 params[IDFF_orderBlur          ]=Tparam(&cfg.orderBlur          ,MIN_ORDER,MAX_ORDER);
 params[IDFF_blurStrength       ]=Tparam(&cfg.blurStrength       ,1,255);

 params[IDFF_isSharpen          ]=Tparam(&cfg.isSharpen          ,0,0);
 params[IDFF_orderSharpen       ]=Tparam(&cfg.orderSharpen       ,MIN_ORDER,MAX_ORDER);
 params[IDFF_sharpenMethod      ]=Tparam(&cfg.sharpenMethod      ,0,1);
 params[IDFF_xsharp_strength    ]=Tparam(&cfg.xsharp_strength    ,1,127);
 params[IDFF_xsharp_threshold   ]=Tparam(&cfg.xsharp_threshold   ,0,255);
 params[IDFF_unsharp_strength   ]=Tparam(&cfg.unsharp_strength   ,1,127);
 params[IDFF_unsharp_threshold  ]=Tparam(&cfg.unsharp_threshold  ,0,255);
                                                         
 params[IDFF_isNoise            ]=Tparam(&cfg.isNoise            ,0,0);
 params[IDFF_orderNoise         ]=Tparam(&cfg.orderNoise         ,MIN_ORDER,MAX_ORDER);
 params[IDFF_noiseMethod        ]=Tparam(&cfg.noiseMethod        ,0,1);
 params[IDFF_uniformNoise       ]=Tparam(&cfg.uniformNoise       ,0,0);
 params[IDFF_noiseStrength      ]=Tparam(&cfg.noiseStrength      ,0,255);
 params[IDFF_noiseStrengthChroma]=Tparam(&cfg.noiseStrengthChroma,0,127);
                                                         
 params[IDFF_idct               ]=Tparam(&cfg.idct               ,0,3,&TffDecoder::idctChanged);
                                                         
 params[IDFF_isResize           ]=Tparam(&cfg.isResize           ,0,0);
 params[IDFF_resizeDx           ]=Tparam(&cfg.resizeDx           ,8,2048);
 params[IDFF_resizeDy           ]=Tparam(&cfg.resizeDy           ,8,2048);
 params[IDFF_resizeAspect       ]=Tparam(&cfg.resizeAspect       ,0,0,&TffDecoder::resizeChanged);
 params[IDFF_resizeFirst        ]=Tparam(&cfg.resizeFirst        ,0,0,&TffDecoder::resizeChanged);
 params[IDFF_resizeMethod       ]=Tparam(&cfg.resizeMethod       ,0,5,&TffDecoder::resizeChanged);
 params[IDFF_aspectRatio        ]=Tparam(&cfg.aspectRatio        ,0,0,&TffDecoder::resizeChanged);
 params[IDFF_resizeGblurLum     ]=Tparam(&cfg.resizeGblurLum     ,0,200,&TffDecoder::resizeChanged);
 params[IDFF_resizeGblurChrom   ]=Tparam(&cfg.resizeGblurChrom   ,0,200,&TffDecoder::resizeChanged);
 params[IDFF_resizeSharpenLum   ]=Tparam(&cfg.resizeSharpenLum   ,0,200,&TffDecoder::resizeChanged);
 params[IDFF_resizeSharpenChrom ]=Tparam(&cfg.resizeSharpenChrom ,0,200,&TffDecoder::resizeChanged);
 params[IDFF_isCropNzoom        ]=Tparam(&cfg.isCropNzoom        ,0,0,&TffDecoder::resizeChanged);
 params[IDFF_isZoom             ]=Tparam(&cfg.isZoom             ,0,0,&TffDecoder::resizeChanged);
 params[IDFF_magnificationX     ]=Tparam(&cfg.magnificationX     ,0,100,&TffDecoder::resizeChanged);
 params[IDFF_magnificationY     ]=Tparam(&cfg.magnificationY     ,0,100,&TffDecoder::resizeChanged);
 params[IDFF_magnificationLocked]=Tparam(&cfg.magnificationLocked,0,0,&TffDecoder::resizeChanged);
 params[IDFF_cropLeft           ]=Tparam(&cfg.cropLeft           ,0,2048,&TffDecoder::resizeChanged);
 params[IDFF_cropRight          ]=Tparam(&cfg.cropRight          ,0,2048,&TffDecoder::resizeChanged);
 params[IDFF_cropTop            ]=Tparam(&cfg.cropTop            ,0,2048,&TffDecoder::resizeChanged);
 params[IDFF_cropBottom         ]=Tparam(&cfg.cropBottom         ,0,2048,&TffDecoder::resizeChanged);
 params[IDFF_autocrop           ]=Tparam(&cfg.autocrop           ,0,0,&TffDecoder::resizeChanged);

 params[IDFF_isSubtitles        ]=Tparam(&cfg.isSubtitles        ,0,0,&TffDecoder::subsChanged);
 params[IDFF_orderSubtitles     ]=Tparam(&cfg.orderSubtitles     ,MIN_ORDER,MAX_ORDER);
 params[IDFF_fontCharset        ]=Tparam(&cfg.fontCharset        ,0,0,&TffDecoder::subsChanged);
 params[IDFF_fontSize           ]=Tparam(&cfg.fontSize           ,2,255,&TffDecoder::subsChanged);
 params[IDFF_fontWeight         ]=Tparam(&cfg.fontWeight         ,0,1000,&TffDecoder::subsChanged);
 params[IDFF_fontColor          ]=Tparam(&cfg.fontColor          ,0,0,&TffDecoder::subsChanged);
 params[IDFF_fontShadowStrength ]=Tparam(&cfg.fontShadowStrength ,0,100,&TffDecoder::subsChanged);
 params[IDFF_fontShadowRadius   ]=Tparam(&cfg.fontShadowRadius   ,1,100,&TffDecoder::subsChanged);
 params[IDFF_fontAutosize       ]=Tparam(&cfg.fontAutosize       ,0,0,&TffDecoder::subsChanged);
 params[IDFF_fontSpacing        ]=Tparam(&cfg.fontSpacing        ,-10,10,&TffDecoder::subsChanged);
 params[IDFF_subPosX            ]=Tparam(&cfg.subPosX            ,0,100,&TffDecoder::subsChanged);
 params[IDFF_subPosY            ]=Tparam(&cfg.subPosY            ,0,100,&TffDecoder::subsChanged);
 params[IDFF_subDelay           ]=Tparam(&cfg.subDelay           ,0,0,&TffDecoder::subsChanged);
 params[IDFF_subSpeed           ]=Tparam(&cfg.subSpeed           ,0,0,&TffDecoder::subsChanged);
 params[IDFF_subAutoFlnm        ]=Tparam(&cfg.subAutoFlnm        ,0,0);
                                                         
 params[IDFF_xvid               ]=Tparam(&cfg.xvid               ,0,0);
 params[IDFF_div3               ]=Tparam(&cfg.div3               ,0,0);
 params[IDFF_divx               ]=Tparam(&cfg.divx               ,0,0);
 params[IDFF_dx50               ]=Tparam(&cfg.dx50               ,0,0);
 params[IDFF_mp43               ]=Tparam(&cfg.mp43               ,0,0);
 params[IDFF_mp42               ]=Tparam(&cfg.mp42               ,0,0);
 params[IDFF_mp41               ]=Tparam(&cfg.mp41               ,0,0);
 params[IDFF_h263               ]=Tparam(&cfg.h263               ,0,0);
}                                                                      
STDMETHODIMP TffDecoder::get_Param(unsigned int paramID, int* value)
{
 if (!value) return S_FALSE;
 if (paramID<0 || paramID>=MAX_PARAMID) return S_FALSE;
 if (!params[paramID].val) return S_FALSE;
 *value=*params[paramID].val;
 return S_OK;
}
STDMETHODIMP TffDecoder::get_Param2(unsigned int paramID)
{
 int val;
 HRESULT res=get_Param(paramID,&val);
 return (res==S_OK)?val:0;
}
STDMETHODIMP TffDecoder::put_Param(unsigned int paramID, int  value)
{
 if (paramID<0 || paramID>=MAX_PARAMID) return S_FALSE;
 Tparam &p=params[paramID];
 if (!p.val) return S_FALSE;
 if (p.min || p.max)
  {
   if (value<p.min) value=p.min;
   if (value>p.max) value=p.max;
  };
 *p.val=value;
 if (p.onNotify) (this->*p.onNotify)();
 return S_OK;
}
STDMETHODIMP TffDecoder::notifyParamsChanged(void)
{
 subsChanged();
 resizeChanged();
 trayIconChanged();
 idctChanged();
 return S_OK;
}
void TffDecoder::subsChanged(void)
{
 cfg.fontChanged=true;
}
void TffDecoder::resizeChanged(void)
{
 if (resizeCtx) resizeCtx->resizeChanged=true;
 if (cfg.magnificationLocked) cfg.magnificationY=cfg.magnificationX;
 //if (AVIdx && AVIdy) calcCrop();
}
void TffDecoder::trayIconChanged(void)
{
 if (cfg.trayIcon) tray->show(); else tray->hide();
}
void TffDecoder::idctChanged(void)
{
 cfg.idctChanged=true;
}
STDMETHODIMP TffDecoder::get_numPresets(unsigned int *value)
{
 *value=0;
 if (!value || !cfg.presets) return E_POINTER;
 *value=cfg.presets->size();
 return S_OK;
}
STDMETHODIMP TffDecoder::get_presetName(unsigned int i,char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (!cfg.presets || i>=cfg.presets->size()) return S_FALSE;
 if (len<(*cfg.presets)[i].size()) return E_OUTOFMEMORY;
 strcpy(buf,(*cfg.presets)[i].c_str());
 return S_OK;
}
STDMETHODIMP TffDecoder::get_activePresetName(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(cfg.presetName)+1) return E_OUTOFMEMORY;
 strcpy(buf,cfg.presetName);
 return S_OK;
}
STDMETHODIMP TffDecoder::get_AVIname(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(AVIname)+1) return E_OUTOFMEMORY;
 strcpy(buf,AVIname);
 return S_OK;
}
STDMETHODIMP TffDecoder::get_AVIfourcc(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(AVIfourcc)+1) return E_OUTOFMEMORY;
 strcpy(buf,AVIfourcc);
 return S_OK;
}
STDMETHODIMP TffDecoder::get_AVIfps(unsigned int *fps)
{
 if (!fps || AVIfps==0) return S_FALSE;
 *fps=1000*AVIfps;
 return S_OK;
}
STDMETHODIMP TffDecoder::loadPreset(const char *name)
{
 cfg.loadPreset(name);
 notifyParamsChanged();
 return S_OK;
}
STDMETHODIMP TffDecoder::savePreset(const char *name)
{
 return cfg.savePreset(name);
}
STDMETHODIMP TffDecoder::savePresetToFile(const char *flnm)
{
 //TODO: check save success
 if (!flnm) return S_FALSE;
 cfg.savePresetToFile(flnm);
 return S_OK;
}
STDMETHODIMP TffDecoder::loadPresetFromFile(const char *flnm)
{
 //TODO: check load success
 if (!flnm) return S_FALSE;
 cfg.loadPresetFromFile(flnm);
 notifyParamsChanged();
 return S_OK;
}
STDMETHODIMP TffDecoder::removePreset(const char *name)
{
 return Tconfig::removePreset(name);
}
STDMETHODIMP TffDecoder::get_avcodec_version(char *buf,unsigned int len)
{
 char *vers;
 libavcodec.getVersion(&cfg,&vers);
 if (!vers) return S_FALSE;
 if (len<strlen(vers)+1) return E_OUTOFMEMORY;
 strcpy(buf,vers);
 return S_OK;
}
STDMETHODIMP TffDecoder::get_AVIdimensions(unsigned int *x,unsigned int *y)
{
 *x=*y=0;
 if (!x || !y || AVIdx==0 || AVIdy==0) return S_FALSE;
 *x=AVIdx;*y=AVIdy;
 return S_OK;
}
STDMETHODIMP TffDecoder::getPPmode(unsigned int *ppmode)
{
 if (!imgFilters || !imgFilters->postproc.ok) 
  { 
   *ppmode=0;
   return S_FALSE;
  }
 *ppmode=Tpostproc::getPPmode(&cfg);  
 return S_OK;
}
STDMETHODIMP TffDecoder::getPostProcDescription(char *buf,unsigned int len)
{
 if (!buf || len<1000) return S_FALSE;
 cfg.getPostProcDescription(buf);
 return S_OK;
}
STDMETHODIMP TffDecoder::getPictPropDescription(char *buf,unsigned int len)
{
 if (!buf || len<1000) return S_FALSE;
 cfg.getPictPropDescription(buf);
 return S_OK;
}
STDMETHODIMP TffDecoder::getNoiseDescription(char *buf,unsigned int len)
{
 if (!buf || len<1000) return S_FALSE;
 cfg.getNoiseDescription(buf);
 return S_OK;
}
STDMETHODIMP TffDecoder::getBlurDescription(char *buf,unsigned int len)
{
 if (!buf || len<1000) return S_FALSE;
 cfg.getBlurDescription(buf);
 return S_OK;
}
STDMETHODIMP TffDecoder::getSharpenDescription(char *buf,unsigned int len)
{
 if (!buf || len<1000) return S_FALSE;
 cfg.getSharpenDescription(buf);
 return S_OK;
}
STDMETHODIMP TffDecoder::getCropDescription(char *buf,unsigned int len)
{
 if (!buf || len<1000) return S_FALSE;
 cfg.getCropDescription(buf);
 return S_OK;
}
STDMETHODIMP TffDecoder::getFontName(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(cfg.fontName)+1) return E_OUTOFMEMORY;
 strcpy(buf,cfg.fontName);
 return S_OK;
}
STDMETHODIMP TffDecoder::setFontName(const char *name)
{
 if (!name) return E_POINTER;
 if (strlen(name)>255) return  S_FALSE;
 strcpy(cfg.fontName,name);
 notifyParamsChanged();
 return S_OK;
}
STDMETHODIMP TffDecoder::getSubFlnm(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(cfg.subFlnm)+1) return E_OUTOFMEMORY;
 strcpy(buf,cfg.subFlnm);
 return S_OK;
}
STDMETHODIMP TffDecoder::loadSubtitles(const char *flnm)
{
 if (!flnm) return E_POINTER;
 if (strlen(flnm)>255) return  S_FALSE;
 if (sub)
  {
   sub->init(NULL,flnm,AVIfps);
   strcpy(cfg.subFlnm,sub->flnm);
  }
 else strcpy(cfg.subFlnm,flnm);  
 return S_OK;
}
STDMETHODIMP TffDecoder::getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom)
{
 if (!left || !top || !right || !bottom) return E_POINTER;
 if (!AVIdx || !AVIdy)
  {
   *left=cfg.cropLeft;*top=cfg.cropTop;*right=cfg.cropRight;*bottom=cfg.cropBottom;
  }
 else
  {
   *left=cropLeft;*top=cropTop;
   *right=AVIdx-(cropLeft+cropDx);*bottom=AVIdy-(cropTop+cropDy);
  } 
 return S_OK; 
}
STDMETHODIMP TffDecoder::getMinOrder2(void)
{
 return MIN_ORDER;
}
STDMETHODIMP TffDecoder::getMaxOrder2(void)
{
 return MAX_ORDER;
}

// query interfaces 
STDMETHODIMP TffDecoder::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
 CheckPointer(ppv, E_POINTER);

 if (riid == IID_IffDecoder)
  {
   return GetInterface((IffDecoder *) this, ppv);
  }

 if (riid == IID_ISpecifyPropertyPages)
  {
   return GetInterface((ISpecifyPropertyPages *) this, ppv);
  }

 return CVideoTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

// dummy decore() 
static int dummy_xvid_decore(void * handle, int opt, void * param1, void * param2)
{
 return XVID_ERR_FAIL;
}

// constructor 
TffDecoder::TffDecoder(LPUNKNOWN punk, HRESULT *phr):CVideoTransformFilter(NAME("CffDecoder"),punk,/*CLSID_XVID*/CLSID_FFDSHOW)
{
 DEBUGS("Constructor");
 ASSERT(phr);

 InitCommonControls();
 
 AVIname[0]=AVIfourcc[0]='\0';
 AVIdx=AVIdy=cropLeft=cropTop=cropDx=cropDy=AVIfps=0;
 
 fillParams();
 cfg.init();
 cfg.fontChanged=true;
 cfg.loadActivePreset();
 
 tray=new TtrayIcon(this,g_hInst);
 
 avctx=NULL;

 resizeCtx=NULL;
 imgFilters=(cfg.inPlayer)?new TimgFilters(&cfg):NULL;
 sub=(cfg.inPlayer)?new Tsubtitles:NULL;
}

// destructor 
TffDecoder::~TffDecoder()
{
 __asm {emms};
 DEBUGS("Destructor");
 if (avctx)
  {
   libavcodec.avcodec_close(avctx);
   free(avctx);
   avctx=NULL;
  }; 
 if (imgFilters) delete imgFilters;
 libavcodec.done();
 cfg.done(true);
 if (resizeCtx) delete resizeCtx;
 if (sub) delete sub;
 delete tray;
}

// check input type 
HRESULT TffDecoder::CheckInputType(const CMediaType * mtIn)
{
 DEBUGS("CheckInputType");
 BITMAPINFOHEADER * hdr;
 
 if (*mtIn->Type() != MEDIATYPE_Video)
  {
   return VFW_E_TYPE_NOT_ACCEPTED;
  }
 const GUID formatType=*mtIn->FormatType();
 __asm emms;
 if (formatType == FORMAT_VideoInfo)
  {
   VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *) mtIn->Format();
   AVIfps=10000000.0/vih->AvgTimePerFrame;
   hdr = &vih->bmiHeader;
  }
 else if (formatType == FORMAT_VideoInfo2)
  {
   VIDEOINFOHEADER2 * vih2 = (VIDEOINFOHEADER2 *) mtIn->Format();
   AVIfps=10000000.0/vih2->AvgTimePerFrame;
   hdr = &vih2->bmiHeader;
  }
 #ifdef  FF__MPEG
 else if (formatType == FORMAT_MPEGVideo)
  {
   MPEG1VIDEOINFO *mpeg1info=(MPEG1VIDEOINFO*)mtIn->Format();
   hdr = &mpeg1info->hdr.bmiHeader;
   AVIdx = hdr->biWidth;
   AVIdy = hdr->biHeight;
   codecName="mpegvideo";strcpy(AVIfourcc,"mpeg1");
   mpegBuf=new TmpegBuffer(10000000);
   return S_OK;
  } 
 #endif 
 else
  {
   return VFW_E_TYPE_NOT_ACCEPTED;
  }

 if (hdr->biHeight < 0)
  {
   DEBUGS("colorspace: inverted input format not supported");
  }

 AVIdx=hdr->biWidth;
 AVIdy=hdr->biHeight;
 loadAVInameAndPreset();
 if (!resizeCtx)
  {
   if (!imgFilters->postproc.ok) cfg.isResize=false;
   resizeCtx=new TresizeCtx(&cfg);
   if (cfg.isResize)
    resizeCtx->allocate(cfg.resizeDx,cfg.resizeDy);
   else
    resizeCtx->allocate(AVIdx,AVIdy);
  };  
 
 codecName="";AVIfourcc[0]='\0';
 switch(hdr->biCompression)
  {
  case FOURCC_XVID:
  case FOURCC_xvid:
   if (cfg.xvid) 
    {
     codecName="mpeg4";strcpy(AVIfourcc,"XVID");
     return S_OK;
    }
   break;
  case FOURCC_DIVX:
  case FOURCC_divx:
   if (cfg.divx) 
    {
     codecName="mpeg4";strcpy(AVIfourcc,"DIVX");
     return S_OK;
    } 
   break;
  case FOURCC_DX50:
  case FOURCC_dx50:
   if (cfg.dx50) 
    {
     codecName="mpeg4";strcpy(AVIfourcc,"DX50");
     return S_OK;
    }
   break;
  case FOURCC_DIV3:
  case FOURCC_div3:
   if (cfg.div3) 
    {
     codecName="msmpeg4";strcpy(AVIfourcc,"DIV3");
     return S_OK;
    } 
   break;
  case FOURCC_MP43:
  case FOURCC_mp43:
   if (cfg.mp43)
    {
     codecName="msmpeg4";strcpy(AVIfourcc,"MP43");
     return S_OK;
    } 
   break;
  case FOURCC_MP42:
  case FOURCC_mp42:
   if (cfg.mp42)
    {
     codecName="msmpeg4v2";strcpy(AVIfourcc,"MP42");
     return S_OK;
    } 
   break;
  case FOURCC_MP41:
  case FOURCC_mp41:
   if (cfg.mp41)
    {
     codecName="msmpeg4v1";strcpy(AVIfourcc,"MP41");
     return S_OK;
    } 
   break;
  case FOURCC_H263:
  case FOURCC_h263:
   if (cfg.h263)
    {
     codecName="h263";strcpy(AVIfourcc,"H263");
     return S_OK;
    } 
   break;
  /*
  case FOURCC_WMV1:
  case FOURCC_wmv1:
   if (cfg.wmv1)
    {
     codecName="wmv1";strcpy(AVIfourcc,"WMV1");
     simpleIdct=true;
     return S_OK;
    } 
   break;
  */
  }

 return VFW_E_TYPE_NOT_ACCEPTED;
}


// get list of supported output colorspaces 
HRESULT TffDecoder::GetMediaType(int iPosition, CMediaType *mtOut)
{
 DEBUGS("GetMediaType");

 if (m_pInput->IsConnected() == FALSE) return E_UNEXPECTED;

 VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)mtOut->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
 if (vih == NULL) return E_OUTOFMEMORY;

 ZeroMemory(vih, sizeof (VIDEOINFOHEADER));
 vih->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
 vih->bmiHeader.biWidth  = (cfg.isResize)?cfg.resizeDx:AVIdx;
 vih->bmiHeader.biHeight = (cfg.isResize)?cfg.resizeDy:AVIdy;
 vih->bmiHeader.biPlanes = 1;

 if (iPosition < 0) return E_INVALIDARG;

 switch(iPosition)
  {
  case 0  :
   vih->bmiHeader.biCompression = MEDIASUBTYPE_YV12.Data1;
   vih->bmiHeader.biBitCount = 12;
   mtOut->SetSubtype(&MEDIASUBTYPE_YV12);
   break;

  case 1:
   vih->bmiHeader.biCompression = MEDIASUBTYPE_YUY2.Data1;
   vih->bmiHeader.biBitCount = 16;
   mtOut->SetSubtype(&MEDIASUBTYPE_YUY2);
   break;

  case 2:
   vih->bmiHeader.biCompression = MEDIASUBTYPE_YVYU.Data1;
   vih->bmiHeader.biBitCount = 16;
   mtOut->SetSubtype(&MEDIASUBTYPE_YVYU);
   break;

  case 3:
   vih->bmiHeader.biCompression = MEDIASUBTYPE_UYVY.Data1;
   vih->bmiHeader.biBitCount = 16;
   mtOut->SetSubtype(&MEDIASUBTYPE_UYVY);
   break;

  case 4:
   vih->bmiHeader.biCompression = BI_RGB;
   vih->bmiHeader.biBitCount = 32;
   mtOut->SetSubtype(&MEDIASUBTYPE_RGB32);
   break;

  case 5:
   vih->bmiHeader.biCompression = BI_RGB;
   vih->bmiHeader.biBitCount = 24;
   mtOut->SetSubtype(&MEDIASUBTYPE_RGB24);
   break;

  case 6:
   vih->bmiHeader.biCompression = BI_RGB;
   vih->bmiHeader.biBitCount = 16;
   mtOut->SetSubtype(&MEDIASUBTYPE_RGB555);
   break;

  case 7:
   vih->bmiHeader.biCompression = BI_RGB;
   vih->bmiHeader.biBitCount = 16;
   mtOut->SetSubtype(&MEDIASUBTYPE_RGB565);
   break;

  default :
   return VFW_S_NO_MORE_ITEMS;
  }

 vih->bmiHeader.biSizeImage = GetBitmapSize(&vih->bmiHeader);

 mtOut->SetType(&MEDIATYPE_Video);
 mtOut->SetFormatType(&FORMAT_VideoInfo);
 mtOut->SetTemporalCompression(FALSE);
 mtOut->SetSampleSize(vih->bmiHeader.biSizeImage);

 return S_OK;
}

// (internal function) change colorspace 
HRESULT TffDecoder::ChangeColorspace(GUID subtype, GUID formattype, void * format)
{
 if (subtype == MEDIASUBTYPE_YV12)
  {
   DEBUGS("YV12");
   m_frame.colorspace = XVID_CSP_YV12;
  }
 else if (subtype == MEDIASUBTYPE_YUY2)
  {
   DEBUGS("YUY2");
   m_frame.colorspace = XVID_CSP_YUY2;
  }
 else if (subtype == MEDIASUBTYPE_YVYU)
  {
   DEBUGS("YVYU");
   m_frame.colorspace = XVID_CSP_YVYU;
  }
 else if (subtype == MEDIASUBTYPE_UYVY)
  {
   DEBUGS("UYVY");
   m_frame.colorspace = XVID_CSP_UYVY;
  }
 else if (subtype == MEDIASUBTYPE_RGB32)
  {
   DEBUGS("RGB32");
   m_frame.colorspace = XVID_CSP_VFLIP | XVID_CSP_RGB32;
  }
 else if (subtype == MEDIASUBTYPE_RGB24)
  {
   DEBUGS("RGB24");
   m_frame.colorspace = XVID_CSP_VFLIP | XVID_CSP_RGB24;
  }
 else if (subtype == MEDIASUBTYPE_RGB555)
  {
   DEBUGS("RGB555");
   m_frame.colorspace = XVID_CSP_VFLIP | XVID_CSP_RGB555;
  }
 else if (subtype == MEDIASUBTYPE_RGB565)
  {
   DEBUGS("RGB565");
   m_frame.colorspace = XVID_CSP_VFLIP | XVID_CSP_RGB565;
  }
 else if (subtype == GUID_NULL)
  {
   m_frame.colorspace = XVID_CSP_NULL;
  }
 else
  {
   m_frame.colorspace = XVID_CSP_NULL;
   return S_FALSE;
  }

 if (formattype == FORMAT_VideoInfo)
  {
   VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )format;
   m_frame.stride = vih->bmiHeader.biWidth;
   m_frame.bpp=vih->bmiHeader.biBitCount;
  }
 else if (formattype == FORMAT_VideoInfo2)
  {
   VIDEOINFOHEADER2 * vih2 = (VIDEOINFOHEADER2 * )format;
   m_frame.stride = vih2->bmiHeader.biWidth;
   m_frame.bpp=vih2->bmiHeader.biBitCount;
  }
 else
  return S_FALSE;

 return S_OK;
}

// set output colorspace 
HRESULT TffDecoder::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
 DEBUGS("SetMediaType");

 if (direction == PINDIR_OUTPUT)
  return ChangeColorspace(*pmt->Subtype(), *pmt->FormatType(), pmt->Format());

 return S_OK;
}

// check input<->output compatiblity 
HRESULT TffDecoder::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
 DEBUGS("CheckTransform");
 return S_OK;
}

// alloc output buffer 
HRESULT TffDecoder::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest)
{
 DEBUGS("DecideBufferSize");
 HRESULT result;
 ALLOCATOR_PROPERTIES ppropActual;

 if (m_pInput->IsConnected() == FALSE) return E_UNEXPECTED;

 ppropInputRequest->cBuffers = 1;
 ppropInputRequest->cbBuffer = (resizeCtx->isResize?resizeCtx->FFdx:AVIdx) * (resizeCtx->isResize?resizeCtx->FFdy:AVIdy) * 4;
 // cbAlign causes problems with the resize filter */
 // ppropInputRequest->cbAlign = 16;
 ppropInputRequest->cbPrefix = 0;

 result = pAlloc->SetProperties(ppropInputRequest, &ppropActual);
 if (result != S_OK)  return result;
 if (ppropActual.cbBuffer < ppropInputRequest->cbBuffer) return E_FAIL;

 return S_OK;
}

void TffDecoder::loadAVInameAndPreset(void)
{
 IEnumFilters *eff=NULL;
 AVIname[0]='\0';
 if (m_pGraph->EnumFilters(&eff)==S_OK)
  {
   eff->Reset();
   IBaseFilter *bff;
   while (eff->Next(1,&bff,NULL)==S_OK || AVIname[0]=='\0')
    {
     FILTER_INFO iff;
     CLSID iffclsid;
     bff->GetClassID(&iffclsid);
     bff->QueryFilterInfo(&iff);
     IFileSourceFilter *ifsf=NULL;
     bff->QueryInterface(IID_IFileSourceFilter,(void**)&ifsf);
     if (ifsf)
      {
       LPOLESTR aviNameL;
       ifsf->GetCurFile(&aviNameL,NULL);
       ifsf->Release();
       WideCharToMultiByte(CP_ACP,0,aviNameL,-1,AVIname,511, NULL, NULL );
       if (cfg.autoPreset) cfg.autoPresetLoad(AVIname);
       sub->init(AVIname,NULL,AVIfps);strcpy(cfg.subFlnm,sub->flnm);
      }
     if (iff.pGraph) iff.pGraph->Release();
     bff->Release();
    }
   eff->Release();
  }
}
void TffDecoder::calcCrop(void)
{
 if (!cfg.isCropNzoom)
  {
   cropLeft=cropTop=0;
   cropDx=AVIdx;cropDy=AVIdy;
   return;
  }
 if (cfg.isZoom)
  {
   cropDx=((100-cfg.magnificationX)*AVIdx)/100;
   cropDy=((100-cfg.magnificationY)*AVIdy)/100;
   cropLeft=(AVIdx-cropDx)/2;
   cropTop =(AVIdy-cropDy)/2;
  }
 else
  { 
   cropDx=AVIdx-(cfg.cropLeft+cfg.cropRight);
   cropDy=AVIdy-(cfg.cropTop+cfg.cropBottom);
   cropLeft=cfg.cropLeft;cropTop=cfg.cropTop;
  }; 
 cropDx&=~7;cropDy&=~7;if (cropDx==0) cropDx=8;if (cropDy==0) cropDy=8;
 if (cropLeft+cropDx>=AVIdx) cropLeft=AVIdx-cropDx;if (cropTop+cropDy>=AVIdy) cropTop=AVIdy-cropDy;
 if (cropLeft>=AVIdx-8) cropLeft=AVIdx-8;if (cropTop>=AVIdy-8) cropTop=AVIdy-8;
}

// decode frame 
HRESULT TffDecoder::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
 //DEBUGS("Transform");
 if (!libavcodec.inited) 
  {
   libavcodec.init(&cfg);
   if (cfg.trayIcon) tray->show();
  }; 
  
 LONGLONG t1,t2;pIn->GetMediaTime(&t1,&t2);
 if (t1==0 && avctx) 
  {
   //char pomS[256];sprintf(pomS,"time: %i\n",int(t1));OutputDebugString(pomS);
   libavcodec.avcodec_close(avctx);
   free(avctx);
   avctx=NULL;
  }

 if (!avctx)
  {
   DEBUGS("creating avctx");
   if (!libavcodec.ok) return S_FALSE;
   avctx=(AVCodecContext*)malloc(sizeof(AVCodecContext));
   memset(avctx,0,sizeof(AVCodecContext));
   avctx->width =AVIdx;
   avctx->height=AVIdy;
   DEBUGS("avcodec_find_decoder_by_name before");
   AVCodec *avcodec=libavcodec.avcodec_find_decoder_by_name(codecName);
   DEBUGS("avcodec_find_decoder_by_name after");
   DEBUGS("avcodec_open before");
   if (libavcodec.avcodec_open(avctx,avcodec)<0)
    return S_FALSE;
   DEBUGS("avcodec_open after");
   firstFrame=true;
  };
 if (cfg.idctChanged)
  {
   cfg.idctChanged=0;
   switch (cfg.idct)
    {  
     case 2:libavcodec.set_ff_idct(idct_xref);break;
     case 3:libavcodec.set_ff_idct((void*)3);break;
     case 1:libavcodec.set_ff_idct((void*)2);break;
     case 0:
     default:libavcodec.set_ff_idct((void*)1);break;
    }
  } 

 AM_MEDIA_TYPE * mtOut;
 pOut->GetMediaType(&mtOut);
 if (mtOut != NULL)
  {
   HRESULT result;
   result = ChangeColorspace(mtOut->subtype, mtOut->formattype, mtOut->pbFormat);
   DeleteMediaType(mtOut);
   if (result != S_OK)
    return result;
  }
 if (pIn ->GetPointer((BYTE**)&m_frame.bitstream)!=S_OK) return S_FALSE;
 if (pOut->GetPointer((BYTE**)&m_frame.image    )!=S_OK) return S_FALSE;
 m_frame.outLenght=pOut->GetActualDataLength();//GetSize();
 m_frame.length   =pIn ->GetActualDataLength();//GetSize();

 #ifdef FF__MPEG
 mpegBuf->push(m_frame.bitstream,m_frame.length);
 firstFrame=false;
 static int count=0,mpeglen=0;
 char pomS[256];sprintf(pomS,"%i : m_frame.length:%i, %i\n",count++,m_frame.length,mpeglen+=m_frame.length);
 OutputDebugString(pomS);
 #endif
 
 if (m_frame.length<10) {m_frame.bitstream=NULL;m_frame.length=0;}; 

 AVPicture avpict;
 int got_picture=0;
 if (firstFrame)
  {
   firstFrame=false;
   if (AVIname[0]=='\0') loadAVInameAndPreset();
  }
 int ret=libavcodec.avcodec_decode_video(avctx,&avpict,&got_picture,(UINT8*)m_frame.bitstream,m_frame.length);
 //char pomS[256];sprintf(pomS,"framelen:%i ret:%i gotpicture:%i\n",m_frame.length,ret,got_picture);OutputDebugString(pomS);
 if (pIn->IsPreroll()==S_OK) 
  {
   DEBUGS("avcodec_decode_video preroll==S_OK");
   return S_FALSE;  
  };
 if (m_frame.length==0 && ret<0) return S_OK;
 if (ret<0 || !got_picture) 
  {
   memset(avpict.data[0],0,avpict.linesize[0]*AVIdy);
   memset(avpict.data[1],128,avpict.linesize[1]*AVIdy/2);
   memset(avpict.data[2],128,avpict.linesize[2]*AVIdy/2);
  }
 //else DEBUGS("got image"); 
 
 #ifdef FF__MPEG 
 static int kcount=0;
 sprintf(pomS,"kreslim:%i\n",kcount++);
 OutputDebugString(pomS);
 #endif
 
 cfg.sub=sub->getSubtitle(t1);
 if (resizeCtx->resizeChanged)
  {
   resizeCtx->resizeChanged=false;
   calcCrop();
   if (resizeCtx->isResize || cfg.resizeAspect==2 || cfg.isCropNzoom) 
    {
     if (!cfg.resizeAspect)
      {
       resizeCtx->imgDx=resizeCtx->FFdx;
       resizeCtx->imgDy=resizeCtx->FFdy;
       resizeCtx->diffX=resizeCtx->diffY=0;
      }
     else
      {
       int ax,ay;
       if (cfg.resizeAspect==1)
        {
         ax=AVIdx;
         ay=AVIdy;
        }
       else
        { 
         ax=cfg.aspectRatio;
         ay=1<<16;
        }; 
       resizeCtx->imgDx=resizeCtx->FFdx;
       resizeCtx->imgDy=resizeCtx->FFdx*ay/ax;
       if (resizeCtx->imgDy>resizeCtx->FFdy)
        {
         resizeCtx->imgDx=resizeCtx->FFdy*ax/ay;
         resizeCtx->imgDy=resizeCtx->FFdy;
        }
       resizeCtx->imgDx&=~7;resizeCtx->imgDy&=~7;
       resizeCtx->diffX=(resizeCtx->FFdx-resizeCtx->imgDx)/2;resizeCtx->diffX&=~3;
       resizeCtx->diffY=(resizeCtx->FFdy-resizeCtx->imgDy)/2;resizeCtx->diffY&=~3;
      }
      
     if (cfg.resizeFirst)
      imgFilters->init(resizeCtx->FFdx,resizeCtx->strideY,resizeCtx->FFdy,resizeCtx->diffX,resizeCtx->diffY);
     else
      imgFilters->init(cropDx,avpict.linesize[0],cropDy,0,0);
     if (imgFilters->postproc.ok)
      resizeCtx->initResize(&imgFilters->postproc,cropDx,cropDy);
     resizeCtx->clear(); 
    }  
   else 
    imgFilters->init(AVIdx,avpict.linesize[0],AVIdy,0,0);
  };  
 IMAGE destPict;
 if ((resizeCtx->isResize || cfg.resizeAspect==2 || cfg.isCropNzoom) && imgFilters->postproc.ok)
  {
   int cropDiffY=avpict.linesize[0]*cropTop+cropLeft,cropDiffUV=avpict.linesize[1]*(cropTop/2)+(cropLeft/2);
   if (cfg.resizeFirst)
    {
     resizeCtx->resize(avpict.data[0]+cropDiffY,avpict.data[1]+cropDiffUV,avpict.data[2]+cropDiffUV,
                       avpict.linesize[0],avpict.linesize[1],avpict.linesize[2],
                       AVIdy);
     imgFilters->process(resizeCtx->imgResizeY,resizeCtx->imgResizeU,resizeCtx->imgResizeV,
                         &destPict.y,&destPict.u,&destPict.v,
                         /*libavcodec.quant_store*/NULL);
    }
   else
    {
     unsigned char *src[3];
     imgFilters->process(avpict.data[0]+cropDiffY,avpict.data[1]+cropDiffUV,avpict.data[2]+cropDiffUV,
                         &src[0],&src[1],&src[2],
                         libavcodec.quant_store);
     resizeCtx->resize(src[0],src[1],src[2],
                       avpict.linesize[0],avpict.linesize[1],avpict.linesize[2],
                       AVIdy);
     destPict.y=resizeCtx->imgResizeY;destPict.u=resizeCtx->imgResizeU;destPict.v=resizeCtx->imgResizeV;
    }
   if (m_frame.stride<resizeCtx->FFdx) 
    {
     char pomS[256];sprintf(pomS,"bola by chyba: stride:%i, AVIdx:%i\n",m_frame.stride,resizeCtx->FFdx);DEBUGS(pomS);
     return S_FALSE;
    }; 
   image_output(&destPict,
                resizeCtx->FFdx,resizeCtx->FFdy,resizeCtx->strideY,
                (unsigned char*)m_frame.image,
                m_frame.stride,
                m_frame.colorspace^(cfg.flip?XVID_CSP_VFLIP:0));
  }
 else   
  {
   if (m_frame.stride<AVIdx) 
    {
     char pomS[256];sprintf(pomS,"bola by chyba: stride:%i, AVIdx:%i\n",m_frame.stride,AVIdx);DEBUGS(pomS);
     return S_FALSE;
    }; 
   imgFilters->process(avpict.data[0],avpict.data[1],avpict.data[2],&destPict.y,&destPict.u,&destPict.v,libavcodec.quant_store);
   image_output(&destPict,
                AVIdx,AVIdy,avpict.linesize[0],
                (unsigned char*)m_frame.image,
                m_frame.stride,
                m_frame.colorspace^(cfg.flip?XVID_CSP_VFLIP:0));
  }; 
 return S_OK;
}

// get property page list 
STDMETHODIMP TffDecoder::GetPages(CAUUID * pPages)
{
 DEBUGS("GetPages");
 
 if (cfg.trayIcon) tray->show();

 pPages->cElems = 1;
 pPages->pElems = (GUID *)CoTaskMemAlloc(pPages->cElems * sizeof(GUID));
 if (pPages->pElems == NULL) return E_OUTOFMEMORY;
 pPages->pElems[0] = CLSID_TFFDSHOWPAGE;

 return S_OK;
}
// cleanup pages 
STDMETHODIMP TffDecoder::FreePages(CAUUID * pPages)
{
 DEBUGS("FreePages");
 CoTaskMemFree(pPages->pElems);
 return S_OK;
}

#ifdef DEBUG
static int refcnt=0;
STDMETHODIMP_(ULONG) TffDecoder::AddRef()
{
 refcnt++;
// DEBUGS1("TffDecoder::AddRef",refcnt);
 return GetOwner()->AddRef();
}
STDMETHODIMP_(ULONG) TffDecoder::Release()
{
 refcnt--;
// DEBUGS1("TffDecoder::Release",refcnt);
 return GetOwner()->Release();
}
#endif
