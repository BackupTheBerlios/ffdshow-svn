include ../config.mak

CFLAGS= $(OPTFLAGS) -Wall -DHAVE_AV_CONFIG_H \
-pipe -mno-cygwin -I/usr/local/include -I. -I.. 
       
LDFLAGS= 

OBJS= common.o utils.o mpegvideo.o h263.o \
      dsputil.o \
      msmpeg4.o \
      mpeg12.o h263dec.o rv10.o jrevdct.o simple_idct.o \
      my_utils.o
ASM_OBJS=

ifeq ($(TARGET_GPROF),yes)
CFLAGS+=-p
LDFLAGS+=-p
endif

# i386 mmx specific stuff
ifeq ($(TARGET_MMX),yes)
OBJS += i386/cputest.o \
	i386/dsputil_mmx.o i386/mpegvideo_mmx.o \
        i386/idct_mmx.o i386/motion_est_mmx.o \
	i386/simple_idct_mmx16383.o i386/simple_idct_mmx16384.o
endif

SRCS = $(OBJS:.o=.c) $(ASM_OBJS:.o=.s)

LIB= libavcodec.lib
ifeq ($(BUILD_SHARED),yes)
SLIB= libavcodec.dll
endif
TESTS= imgresample-test dct-test motion-test

all: $(SLIB)
tests: apiexample cpuid_test $(TESTS)

$(SLIB): $(OBJS) $(ASM_OBJS)
	rm -f $@
#	$(CC) -mno-cygwin -shared -o $@ $(OBJS) $(ASM_OBJS) -lmsvcrt -lcrtdll -L/usr/lib/mingw 
# OK	dllwrap -mno-cygwin --target=i386-mingw32 --export-all-symbols -o $@ $(OBJS) $(ASM_OBJS) -lmsvcrt -lcrtdll -L/usr/lib/mingw 
	dllwrap -mno-cygwin --target=i386-mingw32 --def libavcodec.def -o $@ $(OBJS) $(ASM_OBJS) -lmsvcrt -lcrtdll -L/usr/lib/mingw 
#	strip $@

dsputil.o: dsputil.c dsputil.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

# depend only used by mplayer now
dep:	depend

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend

clean: 
	rm -f *.o *~ $(LIB) $(SLIB) *.so i386/*.o i386/*~ \
	   armv4l/*.o armv4l/*~ \
	   mlib/*.o mlib/*~ \
	   alpha/*.o alpha/*~ \
           libac3/*.o libac3/*~ \
           apiexample $(TESTS)

distclean: clean
	rm -f Makefile.bak .depend

# api example program
apiexample: apiexample.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $< $(LIB) -lm

# cpuid test
cpuid_test: i386/cputest.c
	$(CC) $(CFLAGS) -D__TEST__ -o $@ $<

# testing progs

imgresample-test: imgresample.c
	$(CC) $(CFLAGS) -DTEST -o $@ $^ 

dct-test: dct-test.o jfdctfst.o i386/fdct_mmx.o \
          fdctref.o jrevdct.o i386/idct_mmx.o
	$(CC) -o $@ $^

motion-test: motion_test.o $(LIB)
	$(CC) -o $@ $^

install: all
#	install -m 644 $(LIB) $(prefix)/lib
ifeq ($(BUILD_SHARED),yes)
	install -s -m 755 $(SLIB) $(prefix)/lib
	ln -sf $(prefix)/lib/$(SLIB) $(prefix)/lib/libffmpeg.so
	ldconfig
	mkdir -p $(prefix)/include/libffmpeg
	install -m 644 avcodec.h $(prefix)/include/libffmpeg/avcodec.h
	install -m 644 common.h $(prefix)/include/libffmpeg/common.h
endif
#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
