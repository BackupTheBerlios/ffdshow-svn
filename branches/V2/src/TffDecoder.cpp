/*
 * Copyright (c) 2002 Milan Cutka
 * based on CXvidDecoder.cpp from XviD DirectShow filter
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
#include "stdafx.h"
#include <streams.h>
#include <initguid.h>
#include <olectl.h>
#if (1100 > _MSC_VER)
#include <olectlid.h>
#endif
#include <dvdmedia.h>   // VIDEOINFOHEADER2
#include <vector>

using namespace std;

#include "Tconfig.h"
#include "TpresetSettings.h"
#include "Tfilter.h"
#include "TfilterResizeNaspect.h"
#include "TimgFilter.h"
#include "ffdebug.h"
#include "ffmpeg\libavcodec\avcodec.h"
#include "xvid\xvid.h"
#include "xvid\image\image.h"
#include "TmovieSource.h"
#include "TmovieSourceLibavcodec.h"
#include "TmovieSourceXviD.h"

#include "TffdshowPage.h"
#include "TffDecoder.h"
#include "TtrayIcon.h"
#include "TffRect.h"
#include "TffPict.h"
#include "subtitles\Tsubtitles.h"

#include "ffdshow_mediaguids.h"
#include "ffdshow_dshowreg.h"

// create instance
CUnknown * WINAPI TffDecoder::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
#ifdef DEBUG
 DbgSetModuleLevel(0xffffffff,0);
#endif
 TffDecoder * pNewObject=new TffDecoder(punk,phr);
 if (pNewObject==NULL) *phr=E_OUTOFMEMORY;
 return pNewObject;
}
// query interfaces
STDMETHODIMP TffDecoder::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_IffDecoder) return GetInterface((IffDecoder *) this, ppv);
 if (riid==IID_ISpecifyPropertyPages) return GetInterface((ISpecifyPropertyPages *) this, ppv);
 return CVideoTransformFilter::NonDelegatingQueryInterface(riid, ppv);
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
 onTrayIconChanged();
 return S_OK;
}
void TffDecoder::onTrayIconChanged(void)
{
 if (!tray) return;
 if (globalSettings.trayIcon) tray->show(); else tray->hide();
}
STDMETHODIMP TffDecoder::getNumPresets(unsigned int *value)
{
 if (!value) return E_POINTER;
 *value=presets.size();
 return S_OK;
}
STDMETHODIMP TffDecoder::getPresetName(unsigned int i,char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (i>=presets.size()) return S_FALSE;
 if (len<strlen(presets[i]->settings.presetName)+1) return E_OUTOFMEMORY;
 strcpy(buf,presets[i]->settings.presetName);
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
 presetSettings=preset;//currentq=presetSettings->postproc.settings.qual;
 notifyParamsChanged();
 sendOnChange();
 return S_OK;
}
STDMETHODIMP TffDecoder::getActivePresetName(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (!presetSettings) return S_FALSE;
 if (len<strlen(presetSettings->settings.presetName)+1) return E_OUTOFMEMORY;
 strcpy(buf,presetSettings->settings.presetName);
 return S_OK;
}
STDMETHODIMP TffDecoder::setActivePreset(const char *name)
{
 if (!name) return S_FALSE;
 TpresetSettings *preset=presets.getPreset(name);
 if (preset)
  {
   setPresetPtr(preset);
   return S_OK;
  }
 else return S_FALSE;
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
STDMETHODIMP TffDecoder::getAVIfps(unsigned int *fps1000)
{
 if (!fps1000 || AVIfps==0) return S_FALSE;
 *fps1000=1000*AVIfps;
 return S_OK;
}
STDMETHODIMP TffDecoder::getAVIfps2(void)
{
 return 1000*AVIfps;
}
STDMETHODIMP TffDecoder::getCurrentFrame2(void)
{
 return t1;
}
STDMETHODIMP TffDecoder::saveActivePreset(const char *name)
{
 if (!presetSettings) return S_FALSE;
 presets.savePreset(presetSettings,name);
 return S_OK;
}
STDMETHODIMP TffDecoder::saveActivePresetToFile(const char *flnm)
{
 //TODO: check save success
 if (!flnm || !presetSettings) return S_FALSE;
 presets.savePresetFile(presetSettings,flnm);
 return S_OK;
}
STDMETHODIMP TffDecoder::loadActivePresetFromFile(const char *flnm)
{
 //TODO: check load success
 if (!flnm) return S_FALSE;
 if (!presetSettings) presetSettings=new TpresetSettings(this);
 presetSettings->loadFile(flnm);//currentq=presetSettings->postproc.settings.qual;
 presets.storePreset(presetSettings);
 notifyParamsChanged();
 return S_OK;
}
STDMETHODIMP TffDecoder::removePreset(const char *name)
{
 if (_stricmp(presetSettings->settings.presetName,globalSettings.defaultPreset)==0)
  setDefaultPresetName(presets[0]->settings.presetName);
 return presets.removePreset(name)?S_OK:S_FALSE;
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
STDMETHODIMP TffDecoder::getXvidVersion(char *buf,unsigned int len)
{
 char *vers;
 TmovieSourceXviD::getVersion(&vers);
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
STDMETHODIMP TffDecoder::getOutputDimensions(unsigned int *x,unsigned int *y)
{
 *x=*y=0;
 if (!x || !y || outDx==0 || outDy==0) return S_FALSE;
 *x=outDx;*y=outDx;
 return S_OK;
}
/*
STDMETHODIMP TffDecoder::getFontName(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(presetSettings->subtitles.fontSettings.settings.name)+1) return E_OUTOFMEMORY;
 strcpy(buf,presetSettings->subtitles.fontSettings.settings.name);
 return S_OK;
}
STDMETHODIMP TffDecoder::setFontName(const char *name)
{
 if (!name) return E_POINTER;
 if (strlen(name)>255) return  S_FALSE;
 strcpy(presetSettings->subtitles.fontSettings.settings.name,name);
 sendOnChange();
 return S_OK;
}
STDMETHODIMP TffDecoder::getSubFlnm(char *buf,unsigned int len)
{
 if (!buf) return E_POINTER;
 if (len<strlen(presetSettings->subtitles.settings.flnm)+1) return E_OUTOFMEMORY;
 strcpy(buf,presetSettings->subtitles.settings.flnm);
 return S_OK;
}
STDMETHODIMP TffDecoder::loadSubtitles(const char *flnm)
{
 if (!flnm) return E_POINTER;
 if (strlen(flnm)>255) return  S_FALSE;
 if (subs)
  {
   subs->init(NULL,flnm,AVIfps);
   strcpy(presetSettings->subtitles.settings.flnm,subs->flnm);
  }
 else strcpy(presetSettings->subtitles.settings.flnm,flnm);
 sendOnChange();
 return S_OK;
}
*/
STDMETHODIMP TffDecoder::getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom)
{
 return E_NOTIMPL;
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
 int res=_stricmp(presetSettings->settings.presetName,globalSettings.defaultPreset);
 strcpy(presetSettings->settings.presetName,newName);
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
STDMETHODIMP TffDecoder::setOnInfoMsg(HWND wnd,unsigned int msg1,unsigned int msg2)
{
 onInfoWnd=wnd;onInfoMsg1=msg1;onInfoMsg2=msg2;
 return S_OK;
}
STDMETHODIMP TffDecoder::showCfgDlg(HWND owner)
{
 if (cfgDlgHnwd) return S_FALSE;
 ISpecifyPropertyPages *ispp;
 QueryInterface(IID_ISpecifyPropertyPages,(void**)&ispp);
 CAUUID pages;
 ispp->GetPages(&pages);
 IUnknown *ifflist[]={ispp};
 OleCreatePropertyFrame(owner,10,10,L"ffdshow",
                        1,ifflist,
                        pages.cElems,pages.pElems,
                        LOCALE_SYSTEM_DEFAULT,
                        0,0
                       );
 ispp->Release();
 return S_OK;
}
STDMETHODIMP TffDecoder::getMovieSource(TmovieSource* *moviePtr)
{
 if (!moviePtr) return S_FALSE;
 *moviePtr=movie;
 return S_OK;
}
STDMETHODIMP TffDecoder::getPostproc(Tpostproc* *postprocPtr)
{
 if (!postprocPtr) return S_FALSE;
 *postprocPtr=&postproc;
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetNumFilters(unsigned int *num)
{
 *num=-1;
 if (!presetSettings) return S_FALSE;
 *num=presetSettings->filters.size();
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetNumFilters2(void)
{
 return (presetSettings)?presetSettings->filters.size():-1;
}
STDMETHODIMP TffDecoder::presetGetFilterName(unsigned int i,char *buf,unsigned int len)
{
 if (!presetSettings || i>=presetSettings->filters.size()) return S_FALSE;
 const char *name=presetSettings->filters[i]->getName();
 if (len<strlen(name)+1) return E_OUTOFMEMORY;
 strcpy(buf,name);
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetFilterIs2(unsigned int i)
{
 if (!presetSettings || i>=presetSettings->filters.size()) return -1;
 return presetSettings->filters[i]->is;
}
STDMETHODIMP TffDecoder::presetSetFilterIs(unsigned int i,int Iis)
{
 if (!presetSettings || i>=presetSettings->filters.size()) return S_FALSE;
 presetSettings->filters[i]->is=Iis;
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetFilterFull2(unsigned int i)
{
 if (!presetSettings || i>=presetSettings->filters.size()) return -1;
 return presetSettings->filters[i]->full;
}
STDMETHODIMP TffDecoder::presetSetFilterFull(unsigned int i,int Ifull)
{
 if (!presetSettings || i>=presetSettings->filters.size()) return S_FALSE;
 presetSettings->filters[i]->full=Ifull;
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetConfPage(unsigned int i,TconfPage* *page)
{
 if (!presetSettings || i>=presetSettings->filters.size()) return S_FALSE;
 *page=presetSettings->filters[i]->getConfPage();
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetConfSubPage(unsigned int i,TconfPage* *subPage)
{
 if (!presetSettings || i>=presetSettings->filters.size()) return S_FALSE;
 *subPage=presetSettings->filters[i]->getConfPage();
 return S_OK;
}
STDMETHODIMP TffDecoder::presetSetFilterOrder(unsigned int oldOrder,unsigned int newOrder)
{
 //TODO: set filter order
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetPPmode(unsigned int index,unsigned int *ppmode)
{
 *ppmode=(postproc.ok)?Tpostproc::getPPmode(presetSettings->filters[index],currentq):0;
 return S_OK;
}
STDMETHODIMP TffDecoder::presetGetParam(unsigned int index,unsigned int paramID,int *value)
{
 if (!presetSettings || index>=presetSettings->filters.size()) return S_FALSE;
 return presetSettings->filters[index]->getParam(paramID,*value)?S_OK:S_FALSE;
}
STDMETHODIMP TffDecoder::presetGetParam2(unsigned int index,unsigned int paramID)
{
 if (!presetSettings || index>=presetSettings->filters.size()) return 0;
 int val;
 presetSettings->filters[index]->getParam(paramID,val);
 return val;
}
STDMETHODIMP TffDecoder::presetPutParam(unsigned int index,unsigned int paramID,int value)
{
 if (!presetSettings || index>=presetSettings->filters.size()) return S_FALSE;
 return presetSettings->filters[index]->setParam(paramID,value)?(sendOnChange(),S_OK):S_FALSE;
}
STDMETHODIMP TffDecoder::presetGetParamStr(unsigned int index,unsigned int paramID,char *value,unsigned int len)
{
 if (!presetSettings || index>=presetSettings->filters.size()) return S_FALSE;
 char pomS[1024];
 bool ok=presetSettings->filters[index]->getParam(paramID,pomS);
 if (!ok) return S_FALSE;
 if (len<strlen(pomS)+1) return E_OUTOFMEMORY;
 strcpy(value,pomS);
 return S_OK;
}
STDMETHODIMP TffDecoder::presetPutParamStr(unsigned int index,unsigned int paramID,char *value)
{
 if (!presetSettings || index>=presetSettings->filters.size()) return S_FALSE;
 return presetSettings->filters[index]->setParam(paramID,value)?(sendOnChange(),S_OK):S_FALSE;
}
STDMETHODIMP TffDecoder::getCfcs(void* *cfcs)
{
 *cfcs=&this->cfcs;
 return S_OK;
}

// constructor
TffDecoder::TffDecoder(LPUNKNOWN punk, HRESULT *phr):CVideoTransformFilter(NAME("CffDecoder"),punk,CLSID_FFDSHOW)
{
 DEBUGS("Constructor");
 ASSERT(phr);

 InitCommonControls();
 Tfilter::init(&cfcs,this);

 AVIname[0]=AVIfourcc[0]='\0';
 AVIdx=AVIdy=outDx=outDy=0;AVIfps=0;t1=0;
 cfgDlgHnwd=NULL;inPlayer=1;
 movie=NULL;tray=NULL;
 onChangeWnd=NULL;onChangeMsg=0;
 onInfoWnd=NULL;onInfoMsg1=onInfoMsg2=0;
 codecId=CODEC_ID_NONE;
 lastTime=clock();

 config.init();
 globalSettings.load();
 dialogSettings.load();
 presets.init(this);
 setActivePreset(globalSettings.defaultPreset);
 filterResizeNaspect=(TfilterResizeNaspect*)(presetSettings->getFilter(IDFF_filterResizeNaspect));

 tray=new TtrayIcon(this,g_hInst);
}

// destructor
TffDecoder::~TffDecoder()
{
 __asm emms;
 DEBUGS("Destructor");
 if (movie) delete movie;movie=NULL;
 postproc.done();
 Tfilter::done();
 delete tray;tray=NULL;
}

// check input type
HRESULT TffDecoder::CheckInputType(const CMediaType * mtIn)
{
 DEBUGS("CheckInputType");

 if (*mtIn->Type()!=MEDIATYPE_Video) return VFW_E_TYPE_NOT_ACCEPTED;
 const GUID formatType=*mtIn->FormatType();
 BITMAPINFOHEADER * hdr;
 if (formatType==FORMAT_VideoInfo)
  {
   VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)mtIn->Format();
   AVIfps=(vih->AvgTimePerFrame)?10000000.0/vih->AvgTimePerFrame:25;
   hdr=&vih->bmiHeader;
  }
 else if (formatType==FORMAT_VideoInfo2)
  {
   VIDEOINFOHEADER2 *vih2=(VIDEOINFOHEADER2*)mtIn->Format();
   AVIfps=(vih2->AvgTimePerFrame)?10000000.0/vih2->AvgTimePerFrame:25;
   hdr=&vih2->bmiHeader;
  }
 #ifdef  FF__MPEG
 else if (formatType==FORMAT_MPEGVideo)
  {
   MPEG1VIDEOINFO *mpeg1info=(MPEG1VIDEOINFO*)mtIn->Format();
   hdr=&mpeg1info->hdr.bmiHeader;
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
  return VFW_E_TYPE_NOT_ACCEPTED;
 if (hdr->biHeight<0)
  {
   DEBUGS("colorspace: inverted input format not supported");
   return VFW_E_TYPE_NOT_ACCEPTED;
  }

 //char pomS[5];memcpy(pomS,&hdr->biCompression,4);
 DWORD biCompression=hdr->biCompression;
 if (biCompression==0)
  switch (hdr->biBitCount)
   {
    case 32:biCompression=FOURCC_RGB3;break;
    case 24:biCompression=FOURCC_RGB2;break;
    case 16:biCompression=FOURCC_RGB6;break;
    case 15:biCompression=FOURCC_RGB5;break;
   }
 if (codecId==CODEC_ID_NONE)
  codecId=globalSettings.codecSupported(biCompression,AVIfourcc);
 if (codecId!=CODEC_ID_NONE)
  {
   AVIdx=hdr->biWidth;
   AVIdy=hdr->biHeight;
   int cnt=loadAVInameAndPreset();
   if (cnt>1) return VFW_E_TYPE_NOT_ACCEPTED;
   postproc.init();
   isResize=(filterResizeNaspect->is!=0)&&postproc.ok;
   outDx=(isResize)?filterResizeNaspect->settings.dx:AVIdx;outDy=(isResize)?filterResizeNaspect->settings.dy:AVIdy;
   #ifdef FF__MPEG
   if (formatType==FORMAT_MPEGVideo)
    {
     codecName="mpegvideo";strcpy(AVIfourcc,"mpeg1");
     return S_OK;
    }
   #endif
   return S_OK;
  }
 else return VFW_E_TYPE_NOT_ACCEPTED;
}

// get list of supported output colorspaces
HRESULT TffDecoder::GetMediaType(int iPosition, CMediaType *mtOut)
{
 DEBUGS("GetMediaType");

 if (m_pInput->IsConnected()==FALSE) return E_UNEXPECTED;

 VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)mtOut->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
 if (vih==NULL) return E_OUTOFMEMORY;

 ZeroMemory(vih,sizeof(VIDEOINFOHEADER));
 vih->bmiHeader.biSize  =sizeof(BITMAPINFOHEADER);
 vih->bmiHeader.biWidth =outDx;
 vih->bmiHeader.biHeight=outDy;
 vih->bmiHeader.biPlanes=1;

 if (iPosition<0) return E_INVALIDARG;
 if (iPosition>=globalSettings.outputColorspaces.size()) iPosition=globalSettings.outputColorspaces.size()-1;
 const TglobalSettings::ToutputColorspace *c=&globalSettings.outputColorspaces[iPosition];
 if (!c->g) return VFW_S_NO_MORE_ITEMS;
 vih->bmiHeader.biCompression=c->biCompression;
 vih->bmiHeader.biBitCount=c->biBitCount;
 mtOut->SetSubtype(c->g);

 vih->bmiHeader.biSizeImage=GetBitmapSize(&vih->bmiHeader);

 mtOut->SetType(&MEDIATYPE_Video);
 mtOut->SetFormatType(&FORMAT_VideoInfo);
 mtOut->SetTemporalCompression(FALSE);
 mtOut->SetSampleSize(vih->bmiHeader.biSizeImage);

 return S_OK;
}

// (internal function) change colorspace
HRESULT TffDecoder::ChangeColorspace(GUID subtype, GUID formattype, void * format)
{
 if (subtype==MEDIASUBTYPE_YV12)
  {
   DEBUGS("YV12");
   m_frame.colorspace=XVID_CSP_YV12;
  }
 else if (subtype==MEDIASUBTYPE_YUY2)
  {
   DEBUGS("YUY2");
   m_frame.colorspace=XVID_CSP_YUY2;
  }
 else if (subtype==MEDIASUBTYPE_YVYU)
  {
   DEBUGS("YVYU");
   m_frame.colorspace=XVID_CSP_YVYU;
  }
 else if (subtype==MEDIASUBTYPE_UYVY)
  {
   DEBUGS("UYVY");
   m_frame.colorspace=XVID_CSP_UYVY;
  }
 else if (subtype==MEDIASUBTYPE_RGB32)
  {
   DEBUGS("RGB32");
   m_frame.colorspace=XVID_CSP_RGB32|XVID_CSP_VFLIP;
  }
 else if (subtype==MEDIASUBTYPE_RGB24)
  {
   DEBUGS("RGB24");
   m_frame.colorspace=XVID_CSP_RGB24|XVID_CSP_VFLIP;
  }
 else if (subtype==MEDIASUBTYPE_RGB555)
  {
   DEBUGS("RGB555");
   m_frame.colorspace=XVID_CSP_RGB555|XVID_CSP_VFLIP;
  }
 else if (subtype==MEDIASUBTYPE_RGB565)
  {
   DEBUGS("RGB565");
   m_frame.colorspace=XVID_CSP_RGB565|XVID_CSP_VFLIP;
  }
 else if (subtype==GUID_NULL)
  {
   m_frame.colorspace=XVID_CSP_NULL;
  }
 else
  {
   m_frame.colorspace=XVID_CSP_NULL;
   return S_FALSE;
  }

 if (formattype==FORMAT_VideoInfo)
  {
   VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)format;
   m_frame.stride=vih->bmiHeader.biWidth;
   m_frame.bpp   =vih->bmiHeader.biBitCount;
  }
 else if (formattype==FORMAT_VideoInfo2)
  {
   VIDEOINFOHEADER2 *vih2=(VIDEOINFOHEADER2*)format;
   m_frame.stride=vih2->bmiHeader.biWidth;
   m_frame.bpp   =vih2->bmiHeader.biBitCount;
  }
 else
  return S_FALSE;

 return S_OK;
}

// set output colorspace
HRESULT TffDecoder::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
 DEBUGS("SetMediaType");
 if (direction==PINDIR_OUTPUT)
  return ChangeColorspace(*pmt->Subtype(),*pmt->FormatType(),pmt->Format());
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
 if (m_pInput->IsConnected()==FALSE) return E_UNEXPECTED;

 ppropInputRequest->cBuffers=1;
 ppropInputRequest->cbBuffer=outDx*outDy*4;
 // cbAlign causes problems with the resize filter */
 // ppropInputRequest->cbAlign = 16;
 ppropInputRequest->cbPrefix=0;

 ALLOCATOR_PROPERTIES ppropActual;
 HRESULT result=pAlloc->SetProperties(ppropInputRequest,&ppropActual);
 if (result!=S_OK) return result;
 if (ppropActual.cbBuffer<ppropInputRequest->cbBuffer) return E_FAIL;
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

// decode frame
HRESULT TffDecoder::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
 //DEBUGS("Transform");
 t1=-1;LONGLONG t2=-1;
 pIn->GetMediaTime(&t1,&t2);
 if (onInfoMsg1)
  {
   clock_t t=clock();
   int fps=(t!=lastTime)?1000*CLOCKS_PER_SEC/(t-lastTime):0;
   PostMessage(onInfoWnd,onInfoMsg1,fps,(int)t1);
   lastTime=t;
  }
  
 if (!movie && globalSettings.trayIcon) tray->show();

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
  }

 AM_MEDIA_TYPE *mtOut;
 pOut->GetMediaType(&mtOut);
 if (mtOut!=NULL)
  {
   HRESULT result;
   result=ChangeColorspace(mtOut->subtype,mtOut->formattype,mtOut->pbFormat);
   DeleteMediaType(mtOut);
   if (result!=S_OK) return result;
  }
  
 if (pIn ->GetPointer((BYTE**)&m_frame.bitstream)!=S_OK) return S_FALSE;
 if (pOut->GetPointer((BYTE**)&m_frame.image    )!=S_OK) return S_FALSE;
 m_frame.outLength=pOut->GetActualDataLength();//GetSize();
 m_frame.length   =pIn ->GetActualDataLength();//GetSize();
 if (onInfoMsg2) PostMessage(onInfoWnd,onInfoMsg2,m_frame.length,m_frame.colorspace);

 #ifdef FF__MPEG
 firstFrame=false;
 static int count=0,mpeglen=0;
 char pomS[256];sprintf(pomS,"%i : m_frame.length:%i, %i\n",count++,m_frame.length,mpeglen+=m_frame.length);
 OutputDebugString(pomS);
 #endif

 if (m_frame.length<10)
  {
   m_frame.bitstream=NULL;
   m_frame.length=0;
  }

 TpresetSettings presetSettings=*this->presetSettings;
 filterResizeNaspect->is=isResize;filterResizeNaspect->settings.dx=outDx;filterResizeNaspect->settings.dy=outDy;
 AVPicture avpict;memset(&avpict,0,sizeof(avpict));
 int got_picture=0,ret;
 TffPict2 pict=movie->getFrame(&globalSettings,&presetSettings,(unsigned char*)m_frame.bitstream,m_frame.length,ret,got_picture);
 //char pomS[256];sprintf(pomS,"framelen:%i ret:%i gotpicture:%i\n",m_frame.length,ret,got_picture);OutputDebugString(pomS);
 if (pIn->IsPreroll()==S_OK)
  {
   DEBUGS("avcodec_decode_video preroll==S_OK");
   return S_FALSE;
  }
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

 IMAGE destPict;
 if (m_frame.stride<outDx)
  {
   char pomS[256];sprintf(pomS,"here would be an error: stride:%u, AVIdx:%u\n",m_frame.stride,AVIdx);DEBUGS(pomS);
   return S_FALSE;
  }
 //imgFilters->process(&globalSettings,&presetSettings,pict);
 for (vector<Tfilter*>::const_iterator i=presetSettings.filters.begin();i!=presetSettings.filters.end();i++)
  {
   Tfilter *fs=*i;
   TimgFilter *f=fs->getImgFilter();
   f->process(pict,fs);
  } 
 
 destPict.y=pict.y;destPict.u=pict.u;destPict.v=pict.v;
 image_output(&destPict,
              pict.rect.full.dx,pict.rect.full.dy,pict.rect.stride,
              (unsigned char*)m_frame.image,
              m_frame.stride,
              m_frame.colorspace^(presetSettings.settings.flip?XVID_CSP_VFLIP:0));
 return S_OK;
}

// get property page list
STDMETHODIMP TffDecoder::GetPages(CAUUID * pPages)
{
 DEBUGS("GetPages");
 if (cfgDlgHnwd)
  {
   pPages->cElems=0;
   pPages->pElems=NULL;
   return S_FALSE;
  }

 if (globalSettings.trayIcon) tray->show();

 pPages->cElems=1;
 pPages->pElems=(GUID *)CoTaskMemAlloc(pPages->cElems*sizeof(GUID));
 if (pPages->pElems==NULL) return E_OUTOFMEMORY;
 pPages->pElems[0]=CLSID_TFFDSHOWPAGE;

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

STDMETHODIMP TffDecoder::JoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName)
{
 if (!pGraph)
  if (cfgDlgHnwd) SendMessage(HWND(cfgDlgHnwd),WM_CLOSE,0,0);
 return CVideoTransformFilter::JoinFilterGraph(pGraph,pName);
}
