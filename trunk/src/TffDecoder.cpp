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

#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#pragma hdrstop
#include <streams.h>
#include <initguid.h>
#include <olectl.h>
#if (1100 > _MSC_VER)
#include <olectlid.h>
#endif
#include <dvdmedia.h>   // VIDEOINFOHEADER2
#include <assert.h>
#include <time.h>

#include "Tconfig.h"
#include "ffdebug.h"
#include "ffmpeg\libavcodec\avcodec.h"
#include "xvid\xvid.h"
#include "xvid\image\image.h"
#include "movie_source\TmovieSource.h"
#include "movie_source\TmovieSourceLibavcodec.h"

#include "TffdshowPage.h"
#include "TffDecoder.h"
#include "TresizeCtx.h"
#include "TtrayIcon.h"
#include "subtitles\Tsubtitles.h"

#include "ffdshow_mediaguids.h"
#include "ffdshow_dshowreg.h"

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

STDMETHODIMP TffDecoder::getParam(unsigned int paramID, int* value)
{
 if (!value) return S_FALSE;
 int val; 
 switch (paramID)
  {
   #undef _PARAM_OP
   #define _PARAM_OP PARAM_GET
   #include "ffdshow_params.h"
   default:*value=0;return S_FALSE;
  }
 *value=val; 
 return S_OK;
}
STDMETHODIMP TffDecoder::getParam2(unsigned int paramID)
{
 int val; 
 switch (paramID)
  {
   #undef _PARAM_OP
   #define _PARAM_OP PARAM_GET
   #include "ffdshow_params.h"
   default:val=0;break;
  }
 return val; 
}
STDMETHODIMP TffDecoder::putParam(unsigned int paramID, int  val)
{
 CAutoLock cAutolock(&lock);
 switch (paramID)
  {
   #undef _PARAM_OP
   #define _PARAM_OP PARAM_SET
   #include "ffdshow_params.h"
   default:return S_FALSE;
  }
 if (paramID!=IDFF_lastPage) sendOnChange();
 return S_OK; 
}
STDMETHODIMP TffDecoder::notifyParamsChanged(void)
{
 subsChanged();
 resizeChanged();
 trayIconChanged();
 return S_OK;
}
void TffDecoder::subsChanged(void)
{
 presetSettings->fontChanged=true;
}
void TffDecoder::resizeChanged(void)
{
 if (resizeCtx) resizeCtx->resizeChanged=true;
 if (presetSettings->magnificationLocked) presetSettings->magnificationY=presetSettings->magnificationX;
 //if (AVIdx && AVIdy) calcCrop();
}
void TffDecoder::trayIconChanged(void)
{
 if (!tray) return;
 if (globalSettings.trayIcon) tray->show(); else tray->hide();
}
STDMETHODIMP TffDecoder::getNumPresets(unsigned int *value)
{
 *value=0;
 if (!value) return E_POINTER;
 *value=presets.size();
 return S_OK;
}
STDMETHODIMP TffDecoder::getPresetName(unsigned int i,char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (i>=presets.size()) return S_FALSE;
 if (len<strlen(presets[i]->presetName)+1) return E_OUTOFMEMORY;
 strcpy(buf,presets[i]->presetName);
 return S_OK;
}
STDMETHODIMP TffDecoder::getPresets(Tpresets *presets2)
{
 if (!presets2) return E_POINTER;
 presets2->done();
 for (Tpresets::iterator i=presets.begin();i!=presets.end();i++)
  presets2->push_back(new TpresetSettings(**i));
 return S_OK;       
}
STDMETHODIMP TffDecoder::setPresets(const Tpresets *presets2)
{
 if (!presets2) return E_POINTER;
 CAutoLock cAutolock(&lock);
 presets.done();
 for (Tpresets::const_iterator i=presets2->begin();i!=presets2->end();i++)
  presets.push_back(new TpresetSettings(**i));
 return S_OK;       
}
STDMETHODIMP TffDecoder::savePresets(void)
{
 presets.saveRegAll();
 return S_OK;
}
STDMETHODIMP TffDecoder::setPresetPtr(TpresetSettings *preset)
{
 if (!preset) return E_POINTER;
 CAutoLock cAutolock(&lock);
 presetSettings=preset;
 notifyParamsChanged();
 sendOnChange();
 return S_OK;
}
STDMETHODIMP TffDecoder::getActivePresetName(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(presetSettings->presetName)+1) return E_OUTOFMEMORY;
 strcpy(buf,presetSettings->presetName);
 return S_OK;
}
STDMETHODIMP TffDecoder::getDefaultPresetName(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(globalSettings.defaultPreset)+1) return E_OUTOFMEMORY;
 strcpy(buf,globalSettings.defaultPreset);
 return S_OK;
}
STDMETHODIMP TffDecoder::isDefaultPreset(const char *presetName)
{
 return (_stricmp(globalSettings.defaultPreset,presetName)==0)?S_OK:S_FALSE;
}
STDMETHODIMP TffDecoder::setDefaultPresetName(const char *presetName)
{
 if (!presetName) return E_POINTER;
 strcpy(globalSettings.defaultPreset,presetName);
 sendOnChange();
 return S_OK; 
}
STDMETHODIMP TffDecoder::getAVIname(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(AVIname)+1) return E_OUTOFMEMORY;
 strcpy(buf,AVIname);
 return S_OK;
}
STDMETHODIMP TffDecoder::getAVIfourcc(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(AVIfourcc)+1) return E_OUTOFMEMORY;
 strcpy(buf,AVIfourcc);
 return S_OK;
}
STDMETHODIMP TffDecoder::getAVIfps(unsigned int *fps)
{
 if (!fps || AVIfps==0) return S_FALSE;
 *fps=1000*AVIfps;
 return S_OK;
}
STDMETHODIMP TffDecoder::loadPreset(const char *name)
{
 presetSettings=presets.getPreset((name)?name:presetSettings->presetName);
 notifyParamsChanged();
 return S_OK;
}
STDMETHODIMP TffDecoder::savePreset(const char *name)
{
 presets.savePreset(presetSettings,name);
 return S_OK;
}
STDMETHODIMP TffDecoder::savePresetToFile(const char *flnm)
{
 //TODO: check save success
 if (!flnm) return S_FALSE;
 presetSettings->saveFile(flnm);
 return S_OK;
}
STDMETHODIMP TffDecoder::loadPresetFromFile(const char *flnm)
{
 //TODO: check load success
 if (!flnm) return S_FALSE;
 presetSettings->loadFile(flnm);
 presets.storePreset(presetSettings);
 notifyParamsChanged();
 return S_OK;
}
STDMETHODIMP TffDecoder::removePreset(const char *name)
{
 if (_stricmp(presetSettings->presetName,globalSettings.defaultPreset)==0)
  setDefaultPresetName(presets[0]->presetName);
 presets.removePreset(name);
 return S_OK;
}
STDMETHODIMP TffDecoder::getAVcodecVersion(char *buf,unsigned int len)
{
 char *vers;
 TmovieSourceLibavcodec::getVersion(&vers);
 if (!vers) return S_FALSE;
 if (len<strlen(vers)+1) return E_OUTOFMEMORY;
 strcpy(buf,vers);
 return S_OK;
}
STDMETHODIMP TffDecoder::getAVIdimensions(unsigned int *x,unsigned int *y)
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
 *ppmode=Tpostproc::getPPmode(presetSettings);  
 return S_OK;
}
STDMETHODIMP TffDecoder::getFontName(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(presetSettings->fontName)+1) return E_OUTOFMEMORY;
 strcpy(buf,presetSettings->fontName);
 return S_OK;
}
STDMETHODIMP TffDecoder::setFontName(const char *name)
{
 if (!name) return E_POINTER;
 if (strlen(name)>255) return  S_FALSE;
 strcpy(presetSettings->fontName,name);
 subsChanged();
 sendOnChange();
 return S_OK;
}
STDMETHODIMP TffDecoder::getSubFlnm(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(presetSettings->subFlnm)+1) return E_OUTOFMEMORY;
 strcpy(buf,presetSettings->subFlnm);
 return S_OK;
}
STDMETHODIMP TffDecoder::loadSubtitles(const char *flnm)
{
 if (!flnm) return E_POINTER;
 if (strlen(flnm)>255) return  S_FALSE;
 if (subs)
  {
   subs->init(NULL,flnm,AVIfps);
   strcpy(presetSettings->subFlnm,subs->flnm);
  }
 else strcpy(presetSettings->subFlnm,flnm);  
 sendOnChange();
 return S_OK;
}
STDMETHODIMP TffDecoder::getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom)
{
 if (!left || !top || !right || !bottom) return E_POINTER;
 if (!AVIdx || !AVIdy)
  {
   *left=presetSettings->cropLeft;*top=presetSettings->cropTop;*right=presetSettings->cropRight;*bottom=presetSettings->cropBottom;
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
 return TpresetSettings::min_order;
}
STDMETHODIMP TffDecoder::getMaxOrder2(void)
{
 return TpresetSettings::max_order;
}
STDMETHODIMP TffDecoder::saveGlobalSettings(void)
{
 globalSettings.save();
 return S_OK;
}
STDMETHODIMP TffDecoder::loadGlobalSettings(void)
{
 globalSettings.load();
 return S_OK;
}
STDMETHODIMP TffDecoder::saveDialogSettings(void)
{
 dialogSettings.save();
 return S_OK;
}
STDMETHODIMP TffDecoder::loadDialogSettings(void)
{
 dialogSettings.load();
 return S_OK;
}
STDMETHODIMP TffDecoder::renameActivePreset(const char *newName)
{
 if (!newName) return E_POINTER;
 if (strlen(newName)>260) return E_OUTOFMEMORY;
 int res=_stricmp(presetSettings->presetName,globalSettings.defaultPreset);
 strcpy(presetSettings->presetName,newName);
 if (res==0) setDefaultPresetName(newName);
 sendOnChange();
 return S_OK;
}
void TffDecoder::sendOnChange(void)
{
 if (onChangeWnd && onChangeMsg) 
  SendMessage(onChangeWnd,onChangeMsg,0,0);
}
STDMETHODIMP TffDecoder::setOnChangeMsg(HWND wnd,unsigned int msg)
{
 onChangeWnd=wnd;onChangeMsg=msg;
 return S_OK;
}
STDMETHODIMP TffDecoder::setOnInfoMsg(HWND wnd,unsigned int msg)
{
 onInfoWnd=wnd;onInfoMsg=msg;
 return S_OK;
}
STDMETHODIMP TffDecoder::showCfgDlg(void)
{
 CAUUID pages;
 ISpecifyPropertyPages *ispp;
 QueryInterface(IID_ISpecifyPropertyPages,(void**)&ispp);
 ispp->GetPages(&pages);
 IUnknown *ifflist[]={ispp};
 OleCreatePropertyFrame(NULL,10,10,L"ffdshow",
                        1,ifflist,
                        pages.cElems,pages.pElems,
                        LOCALE_SYSTEM_DEFAULT,
                        0,0
                       );
 ispp->Release();                       
 return S_OK;
}

// query interfaces 
STDMETHODIMP TffDecoder::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_IffDecoder) return GetInterface((IffDecoder *) this, ppv);
 if (riid==IID_ISpecifyPropertyPages) return GetInterface((ISpecifyPropertyPages *) this, ppv);
 return CVideoTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

// constructor 
TffDecoder::TffDecoder(LPUNKNOWN punk, HRESULT *phr):CVideoTransformFilter(NAME("CffDecoder"),punk,/*CLSID_XVID*/CLSID_FFDSHOW)
{
 DEBUGS("Constructor");
 ASSERT(phr);

 InitCommonControls();
 
 AVIname[0]=AVIfourcc[0]='\0';
 AVIdx=AVIdy=cropLeft=cropTop=cropDx=cropDy=AVIfps=0;
 isDlg=0;inPlayer=1;
 movie=NULL;
 resizeCtx=NULL;tray=NULL;
 onChangeWnd=NULL;onChangeMsg=0;
 onInfoWnd=NULL;onInfoMsg=0;
 codecId=CODEC_ID_NONE;
 lastTime=clock();
 firstFrame=firstTransform=true;
 
 config.init();
 globalSettings.load();
 dialogSettings.load();
 presets.init();
 loadPreset(globalSettings.defaultPreset);
 
 tray=new TtrayIcon(this,g_hInst);
 
 //TODO: prevent creation of imgFilters and sub in cfg dialog only mode
 imgFilters=new TimgFilters;
 subs=new Tsubtitles;
}

// destructor 
TffDecoder::~TffDecoder()
{
 __asm {emms};
 DEBUGS("Destructor");
 if (movie)
  {
   delete movie;
   movie=NULL;
  }; 
 if (imgFilters) delete imgFilters;
 if (resizeCtx) delete resizeCtx;
 if (subs) delete subs;
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
   AVIfps=(vih->AvgTimePerFrame)?10000000.0/vih->AvgTimePerFrame:25;
   hdr = &vih->bmiHeader;
  }
 else if (formatType == FORMAT_VideoInfo2)
  {
   VIDEOINFOHEADER2 * vih2 = (VIDEOINFOHEADER2 *) mtIn->Format();
   AVIfps=(vih2->AvgTimePerFrame)?10000000.0/vih2->AvgTimePerFrame:25;
   hdr = &vih2->bmiHeader;
  }
 #ifdef  FF__MPEG
 else if (formatType == FORMAT_MPEGVideo)
  {
   MPEG1VIDEOINFO *mpeg1info=(MPEG1VIDEOINFO*)mtIn->Format();
   hdr = &mpeg1info->hdr.bmiHeader;
  } 
 #endif 
 #ifdef FF__MPEG2
 else if (formatType==FORMAT_MPEG2Video)
  {
   MPEG2VIDEOINFO *mpeg2info=(MPEG2VIDEOINFO*)mtIn->Format();
   hdr=&mpeg2info->hdr.bmiHeader;
   codecId=CODEC_ID_MPEG1VIDEO;
  }
 #endif 
 else
  {
   return VFW_E_TYPE_NOT_ACCEPTED;
  }
 if (hdr->biHeight<0) DEBUGS("colorspace: inverted input format not supported");

 if (hdr->biCompression==0)
  if (hdr->biBitCount==24 || hdr->biBitCount==32)
   hdr->biCompression=FOURCC_RGB2;
 if (codecId==CODEC_ID_NONE)  
  codecId=globalSettings.codecSupported(hdr->biCompression,AVIfourcc);
 if (codecId!=CODEC_ID_NONE)
  {
   AVIdx=hdr->biWidth;
   AVIdy=hdr->biHeight;
   int cnt=loadAVInameAndPreset();
   if (cnt>1) return VFW_E_TYPE_NOT_ACCEPTED;
   if (!resizeCtx)
    {
     if (!imgFilters->postproc.ok) presetSettings->isResize=false;
     resizeCtx=new TresizeCtx(presetSettings);
     if (presetSettings->isResize)
      resizeCtx->allocate(presetSettings->resizeDx,presetSettings->resizeDy);
     else
      resizeCtx->allocate(AVIdx,AVIdy);
    };  
   #ifdef FF__MPEG
   if (formatType==FORMAT_MPEGVideo)
    {
     codecName="mpegvideo";strcpy(AVIfourcc,"mpeg1");
     return S_OK;
    }; 
   #endif 
   return S_OK;
  }
 else return VFW_E_TYPE_NOT_ACCEPTED;
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
 vih->bmiHeader.biWidth  = (presetSettings->isResize)?presetSettings->resizeDx:AVIdx;
 vih->bmiHeader.biHeight = (presetSettings->isResize)?presetSettings->resizeDy:AVIdy;
 vih->bmiHeader.biPlanes = 1;

 if (iPosition < 0) return E_INVALIDARG;

 switch(iPosition)
  {
   case 0:
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

int TffDecoder::loadAVInameAndPreset(void)
{
 IEnumFilters *eff=NULL;
 int cnt=0;
 if (m_pGraph->EnumFilters(&eff)==S_OK)
  {
   eff->Reset();
   IBaseFilter *bff;
   while (eff->Next(1,&bff,NULL)==S_OK)
    {
     CLSID iffclsid;
     bff->GetClassID(&iffclsid);
     if (iffclsid==CLSID_FFDSHOW) cnt++;
     if (AVIname[0]=='\0')
      {
       FILTER_INFO iff;
       bff->QueryFilterInfo(&iff);
       IFileSourceFilter *ifsf=NULL;
       bff->QueryInterface(IID_IFileSourceFilter,(void**)&ifsf);
       if (ifsf)
        {
         LPOLESTR aviNameL;
         ifsf->GetCurFile(&aviNameL,NULL);
         ifsf->Release();
         WideCharToMultiByte(CP_ACP,0,aviNameL,-1,AVIname,511, NULL, NULL );
         if (globalSettings.autoPreset)
          {
           TpresetSettings *preset=presets.getAutoPreset(AVIname,globalSettings.autoPresetFileFirst);
           if (preset)
            setPresetPtr(preset);
          }  
        }
       if (iff.pGraph) iff.pGraph->Release();
      } 
     bff->Release();
    }
   eff->Release();
  }
 return cnt; 
}
void TffDecoder::calcCrop(void)
{
 if (!presetSettings->isCropNzoom)
  {
   cropLeft=cropTop=0;
   cropDx=AVIdx;cropDy=AVIdy;
   return;
  }
 if (presetSettings->isZoom)
  {
   cropDx=((100-presetSettings->magnificationX)*AVIdx)/100;
   cropDy=((100-presetSettings->magnificationY)*AVIdy)/100;
   cropLeft=(AVIdx-cropDx)/2;
   cropTop =(AVIdy-cropDy)/2;
  }
 else
  { 
   cropDx=AVIdx-(presetSettings->cropLeft+presetSettings->cropRight );
   cropDy=AVIdy-(presetSettings->cropTop +presetSettings->cropBottom);
   cropLeft=presetSettings->cropLeft;cropTop=presetSettings->cropTop;
  }; 
 cropDx&=~7;cropDy&=~7;if (cropDx<=0) cropDx=8;if (cropDy<=0) cropDy=8;
 if (cropLeft+cropDx>=AVIdx) cropLeft=AVIdx-cropDx;if (cropTop+cropDy>=AVIdy) cropTop=AVIdy-cropDy;
 if (cropLeft>=AVIdx-8) cropLeft=AVIdx-8;if (cropTop>=AVIdy-8) cropTop=AVIdy-8;
}

// decode frame 
HRESULT TffDecoder::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
 CAutoLock cAutolock(&lock);
 LONGLONG t1=-1,t2=-1;pIn->GetMediaTime(&t1,&t2);
 if (onInfoMsg)
  {
   clock_t t=clock();
   int fps=(t!=lastTime)?1000*CLOCKS_PER_SEC/(t-lastTime):0;
   PostMessage(onInfoWnd,onInfoMsg,fps,(int)t1);
   lastTime=t;
  }; 
 //DEBUGS("Transform");
 if (firstTransform) 
  {
   firstTransform=false;
   if (globalSettings.trayIcon) tray->show();
  }; 
  
 if (t1==0 && movie) 
  {
   //char pomS[256];sprintf(pomS,"time: %i\n",int(t1));OutputDebugString(pomS);
   delete movie;
   movie=NULL;
  }

 if (!movie)
  {
   DEBUGS("creating movie source");
   movie=TmovieSource::initSource(codecId,AVIdx,AVIdy);
   if (!movie) return S_FALSE;
   firstFrame=true;
  };

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
 firstFrame=false;
 static int count=0,mpeglen=0;
 char pomS[256];sprintf(pomS,"%i : m_frame.length:%i, %i\n",count++,m_frame.length,mpeglen+=m_frame.length);
 OutputDebugString(pomS);
 #endif
 
 if (m_frame.length<10) {m_frame.bitstream=NULL;m_frame.length=0;}; 

 AVPicture avpict;memset(&avpict,0,sizeof(avpict));
 int got_picture=0;
 if (firstFrame)
  {
   firstFrame=false;
   if (AVIname[0]=='\0') loadAVInameAndPreset();
   subs->init(AVIname,NULL,AVIfps);strcpy(presetSettings->subFlnm,subs->flnm);
  }
 int ret=movie->getFrame(&globalSettings,presetSettings,(unsigned char*)m_frame.bitstream,m_frame.length,&avpict,got_picture);
 //char pomS[256];sprintf(pomS,"framelen:%i ret:%i gotpicture:%i\n",m_frame.length,ret,got_picture);OutputDebugString(pomS);
 if (pIn->IsPreroll()==S_OK) 
  {
   DEBUGS("avcodec_decode_video preroll==S_OK");
   return S_FALSE;  
  };
 if (m_frame.length==0 && ret<0) return S_OK;
 if (ret<0 || !got_picture) 
//#ifdef FF__MPEG
  if (!avpict.data[0]) return S_FALSE;
  else
//#endif 
   {
    memset(avpict.data[0],  0,avpict.linesize[0]*AVIdy  );
    memset(avpict.data[1],128,avpict.linesize[1]*AVIdy/2);
    memset(avpict.data[2],128,avpict.linesize[2]*AVIdy/2);
   }
 //else DEBUGS("got image"); 
 
 #ifdef FF__MPEG 
 static int kcount=0;
 sprintf(pomS,"kreslim:%i\n",kcount++);
 OutputDebugString(pomS);
 #endif
 
 imgFilters->setSubtitle(subs->getSubtitle(t1));
 if (resizeCtx->resizeChanged)
  {
   resizeCtx->resizeChanged=false;
   calcCrop();
   if (resizeCtx->isResize || presetSettings->resizeAspect==2 || presetSettings->isCropNzoom) 
    {
     if (!presetSettings->resizeAspect)
      {
       resizeCtx->imgDx=resizeCtx->FFdx;
       resizeCtx->imgDy=resizeCtx->FFdy;
       resizeCtx->diffX=resizeCtx->diffY=0;
      }
     else
      {
       int ax,ay;
       if (presetSettings->resizeAspect==1)
        {
         ax=AVIdx;
         ay=AVIdy;
        }
       else
        { 
         ax=presetSettings->aspectRatio;
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
     DEBUGS("imgFilters->init before"); 
     if (presetSettings->resizeFirst)
      imgFilters->init(resizeCtx->FFdx,resizeCtx->strideY,resizeCtx->FFdy,resizeCtx->diffX,resizeCtx->diffY);
     else
      imgFilters->init(cropDx,avpict.linesize[0],cropDy,0,0);
     DEBUGS("imgFilters->init after"); 
     DEBUGS("initResize before");
     DEBUGS2("cropDx, cropDy",cropDx,cropDy);
     if (imgFilters->postproc.ok)
      resizeCtx->initResize(&imgFilters->postproc,cropDx,cropDy,presetSettings);
     DEBUGS("initResize after");
     DEBUGS("resizeCtx->clear before");
     resizeCtx->clear(); 
     DEBUGS("resizeCtx->clear after");
    }  
   else 
    imgFilters->init(AVIdx,avpict.linesize[0],AVIdy,0,0);
  };  
 IMAGE destPict;
 if ((resizeCtx->isResize || presetSettings->resizeAspect==2 || presetSettings->isCropNzoom) && imgFilters->postproc.ok)
  {
   int cropDiffY=avpict.linesize[0]*cropTop+cropLeft,cropDiffUV=avpict.linesize[1]*(cropTop/2)+(cropLeft/2);
   if (presetSettings->resizeFirst)
    {
     resizeCtx->resize(avpict.data[0]+cropDiffY,avpict.data[1]+cropDiffUV,avpict.data[2]+cropDiffUV,
                       avpict.linesize[0],avpict.linesize[1],avpict.linesize[2],
                       AVIdy);
     imgFilters->process(presetSettings,
                         resizeCtx->imgResizeY,resizeCtx->imgResizeU,resizeCtx->imgResizeV,
                         &destPict.y,&destPict.u,&destPict.v,
                         /*libavcodec.quant_store*/NULL);
    }
   else
    {
     unsigned char *src[3];
     imgFilters->process(presetSettings,
                         avpict.data[0]+cropDiffY,avpict.data[1]+cropDiffUV,avpict.data[2]+cropDiffUV,
                         &src[0],&src[1],&src[2],
                         movie->getQuant());
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
                m_frame.colorspace^(presetSettings->flip?XVID_CSP_VFLIP:0));
  }
 else   
  {
   if (m_frame.stride<AVIdx) 
    {
     char pomS[256];sprintf(pomS,"there would be an error: stride:%i, AVIdx:%i\n",m_frame.stride,AVIdx);DEBUGS(pomS);
     return S_FALSE;
    }; 
   imgFilters->process(presetSettings,avpict.data[0],avpict.data[1],avpict.data[2],&destPict.y,&destPict.u,&destPict.v,movie->getQuant());
   image_output(&destPict,
                AVIdx,AVIdy,avpict.linesize[0],
                (unsigned char*)m_frame.image,
                m_frame.stride,
                m_frame.colorspace^(presetSettings->flip?XVID_CSP_VFLIP:0));
  }; 
 return S_OK;
}

// get property page list 
STDMETHODIMP TffDecoder::GetPages(CAUUID * pPages)
{
 DEBUGS("GetPages");
 
 if (globalSettings.trayIcon) tray->show();

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
