/* -------- This file has been automatically generated by configure ---------
   Note: Any changes in it will be lost when you run configure again. */

/* use GNU internationalisation */
#undef USE_I18N

/* Runtime Cpudetection */
#define RUNTIME_CPUDETECT 1

#define PREFIX "/usr/local"

/* define this to use simple idct with patched libavcodec */
#define SIMPLE_IDCT 1

#define USE_OSD 1
#define USE_SUB 1

/* Toggles debugging informations */
#undef MP_DEBUG

/* Indicates is Ogle's libdvdread is available for DVD playback */
#undef USE_DVDREAD

/* Common data directory (for fonts, etc) */
#define DATADIR "/usr/local/share/mplayer"
#define CONFDIR "/usr/local/share/mplayer"
#define LIBDIR "/usr/local/lib/mplayer"

/* Define this to compile stream-caching support, it can be enabled via
   -cache <kilobytes> */
#define USE_STREAM_CACHE 1

/* Define to include support for XviD/Divx4Linux/OpenDivx */
#define USE_DIVX

/* Define for using new XviD/DivX4Linux library, instead of open-source OpenDivX */
/* You have to change DECORE_LIBS in config.mak too! */
#undef NEW_DECORE

/* Define if you are using DivX5Linux Decore library */
#undef DECORE_DIVX5

/* If build mencoder */
#define HAVE_MENCODER

/* Indicates if XviD/Divx4linux encore is available
   Note: for mencoder */
#undef HAVE_DIVX4ENCORE

/* Indicates if libmp3lame is available
   Note: for mencoder */
#undef HAVE_MP3LAME

/* Define libmp1e for realtime mpeg encoding (for DXR3 and DVB cards) */
#undef USE_MP1E

/* Define this to enable avg. byte/sec-based AVI sync method by default:
   (use -bps or -nobps commandline option for run-time method selection)
   -bps gives better sync for vbr mp3 audio, it is now default */
#define AVI_SYNC_BPS 1

/* Undefine this if you do not want to select mono audio (left or right)
   with a stereo MPEG layer 2/3 audio stream. The command-line option
   -stereo has three possible values (0 for stereo, 1 for left-only, 2 for
   right-only), with 0 being the default.
   */
#define USE_FAKE_MONO 1

/* Undefine this if your soundcard driver has no working select().
   If you have kernel Oops, player hangups, or just no audio, you should
   try to recompile MPlayer with this option disabled! */
#define HAVE_AUDIO_SELECT 1

/* define this to use iconv(3) function to codepage conversions */
#undef USE_ICONV

/* define this to use RTC (/dev/rtc) for video timers (LINUX only) */
#undef HAVE_RTC

/* set up max. outburst. use 65536 for ALSA 0.5, for others 16384 is enough */
#define MAX_OUTBURST 65536

/* set up audio OUTBURST. Do not change this! */
#define OUTBURST 512

/* Define this if your system has the header file for the OSS sound interface */
#define HAVE_SYS_SOUNDCARD_H 1

/* Define this if your system has the "malloc.h" header file */
#define HAVE_MALLOC_H 1

/* memalign is mapped to malloc if unsupported */
#undef HAVE_MEMALIGN
#ifndef HAVE_MEMALIGN
# define memalign(a,b) xvid_malloc(b,a)
#endif

/* Define this if your system has the "alloca.h" header file */
#define HAVE_ALLOCA_H 1

/* Define this if your system has the "sys/mman.h" header file */
#define HAVE_SYS_MMAN_H 1

/* Define this if you have the elf dynamic linker -ldl library */
#define HAVE_LIBDL 1

/* Define this if you have the kstat kernel statistics library */
#undef HAVE_LIBKSTAT

/* Define this if you have zlib */
#define HAVE_ZLIB 1

/* Define this if you have shm support */
#undef HAVE_SHM

/* Define this if your system has vsscanf */
#define HAVE_VSSCANF 1

/* LIRC (remote control, see www.lirc.org) support: */
#undef HAVE_LIRC

/* DeCSS support using libcss */
#undef HAVE_LIBCSS

/* Define this to enable MPEG 1/2 image postprocessing (requires FAST cpu!) */
#define MPEG12_POSTPROC 1

/* Define this to enable image postprocessing in libavcodec (requires FAST cpu!) */
#undef FF_POSTPROCESS

/* Define to include support for OpenDivx postprocessing */
#undef HAVE_ODIVX_POSTPROCESS 

/* Win32 DLL support */
#undef USE_WIN32DLL

/* DirectShow support */
#undef USE_DIRECTSHOW

/* ffmpeg's libavcodec support (requires libavcodec source) */
#undef USE_LIBAVCODEC
#undef USE_LIBAVCODEC_SO

/* Use libavcodec's decoders */
#define CONFIG_DECODERS 1
/* Use libavcodec's encoders */
#define CONFIG_ENCODERS 1

/* XAnim DLL support */
#undef USE_XANIM
/* Default search path */
#undef XACODEC_PATH

/* Use 3dnow/mmxext/sse/mmx optimized fast memcpy() [maybe buggy... signal 4]*/
#define USE_FASTMEMCPY 1

/* gui support, please do not edit this option */
#undef HAVE_NEW_GUI

/* Audio output drivers */
#define USE_OSS_AUDIO 1
#define PATH_DEV_DSP "/dev/dsp"
#define PATH_DEV_MIXER "/dev/mixer"
#undef HAVE_ALSA5
#undef HAVE_ALSA9
#undef USE_SUN_AUDIO
#undef USE_SGI_AUDIO
#undef HAVE_NAS

/* Enable fast OSD/SUB renderer (looks ugly, but uses less CPU power) */
#undef FAST_OSD
#undef FAST_OSD_TABLE

/* Enable TV Interface support */
#define USE_TV 1

/* Enable Video 4 Linux TV interface support */
#undef HAVE_TV_V4L

/* Enable *BSD BrookTree TV interface support */
#undef HAVE_TV_BSDBT848

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#undef WORDS_BIGENDIAN

#define ARCH_X86 1

/* Define this for Cygwin build for win32 */
#define WIN32

/* Define this to any prefered value from 386 up to infinity with step 100 */
#define __CPU__ 686

#undef TARGET_LINUX

#undef HAVE_VCD

#ifdef sun
#define DEFAULT_CDROM_DEVICE    "/vol/dev/aliases/cdrom0"
#define DEFAULT_DVD_DEVICE      DEFAULT_CDROM_DEVICE
#else
#define DEFAULT_CDROM_DEVICE    "/dev/cdrom"
#define DEFAULT_DVD_DEVICE      "/dev/dvd"
#endif


/*----------------------------------------------------------------------------
**
** NOTE: Instead of modifying these definitions here, use the
**       --enable/--disable options of the ./configure script!
**       See ./configure --help for details.
**
*---------------------------------------------------------------------------*/

/* nanosleep support */
#undef HAVE_NANOSLEEP

/* termcap flag for getch2.c */
#define USE_TERMCAP 1

/* termios flag for getch2.c */
#define HAVE_TERMIOS 1
#undef HAVE_TERMIOS_H
#define HAVE_SYS_TERMIOS_H 1

/* enable PNG support */
#define HAVE_PNG 1

/* enable JPEG support */
#undef HAVE_JPEG

/* libmad support */
#undef USE_LIBMAD

/* enable OggVorbis support */
#define HAVE_OGGVORBIS 1

/* enable FAAD (AAC) support */
#undef HAVE_FAAD

/* enable streaming */
#undef STREAMING

/* enables / disables vidix usage */
#define CONFIG_VIDIX 1

/* enables / disables new input */
#define HAVE_NEW_INPUT 1

/* enables / disables new input joystick support */
#undef HAVE_JOYSTICK

/* Extension defines */
#define HAVE_3DNOW 1    // only define if you have 3DNOW (AMD k6-2, AMD Athlon, iDT WinChip, etc.)
#define HAVE_3DNOWEX 1  // only define if you have 3DNOWEX (AMD Athlon, etc.)
#define HAVE_MMX 1      // only define if you have MMX (newer x86 chips, not P54C/PPro)
#define HAVE_MMX2 1     // only define if you have MMX2 (Athlon/PIII/4/CelII)
#define HAVE_SSE 1      // only define if you have SSE (Intel Pentium III/4 or Celeron II)
#define HAVE_SSE2 1     // only define if you have SSE2 (Intel Pentium 4)

#ifdef HAVE_MMX
#define USE_MMX_IDCT 1
#endif

#undef HAVE_MLIB  // Sun mediaLib, available only on solaris

/* libmpeg2 uses a different feature test macro for mediaLib */
#ifdef HAVE_MLIB
#define LIBMPEG2_MLIB 1
#endif

/* libvo options */
#define SCREEN_SIZE_X 1
#define SCREEN_SIZE_Y 1
#undef HAVE_X11
#undef HAVE_XV
#undef HAVE_XF86VM
#undef HAVE_XINERAMA
#undef HAVE_GL
#undef HAVE_DGA
#undef HAVE_DGA2
#undef HAVE_SDL
/* defined for SDLlib with keyrepeat bugs (before 1.2.1) */

#undef HAVE_GGI
#undef HAVE_3DFX
#undef HAVE_TDFXFB
#undef HAVE_DIRECTFB

#undef HAVE_ZR
#undef HAVE_MGA
#undef HAVE_XMGA
#undef HAVE_SYNCFB
#undef HAVE_FBDEV
#undef USE_CONVERT2FB
#undef HAVE_DXR3
#undef HAVE_DVB
#undef HAVE_SVGALIB
#undef HAVE_VESA
#undef HAVE_XDPMS
#undef HAVE_AA

/* used by GUI: */


#if defined(HAVE_GL) || defined(HAVE_X11) || defined(HAVE_XV)
#define X11_FULLSCREEN 1
#endif

#undef __ENABLE_XP

#ifdef __ENABLE_XP
#define __ENABLE_DEC_AHEAD 1
#endif

#define printf(...) /**/