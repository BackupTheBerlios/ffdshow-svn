#define PARAM_GET(id,settingsClass,valName,min,max,onChange)  \
 case id:val=settingsClass##valName;break;
#define PARAM_SET(id,settingsClass,valName,min,max,_onNotify) \
 case id:                                                     \
  {                                                           \
   if (min==-1 && max==-1) break;                             \
   int *dst=(int*)&settingsClass##valName;                    \
   int oldVal=*dst;                                           \
   *dst=val;                                                  \
   if (min || max)                                            \
    {                                                         \
     if (*dst<min) *dst=min;                                  \
     if (*dst>max) *dst=max;                                  \
    }                                                         \
   void (TffDecoder::*onNotify)(void)=_onNotify;              \
   if (onNotify && oldVal!=*dst) (this->*onNotify)();         \
  }                                                           \
  break;
  
#pragma warning (once:4127)

//state variables
_PARAM_OP(IDFF_inPlayer           ,this->,inPlayer          ,0,0,NULL)
_PARAM_OP(IDFF_cfgDlgHnwd         ,this->,cfgDlgHnwd        ,0,0,NULL)
//dialog settings
_PARAM_OP(IDFF_lastPage           ,dialogSettings.,lastPage     ,0,0,NULL)
_PARAM_OP(IDFF_dlgRestorePos      ,dialogSettings.,dlgRestorePos,0,0,NULL)
_PARAM_OP(IDFF_dlgPosX            ,dialogSettings.,dlgPosX      ,-4096,4096,NULL)
_PARAM_OP(IDFF_dlgPosY            ,dialogSettings.,dlgPosY      ,-4096,4096,NULL)
_PARAM_OP(IDFF_lvWidth0           ,dialogSettings.,lvWidth0     ,1,2048,NULL)
_PARAM_OP(IDFF_showHints,         ,dialogSettings.,showHints    ,0,0,NULL)
//global settings
_PARAM_OP(IDFF_autoPreset         ,globalSettings.,autoPreset         ,0,0,NULL)
_PARAM_OP(IDFF_autoPresetFileFirst,globalSettings.,autoPresetFileFirst,0,0,NULL)
_PARAM_OP(IDFF_trayIcon           ,globalSettings.,trayIcon           ,0,0,&TffDecoder::onTrayIconChanged)
_PARAM_OP(IDFF_xvid               ,globalSettings.,xvid               ,0,0,NULL)
_PARAM_OP(IDFF_div3               ,globalSettings.,div3               ,0,0,NULL)
_PARAM_OP(IDFF_divx               ,globalSettings.,divx               ,0,0,NULL)
_PARAM_OP(IDFF_dx50               ,globalSettings.,dx50               ,0,0,NULL)
_PARAM_OP(IDFF_mp43               ,globalSettings.,mp43               ,0,0,NULL)
_PARAM_OP(IDFF_mp42               ,globalSettings.,mp42               ,0,0,NULL)
_PARAM_OP(IDFF_mp41               ,globalSettings.,mp41               ,0,0,NULL)
_PARAM_OP(IDFF_h263               ,globalSettings.,h263               ,0,0,NULL)
_PARAM_OP(IDFF_blz0               ,globalSettings.,blz0               ,0,0,NULL)
_PARAM_OP(IDFF_wmv1               ,globalSettings.,wmv1               ,0,0,NULL)
_PARAM_OP(IDFF_rawv               ,globalSettings.,rawv               ,0,0,NULL)
_PARAM_OP(IDFF_outYV12            ,globalSettings.,outYV12            ,0,0,NULL)
_PARAM_OP(IDFF_outYUY2            ,globalSettings.,outYUY2            ,0,0,NULL)
_PARAM_OP(IDFF_outYVYU            ,globalSettings.,outYVYU            ,0,0,NULL)
_PARAM_OP(IDFF_outUYVY            ,globalSettings.,outUYVY            ,0,0,NULL)
_PARAM_OP(IDFF_outRGB32           ,globalSettings.,outRGB32           ,0,0,NULL)
_PARAM_OP(IDFF_outRGB24           ,globalSettings.,outRGB24           ,0,0,NULL)
_PARAM_OP(IDFF_outRGB555          ,globalSettings.,outRGB555          ,0,0,NULL)
_PARAM_OP(IDFF_outRGB565          ,globalSettings.,outRGB565          ,0,0,NULL)

//preset settings
_PARAM_OP(IDFF_autoLoadedFromFile ,presetSettings->,settings.autoLoadedFromFile ,0,0,NULL)
_PARAM_OP(IDFF_flip               ,presetSettings->,settings.flip               ,0,0,NULL)
_PARAM_OP(IDFF_idct               ,presetSettings->,settings.idct               ,0,4,NULL)

_PARAM_OP(IDFF_currentq           ,this->          ,currentq           ,0,6,NULL)
                                                        
