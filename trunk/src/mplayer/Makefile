
include config.mak

LIBNAME= libpostproc.lib
SOLIBNAME = libpostproc.dll

SRCS=   \
	postproc/postprocess.c postproc/swscale.c postproc/rgb2rgb.c postproc/yuv2rgb.c \
	mplayer.c cpudetect.c mem_align.c
OBJS=$(SRCS:.c=.o)

CFLAGS  = -Wall -Dsnprintf=_snprintf -mno-cygwin $(OPTFLAGS) $(MLIB_INC) -I. -I.. -Wall $(EXTRA_INC)

.SUFFIXES: .c .o

# .PHONY: all clean

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

#$(LIBNAME): $(OBJS)
#	rm -f $@
#	$(AR) rcs $@ $(OBJS)

$(SOLIBNAME):  $(OBJS)
	dllwrap -mno-cygwin --target=i386-mingw32 --def libpostproc.def -o $(SOLIBNAME) $(OBJS) -L/usr/lib/mingw -lcrtdll -lmsvcrt


all: $(LIBNAME)

clean:
	rm -f *.o postproc/*.o libmpcodecs/*.o *.dll *.lib *~

distclean:
	rm -f Makefile.bak *.o *.dll *.lib *~ .depend

