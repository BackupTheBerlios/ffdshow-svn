include config.mak

CFLAGS= $(OPTFLAGS) -Wall -DHAVE_AV_CONFIG_H \
-pipe -mno-cygwin -I/usr/local/include -I. -I.. -I../..
       
LDFLAGS= 

SRCDIR=libavcodec

OBJS= $(SRCDIR)/allcodecs.o $(SRCDIR)/mem.o $(SRCDIR)/common.o $(SRCDIR)/utils.o $(SRCDIR)/mpegvideo.o $(SRCDIR)/h263.o \
      $(SRCDIR)/dsputil.o \
      $(SRCDIR)/msmpeg4.o \
      $(SRCDIR)/mpeg12.o $(SRCDIR)/h263dec.o $(SRCDIR)/rv10.o $(SRCDIR)/jrevdct.o $(SRCDIR)/simple_idct.o \
      $(SRCDIR)/my_utils.o
ASM_OBJS=

ifeq ($(TARGET_GPROF),yes)
CFLAGS+=-p
LDFLAGS+=-p
endif

# i386 mmx specific stuff
ifeq ($(TARGET_MMX),yes)
OBJS += $(SRCDIR)/i386/cputest.o \
	$(SRCDIR)/i386/dsputil_mmx.o $(SRCDIR)/i386/mpegvideo_mmx.o \
        $(SRCDIR)/i386/idct_mmx.o $(SRCDIR)/i386/motion_est_mmx.o \
	$(SRCDIR)/i386/simple_idct_mmx.o
endif

SRCS = $(OBJS:.o=.c) $(ASM_OBJS:.o=.s)

SLIB= libavcodec.dll

all: $(SLIB)

$(SLIB): $(OBJS) $(ASM_OBJS)
	rm -f $@
#	$(CC) -mno-cygwin -shared -o $@ $(OBJS) $(ASM_OBJS) -lmsvcrt -lcrtdll -L/usr/lib/mingw 
# OK	dllwrap -mno-cygwin --target=i386-mingw32 --export-all-symbols -o $@ $(OBJS) $(ASM_OBJS) -lmsvcrt -lcrtdll -L/usr/lib/mingw
	dllwrap -mno-cygwin --target=i386-mingw32 --def libavcodec.def -o $@ $(OBJS) $(ASM_OBJS) -lmsvcrt -lcrtdll -L/usr/lib/mingw
#	strip $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f $(SRCDIR)/*.o *~ $(SLIB) $(SRCDIR)/i386/*.o $(SRCDIR)/i386/*~

distclean: clean
	rm -f Makefile.bak .depend

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
