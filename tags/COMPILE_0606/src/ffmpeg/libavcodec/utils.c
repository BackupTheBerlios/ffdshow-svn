/*
 * utils for libavcodec
 * Copyright (c) 2001 Gerard Lantau.
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
#include "avcodec.h"
#include "dsputil.h"
#include "mpegvideo.h"
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

static void *xvid_malloc(size_t size, uint8_t alignment)
{
	uint8_t *mem_ptr;
  
	if(!alignment)
	{

		/* We have not to satisfy any alignment */
		if((mem_ptr = (uint8_t *) malloc(size + 1)) != NULL)
		{

			/* Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) */
			*mem_ptr = 0;

			/* Return the mem_ptr pointer */
			return (void *) mem_ptr++;

		}

	}
	else
	{
		uint8_t *tmp;
	
		/*
		 * Allocate the required size memory + alignment so we
		 * can realign the data if necessary
		 */

		if((tmp = (uint8_t *) malloc(size + alignment)) != NULL)
		{

			/* Align the tmp pointer */
			mem_ptr = (uint8_t *)((uint32_t)(tmp + alignment - 1)&(~(uint32_t)(alignment - 1)));

			/*
			 * Special case where malloc have already satisfied the alignment
			 * We must add alignment to mem_ptr because we must store
			 * (mem_ptr - tmp) in *(mem_ptr-1)
			 * If we do not add alignment to mem_ptr then *(mem_ptr-1) points
			 * to a forbidden memory space
			 */
			if(mem_ptr == tmp) mem_ptr += alignment;

			/*
			 * (mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve
			 * the real malloc block allocated and free it in xvid_free
			 */
			*(mem_ptr - 1) = (uint8_t)(mem_ptr - tmp);

			/* Return the aligned pointer */
			return (void *) mem_ptr;

		}
	}

	return NULL;

}
/* memory alloc */
void *av_malloc(int size)
{
    void *ptr;
#if defined ( ARCH_X86 ) && defined ( HAVE_MEMALIGN )
    ptr = memalign(64,size);
    /* Why 64? 
       Indeed, we should align it:
         on 4 for 386
         on 16 for 486
	 on 32 for 586, PPro - k6-III
	 on 64 for K7 (maybe for P3 too).
       Because L1 and L2 caches are aligned on those values.
       But I don't want to code such logic here!
     */
#else
    ptr = xvid_malloc(size,64);
#endif
    if (!ptr)
        return NULL;
    memset(ptr, 0, size);
    return ptr;
}

void *av_mallocz(int size)
{
    void *ptr;
    ptr = av_malloc(size);
    if (!ptr)
        return NULL;
    memset(ptr, 0, size);
    return ptr;
}

static void xvid_free(void *mem_ptr)
{
 /* *(mem_ptr - 1) give us the offset to the real malloc block */
 free((uint8_t*)mem_ptr - *((uint8_t*)mem_ptr - 1));
}

/* NOTE: ptr = NULL is explicetly allowed */
void av_free(void *ptr)
{
    /* XXX: this test should not be needed on most libcs */
    if (ptr)
        xvid_free(ptr);
}

/* cannot call it directly because of 'void **' casting is not automatic */
void __av_freep(void **ptr)
{
    av_free(*ptr);
    *ptr = NULL;
}

/* encoder management */
AVCodec *first_avcodec;

void register_avcodec(AVCodec *format)
{
    AVCodec **p;
    p = &first_avcodec;
    while (*p != NULL) p = &(*p)->next;
    *p = format;
    format->next = NULL;
}

int avcodec_open(AVCodecContext *avctx, AVCodec *codec)
{
    int ret;

    avctx->codec = codec;
    avctx->frame_number = 0;
    if (codec->priv_data_size > 0) {
    avctx->priv_data = av_mallocz(codec->priv_data_size);
    if (!avctx->priv_data) 
        return -ENOMEM;
    } else {
        avctx->priv_data = NULL;
    }
    ret = avctx->codec->init(avctx);
    if (ret < 0) {
        av_freep(&avctx->priv_data);
        return ret;
    }
    return 0;
}

int avcodec_encode_audio(AVCodecContext *avctx, UINT8 *buf, int buf_size, 
                         const short *samples)
{
    int ret;

    ret = avctx->codec->encode(avctx, buf, buf_size, (void *)samples);
    avctx->frame_number++;
    return ret;
}

int avcodec_encode_video(AVCodecContext *avctx, UINT8 *buf, int buf_size, 
                         const AVPicture *pict)
{
    int ret;

    ret = avctx->codec->encode(avctx, buf, buf_size, (void *)pict);
    avctx->frame_number++;
    return ret;
}

/* decode a frame. return -1 if error, otherwise return the number of
   bytes used. If no frame could be decompressed, *got_picture_ptr is
   zero. Otherwise, it is non zero */
int avcodec_decode_video(AVCodecContext *avctx, AVPicture *picture, 
                         int *got_picture_ptr,
                         UINT8 *buf, int buf_size)
{
    int ret;

    ret = avctx->codec->decode(avctx, picture, got_picture_ptr, 
                               buf, buf_size);
    if (*got_picture_ptr)                           
        avctx->frame_number++;
    return ret;
}

/* decode an audio frame. return -1 if error, otherwise return the
   *number of bytes used. If no frame could be decompressed,
   *frame_size_ptr is zero. Otherwise, it is the decompressed frame
   *size in BYTES. */
int avcodec_decode_audio(AVCodecContext *avctx, INT16 *samples, 
                         int *frame_size_ptr,
                         UINT8 *buf, int buf_size)
{
    int ret;

    ret = avctx->codec->decode(avctx, samples, frame_size_ptr, 
                               buf, buf_size);
    avctx->frame_number++;
    return ret;
}

int avcodec_close(AVCodecContext *avctx)
{
    if (avctx->codec->close)
        avctx->codec->close(avctx);
    av_freep(&avctx->priv_data);
    avctx->codec = NULL;
    return 0;
}

AVCodec *avcodec_find_decoder(int id)
{
    AVCodec *p;
    p = first_avcodec;
    while (p) {
        if (p->decode != NULL && p->id == id)
            return p;
        p = p->next;
    }
    return NULL;
}

AVCodec *avcodec_find_decoder_by_name(const char *name)
{
    AVCodec *p;
    p = first_avcodec;
    while (p) {
        if (p->decode != NULL && strcmp(name,p->name) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}

AVCodec *avcodec_find(int id)
{
    AVCodec *p;
    p = first_avcodec;
    while (p) {
        if (p->id == id)
            return p;
        p = p->next;
    }
    return NULL;
}

const char *pix_fmt_str[] = {
    "??",
    "yuv420p",
    "yuv422",
    "rgb24",
    "bgr24",
    "yuv422p",
    "yuv444p",
};
    
void avcodec_string(char *buf, int buf_size, AVCodecContext *enc, int encode)
{
    const char *codec_name;
    AVCodec *p;
    char buf1[32];
    char channels_str[100];
    int bitrate;

    p = avcodec_find_decoder(enc->codec_id);

    if (p) {
        codec_name = p->name;
    } else if (enc->codec_name[0] != '\0') {
        codec_name = enc->codec_name;
    } else {
        /* output avi tags */
        if (enc->codec_type == CODEC_TYPE_VIDEO) {
            snprintf(buf1, sizeof(buf1), "%c%c%c%c", 
                     enc->codec_tag & 0xff,
                     (enc->codec_tag >> 8) & 0xff,
                     (enc->codec_tag >> 16) & 0xff,
                     (enc->codec_tag >> 24) & 0xff);
        } else {
            snprintf(buf1, sizeof(buf1), "0x%04x", enc->codec_tag);
        }
        codec_name = buf1;
    }

    switch(enc->codec_type) {
    case CODEC_TYPE_VIDEO:
        snprintf(buf, buf_size,
                 "Video: %s%s",
                 codec_name, enc->flags & CODEC_FLAG_HQ ? " (hq)" : "");
        if (enc->codec_id == CODEC_ID_RAWVIDEO) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %s",
                     pix_fmt_str[enc->pix_fmt]);
        }
        if (enc->width) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %dx%d, %0.2f fps",
                     enc->width, enc->height, 
                     (float)enc->frame_rate / FRAME_RATE_BASE);
        }
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                ", q=%d-%d", enc->qmin, enc->qmax);

        bitrate = enc->bit_rate;
        break;
    default:
        abort();
    }
    if (bitrate != 0) {
        snprintf(buf + strlen(buf), buf_size - strlen(buf), 
                 ", %d kb/s", bitrate / 1000);
    }
}

/* Picture field are filled with 'ptr' addresses */
void avpicture_fill(AVPicture *picture, UINT8 *ptr,
                    int pix_fmt, int width, int height)
{
    int size;

    size = width * height;
    switch(pix_fmt) {
    case PIX_FMT_YUV420P:
        picture->data[0] = ptr;
        picture->data[1] = picture->data[0] + size;
        picture->data[2] = picture->data[1] + size / 4;
        picture->linesize[0] = width;
        picture->linesize[1] = width / 2;
        picture->linesize[2] = width / 2;
        break;
    case PIX_FMT_YUV422P:
        picture->data[0] = ptr;
        picture->data[1] = picture->data[0] + size;
        picture->data[2] = picture->data[1] + size / 2;
        picture->linesize[0] = width;
        picture->linesize[1] = width / 2;
        picture->linesize[2] = width / 2;
        break;
    case PIX_FMT_YUV444P:
        picture->data[0] = ptr;
        picture->data[1] = picture->data[0] + size;
        picture->data[2] = picture->data[1] + size;
        picture->linesize[0] = width;
        picture->linesize[1] = width;
        picture->linesize[2] = width;
        break;
    case PIX_FMT_RGB24:
    case PIX_FMT_BGR24:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width * 3;
        break;
    case PIX_FMT_YUV422:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width * 2;
        break;
    default:
        picture->data[0] = NULL;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        break;
    }
}

int avpicture_get_size(int pix_fmt, int width, int height)
{
    int size;

    size = width * height;
    switch(pix_fmt) {
    case PIX_FMT_YUV420P:
        size = (size * 3) / 2;
        break;
    case PIX_FMT_YUV422P:
        size = (size * 2);
        break;
    case PIX_FMT_YUV444P:
        size = (size * 3);
        break;
    case PIX_FMT_RGB24:
    case PIX_FMT_BGR24:
        size = (size * 3);
        break;
    case PIX_FMT_YUV422:
        size = (size * 2);
        break;
    default:
        size = -1;
        break;
    }
    return size;
}

unsigned avcodec_version( void )
{
  return LIBAVCODEC_VERSION_INT;
}

unsigned avcodec_build( void )
{
  return LIBAVCODEC_BUILD;
}

/* must be called before any other functions */
void avcodec_init(void)
{
    static int inited = 0;

    if (inited != 0)
	return;
    inited = 1;

    dsputil_init(1,1);
}

/* simple call to use all the codecs */
void avcodec_register_all(void)
{
    static int inited = 0;
    
    if (inited != 0)
	return;
    inited = 1;

    register_avcodec(&h263_decoder);
    register_avcodec(&mpeg4_decoder);
    register_avcodec(&msmpeg4v1_decoder);
    register_avcodec(&msmpeg4v2_decoder);
    register_avcodec(&msmpeg4v3_decoder);
    register_avcodec(&wmv1_decoder);
    register_avcodec(&mpeg_decoder);
    register_avcodec(&h263i_decoder);
}

/* this should be called after seeking and before trying to decode the next frame */
void avcodec_flush_buffers(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;
    s->num_available_buffers=0;
}


static int encode_init(AVCodecContext *s)
{
    return 0;
}

static int decode_frame(AVCodecContext *avctx, 
                        void *data, int *data_size,
                        UINT8 *buf, int buf_size)
{
    return -1;
}

static int encode_frame(AVCodecContext *avctx,
                        unsigned char *frame, int buf_size, void *data)
{
    return -1;
}

AVCodec rawvideo_codec = {
    "rawvideo",
    CODEC_TYPE_VIDEO,
    CODEC_ID_RAWVIDEO,
    0,
    encode_init,
    encode_frame,
    NULL,
    decode_frame,
};
