#ifndef IFFCONSTANTS_H_
#define IFFCONSTANTS_H_

// {00F99063-70D5-4bcc-9D88-3801F3E3881B}
DEFINE_GUID(IID_IffDecoder, 0x00f99063, 0x70d5, 0x4bcc, 0x9d, 0x88, 0x38, 0x01, 0xf3, 0xe3, 0x88, 0x1b);
// {04FE9017-F873-410e-871E-AB91661A4EF7}
DEFINE_GUID(CLSID_FFDSHOW , 0x04fe9017, 0xf873, 0x410e, 0x87, 0x1e, 0xab, 0x91, 0x66, 0x1a, 0x4e, 0xf7);

#define FFDSHOW_NAME_L  L"ffdshow MPEG-4 Video Decoder"
#define FFPRESET_EXT "ffpreset"
#define FFPRESET_DEFAULT "default"

#define IDFF_autoPreset            1  //automatic preset loading enabled
#define IDFF_trayIcon              3  //is tray icon visible
#define IDFF_cfgDlgHnwd            4  //handle to configuration dialog
#define IDFF_autoPresetFileFirst   5  //try to load preset from file
#define IDFF_autoLoadedFromFile    6
#define IDFF_inPlayer              7

#define IDFF_dlgRestorePos         9
#define IDFF_dlgPosX              10
#define IDFF_dlgPosY              11
#define IDFF_lvWidth0             12
#define IDFF_showHints            13
#define IDFF_lastPage              2  //last active page in configuration dialog
                               
#define IDFF_flip                301  //should output video be flipped?
#define IDFF_idct                601  //IDCT function user (0 = libavcodec simple 16383, 1 = libavcodec normal, 2 = reference, 3 = libavcodec simple 16384)
                               
#define IDFF_filterIs           2001
#define IDFF_filterOrder        2002
#define IDFF_filterFull         2003

#define IDFF_filterPostproc      100                               
#define IDFF_ppqual              101  //postprocessing quality set by user (active when not autoq)
#define IDFF_autoq               102  //is automatic postprocessing control enabled?
#define IDFF_ppIsCustom          103  //custom postprocessing settings are enabled
#define IDFF_ppcustom            104  //custom postprocessing settings
#define IDFF_currentq            105
#define IDFF_deblockStrength     110
#define IDFF_levelFixLum         107
#define IDFF_levelFixChrom       108
                               
#define IDFF_filterPictProp      200
#define IDFF_lumGain             201  //luminance gain
#define IDFF_lumOffset           202  //luminance offset
#define IDFF_gammaCorrection     206  //gamma correction (*100)
#define IDFF_hue                 203  //hue
#define IDFF_saturation          204  //saturation
#define IDFF_lumGainDef          208
#define IDFF_lumOffsetDef        209
#define IDFF_gammaCorrectionDef  210
#define IDFF_hueDef              211
#define IDFF_saturationDef       212
                               
#define IDFF_filterSharpen       400
#define IDFF_sharpenMethod       406 //0 - xsharpen, 1 - unsharp
#define IDFF_xsharp_strength     402 //xsharpen filter strength
#define IDFF_xsharp_threshold    403 //xsharpen filter threshold
#define IDFF_unsharp_strength    404 //xsharpen filter strength
#define IDFF_unsharp_threshold   405 //xsharpen filter threshold
                                 
#define IDFF_filterNoise         500
#define IDFF_noiseMethod         505 //0 - my noise, 1 - avih noise
#define IDFF_uniformNoise        502 //is uniform noise active (applies only to luma noise now)?
#define IDFF_noiseStrength       503 //luma noise strength
#define IDFF_noiseStrengthChroma 504 //chroma noise strength
                                 
#define IDFF_filterResizeNaspect 700
#define IDFF_resizeDx            702 //new width
#define IDFF_resizeDy            703 //new height
#define IDFF_isAspect            704 //0 - no aspect ratio correctio, 1 - keep original aspect, 2 - aspect ration is set in IDFF_aspectRatio
#define IDFF_aspectRatio         707 //aspect ratio (<<16)
#define IDFF_resizeMethod        706
#define IDFF_resizeGblurLum      708 // *100
#define IDFF_resizeGblurChrom    709 // *100
#define IDFF_resizeSharpenLum    710 // *100
#define IDFF_resizeSharpenChrom  711 // *100

#define IDFF_filterCropNzoom     725
#define IDFF_isZoom              713
#define IDFF_magnificationX      714
#define IDFF_cropLeft            715
#define IDFF_cropRight           716
#define IDFF_cropTop             717
#define IDFF_cropBottom          718
#define IDFF_autocrop            719
#define IDFF_magnificationY      720
#define IDFF_magnificationLocked 721
#define IDFF_cropChanged         724 

#define IDFF_filterSubtitles     800                                 
#define IDFF_subPosX             810
#define IDFF_subPosY             811
#define IDFF_subDelay            812
#define IDFF_subDelayDef         818
#define IDFF_subSpeed            813
#define IDFF_subSpeedDef         819
#define IDFF_subAutoFlnm         814
#define IDFF_subFlnm             820
#define IDFF_fontCharset         802
#define IDFF_fontSize            803
#define IDFF_fontWeight          804
#define IDFF_fontShadowStrength  805  //shadow strength (0..100) 100 - subtitles aren't transparent
#define IDFF_fontShadowRadius    806  //shadow radius
#define IDFF_fontSpacing         808
#define IDFF_fontColor           809
#define IDFF_fontName            821

#define IDFF_filterBlur          900                                 
#define IDFF_blurStrength        902
#define IDFF_tempSmooth          904

#define IDFF_filterOffset       1100
#define IDFF_offsetY_X          1103
#define IDFF_offsetY_Y          1104
#define IDFF_offsetU_X          1105
#define IDFF_offsetU_Y          1106
#define IDFF_offsetV_X          1107
#define IDFF_offsetV_Y          1108

#define IDFF_filterShowMV       1200

#define IDFF_xvid               1001 //are AVIs with this FOURCC played by ffdshow?
#define IDFF_div3               1002
#define IDFF_divx               1003
#define IDFF_dx50               1004
#define IDFF_mp43               1005
#define IDFF_mp42               1006
#define IDFF_mp41               1007
#define IDFF_h263               1008
#define IDFF_blz0               1010
#define IDFF_wmv1               1011
#define IDFF_rawv               1009
                              
#define IDFF_outYV12            1301
#define IDFF_outYUY2            1302
#define IDFF_outYVYU            1303
#define IDFF_outUYVY            1304
#define IDFF_outRGB32           1305
#define IDFF_outRGB24           1306
#define IDFF_outRGB555          1307
#define IDFF_outRGB565          1308
                              
#endif                        
