#ifndef _IFFDECODER_H_
#define _IFFDECODER_H_

#ifdef __cplusplus
extern "C" {
#endif

// {00F99063-70D5-4bcc-9D88-3801F3E3881B}
DEFINE_GUID(IID_IffDecoder, 0x00f99063, 0x70d5, 0x4bcc, 0x9d, 0x88, 0x38, 0x01, 0xf3, 0xe3, 0x88, 0x1b);
// {04FE9017-F873-410e-871E-AB91661A4EF7}
DEFINE_GUID(CLSID_FFDSHOW , 0x04fe9017, 0xf873, 0x410e, 0x87, 0x1e, 0xab, 0x91, 0x66, 0x1a, 0x4e, 0xf7);

#define FFDSHOW_NAME_L  L"ffdshow MPEG-4 Video Decoder"

#define IDFF_autoPreset            1  //automatic preset loading enabled
#define IDFF_lastPage              2  //last active page in configuration dialog
#define IDFF_trayIcon              3  //is tray icon visible
#define IDFF_isDlg                 4  //is configuration dialog visible
#define IDFF_autoPresetFileFirst   5  //try to load preset from file 
#define IDFF_presetShouldBeSaved   6

#define IDFF_isPostproc          106  
#define IDFF_ppqual              101  //postprocessing quality set by user (active when not autoq)
#define IDFF_autoq               102  //is automatic postprocessing control enabled?
#define IDFF_ppIsCustom          103  //custom postprocessing settings are enabled
#define IDFF_ppcustom            104  //custom postprocessing settings
#define IDFF_currentq            105
#define IDFF_levelFixLum         107
#define IDFF_levelFixChrom       108

#define IDFF_isPictProp          205 
#define IDFF_lumGain             201  //luminance gain
#define IDFF_lumOffset           202  //luminance offset
#define IDFF_gammaCorrection     206  //gamma correction (*100)
#define IDFF_hue                 203  //hue 
#define IDFF_saturation          204  //saturation

#define IDFF_flip                301 //should output video be flipped?

#define IDFF_isSharpen           401 //is xshapen filter active?
#define IDFF_sharpenMethod       406 //0 - xsharpen, 1 - unsharp
#define IDFF_xsharp_strength     402 //xsharpen filter strength
#define IDFF_xsharp_threshold    403 //xsharpen filter threshold
#define IDFF_unsharp_strength    404 //xsharpen filter strength
#define IDFF_unsharp_threshold   405 //xsharpen filter threshold
 
#define IDFF_isNoise             501 //is noising filter active?
#define IDFF_noiseMethod         505 //0 - my noise, 1 - avih noise
#define IDFF_uniformNoise        502 //is uniform noise active (applies only to luma noise now)?
#define IDFF_noiseStrength       503 //luma noise strength
#define IDFF_noiseStrengthChroma 504 //chroma noise strength
 
#define IDFF_idct                601 //IDCT function user (0 = libavcodec simple 16383, 1 = libavcodec normal, 2 = reference, 3 = libavcodec simple 16384)

#define IDFF_isResize            701 //is resizing active (or will be resizing active)
#define IDFF_resizeDx            702 //new width
#define IDFF_resizeDy            703 //new height
#define IDFF_resizeAspect        704 //0 - no aspect ratio correctio, 1 - keep original aspect, 2 - aspect ration is set in IDFF_aspectRatio
#define IDFF_resizeFirst         705 //should be resizing applied before other filters
#define IDFF_resizeMethod        706
#define IDFF_aspectRatio         707 //aspect ratio (<<16)
#define IDFF_resizeGblurLum      708 // *100
#define IDFF_resizeGblurChrom    709 // *100
#define IDFF_resizeSharpenLum    710 // *100
#define IDFF_resizeSharpenChrom  711 // *100

#define IDFF_isSubtitles         801
#define IDFF_fontCharset         802
#define IDFF_fontSize            803
#define IDFF_fontWeight          804
#define IDFF_fontShadowStrength  805  //shadow strength (0..100) 100 - subtitles aren't transparent
#define IDFF_fontShadowRadius    806  //shadow radius  
#define IDFF_fontAutosize        807
#define IDFF_fontSpacing         808
#define IDFF_fontColor           809
#define IDFF_subPosX             810
#define IDFF_subPosY             811
#define IDFF_subDelay            812
#define IDFF_subSpeed            813
#define IDFF_subAutoFlnm         814
              
#define IDFF_xvid               1001 //are AVIs with this FOURCC played by ffdshow?
#define IDFF_div3               1002
#define IDFF_divx               1003
#define IDFF_dx50               1004
#define IDFF_mp43               1005
#define IDFF_mp42               1006
#define IDFF_mp41               1007
#define IDFF_h263               1008

DECLARE_INTERFACE_(IffDecoder, IUnknown)
{
 STDMETHOD (get_Param)(int paramID, int* value) PURE;
 STDMETHOD (get_Param2)(int paramID) PURE;
 STDMETHOD (put_Param)(int paramID, int  value) PURE;
 STDMETHOD (get_numPresets)(int *value) PURE;
 STDMETHOD (get_presetName)(unsigned int i,char *buf,unsigned int len) PURE;
 STDMETHOD (get_activePresetName)(char *buf,unsigned int len) PURE;
 STDMETHOD (get_AVIname)(char *buf,unsigned int len) PURE;
 STDMETHOD (get_AVIfourcc)(char *buf,unsigned int len) PURE;
 STDMETHOD (get_AVIdimensions)(int *x,int *y) PURE;
 STDMETHOD (get_AVIfps)(int *fps) PURE;
 STDMETHOD (savePreset)(const char *name) PURE;
 STDMETHOD (savePresetToFile)(const char *flnm) PURE;
 STDMETHOD (loadPreset)(const char *name) PURE;
 STDMETHOD (loadPresetFromFile)(const char *flnm) PURE;
 STDMETHOD (removePreset)(const char *name) PURE;
 STDMETHOD (notifyParamsChanged)(void) PURE;
 STDMETHOD (get_avcodec_version)(char *buf,unsigned int len) PURE;
 STDMETHOD (getPPmode)(int *ppmode) PURE;
 STDMETHOD (getPostProcDescription)(char *buf,unsigned int len) PURE;
 STDMETHOD (getPictPropDescription)(char *buf,unsigned int len) PURE;
 STDMETHOD (getNoiseDescription)(char *buf,unsigned int len) PURE;
 STDMETHOD (getSharpenDescription)(char *buf,unsigned int len) PURE;
 STDMETHOD (getFontName)(char *buf,unsigned int len) PURE;
 STDMETHOD (setFontName)(const char *name) PURE;
 STDMETHOD (getSubFlnm)(char *buf,unsigned int len) PURE;
 STDMETHOD (loadSubtitles)(const char *flnm) PURE;
};

#ifdef __cplusplus
}
#endif

#endif 
