/*
 * The simplest mpeg encoder (well, it was the simplest!)
 * Copyright (c) 2000,2001 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 4MV & hq & b-frame encoding stuff by Michael Niedermayer <michaelni@gmx.at>
 */
#include "avcodec.h"
#include "dsputil.h"
#include "mpegvideo.h"

#ifdef USE_FASTMEMCPY
#include "fastmemcpy.h"
#endif

static void dct_unquantize_mpeg1_c(MpegEncContext *s, 
                                   DCTELEM *block, int n, int qscale);
static void dct_unquantize_mpeg2_c(MpegEncContext *s,
                                   DCTELEM *block, int n, int qscale);
static void dct_unquantize_h263_c(MpegEncContext *s, 
                                  DCTELEM *block, int n, int qscale);
static void draw_edges_c(UINT8 *buf, int wrap, int width, int height, int w);

void (*draw_edges)(UINT8 *buf, int wrap, int width, int height, int w)= draw_edges_c;

#define EDGE_WIDTH 16

/* enable all paranoid tests for rounding, overflows, etc... */
//#define PARANOID

//#define DEBUG


/* for jpeg fast DCT */
#define CONST_BITS 14

static const unsigned short aanscales[64] = {
    /* precomputed values scaled up by 14 bits */
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
    21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
    19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
    8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
    4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
};

static UINT8 h263_chroma_roundtab[16] = {
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
};

extern UINT8 zigzag_end[64];

/* default motion estimation */
int motion_estimation_method = ME_EPZS;

// move into common.c perhaps 
#define CHECKED_ALLOCZ(p, size)\
{\
    p= av_mallocz(size);\
    if(p==NULL){\
        perror("malloc");\
        goto fail;\
    }\
}

/* init common structure for both encoder and decoder */
int MPV_common_init(MpegEncContext *s)
{
    int c_size, i;
    UINT8 *pict;

    s->dct_unquantize_h263 = dct_unquantize_h263_c;
    s->dct_unquantize_mpeg1 = dct_unquantize_mpeg1_c;
    s->dct_unquantize_mpeg2 = dct_unquantize_mpeg2_c;
        
#ifdef HAVE_MMX
    MPV_common_init_mmx(s);
#endif
    //setup default unquantizers (mpeg4 might change it later)
    if(s->out_format == FMT_H263)
        s->dct_unquantize = s->dct_unquantize_h263;
    else
        s->dct_unquantize = s->dct_unquantize_mpeg1;
    
    s->mb_width = (s->width + 15) / 16;
    s->mb_height = (s->height + 15) / 16;
    s->mb_num = s->mb_width * s->mb_height;
    s->linesize = s->mb_width * 16 + 2 * EDGE_WIDTH;

    for(i=0;i<3;i++) {
        int w, h, shift, pict_start;

        w = s->linesize;
        h = s->mb_height * 16 + 2 * EDGE_WIDTH;
        shift = (i == 0) ? 0 : 1;
        c_size = (w >> shift) * (h >> shift);
        pict_start = (w >> shift) * (EDGE_WIDTH >> shift) + (EDGE_WIDTH >> shift);

        CHECKED_ALLOCZ(pict, c_size)
        s->last_picture_base[i] = pict;
        s->last_picture[i] = pict + pict_start;
        if(i>0) memset(s->last_picture_base[i], 128, c_size);
    
        CHECKED_ALLOCZ(pict, c_size)
        s->next_picture_base[i] = pict;
        s->next_picture[i] = pict + pict_start;
        if(i>0) memset(s->next_picture_base[i], 128, c_size);

        if (s->has_b_frames || s->codec_id==CODEC_ID_MPEG4) {
        /* Note the MPEG4 stuff is here cuz of buggy encoders which dont set the low_delay flag but 
           do low-delay encoding, so we cant allways distinguish b-frame containing streams from low_delay streams */
            CHECKED_ALLOCZ(pict, c_size)
            s->aux_picture_base[i] = pict;
            s->aux_picture[i] = pict + pict_start;
            if(i>0) memset(s->aux_picture_base[i], 128, c_size);
        }
    }
    
    if (s->encoding) {
        int j;
        int mv_table_size= (s->mb_width+2)*(s->mb_height+2);

        CHECKED_ALLOCZ(s->mb_var   , s->mb_num * sizeof(INT16))
        CHECKED_ALLOCZ(s->mc_mb_var, s->mb_num * sizeof(INT16))
        
        /* Allocate MV tables */
        CHECKED_ALLOCZ(s->p_mv_table            , mv_table_size * 2 * sizeof(INT16))
        CHECKED_ALLOCZ(s->b_forw_mv_table       , mv_table_size * 2 * sizeof(INT16))
        CHECKED_ALLOCZ(s->b_back_mv_table       , mv_table_size * 2 * sizeof(INT16))
        CHECKED_ALLOCZ(s->b_bidir_forw_mv_table , mv_table_size * 2 * sizeof(INT16))
        CHECKED_ALLOCZ(s->b_bidir_back_mv_table , mv_table_size * 2 * sizeof(INT16))
        CHECKED_ALLOCZ(s->b_direct_forw_mv_table, mv_table_size * 2 * sizeof(INT16))
        CHECKED_ALLOCZ(s->b_direct_back_mv_table, mv_table_size * 2 * sizeof(INT16))
        CHECKED_ALLOCZ(s->b_direct_mv_table     , mv_table_size * 2 * sizeof(INT16))

        CHECKED_ALLOCZ(s->me_scratchpad,  s->linesize*16*3*sizeof(uint8_t))

        CHECKED_ALLOCZ(s->me_map      , ME_MAP_SIZE*sizeof(uint32_t))
        CHECKED_ALLOCZ(s->me_score_map, ME_MAP_SIZE*sizeof(uint16_t))

        if(s->max_b_frames){
            for(j=0; j<REORDER_BUFFER_SIZE; j++){
                int i;
                for(i=0;i<3;i++) {
                    int w, h, shift;

                    w = s->linesize;
                    h = s->mb_height * 16;
                    shift = (i == 0) ? 0 : 1;
                    c_size = (w >> shift) * (h >> shift);

                    CHECKED_ALLOCZ(pict, c_size);
                    s->picture_buffer[j][i] = pict;
                }
            }
        }

        if(s->codec_id==CODEC_ID_MPEG4){
            CHECKED_ALLOCZ(s->tex_pb_buffer, PB_BUFFER_SIZE);
            CHECKED_ALLOCZ(   s->pb2_buffer, PB_BUFFER_SIZE);
        }
    }
    
    if (s->out_format == FMT_H263 || s->encoding) {
        int size;
        /* Allocate MB type table */
        CHECKED_ALLOCZ(s->mb_type  , s->mb_num * sizeof(UINT8))

        /* MV prediction */
        size = (2 * s->mb_width + 2) * (2 * s->mb_height + 2);
        CHECKED_ALLOCZ(s->motion_val, size * 2 * sizeof(INT16));
    }

    if (s->h263_pred || s->h263_plus) {
        int y_size, c_size, i, size;
        
        /* dc values */

        y_size = (2 * s->mb_width + 2) * (2 * s->mb_height + 2);
        c_size = (s->mb_width + 2) * (s->mb_height + 2);
        size = y_size + 2 * c_size;
        CHECKED_ALLOCZ(s->dc_val[0], size * sizeof(INT16));
        s->dc_val[1] = s->dc_val[0] + y_size;
        s->dc_val[2] = s->dc_val[1] + c_size;
        for(i=0;i<size;i++)
            s->dc_val[0][i] = 1024;

        /* ac values */
        CHECKED_ALLOCZ(s->ac_val[0], size * sizeof(INT16) * 16);
        s->ac_val[1] = s->ac_val[0] + y_size;
        s->ac_val[2] = s->ac_val[1] + c_size;
        
        /* cbp values */
        CHECKED_ALLOCZ(s->coded_block, y_size);

        /* which mb is a intra block */
        CHECKED_ALLOCZ(s->mbintra_table, s->mb_num);
        memset(s->mbintra_table, 1, s->mb_num);
        
        /* divx501 bitstream reorder buffer */
        CHECKED_ALLOCZ(s->bitstream_buffer, BITSTREAM_BUFFER_SIZE);
        
        /* cbp, ac_pred, pred_dir */
        CHECKED_ALLOCZ(s->cbp_table  , s->mb_num * sizeof(UINT8))
        CHECKED_ALLOCZ(s->pred_dir_table, s->mb_num * sizeof(UINT8))
        
        CHECKED_ALLOCZ(s->qscale_table  , s->mb_num * sizeof(UINT8))
    }
    /* default structure is frame */
    s->picture_structure = PICT_FRAME;

    /* init macroblock skip table */
    CHECKED_ALLOCZ(s->mbskip_table, s->mb_num);
    
    s->block= s->blocks[0];

    s->context_initialized = 1;
    return 0;
 fail:
    MPV_common_end(s);
    return -1;
}


//extern int sads;

/* init common structure for both encoder and decoder */
void MPV_common_end(MpegEncContext *s)
{
    int i;

    av_freep(&s->mb_type);
    av_freep(&s->mb_var);
    av_freep(&s->mc_mb_var);
    av_freep(&s->p_mv_table);
    av_freep(&s->b_forw_mv_table);
    av_freep(&s->b_back_mv_table);
    av_freep(&s->b_bidir_forw_mv_table);
    av_freep(&s->b_bidir_back_mv_table);
    av_freep(&s->b_direct_forw_mv_table);
    av_freep(&s->b_direct_back_mv_table);
    av_freep(&s->b_direct_mv_table);
    av_freep(&s->motion_val);
    av_freep(&s->dc_val[0]);
    av_freep(&s->ac_val[0]);
    av_freep(&s->coded_block);
    av_freep(&s->mbintra_table);
    av_freep(&s->cbp_table);
    av_freep(&s->pred_dir_table);
    av_freep(&s->qscale_table);
    av_freep(&s->me_scratchpad);
    av_freep(&s->me_map);
    av_freep(&s->me_score_map);

    av_freep(&s->mbskip_table);
    av_freep(&s->bitstream_buffer);
    av_freep(&s->tex_pb_buffer);
    av_freep(&s->pb2_buffer);
    for(i=0;i<3;i++) {
        int j;
        av_freep(&s->last_picture_base[i]);
        av_freep(&s->next_picture_base[i]);
        av_freep(&s->aux_picture_base[i]);
        for(j=0; j<REORDER_BUFFER_SIZE; j++){
            av_freep(&s->picture_buffer[j][i]);
        }
    }
    s->context_initialized = 0;
}

/* draw the edges of width 'w' of an image of size width, height */
static void draw_edges_c(UINT8 *buf, int wrap, int width, int height, int w)
{
    UINT8 *ptr, *last_line;
    int i;

    last_line = buf + (height - 1) * wrap;
    for(i=0;i<w;i++) {
        /* top and bottom */
        memcpy(buf - (i + 1) * wrap, buf, width);
        memcpy(last_line + (i + 1) * wrap, last_line, width);
    }
    /* left and right */
    ptr = buf;
    for(i=0;i<height;i++) {
        memset(ptr - w, ptr[0], w);
        memset(ptr + width, ptr[width-1], w);
        ptr += wrap;
    }
    /* corners */
    for(i=0;i<w;i++) {
        memset(buf - (i + 1) * wrap - w, buf[0], w); /* top left */
        memset(buf - (i + 1) * wrap + width, buf[width-1], w); /* top right */
        memset(last_line + (i + 1) * wrap - w, last_line[0], w); /* top left */
        memset(last_line + (i + 1) * wrap + width, last_line[width-1], w); /* top right */
    }
}

/* generic function for encode/decode called before a frame is coded/decoded */
void MPV_frame_start(MpegEncContext *s)
{
    int i;
    UINT8 *tmp;

    s->mb_skiped = 0;
    s->decoding_error=0;

    if (s->pict_type == B_TYPE) {
        for(i=0;i<3;i++) {
            s->current_picture[i] = s->aux_picture[i];
        }
    } else {
        for(i=0;i<3;i++) {
            /* swap next and last */
            tmp = s->last_picture[i];
            s->last_picture[i] = s->next_picture[i];
            s->next_picture[i] = tmp;
            s->current_picture[i] = tmp;
        }
    }
}

/* generic function for encode/decode called after a frame has been coded/decoded */
void MPV_frame_end(MpegEncContext *s)
{
//    if((s->picture_number%100)==0 && s->encoding) printf("sads:%d //\n", sads);

    /* draw edge for correct motion prediction if outside */
    if (s->pict_type != B_TYPE && !s->intra_only) {
      if(s->avctx==NULL || s->avctx->codec->id!=CODEC_ID_MPEG4 || s->divx_version>=500){
        draw_edges(s->current_picture[0], s->linesize, s->mb_width*16, s->mb_height*16, EDGE_WIDTH);
        draw_edges(s->current_picture[1], s->linesize/2, s->mb_width*8, s->mb_height*8, EDGE_WIDTH/2);
        draw_edges(s->current_picture[2], s->linesize/2, s->mb_width*8, s->mb_height*8, EDGE_WIDTH/2);
      }else{
        /* mpeg4? / opendivx / xvid */
        draw_edges(s->current_picture[0], s->linesize, s->width, s->height, EDGE_WIDTH);
        draw_edges(s->current_picture[1], s->linesize/2, s->width/2, s->height/2, EDGE_WIDTH/2);
        draw_edges(s->current_picture[2], s->linesize/2, s->width/2, s->height/2, EDGE_WIDTH/2);
      }
    }
    emms_c();
    
    if(s->pict_type!=B_TYPE){
        s->last_non_b_pict_type= s->pict_type;
        s->last_non_b_qscale= s->qscale;
        s->last_non_b_mc_mb_var= s->mc_mb_var_sum;
        s->num_available_buffers++;
        if(s->num_available_buffers>2) s->num_available_buffers= 2;
}
}

/* reorder input for encoding */
void reorder_input(MpegEncContext *s, AVPicture *pict)
{
    int i, j, index;
            
    if(s->max_b_frames > FF_MAX_B_FRAMES) s->max_b_frames= FF_MAX_B_FRAMES;

//        delay= s->max_b_frames+1; (or 0 if no b frames cuz decoder diff)

    for(j=0; j<REORDER_BUFFER_SIZE-1; j++){
        s->coded_order[j]= s->coded_order[j+1];
    }
    s->coded_order[j].picture[0]= s->coded_order[j].picture[1]= s->coded_order[j].picture[2]= NULL; //catch uninitalized buffers
    s->coded_order[j].pict_type=0;

    switch(s->input_pict_type){
    default: 
    case I_TYPE:
    case S_TYPE:
    case P_TYPE:
        index= s->max_b_frames - s->b_frames_since_non_b;
        s->b_frames_since_non_b=0;
        break;            
    case B_TYPE:
        index= s->max_b_frames + 1;
        s->b_frames_since_non_b++;
        break;          
    }
//printf("index:%d type:%d strides: %d %d\n", index, s->input_pict_type, pict->linesize[0], s->linesize);
    if(   (index==0 || (s->flags&CODEC_FLAG_INPUT_PRESERVED))
       && pict->linesize[0] == s->linesize
       && pict->linesize[1] == s->linesize>>1
       && pict->linesize[2] == s->linesize>>1){
//printf("ptr\n");
        for(i=0; i<3; i++){
            s->coded_order[index].picture[i]= pict->data[i];
        }
    }else{
//printf("copy\n");
        for(i=0; i<3; i++){
            uint8_t *src = pict->data[i];
            uint8_t *dest;
            int src_wrap = pict->linesize[i];
            int dest_wrap = s->linesize;
            int w = s->width;
            int h = s->height;

            if(index==0) dest= s->last_picture[i]+16; //is current_picture indeed but the switch hapens after reordering
            else         dest= s->picture_buffer[s->picture_buffer_index][i];

            if (i >= 1) {
                dest_wrap >>= 1;
                w >>= 1;
                h >>= 1;
            }

            s->coded_order[index].picture[i]= dest;
            for(j=0;j<h;j++) {
                memcpy(dest, src, w);
                dest += dest_wrap;
                src += src_wrap;
            }
        }
        if(index!=0){
            s->picture_buffer_index++;
            if(s->picture_buffer_index >= REORDER_BUFFER_SIZE-1) s->picture_buffer_index=0;
        }
    }
    s->coded_order[index].pict_type = s->input_pict_type;
    s->coded_order[index].qscale    = s->input_qscale;
    s->coded_order[index].force_type= s->force_input_type;
    s->coded_order[index].picture_in_gop_number= s->input_picture_in_gop_number;
    s->coded_order[index].picture_number= s->input_picture_number;

    for(i=0; i<3; i++){
        s->new_picture[i]= s->coded_order[0].picture[i];
    }
}

static inline void gmc1_motion(MpegEncContext *s,
                               UINT8 *dest_y, UINT8 *dest_cb, UINT8 *dest_cr,
                               int dest_offset,
                               UINT8 **ref_picture, int src_offset,
                               int h)
{
    UINT8 *ptr;
    int offset, src_x, src_y, linesize;
    int motion_x, motion_y;

    if(s->real_sprite_warping_points>1) printf("more than 1 warp point isnt supported\n");
    motion_x= s->sprite_offset[0][0];
    motion_y= s->sprite_offset[0][1];
    src_x = s->mb_x * 16 + (motion_x >> (s->sprite_warping_accuracy+1));
    src_y = s->mb_y * 16 + (motion_y >> (s->sprite_warping_accuracy+1));
    motion_x<<=(3-s->sprite_warping_accuracy);
    motion_y<<=(3-s->sprite_warping_accuracy);
    src_x = clip(src_x, -16, s->width);
    if (src_x == s->width)
        motion_x =0;
    src_y = clip(src_y, -16, s->height);
    if (src_y == s->height)
        motion_y =0;
    
    linesize = s->linesize;
    ptr = ref_picture[0] + (src_y * linesize) + src_x + src_offset;

    dest_y+=dest_offset;
    gmc1(dest_y  , ptr  , linesize, h, motion_x&15, motion_y&15, s->no_rounding);
    gmc1(dest_y+8, ptr+8, linesize, h, motion_x&15, motion_y&15, s->no_rounding);

    motion_x= s->sprite_offset[1][0];
    motion_y= s->sprite_offset[1][1];
    src_x = s->mb_x * 8 + (motion_x >> (s->sprite_warping_accuracy+1));
    src_y = s->mb_y * 8 + (motion_y >> (s->sprite_warping_accuracy+1));
    motion_x<<=(3-s->sprite_warping_accuracy);
    motion_y<<=(3-s->sprite_warping_accuracy);
    src_x = clip(src_x, -8, s->width>>1);
    if (src_x == s->width>>1)
        motion_x =0;
    src_y = clip(src_y, -8, s->height>>1);
    if (src_y == s->height>>1)
        motion_y =0;

    offset = (src_y * linesize>>1) + src_x + (src_offset>>1);
    ptr = ref_picture[1] + offset;
    gmc1(dest_cb + (dest_offset>>1), ptr, linesize>>1, h>>1, motion_x&15, motion_y&15, s->no_rounding);
    ptr = ref_picture[2] + offset;
    gmc1(dest_cr + (dest_offset>>1), ptr, linesize>>1, h>>1, motion_x&15, motion_y&15, s->no_rounding);
    
    return;
}

/* apply one mpeg motion vector to the three components */
static inline void mpeg_motion(MpegEncContext *s,
                               UINT8 *dest_y, UINT8 *dest_cb, UINT8 *dest_cr,
                               int dest_offset,
                               UINT8 **ref_picture, int src_offset,
                               int field_based, op_pixels_func *pix_op,
                               int motion_x, int motion_y, int h)
{
    UINT8 *ptr;
    int dxy, offset, mx, my, src_x, src_y, height, linesize;
if(s->quarter_sample)
{
    motion_x>>=1;
    motion_y>>=1;
}
    dxy = ((motion_y & 1) << 1) | (motion_x & 1);
    src_x = s->mb_x * 16 + (motion_x >> 1);
    src_y = s->mb_y * (16 >> field_based) + (motion_y >> 1);
                
    /* WARNING: do no forget half pels */
    height = s->height >> field_based;
    src_x = clip(src_x, -16, s->width);
    if (src_x == s->width)
        dxy &= ~1;
    src_y = clip(src_y, -16, height);
    if (src_y == height)
        dxy &= ~2;
    linesize = s->linesize << field_based;
    ptr = ref_picture[0] + (src_y * linesize) + (src_x) + src_offset;
    dest_y += dest_offset;
    pix_op[dxy](dest_y, ptr, linesize, h);
    pix_op[dxy](dest_y + 8, ptr + 8, linesize, h);

    if(s->flags&CODEC_FLAG_GRAY) return;

    if (s->out_format == FMT_H263) {
        dxy = 0;
        if ((motion_x & 3) != 0)
            dxy |= 1;
        if ((motion_y & 3) != 0)
            dxy |= 2;
        mx = motion_x >> 2;
        my = motion_y >> 2;
    } else {
        mx = motion_x / 2;
        my = motion_y / 2;
        dxy = ((my & 1) << 1) | (mx & 1);
        mx >>= 1;
        my >>= 1;
    }
    
    src_x = s->mb_x * 8 + mx;
    src_y = s->mb_y * (8 >> field_based) + my;
    src_x = clip(src_x, -8, s->width >> 1);
    if (src_x == (s->width >> 1))
        dxy &= ~1;
    src_y = clip(src_y, -8, height >> 1);
    if (src_y == (height >> 1))
        dxy &= ~2;

    offset = (src_y * (linesize >> 1)) + src_x + (src_offset >> 1);
    ptr = ref_picture[1] + offset;
    pix_op[dxy](dest_cb + (dest_offset >> 1), ptr, linesize >> 1, h >> 1);
    ptr = ref_picture[2] + offset;
    pix_op[dxy](dest_cr + (dest_offset >> 1), ptr, linesize >> 1, h >> 1);
}

static inline void qpel_motion(MpegEncContext *s,
                               UINT8 *dest_y, UINT8 *dest_cb, UINT8 *dest_cr,
                               int dest_offset,
                               UINT8 **ref_picture, int src_offset,
                               int field_based, op_pixels_func *pix_op,
                               qpel_mc_func *qpix_op,
                               int motion_x, int motion_y, int h)
{
    UINT8 *ptr;
    int dxy, offset, mx, my, src_x, src_y, height, linesize;

    dxy = ((motion_y & 3) << 2) | (motion_x & 3);
    src_x = s->mb_x * 16 + (motion_x >> 2);
    src_y = s->mb_y * (16 >> field_based) + (motion_y >> 2);

    height = s->height >> field_based;
    src_x = clip(src_x, -16, s->width);
    if (src_x == s->width)
        dxy &= ~3;
    src_y = clip(src_y, -16, height);
    if (src_y == height)
        dxy &= ~12;
    linesize = s->linesize << field_based;
    ptr = ref_picture[0] + (src_y * linesize) + src_x + src_offset;
    dest_y += dest_offset;
//printf("%d %d %d\n", src_x, src_y, dxy);
    qpix_op[dxy](dest_y                 , ptr                 , linesize, linesize, motion_x&3, motion_y&3);
    qpix_op[dxy](dest_y              + 8, ptr              + 8, linesize, linesize, motion_x&3, motion_y&3);
    qpix_op[dxy](dest_y + linesize*8    , ptr + linesize*8    , linesize, linesize, motion_x&3, motion_y&3);
    qpix_op[dxy](dest_y + linesize*8 + 8, ptr + linesize*8 + 8, linesize, linesize, motion_x&3, motion_y&3);
    
    if(s->flags&CODEC_FLAG_GRAY) return;

    mx= (motion_x>>1) | (motion_x&1);
    my= (motion_y>>1) | (motion_y&1);

    dxy = 0;
    if ((mx & 3) != 0)
        dxy |= 1;
    if ((my & 3) != 0)
        dxy |= 2;
    mx = mx >> 2;
    my = my >> 2;
    
    src_x = s->mb_x * 8 + mx;
    src_y = s->mb_y * (8 >> field_based) + my;
    src_x = clip(src_x, -8, s->width >> 1);
    if (src_x == (s->width >> 1))
        dxy &= ~1;
    src_y = clip(src_y, -8, height >> 1);
    if (src_y == (height >> 1))
        dxy &= ~2;

    offset = (src_y * (linesize >> 1)) + src_x + (src_offset >> 1);
    ptr = ref_picture[1] + offset;
    pix_op[dxy](dest_cb + (dest_offset >> 1), ptr, linesize >> 1, h >> 1);
    ptr = ref_picture[2] + offset;
    pix_op[dxy](dest_cr + (dest_offset >> 1), ptr, linesize >> 1, h >> 1);
}


static inline void MPV_motion(MpegEncContext *s, 
                              UINT8 *dest_y, UINT8 *dest_cb, UINT8 *dest_cr,
                              int dir, UINT8 **ref_picture, 
                              op_pixels_func *pix_op, qpel_mc_func *qpix_op)
{
    int dxy, offset, mx, my, src_x, src_y, motion_x, motion_y;
    int mb_x, mb_y, i;
    UINT8 *ptr, *dest;

    mb_x = s->mb_x;
    mb_y = s->mb_y;

    switch(s->mv_type) {
    case MV_TYPE_16X16:
        if(s->mcsel){
#if 0
            mpeg_motion(s, dest_y, dest_cb, dest_cr, 0,
                        ref_picture, 0,
                        0, pix_op,
                        s->sprite_offset[0][0]>>3,
                        s->sprite_offset[0][1]>>3,
                        16);
#else
            gmc1_motion(s, dest_y, dest_cb, dest_cr, 0,
                        ref_picture, 0,
                        16);
#endif
        }else if(s->quarter_sample && dir==0){ //FIXME
            qpel_motion(s, dest_y, dest_cb, dest_cr, 0,
                        ref_picture, 0,
                        0, pix_op, qpix_op,
                        s->mv[dir][0][0], s->mv[dir][0][1], 16);
        }else{
            mpeg_motion(s, dest_y, dest_cb, dest_cr, 0,
                        ref_picture, 0,
                        0, pix_op,
                        s->mv[dir][0][0], s->mv[dir][0][1], 16);
        }           
        break;
    case MV_TYPE_8X8:
        for(i=0;i<4;i++) {
            motion_x = s->mv[dir][i][0];
            motion_y = s->mv[dir][i][1];

            dxy = ((motion_y & 1) << 1) | (motion_x & 1);
            src_x = mb_x * 16 + (motion_x >> 1) + (i & 1) * 8;
            src_y = mb_y * 16 + (motion_y >> 1) + (i >>1) * 8;
                    
            /* WARNING: do no forget half pels */
            src_x = clip(src_x, -16, s->width);
            if (src_x == s->width)
                dxy &= ~1;
            src_y = clip(src_y, -16, s->height);
            if (src_y == s->height)
                dxy &= ~2;
                    
            ptr = ref_picture[0] + (src_y * s->linesize) + (src_x);
            dest = dest_y + ((i & 1) * 8) + (i >> 1) * 8 * s->linesize;
            pix_op[dxy](dest, ptr, s->linesize, 8);
        }
    
        if(s->flags&CODEC_FLAG_GRAY) break;
        /* In case of 8X8, we construct a single chroma motion vector
           with a special rounding */
        mx = 0;
        my = 0;
        for(i=0;i<4;i++) {
            mx += s->mv[dir][i][0];
            my += s->mv[dir][i][1];
        }
        if (mx >= 0)
            mx = (h263_chroma_roundtab[mx & 0xf] + ((mx >> 3) & ~1));
        else {
            mx = -mx;
            mx = -(h263_chroma_roundtab[mx & 0xf] + ((mx >> 3) & ~1));
        }
        if (my >= 0)
            my = (h263_chroma_roundtab[my & 0xf] + ((my >> 3) & ~1));
        else {
            my = -my;
            my = -(h263_chroma_roundtab[my & 0xf] + ((my >> 3) & ~1));
        }
        dxy = ((my & 1) << 1) | (mx & 1);
        mx >>= 1;
        my >>= 1;

        src_x = mb_x * 8 + mx;
        src_y = mb_y * 8 + my;
        src_x = clip(src_x, -8, s->width/2);
        if (src_x == s->width/2)
            dxy &= ~1;
        src_y = clip(src_y, -8, s->height/2);
        if (src_y == s->height/2)
            dxy &= ~2;
        
        offset = (src_y * (s->linesize >> 1)) + src_x;
        ptr = ref_picture[1] + offset;
        pix_op[dxy](dest_cb, ptr, s->linesize >> 1, 8);
        ptr = ref_picture[2] + offset;
        pix_op[dxy](dest_cr, ptr, s->linesize >> 1, 8);
        break;
    case MV_TYPE_FIELD:
        if (s->picture_structure == PICT_FRAME) {
            /* top field */
            mpeg_motion(s, dest_y, dest_cb, dest_cr, 0,
                        ref_picture, s->field_select[dir][0] ? s->linesize : 0,
                        1, pix_op,
                        s->mv[dir][0][0], s->mv[dir][0][1], 8);
            /* bottom field */
            mpeg_motion(s, dest_y, dest_cb, dest_cr, s->linesize,
                        ref_picture, s->field_select[dir][1] ? s->linesize : 0,
                        1, pix_op,
                        s->mv[dir][1][0], s->mv[dir][1][1], 8);
        } else {
            

        }
        break;
    }
}


/* put block[] to dest[] */
static inline void put_dct(MpegEncContext *s, 
                           DCTELEM *block, int i, UINT8 *dest, int line_size)
{
    if (!s->mpeg2)
        s->dct_unquantize(s, block, i, s->qscale);
    ff_idct_put (dest, line_size, block);
}

/* add block[] to dest[] */
static inline void add_dct(MpegEncContext *s, 
                           DCTELEM *block, int i, UINT8 *dest, int line_size)
{
    if (s->block_last_index[i] >= 0) {
        ff_idct_add (dest, line_size, block);
    }
}

static inline void add_dequant_dct(MpegEncContext *s, 
                           DCTELEM *block, int i, UINT8 *dest, int line_size)
{
    if (s->block_last_index[i] >= 0) {
                s->dct_unquantize(s, block, i, s->qscale);

        ff_idct_add (dest, line_size, block);
    }
}

/**
 * cleans dc, ac, coded_block for the current non intra MB
 */
void ff_clean_intra_table_entries(MpegEncContext *s)
{
    int wrap = s->block_wrap[0];
    int xy = s->block_index[0];
    
    s->dc_val[0][xy           ] = 
    s->dc_val[0][xy + 1       ] = 
    s->dc_val[0][xy     + wrap] =
    s->dc_val[0][xy + 1 + wrap] = 1024;
    /* ac pred */
    memset(s->ac_val[0][xy       ], 0, 32 * sizeof(INT16));
    memset(s->ac_val[0][xy + wrap], 0, 32 * sizeof(INT16));
    if (s->msmpeg4_version>=3) {
        s->coded_block[xy           ] =
        s->coded_block[xy + 1       ] =
        s->coded_block[xy     + wrap] =
        s->coded_block[xy + 1 + wrap] = 0;
    }
    /* chroma */
    wrap = s->block_wrap[4];
    xy = s->mb_x + 1 + (s->mb_y + 1) * wrap;
    s->dc_val[1][xy] =
    s->dc_val[2][xy] = 1024;
    /* ac pred */
    memset(s->ac_val[1][xy], 0, 16 * sizeof(INT16));
    memset(s->ac_val[2][xy], 0, 16 * sizeof(INT16));
    
    s->mbintra_table[s->mb_x + s->mb_y*s->mb_width]= 0;
}

/* generic function called after a macroblock has been parsed by the
   decoder or after it has been encoded by the encoder.

   Important variables used:
   s->mb_intra : true if intra macroblock
   s->mv_dir   : motion vector direction
   s->mv_type  : motion vector type
   s->mv       : motion vector
   s->interlaced_dct : true if interlaced dct used (mpeg2)
 */
void MPV_decode_mb(MpegEncContext *s, DCTELEM block[6][64])
{
    int mb_x, mb_y;
    const int mb_xy = s->mb_y * s->mb_width + s->mb_x;

    mb_x = s->mb_x;
    mb_y = s->mb_y;

#ifdef FF_POSTPROCESS
    /* Obsolete. Exists for compatibility with mplayer only. */
    quant_store[mb_y][mb_x]=s->qscale;
    //printf("[%02d][%02d] %d\n",mb_x,mb_y,s->qscale);
#else
    if(s->avctx->quant_store) s->avctx->quant_store[mb_y*s->avctx->qstride+mb_x] = s->qscale;
#endif

    /* update DC predictors for P macroblocks */
    if (!s->mb_intra) {
        if (s->h263_pred || s->h263_aic) {
            if(s->mbintra_table[mb_xy])
                ff_clean_intra_table_entries(s);
        } else {
            s->last_dc[0] =
            s->last_dc[1] =
            s->last_dc[2] = 128 << s->intra_dc_precision;
        }
    }
    else if (s->h263_pred || s->h263_aic)
        s->mbintra_table[mb_xy]=1;

    /* update motion predictor, not for B-frames as they need the motion_val from the last P/S-Frame */
    if (s->out_format == FMT_H263 && s->pict_type!=B_TYPE) { //FIXME move into h263.c if possible, format specific stuff shouldnt be here
        int motion_x, motion_y;
        
        const int wrap = s->block_wrap[0];
        const int xy = s->block_index[0];
        if (s->mb_intra) {
            motion_x = 0;
            motion_y = 0;
            goto motion_init;
        } else if (s->mv_type == MV_TYPE_16X16) {
            motion_x = s->mv[0][0][0];
            motion_y = s->mv[0][0][1];
        motion_init:
            /* no update if 8X8 because it has been done during parsing */
            s->motion_val[xy][0] = motion_x;
            s->motion_val[xy][1] = motion_y;
            s->motion_val[xy + 1][0] = motion_x;
            s->motion_val[xy + 1][1] = motion_y;
            s->motion_val[xy + wrap][0] = motion_x;
            s->motion_val[xy + wrap][1] = motion_y;
            s->motion_val[xy + 1 + wrap][0] = motion_x;
            s->motion_val[xy + 1 + wrap][1] = motion_y;
        }
      }
    
    if (!(s->encoding && (s->intra_only || s->pict_type==B_TYPE))) {
        UINT8 *dest_y, *dest_cb, *dest_cr;
        int dct_linesize, dct_offset;
        op_pixels_func *op_pix;
        qpel_mc_func *op_qpix;

        /* avoid copy if macroblock skipped in last frame too 
           dont touch it for B-frames as they need the skip info from the next p-frame */
        if (s->pict_type != B_TYPE) {
            UINT8 *mbskip_ptr = &s->mbskip_table[mb_xy];
            if (s->mb_skiped) {
                s->mb_skiped = 0;
                /* if previous was skipped too, then nothing to do ! 
                   skip only during decoding as we might trash the buffers during encoding a bit */
                if (*mbskip_ptr != 0 && !s->encoding) 
                    goto the_end;
                *mbskip_ptr = 1; /* indicate that this time we skiped it */
            } else {
                *mbskip_ptr = 0; /* not skipped */
            }
        }

        dest_y = s->current_picture[0] + (mb_y * 16 * s->linesize) + mb_x * 16;
        dest_cb = s->current_picture[1] + (mb_y * 8 * (s->linesize >> 1)) + mb_x * 8;
        dest_cr = s->current_picture[2] + (mb_y * 8 * (s->linesize >> 1)) + mb_x * 8;

        if (s->interlaced_dct) {
            dct_linesize = s->linesize * 2;
            dct_offset = s->linesize;
        } else {
            dct_linesize = s->linesize;
            dct_offset = s->linesize * 8;
        }

        if (!s->mb_intra) {
            /* motion handling */
            /* decoding or more than one mb_type (MC was allready done otherwise) */
            if((!s->encoding) || (s->mb_type[mb_xy]&(s->mb_type[mb_xy]-1))){
                if ((!s->no_rounding) || s->pict_type==B_TYPE){                
                op_pix = put_pixels_tab;
                op_qpix= qpel_mc_rnd_tab;
            }else{
                op_pix = put_no_rnd_pixels_tab;
                op_qpix= qpel_mc_no_rnd_tab;
            }

            if (s->mv_dir & MV_DIR_FORWARD) {
                MPV_motion(s, dest_y, dest_cb, dest_cr, 0, s->last_picture, op_pix, op_qpix);
                    if ((!s->no_rounding) || s->pict_type==B_TYPE)
                    op_pix = avg_pixels_tab;
                else
                    op_pix = avg_no_rnd_pixels_tab;
            }
            if (s->mv_dir & MV_DIR_BACKWARD) {
                MPV_motion(s, dest_y, dest_cb, dest_cr, 1, s->next_picture, op_pix, op_qpix);
            }
            }

            /* skip dequant / idct if we are really late ;) */
            if(s->hurry_up>1) goto the_end;

            /* add dct residue */
            if(!s->mpeg2 && (s->encoding || (!s->h263_msmpeg4))){
                add_dequant_dct(s, block[0], 0, dest_y, dct_linesize);
                add_dequant_dct(s, block[1], 1, dest_y + 8, dct_linesize);
                add_dequant_dct(s, block[2], 2, dest_y + dct_offset, dct_linesize);
                add_dequant_dct(s, block[3], 3, dest_y + dct_offset + 8, dct_linesize);

                if(!(s->flags&CODEC_FLAG_GRAY)){
                add_dequant_dct(s, block[4], 4, dest_cb, s->linesize >> 1);
                add_dequant_dct(s, block[5], 5, dest_cr, s->linesize >> 1);
                }
            } else {
            add_dct(s, block[0], 0, dest_y, dct_linesize);
            add_dct(s, block[1], 1, dest_y + 8, dct_linesize);
            add_dct(s, block[2], 2, dest_y + dct_offset, dct_linesize);
            add_dct(s, block[3], 3, dest_y + dct_offset + 8, dct_linesize);

                if(!(s->flags&CODEC_FLAG_GRAY)){
            add_dct(s, block[4], 4, dest_cb, s->linesize >> 1);
            add_dct(s, block[5], 5, dest_cr, s->linesize >> 1);
            }
            }
        } else {
            /* dct only in intra block */
            put_dct(s, block[0], 0, dest_y, dct_linesize);
            put_dct(s, block[1], 1, dest_y + 8, dct_linesize);
            put_dct(s, block[2], 2, dest_y + dct_offset, dct_linesize);
            put_dct(s, block[3], 3, dest_y + dct_offset + 8, dct_linesize);

            if(!(s->flags&CODEC_FLAG_GRAY)){
            put_dct(s, block[4], 4, dest_cb, s->linesize >> 1);
            put_dct(s, block[5], 5, dest_cr, s->linesize >> 1);
        }
    }
    }
 the_end:
    emms_c(); //FIXME remove
}

void ff_copy_bits(PutBitContext *pb, UINT8 *src, int length)
{
#if 1
    int bytes= length>>4;
    int bits= length&15;
    int i;

    if(length==0) return;

    for(i=0; i<bytes; i++) put_bits(pb, 16, be2me_16(((uint16_t*)src)[i]));
    put_bits(pb, bits, be2me_16(((uint16_t*)src)[i])>>(16-bits));
#else
    int bytes= length>>3;
    int bits= length&7;
    int i;

    for(i=0; i<bytes; i++) put_bits(pb, 8, src[i]);
    put_bits(pb, bits, src[i]>>(8-bits));
#endif
}

static void dct_unquantize_mpeg1_c(MpegEncContext *s, 
                                   DCTELEM *block, int n, int qscale)
{
    int i, level, nCoeffs;
    const UINT16 *quant_matrix;

    if(s->alternate_scan) nCoeffs= 64;
    else nCoeffs= s->block_last_index[n]+1;
    
    if (s->mb_intra) {
        if (n < 4) 
            block[0] = block[0] * s->y_dc_scale;
        else
            block[0] = block[0] * s->c_dc_scale;
        /* XXX: only mpeg1 */
        quant_matrix = s->intra_matrix;
        for(i=1;i<nCoeffs;i++) {
            int j= zigzag_direct[i];
            level = block[j];
            if (level) {
                if (level < 0) {
                    level = -level;
                    level = (int)(level * qscale * quant_matrix[j]) >> 3;
                    level = (level - 1) | 1;
                    level = -level;
                } else {
                    level = (int)(level * qscale * quant_matrix[j]) >> 3;
                    level = (level - 1) | 1;
                }
#ifdef PARANOID
                if (level < -2048 || level > 2047)
                    fprintf(stderr, "unquant error %d %d\n", i, level);
#endif
                block[j] = level;
            }
        }
    } else {
        i = 0;
        quant_matrix = s->inter_matrix;
        for(;i<nCoeffs;i++) {
            int j= zigzag_direct[i];
            level = block[j];
            if (level) {
                if (level < 0) {
                    level = -level;
                    level = (((level << 1) + 1) * qscale *
                             ((int) (quant_matrix[j]))) >> 4;
                    level = (level - 1) | 1;
                    level = -level;
                } else {
                    level = (((level << 1) + 1) * qscale *
                             ((int) (quant_matrix[j]))) >> 4;
                    level = (level - 1) | 1;
                }
#ifdef PARANOID
                if (level < -2048 || level > 2047)
                    fprintf(stderr, "unquant error %d %d\n", i, level);
#endif
                block[j] = level;
            }
        }
    }
}

static void dct_unquantize_mpeg2_c(MpegEncContext *s, 
                                   DCTELEM *block, int n, int qscale)
{
    int i, level, nCoeffs;
    const UINT16 *quant_matrix;

    if(s->alternate_scan) nCoeffs= 64;
    else nCoeffs= s->block_last_index[n]+1;
    
    if (s->mb_intra) {
        if (n < 4) 
            block[0] = block[0] * s->y_dc_scale;
        else
            block[0] = block[0] * s->c_dc_scale;
        quant_matrix = s->intra_matrix;
        for(i=1;i<nCoeffs;i++) {
            int j= zigzag_direct[i];
            level = block[j];
            if (level) {
                if (level < 0) {
                    level = -level;
                    level = (int)(level * qscale * quant_matrix[j]) >> 3;
                    level = -level;
                } else {
                    level = (int)(level * qscale * quant_matrix[j]) >> 3;
                }
#ifdef PARANOID
                if (level < -2048 || level > 2047)
                    fprintf(stderr, "unquant error %d %d\n", i, level);
#endif
                block[j] = level;
            }
        }
    } else {
        int sum=-1;
        i = 0;
        quant_matrix = s->inter_matrix;
        for(;i<nCoeffs;i++) {
            int j= zigzag_direct[i];
            level = block[j];
            if (level) {
                if (level < 0) {
                    level = -level;
                    level = (((level << 1) + 1) * qscale *
                             ((int) (quant_matrix[j]))) >> 4;
                    level = -level;
                } else {
                    level = (((level << 1) + 1) * qscale *
                             ((int) (quant_matrix[j]))) >> 4;
                }
#ifdef PARANOID
                if (level < -2048 || level > 2047)
                    fprintf(stderr, "unquant error %d %d\n", i, level);
#endif
                block[j] = level;
                sum+=level;
            }
        }
        block[63]^=sum&1;
    }
}


static void dct_unquantize_h263_c(MpegEncContext *s, 
                                  DCTELEM *block, int n, int qscale)
{
    int i, level, qmul, qadd;
    int nCoeffs;
    
    if (s->mb_intra) {
        if (!s->h263_aic) {
            if (n < 4) 
                block[0] = block[0] * s->y_dc_scale;
            else
                block[0] = block[0] * s->c_dc_scale;
        }
        i = 1;
        nCoeffs= 64; //does not allways use zigzag table 
    } else {
        i = 0;
        nCoeffs= zigzag_end[ s->block_last_index[n] ];
    }

    qmul = s->qscale << 1;
    if (s->h263_aic && s->mb_intra)
        qadd = 0;
    else
        qadd = (s->qscale - 1) | 1;

    for(;i<nCoeffs;i++) {
        level = block[i];
        if (level) {
            if (level < 0) {
                level = level * qmul - qadd;
            } else {
                level = level * qmul + qadd;
            }
#ifdef PARANOID
                if (level < -2048 || level > 2047)
                    fprintf(stderr, "unquant error %d %d\n", i, level);
#endif
            block[i] = level;
        }
    }
}

static void remove_ac(MpegEncContext *s, uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr, int mb_x, int mb_y)
{
    int dc, dcb, dcr, y, i;
    for(i=0; i<4; i++){
        dc= s->dc_val[0][mb_x*2+1 + (i&1) + (mb_y*2+1 + (i>>1))*(s->mb_width*2+2)];
        for(y=0; y<8; y++){
            int x;
            for(x=0; x<8; x++){
                dest_y[x + (i&1)*8 + (y + (i>>1)*8)*s->linesize]= dc/8;
            }
        }
    }
    dcb = s->dc_val[1][mb_x+1 + (mb_y+1)*(s->mb_width+2)];
    dcr= s->dc_val[2][mb_x+1 + (mb_y+1)*(s->mb_width+2)];
    for(y=0; y<8; y++){
        int x;
        for(x=0; x<8; x++){
            dest_cb[x + y*(s->linesize>>1)]= dcb/8;
            dest_cr[x + y*(s->linesize>>1)]= dcr/8;
        }
    }
}

/**
 * will conceal past errors, and allso drop b frames if needed
 *
 */
void ff_conceal_past_errors(MpegEncContext *s, int unknown_pos)
{
    int mb_x= s->mb_x;
    int mb_y= s->mb_y;
    int mb_dist=0;
    int i, intra_count=0, inter_count=0;
    int intra_conceal= s->msmpeg4_version ? 50 : 50; //FIXME finetune
    int inter_conceal= s->msmpeg4_version ? 50 : 50;
    
    // for last block
    if(mb_x>=s->mb_width)  mb_x= s->mb_width -1;
    if(mb_y>=s->mb_height) mb_y= s->mb_height-1;

    if(s->decoding_error==0 && unknown_pos){
        if(s->data_partitioning && s->pict_type!=B_TYPE)
                s->decoding_error= DECODING_AC_LOST;
        else
                s->decoding_error= DECODING_DESYNC;
    }

    if(s->decoding_error==DECODING_DESYNC && s->pict_type!=B_TYPE) s->next_p_frame_damaged=1;

    for(i=mb_x + mb_y*s->mb_width; i>=0; i--){
        if(s->mbintra_table[i]) intra_count++;
        else                    inter_count++;
    }
    
    if(s->decoding_error==DECODING_AC_LOST){
        intra_conceal*=2;
        inter_conceal*=2;
    }else if(s->decoding_error==DECODING_ACDC_LOST){
        intra_conceal*=2;
        inter_conceal*=2;
    }

    if(unknown_pos && (intra_count<inter_count)){
        intra_conceal= inter_conceal= s->mb_num; 
//        printf("%d %d\n",intra_count, inter_count);
    }

    fprintf(stderr, "concealing errors\n");

    /* for all MBs from the current one back until the last resync marker */
    for(; mb_y>=0 && mb_y>=s->resync_mb_y; mb_y--){
        for(; mb_x>=0; mb_x--){
            uint8_t *dest_y  = s->current_picture[0] + (mb_y * 16*  s->linesize      ) + mb_x * 16;
            uint8_t *dest_cb = s->current_picture[1] + (mb_y * 8 * (s->linesize >> 1)) + mb_x * 8;
            uint8_t *dest_cr = s->current_picture[2] + (mb_y * 8 * (s->linesize >> 1)) + mb_x * 8;
            int mb_x_backup= s->mb_x; //FIXME pass xy to mpeg_motion
            int mb_y_backup= s->mb_y;
            s->mb_x=mb_x;
            s->mb_y=mb_y;
            if(s->mbintra_table[mb_y*s->mb_width + mb_x] && mb_dist<intra_conceal){
                if(s->decoding_error==DECODING_AC_LOST){
                    remove_ac(s, dest_y, dest_cb, dest_cr, mb_x, mb_y);
//                    printf("remove ac to %d %d\n", mb_x, mb_y);
                }else{
                    mpeg_motion(s, dest_y, dest_cb, dest_cr, 0, 
                                s->last_picture, 0, 0, put_pixels_tab,
                                0/*mx*/, 0/*my*/, 16);
                }
            }
            else if(!s->mbintra_table[mb_y*s->mb_width + mb_x] && mb_dist<inter_conceal){
                int mx=0;
                int my=0;

                if(s->decoding_error!=DECODING_DESYNC){
                    int xy= mb_x*2+1 + (mb_y*2+1)*(s->mb_width*2+2);
                    mx= s->motion_val[ xy ][0];
                    my= s->motion_val[ xy ][1];
                }

                mpeg_motion(s, dest_y, dest_cb, dest_cr, 0, 
                            s->last_picture, 0, 0, put_pixels_tab,
                            mx, my, 16);
            }
            s->mb_x= mb_x_backup;
            s->mb_y= mb_y_backup;

            if(mb_x== s->resync_mb_x && mb_y== s->resync_mb_y) return;
            if(!s->mbskip_table[mb_x + mb_y*s->mb_width]) mb_dist++;
        }
        mb_x=s->mb_width-1;
    }
}

