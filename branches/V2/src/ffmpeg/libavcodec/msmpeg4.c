/*
 * MSMPEG4 backend for ffmpeg encoder and decoder
 * Copyright (c) 2001 Fabrice Bellard.
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
 * msmpeg4v1 & v2 stuff by Michael Niedermayer <michaelni@gmx.at>
 */
#include "avcodec.h"
#include "dsputil.h"
#include "mpegvideo.h"
//#define PRINT_MB

/*
 * You can also call this codec : MPEG4 with a twist ! 
 *
 * TODO: 
 *        - (encoding) select best mv table (two choices)
 *        - (encoding) select best vlc/dc table 
 */
//#define DEBUG

static UINT32 v2_dc_lum_table[512][2];
static UINT32 v2_dc_chroma_table[512][2];

static inline int msmpeg4_decode_block(MpegEncContext * s, DCTELEM * block,
                                int n, int coded);
static int msmpeg4_decode_dc(MpegEncContext * s, int n, int *dir_ptr);
static int msmpeg4_decode_motion(MpegEncContext * s, 
                                 int *mx_ptr, int *my_ptr);
static void init_h263_dc_for_msmpeg4(void);
static inline void msmpeg4_memsetw(short *tab, int val, int n);



extern UINT32 inverse[256];

#ifdef DEBUG
int intra_count = 0;
int frame_count = 0;
#endif

#include "msmpeg4data.h"

#ifdef STATS

const char *st_names[ST_NB] = {
    "unknown",
    "dc",
    "intra_ac",
    "inter_ac",
    "intra_mb",
    "inter_mb",
    "mv",
};

int st_current_index = 0;
unsigned int st_bit_counts[ST_NB];
unsigned int st_out_bit_counts[ST_NB];

#define set_stat(var) st_current_index = var;

void print_stats(void)
{
    unsigned int total;
    int i;

    printf("Input:\n");
    total = 0;
    for(i=0;i<ST_NB;i++)
        total += st_bit_counts[i];
    if (total == 0)
        total = 1;
    for(i=0;i<ST_NB;i++) {
        printf("%-10s : %10.1f %5.1f%%\n", 
               st_names[i], 
               (double)st_bit_counts[i] / 8.0, 
               (double)st_bit_counts[i] * 100.0 / total);
    }
    printf("%-10s : %10.1f %5.1f%%\n",
           "total", 
           (double)total / 8.0, 
           100.0);

    printf("Output:\n");
    total = 0;
    for(i=0;i<ST_NB;i++)
        total += st_out_bit_counts[i];
    if (total == 0)
        total = 1;
    for(i=0;i<ST_NB;i++) {
        printf("%-10s : %10.1f %5.1f%%\n", 
               st_names[i], 
               (double)st_out_bit_counts[i] / 8.0, 
               (double)st_out_bit_counts[i] * 100.0 / total);
    }
    printf("%-10s : %10.1f %5.1f%%\n",
           "total", 
           (double)total / 8.0, 
           100.0);
}

#else

#define set_stat(var)

#endif

static void common_init(MpegEncContext * s)
{
    static int inited=0;
    
    switch(s->msmpeg4_version){
    case 1:
    case 2:
        s->y_dc_scale_table=
        s->c_dc_scale_table= ff_mpeg1_dc_scale_table;
        break;
    case 3:
        if(s->workaround_bugs){
            s->y_dc_scale_table= old_ff_y_dc_scale_table;
            s->c_dc_scale_table= old_ff_c_dc_scale_table;
        } else{
            s->y_dc_scale_table= ff_mpeg4_y_dc_scale_table;
            s->c_dc_scale_table= ff_mpeg4_c_dc_scale_table;
        }
        break;
    case 4:
        s->y_dc_scale_table= wmv1_y_dc_scale_table;
        s->c_dc_scale_table= wmv1_c_dc_scale_table;
        break;
    }

    if(s->msmpeg4_version==4){
        s->intra_scantable  = wmv1_scantable[1];
        s->intra_h_scantable= wmv1_scantable[2];
        s->intra_v_scantable= wmv1_scantable[3];
        s->inter_scantable  = wmv1_scantable[0];
    }else{
        s->intra_scantable  = zigzag_direct; 
        s->intra_h_scantable= ff_alternate_horizontal_scan; 
        s->intra_v_scantable= ff_alternate_vertical_scan; 
        s->inter_scantable  = zigzag_direct; 
    }
    
    if(!inited){
        int i;
        inited=1;

        init_h263_dc_for_msmpeg4();

        /* permute for IDCT */
        for(i=0; i<WMV1_SCANTABLE_COUNT; i++){
            int k;
            for(k=0;k<64;k++) {
                int j = wmv1_scantable[i][k];
                wmv1_scantable[i][k]= block_permute_op(j);
            }
        }

    }
}

/* predict coded block */
static inline int coded_block_pred(MpegEncContext * s, int n, UINT8 **coded_block_ptr)
{
    int xy, wrap, pred, a, b, c;

    xy = s->block_index[n];
    wrap = s->block_wrap[0];

    /* B C
     * A X 
     */
    a = s->coded_block[xy - 1       ];
    b = s->coded_block[xy - 1 - wrap];
    c = s->coded_block[xy     - wrap];
    
    if (b == c) {
        pred = a;
    } else {
        pred = c;
    }
    
    /* store value */
    *coded_block_ptr = &s->coded_block[xy];

    return pred;
}

static inline void handle_slices(MpegEncContext *s){
    if (s->mb_x == 0) {
        if (s->slice_height && (s->mb_y % s->slice_height) == 0) {
            if(s->msmpeg4_version != 4){
                int wrap;
                /* reset DC pred (set previous line to 1024) */
                wrap = 2 * s->mb_width + 2;
                msmpeg4_memsetw(&s->dc_val[0][(1) + (2 * s->mb_y) * wrap],
                                1024, 2 * s->mb_width);
                wrap = s->mb_width + 2;
                msmpeg4_memsetw(&s->dc_val[1][(1) + (s->mb_y) * wrap],
                                1024, s->mb_width);
                msmpeg4_memsetw(&s->dc_val[2][(1) + (s->mb_y) * wrap],
                                1024, s->mb_width);

                /* reset AC pred (set previous line to 0) */
                wrap = s->mb_width * 2 + 2;
                msmpeg4_memsetw(s->ac_val[0][0] + (1 + (2 * s->mb_y) * wrap)*16,
                                0, 2 * s->mb_width*16);
                wrap = s->mb_width + 2;
                msmpeg4_memsetw(s->ac_val[1][0] + (1 + (s->mb_y) * wrap)*16,
                                0, s->mb_width*16);
                msmpeg4_memsetw(s->ac_val[2][0] + (1 + (s->mb_y) * wrap)*16,
                                0, s->mb_width*16);
            }
            s->first_slice_line = 1;
        } else {
            s->first_slice_line = 0; 
        }
    }
}

/* old ffmpeg msmpeg4v3 mode */
void ff_old_msmpeg4_dc_scale(MpegEncContext * s)
{
    if (s->qscale < 5){
        s->y_dc_scale = 8;
        s->c_dc_scale = 8;
    }else if (s->qscale < 9){
        s->y_dc_scale = 2 * s->qscale;
        s->c_dc_scale = (s->qscale + 13)>>1;
    }else{
        s->y_dc_scale = s->qscale + 8;
        s->c_dc_scale = (s->qscale + 13)>>1;
    }
}

static inline int msmpeg4v1_pred_dc(MpegEncContext * s, int n, 
                           INT32 **dc_val_ptr)
{
    int i;

    if (n < 4) {
        i= 0;
    } else {
        i= n-3;
    }
    
    *dc_val_ptr= &s->last_dc[i];
    return s->last_dc[i]; 
}

static int get_dc(uint8_t *src, int stride, int scale)
{
    int y;
    int sum=0;
    for(y=0; y<8; y++){
        int x;
        for(x=0; x<8; x++){
            sum+=src[x + y*stride];
        }
    }
    return (sum + (scale>>1))/scale;
}

/* dir = 0: left, dir = 1: top prediction */
static inline int msmpeg4_pred_dc(MpegEncContext * s, int n, 
                             UINT16 **dc_val_ptr, int *dir_ptr)
{
    int a, b, c, wrap, pred, scale;
    INT16 *dc_val;

    /* find prediction */
    if (n < 4) {
	scale = s->y_dc_scale;
    } else {
	scale = s->c_dc_scale;
    }
    
    wrap = s->block_wrap[n];
    dc_val= s->dc_val[0] + s->block_index[n];

    /* B C
     * A X 
     */
    a = dc_val[ - 1];
    b = dc_val[ - 1 - wrap];
    c = dc_val[ - wrap];

    /* XXX: the following solution consumes divisions, but it does not
       necessitate to modify mpegvideo.c. The problem comes from the
       fact they decided to store the quantized DC (which would lead
       to problems if Q could vary !) */
#if defined ARCH_X86 && !defined PIC
    asm volatile(
        "movl %3, %%eax		\n\t"
	"shrl $1, %%eax		\n\t"
	"addl %%eax, %2		\n\t"
	"addl %%eax, %1		\n\t"
	"addl %0, %%eax		\n\t"
	"mull %4		\n\t"
	"movl %%edx, %0		\n\t"
	"movl %1, %%eax		\n\t"
	"mull %4		\n\t"
	"movl %%edx, %1		\n\t"
	"movl %2, %%eax		\n\t"
	"mull %4		\n\t"
	"movl %%edx, %2		\n\t"
	: "+b" (a), "+c" (b), "+D" (c)
	: "g" (scale), "S" (inverse[scale])
	: "%eax", "%edx"
    );
#else
    /* #elif defined (ARCH_ALPHA) */
    /* Divisions are extremely costly on Alpha; optimize the most
       common case. But they are costly everywhere...
     */
    if (scale == 8) {
	a = (a + (8 >> 1)) / 8;
	b = (b + (8 >> 1)) / 8;
	c = (c + (8 >> 1)) / 8;
    } else {
	a = (a + (scale >> 1)) / scale;
	b = (b + (scale >> 1)) / scale;
	c = (c + (scale >> 1)) / scale;
    }
#endif
    /* XXX: WARNING: they did not choose the same test as MPEG4. This
       is very important ! */
    if(s->msmpeg4_version>3){
        if(s->inter_intra_pred){
            uint8_t *dest;
            int wrap;
            
            if(n==1){
                pred=a;
                *dir_ptr = 0;
            }else if(n==2){
                pred=c;
                *dir_ptr = 1;
            }else if(n==3){
                if (abs(a - b) < abs(b - c)) {
                    pred = c;
                    *dir_ptr = 1;
                } else {
                    pred = a;
                    *dir_ptr = 0;
                }
            }else{
                if(n<4){
                    wrap= s->linesize;
                    dest= s->current_picture[0] + (((n>>1) + 2*s->mb_y) * 8*  wrap ) + ((n&1) + 2*s->mb_x) * 8;
                }else{
                    wrap= s->linesize>>1;
                    dest= s->current_picture[n-3] + (s->mb_y * 8 * wrap) + s->mb_x * 8;
                }
                if(s->mb_x==0) a= (1024 + (scale>>1))/scale;
                else           a= get_dc(dest-8, wrap, scale*8);
                if(s->mb_y==0) c= (1024 + (scale>>1))/scale;
                else           c= get_dc(dest-8*wrap, wrap, scale*8);
                
                if (s->h263_aic_dir==0) {
                    pred= a;
                    *dir_ptr = 0;
                }else if (s->h263_aic_dir==1) {
                    if(n==0){
                        pred= c;
                        *dir_ptr = 1;
                    }else{
                        pred= a;
                        *dir_ptr = 0;
                    }
                }else if (s->h263_aic_dir==2) {
                    if(n==0){
                        pred= a;
                        *dir_ptr = 0;
                    }else{
                        pred= c;
                        *dir_ptr = 1;
                    }
                } else {
                    pred= c;
                    *dir_ptr = 1;
                }
            }
        }else{
        if (abs(a - b) < abs(b - c)) {
            pred = c;
            *dir_ptr = 1;
        } else {
            pred = a;
            *dir_ptr = 0;
        }
        }
    }else{
    if (abs(a - b) <= abs(b - c)) {
	pred = c;
        *dir_ptr = 1;
    } else {
	pred = a;
        *dir_ptr = 0;
    }
    }

    /* update predictor */
    *dc_val_ptr = &dc_val[0];
    return pred;
}

#define DC_MAX 119

/****************************************/
/* decoding stuff */

static VLC mb_non_intra_vlc;
static VLC mb_intra_vlc;
static VLC dc_lum_vlc[2];
static VLC dc_chroma_vlc[2];
static VLC v2_dc_lum_vlc;
static VLC v2_dc_chroma_vlc;
static VLC cbpy_vlc;
static VLC v2_intra_cbpc_vlc;
static VLC v2_mb_type_vlc;
static VLC v2_mv_vlc;
static VLC v1_intra_cbpc_vlc;
static VLC v1_inter_cbpc_vlc;
static VLC inter_intra_vlc;

/* this table is practically identical to the one from h263 except that its inverted */
static void init_h263_dc_for_msmpeg4(void)
{
        int level, uni_code, uni_len;

        for(level=-256; level<256; level++){
            int size, v, l;
            /* find number of bits */
            size = 0;
            v = abs(level);
            while (v) {
                v >>= 1;
		    size++;
            }

            if (level < 0)
                l= (-level) ^ ((1 << size) - 1);
            else
                l= level;

            /* luminance h263 */
            uni_code= DCtab_lum[size][0];
            uni_len = DCtab_lum[size][1];
            uni_code ^= (1<<uni_len)-1; //M$ doesnt like compatibility

            if (size > 0) {
                uni_code<<=size; uni_code|=l;
                uni_len+=size;
                if (size > 8){
                    uni_code<<=1; uni_code|=1;
                    uni_len++;
                }
            }
            v2_dc_lum_table[level+256][0]= uni_code;
            v2_dc_lum_table[level+256][1]= uni_len;

            /* chrominance h263 */
            uni_code= DCtab_chrom[size][0];
            uni_len = DCtab_chrom[size][1];
            uni_code ^= (1<<uni_len)-1; //M$ doesnt like compatibility
            
            if (size > 0) {
                uni_code<<=size; uni_code|=l;
                uni_len+=size;
                if (size > 8){
                    uni_code<<=1; uni_code|=1;
                    uni_len++;
                }
            }
            v2_dc_chroma_table[level+256][0]= uni_code;
            v2_dc_chroma_table[level+256][1]= uni_len;

        }
    }

/* init all vlc decoding tables */
int ff_msmpeg4_decode_init(MpegEncContext *s)
{
    static int done = 0;
    int i;
    MVTable *mv;

    common_init(s);

    if (!done) {
        done = 1;

    for(i=0;i<NB_RL_TABLES;i++) {
        init_rl(&rl_table[i]);
        init_vlc_rl(&rl_table[i]);
    }
    for(i=0;i<2;i++) {
        mv = &mv_tables[i];
        init_vlc(&mv->vlc, 9, mv->n + 1, 
                 mv->table_mv_bits, 1, 1,
                 mv->table_mv_code, 2, 2);
    }

    init_vlc(&dc_lum_vlc[0], 9, 120, 
             &table0_dc_lum[0][1], 8, 4,
             &table0_dc_lum[0][0], 8, 4);
    init_vlc(&dc_chroma_vlc[0], 9, 120, 
             &table0_dc_chroma[0][1], 8, 4,
             &table0_dc_chroma[0][0], 8, 4);
    init_vlc(&dc_lum_vlc[1], 9, 120, 
             &table1_dc_lum[0][1], 8, 4,
             &table1_dc_lum[0][0], 8, 4);
    init_vlc(&dc_chroma_vlc[1], 9, 120, 
             &table1_dc_chroma[0][1], 8, 4,
             &table1_dc_chroma[0][0], 8, 4);

    init_vlc(&v2_dc_lum_vlc, 9, 512, 
             &v2_dc_lum_table[0][1], 8, 4,
             &v2_dc_lum_table[0][0], 8, 4);
    init_vlc(&v2_dc_chroma_vlc, 9, 512, 
             &v2_dc_chroma_table[0][1], 8, 4,
             &v2_dc_chroma_table[0][0], 8, 4);
    
    init_vlc(&cbpy_vlc, 6, 16,
             &cbpy_tab[0][1], 2, 1,
             &cbpy_tab[0][0], 2, 1);
    init_vlc(&v2_intra_cbpc_vlc, 3, 4,
             &v2_intra_cbpc[0][1], 2, 1,
             &v2_intra_cbpc[0][0], 2, 1);
    init_vlc(&v2_mb_type_vlc, 5, 8,
             &v2_mb_type[0][1], 2, 1,
             &v2_mb_type[0][0], 2, 1);
    init_vlc(&v2_mv_vlc, 9, 33,
             &mvtab[0][1], 2, 1,
             &mvtab[0][0], 2, 1);

    init_vlc(&mb_non_intra_vlc, 9, 128, 
             &table_mb_non_intra[0][1], 8, 4,
             &table_mb_non_intra[0][0], 8, 4);
    init_vlc(&mb_intra_vlc, 9, 64, 
             &table_mb_intra[0][1], 4, 2,
             &table_mb_intra[0][0], 4, 2);
        
        init_vlc(&v1_intra_cbpc_vlc, 6, 8, 
                 intra_MCBPC_bits, 1, 1,
                 intra_MCBPC_code, 1, 1);
        init_vlc(&v1_inter_cbpc_vlc, 6, 25, 
                 inter_MCBPC_bits, 1, 1,
                 inter_MCBPC_code, 1, 1);
        
        init_vlc(&inter_intra_vlc, 3, 4, 
                 &table_inter_intra[0][1], 2, 1,
                 &table_inter_intra[0][0], 2, 1);
    }
    return 0;
}

static int decode012(GetBitContext *gb)
{
    int n;
    n = get_bits1(gb);
    if (n == 0)
        return 0;
    else
        return get_bits1(gb) + 1;
}

int msmpeg4_decode_picture_header(MpegEncContext * s)
{
    int code;

#if 0
{
int i;
for(i=0; i<s->gb.size*8; i++)
    printf("%d", get_bits1(&s->gb));
//    get_bits1(&s->gb);
printf("END\n");
return -1;
}
#endif

    if(s->msmpeg4_version==1){
        int start_code, num;
        start_code = (get_bits(&s->gb, 16)<<16) | get_bits(&s->gb, 16);
        if(start_code!=0x00000100){
            fprintf(stderr, "invalid startcode\n");
            return -1;
        }

        num= get_bits(&s->gb, 5); // frame number */
    }

    s->pict_type = get_bits(&s->gb, 2) + 1;
    if (s->pict_type != I_TYPE &&
        s->pict_type != P_TYPE){
        fprintf(stderr, "invalid picture type\n");
        return -1;
    }
#if 0
{
    static int had_i=0;
    if(s->pict_type == I_TYPE) had_i=1;
    if(!had_i) return -1;
}
#endif
    s->qscale = get_bits(&s->gb, 5);

    if (s->pict_type == I_TYPE) {
        code = get_bits(&s->gb, 5); 
        if(s->msmpeg4_version==1){
            if(code==0 || code>s->mb_height){
                fprintf(stderr, "invalid slice height %d\n", code);
                return -1;
            }

            s->slice_height = code;
        }else{
            /* 0x17: one slice, 0x18: two slices, ... */
            if (code < 0x17){
                fprintf(stderr, "error, slice code was %X\n", code);
            return -1;
            }

        s->slice_height = s->mb_height / (code - 0x16);
        }

        switch(s->msmpeg4_version){
        case 1:
        case 2:
            s->rl_chroma_table_index = 2;
            s->rl_table_index = 2;

            s->dc_table_index = 0; //not used
            break;
        case 3:
        s->rl_chroma_table_index = decode012(&s->gb);
        s->rl_table_index = decode012(&s->gb);

        s->dc_table_index = get_bits1(&s->gb);
            break;
        case 4:
            msmpeg4_decode_ext_header(s, (2+5+5+17+7)/8);

            if(s->bit_rate > 50) s->per_mb_rl_table= get_bits1(&s->gb);
            else                 s->per_mb_rl_table= 0;
            
            if(!s->per_mb_rl_table){
                s->rl_chroma_table_index = decode012(&s->gb);
                s->rl_table_index = decode012(&s->gb);
            }

            s->dc_table_index = get_bits1(&s->gb);
            s->inter_intra_pred= 0;
            break;
        }
        s->no_rounding = 1;
/*	printf("qscale:%d rlc:%d rl:%d dc:%d mbrl:%d slice:%d   \n", 
		s->qscale,
		s->rl_chroma_table_index,
		s->rl_table_index, 
		s->dc_table_index,
                s->per_mb_rl_table,
                s->slice_height);*/
    } else {
        switch(s->msmpeg4_version){
        case 1:
        case 2:
            if(s->msmpeg4_version==1)
                s->use_skip_mb_code = 1;
            else
        s->use_skip_mb_code = get_bits1(&s->gb);
            s->rl_table_index = 2;
            s->rl_chroma_table_index = s->rl_table_index;
            s->dc_table_index = 0; //not used
            s->mv_table_index = 0;
            break;
        case 3:
            s->use_skip_mb_code = get_bits1(&s->gb);
        s->rl_table_index = decode012(&s->gb);
        s->rl_chroma_table_index = s->rl_table_index;

        s->dc_table_index = get_bits1(&s->gb);

        s->mv_table_index = get_bits1(&s->gb);
            break;
        case 4:
            s->use_skip_mb_code = get_bits1(&s->gb);

            if(s->bit_rate > 50) s->per_mb_rl_table= get_bits1(&s->gb);
            else                 s->per_mb_rl_table= 0;

            if(!s->per_mb_rl_table){
                s->rl_table_index = decode012(&s->gb);
                s->rl_chroma_table_index = s->rl_table_index;
            }

            s->dc_table_index = get_bits1(&s->gb);

            s->mv_table_index = get_bits1(&s->gb);
            s->inter_intra_pred= (s->width*s->height < 320*240 && s->bit_rate<=128);
            break;
        }
/*	printf("skip:%d rl:%d rlc:%d dc:%d mv:%d mbrl:%d qp:%d   \n", 
		s->use_skip_mb_code, 
		s->rl_table_index, 
		s->rl_chroma_table_index, 
		s->dc_table_index,
		s->mv_table_index,
                s->per_mb_rl_table,
                s->qscale);*/
	if(s->flipflop_rounding){
	    s->no_rounding ^= 1;
	}else{
	    s->no_rounding = 0;
	}
    }
    
    s->esc3_level_length= 0;
    s->esc3_run_length= 0;
   
#ifdef DEBUG
    printf("*****frame %d:\n", frame_count++);
#endif
    return 0;
}

int msmpeg4_decode_ext_header(MpegEncContext * s, int buf_size)
{
    int left= buf_size*8 - get_bits_count(&s->gb);
    int length= s->msmpeg4_version>=3 ? 17 : 16;
    /* the alt_bitstream reader could read over the end so we need to check it */
    if(left>=length && left<length+8)
    {
        int fps;

        fps= get_bits(&s->gb, 5);
        s->bit_rate= get_bits(&s->gb, 11);
        if(s->msmpeg4_version>=3)
        s->flipflop_rounding= get_bits1(&s->gb);
        else
            s->flipflop_rounding= 0;

//        printf("fps:%2d bps:%2d roundingType:%1d\n", fps, s->bit_rate, s->flipflop_rounding);
    }
    else if(left<length+8)
    {
        s->flipflop_rounding= 0;
        printf("ext header missing, %d left\n", left);
    }
    else
    {
        fprintf(stderr, "I frame too long, ignoring ext header\n");
    }

    return 0;
}

static inline void msmpeg4_memsetw(short *tab, int val, int n)
{
    int i;
    for(i=0;i<n;i++)
        tab[i] = val;
}

/* this is identical to h263 except that its range is multiplied by 2 */
static int msmpeg4v2_decode_motion(MpegEncContext * s, int pred, int f_code)
{
    int code, val, sign, shift;

    code = get_vlc(&s->gb, &v2_mv_vlc);
//     printf("MV code %d at %d %d pred: %d\n", code, s->mb_x,s->mb_y, pred);
    if (code < 0)
        return 0xffff;

    if (code == 0)
        return pred;
    sign = get_bits1(&s->gb);
    shift = f_code - 1;
    val = (code - 1) << shift;
    if (shift > 0)
        val |= get_bits(&s->gb, shift);
    val++;
    if (sign)
        val = -val;

    val += pred;
    if (val <= -64)
        val += 64;
    else if (val >= 64)
        val -= 64;

    return val;
}


static int msmpeg4v12_decode_mb(MpegEncContext *s, 
                      DCTELEM block[6][64])
{
    int cbp, code, i;
    if (s->pict_type == P_TYPE) {
        if (s->use_skip_mb_code) {
            if (get_bits1(&s->gb)) {
                /* skip mb */
                s->mb_intra = 0;
                for(i=0;i<6;i++)
                    s->block_last_index[i] = -1;
                s->mv_dir = MV_DIR_FORWARD;
                s->mv_type = MV_TYPE_16X16;
                s->mv[0][0][0] = 0;
                s->mv[0][0][1] = 0;
                s->mb_skiped = 1;
                return 0;
            }
        }

        if(s->msmpeg4_version==2)
        code = get_vlc(&s->gb, &v2_mb_type_vlc);
        else
            code = get_vlc(&s->gb, &v1_inter_cbpc_vlc);
        if(code<0 || code>7){
            fprintf(stderr, "cbpc %d invalid at %d %d\n", code, s->mb_x, s->mb_y);
            return -1;
        }

        s->mb_intra = code >>2;
    
        cbp = code & 0x3;
    } else {
        s->mb_intra = 1;
        if(s->msmpeg4_version==2)
        cbp= get_vlc(&s->gb, &v2_intra_cbpc_vlc);
        else
            cbp= get_vlc(&s->gb, &v1_intra_cbpc_vlc);
        if(cbp<0 || cbp>3){
            fprintf(stderr, "cbpc %d invalid at %d %d\n", cbp, s->mb_x, s->mb_y);
            return -1;
        }
    }

    if (!s->mb_intra) {
        int mx, my, cbpy;
        
        cbpy= get_vlc(&s->gb, &cbpy_vlc);
        if(cbpy<0){
            fprintf(stderr, "cbpy %d invalid at %d %d\n", cbp, s->mb_x, s->mb_y);
            return -1;
        }

        cbp|= cbpy<<2;
        if(s->msmpeg4_version==1 || (cbp&3) != 3) cbp^= 0x3C;
        
        h263_pred_motion(s, 0, &mx, &my);
        mx= msmpeg4v2_decode_motion(s, mx, 1);
        my= msmpeg4v2_decode_motion(s, my, 1);
        
        s->mv_dir = MV_DIR_FORWARD;
        s->mv_type = MV_TYPE_16X16;
        s->mv[0][0][0] = mx;
        s->mv[0][0][1] = my;
    } else {
        if(s->msmpeg4_version==2){
        s->ac_pred = get_bits1(&s->gb);
            cbp|= get_vlc(&s->gb, &cbpy_vlc)<<2; //FIXME check errors
        } else{
            s->ac_pred = 0;
            cbp|= get_vlc(&s->gb, &cbpy_vlc)<<2; //FIXME check errors
            if(s->pict_type==P_TYPE) cbp^=0x3C;
        }
    }

    for (i = 0; i < 6; i++) {
        if (msmpeg4_decode_block(s, block[i], i, (cbp >> (5 - i)) & 1) < 0)
	{
             fprintf(stderr,"\nerror while decoding block: %d x %d (%d)\n", s->mb_x, s->mb_y, i);
             return -1;
	}
    }
    return 0;
}

int msmpeg4_decode_mb(MpegEncContext *s, 
                      DCTELEM block[6][64])
{
    int cbp, code, i;
    UINT8 *coded_val;

#ifdef PRINT_MB
if(s->mb_x==0){
    printf("\n");
    if(s->mb_y==0) printf("\n");
}
#endif
    /* special slice handling */
    handle_slices(s);

    if(s->msmpeg4_version<=2) return msmpeg4v12_decode_mb(s, block); //FIXME export function & call from outside perhaps
    
    if (s->pict_type == P_TYPE) {
        set_stat(ST_INTER_MB);
        if (s->use_skip_mb_code) {
            if (get_bits1(&s->gb)) {
                /* skip mb */
                s->mb_intra = 0;
                for(i=0;i<6;i++)
                    s->block_last_index[i] = -1;
                s->mv_dir = MV_DIR_FORWARD;
                s->mv_type = MV_TYPE_16X16;
                s->mv[0][0][0] = 0;
                s->mv[0][0][1] = 0;
                s->mb_skiped = 1;
#ifdef PRINT_MB
printf("S ");
#endif
                return 0;
            }
        }
        
        code = get_vlc(&s->gb, &mb_non_intra_vlc);
        if (code < 0)
            return -1;
	//s->mb_intra = (code & 0x40) ? 0 : 1;
	s->mb_intra = (~code & 0x40) >> 6;
            
        cbp = code & 0x3f;
    } else {
        set_stat(ST_INTRA_MB);
        s->mb_intra = 1;
        code = get_vlc(&s->gb, &mb_intra_vlc);
        if (code < 0)
            return -1;
        /* predict coded block pattern */
        cbp = 0;
        for(i=0;i<6;i++) {
            int val = ((code >> (5 - i)) & 1);
            if (i < 4) {
                int pred = coded_block_pred(s, i, &coded_val);
                val = val ^ pred;
                *coded_val = val;
            }
            cbp |= val << (5 - i);
        }
    }

    if (!s->mb_intra) {
        int mx, my;
//printf("P at %d %d\n", s->mb_x, s->mb_y);
        if(s->per_mb_rl_table && cbp){
            s->rl_table_index = decode012(&s->gb);
            s->rl_chroma_table_index = s->rl_table_index;
        }
        set_stat(ST_MV);
        h263_pred_motion(s, 0, &mx, &my);
        if (msmpeg4_decode_motion(s, &mx, &my) < 0)
            return -1;
        s->mv_dir = MV_DIR_FORWARD;
        s->mv_type = MV_TYPE_16X16;
        s->mv[0][0][0] = mx;
        s->mv[0][0][1] = my;
#ifdef PRINT_MB
printf("P ");
#endif
    } else {
//printf("I at %d %d %d %06X\n", s->mb_x, s->mb_y, ((cbp&3)? 1 : 0) +((cbp&0x3C)? 2 : 0), show_bits(&s->gb, 24));
        set_stat(ST_INTRA_MB);
        s->ac_pred = get_bits1(&s->gb);
#ifdef PRINT_MB
printf("%c", s->ac_pred ? 'A' : 'I');
#endif
        if(s->inter_intra_pred){
            s->h263_aic_dir= get_vlc(&s->gb, &inter_intra_vlc);
//            printf("%d%d %d %d/", s->ac_pred, s->h263_aic_dir, s->mb_x, s->mb_y);
        }
        if(s->per_mb_rl_table && cbp){
            s->rl_table_index = decode012(&s->gb);
            s->rl_chroma_table_index = s->rl_table_index;
        }
    }

    for (i = 0; i < 6; i++) {
        if (msmpeg4_decode_block(s, block[i], i, (cbp >> (5 - i)) & 1) < 0)
	{
	    fprintf(stderr,"\nerror while decoding block: %d x %d (%d)\n", s->mb_x, s->mb_y, i);
	    return -1;
	}
    }

    return 0;
}

static inline int msmpeg4_decode_block(MpegEncContext * s, DCTELEM * block,
                              int n, int coded)
{
    int code, level, i, j, last, run, run_diff;
    int dc_pred_dir;
    RLTable *rl;
    const UINT8 *scan_table;
    int qmul, qadd;

    if (s->mb_intra) {
        qmul=1;
        qadd=0;

	/* DC coef */
        set_stat(ST_DC);
        level = msmpeg4_decode_dc(s, n, &dc_pred_dir);
#ifdef PRINT_MB
{
    static int c;
    if(n==0) c=0;
    if(n==4) printf("%X", c);
    c+= c +dc_pred_dir;
}
#endif
        if (level < 0){
            fprintf(stderr, "dc overflow- block: %d qscale: %d//\n", n, s->qscale);
            if(s->inter_intra_pred) level=0;
            else                    return -1;
        }
        if (n < 4) {
            rl = &rl_table[s->rl_table_index];
            if(level > 256*s->y_dc_scale){
                fprintf(stderr, "dc overflow+ L qscale: %d//\n", s->qscale);
                if(!s->inter_intra_pred) return -1;
            }
        } else {
            rl = &rl_table[3 + s->rl_chroma_table_index];
            if(level > 256*s->c_dc_scale){
                fprintf(stderr, "dc overflow+ C qscale: %d//\n", s->qscale);
                if(!s->inter_intra_pred) return -1;
        }
        }
        block[0] = level;

        run_diff = 0;
	i = 1;
        if (!coded) {
            goto not_coded;
        }
        if (s->ac_pred) {
            if (dc_pred_dir == 0) 
                scan_table = s->intra_v_scantable; /* left */
            else
                scan_table = s->intra_h_scantable; /* top */
        } else {
            scan_table = s->intra_scantable;
        }
        set_stat(ST_INTRA_AC);
    } else {
        qmul = s->qscale << 1;
        qadd = (s->qscale - 1) | 1;
	i = 0;
        rl = &rl_table[3 + s->rl_table_index];

        if(s->msmpeg4_version==2)
            run_diff = 0;
        else
        run_diff = 1;

        if (!coded) {
            s->block_last_index[n] = i - 1;
            return 0;
        }
        scan_table = s->inter_scantable;
        set_stat(ST_INTER_AC);
    }

    for(;;) {
        code = get_vlc(&s->gb, &rl->vlc);
        if (code < 0){
            fprintf(stderr, "illegal AC-VLC code at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
        if (code == rl->n) {
            /* escape */
            if (s->msmpeg4_version==1 || get_bits1(&s->gb) == 0) {
                if (s->msmpeg4_version==1 || get_bits1(&s->gb) == 0) {
                    /* third escape */
                    if(s->msmpeg4_version<=3){
                    last = get_bits1(&s->gb);
                    run = get_bits(&s->gb, 6);
                    level = get_bits(&s->gb, 8);
                        level= ((int8_t)level);
                    }else{
                        int sign;
                        last= get_bits1(&s->gb);
                        if(!s->esc3_level_length){
                            int ll;
                            //printf("ESC-3 %X at %d %d\n", show_bits(&s->gb, 24), s->mb_x, s->mb_y);
                            if(s->qscale<8){
                                ll= get_bits(&s->gb, 3);
                                if(ll==0){
                                    if(get_bits1(&s->gb)) printf("cool a new vlc code ,contact the ffmpeg developers and upload the file\n");
                                    ll=8;
                                }
                            }else{
                                ll=2;
                                while(ll<8 && get_bits1(&s->gb)==0) ll++;
                            }

                            s->esc3_level_length= ll;
                            s->esc3_run_length= get_bits(&s->gb, 2) + 3;
//printf("level length:%d, run length: %d\n", ll, s->esc3_run_length);
                        }
                        run= get_bits(&s->gb, s->esc3_run_length);  
                        sign= get_bits1(&s->gb);
                        level= get_bits(&s->gb, s->esc3_level_length);
                        if(sign) level= -level;
                    }
//printf("level: %d, run: %d at %d %d\n", level, run, s->mb_x, s->mb_y);
#if 0 // waste of time / this will detect very few errors
                    {
                        const int abs_level= ABS(level);
                        const int run1= run - rl->max_run[last][abs_level] - run_diff;
                        if(abs_level<=MAX_LEVEL && run<=MAX_RUN){
                            if(abs_level <= rl->max_level[last][run]){
                                fprintf(stderr, "illegal 3. esc, vlc encoding possible\n");
                                return DECODING_AC_LOST;
                            }
                            if(abs_level <= rl->max_level[last][run]*2){
                                fprintf(stderr, "illegal 3. esc, esc 1 encoding possible\n");
                                return DECODING_AC_LOST;
                            }
                            if(run1>=0 && abs_level <= rl->max_level[last][run1]){
                                fprintf(stderr, "illegal 3. esc, esc 2 encoding possible\n");
                                return DECODING_AC_LOST;
                            }
                        }
                    }
#endif
		    //level = level * qmul + (level>0) * qadd - (level<=0) * qadd ;
		    if (level>0) level= level * qmul + qadd;
                    else        level= level * qmul - qadd;
#if 0 // waste of time too :(
                    if(level>2048 || level<-2048){
                        fprintf(stderr, "|level| overflow in 3. esc\n");
                        return DECODING_AC_LOST;
                    }
#endif
                } else {
                    /* second escape */
                    code = get_vlc(&s->gb, &rl->vlc);
                    if (code < 0 || code >= rl->n){
                        fprintf(stderr, "illegal ESC2-VLC code %d at %d %d\n", code, s->mb_x, s->mb_y);
                        return -1;
                    }
                    run = rl->table_run[code];
                    level = rl->table_level[code];
                    last = code >= rl->last;
                    run += rl->max_run[last][level] + run_diff;
                    level= level * qmul + qadd;
                    if (get_bits1(&s->gb))
                        level = -level;
                }
            } else {
                /* first escape */
                code = get_vlc(&s->gb, &rl->vlc);
                if (code < 0 || code >= rl->n){
                    fprintf(stderr, "illegal ESC2-VLC code %d at %d %d\n", code, s->mb_x, s->mb_y);
                    return -1;
                }
                run = rl->table_run[code];
                level = rl->table_level[code];
                last = code >= rl->last;
                level += rl->max_level[last][run];
                level= level * qmul + qadd;
                if (get_bits1(&s->gb))
                    level = -level;
            }
        } else {
            run = rl->table_run[code];
            level = rl->table_level[code] * qmul + qadd;
            last = code >= rl->last;
            if (get_bits1(&s->gb))
                level = -level;
        }
        i += run;
        if (i >= 64){
            fprintf(stderr, "run too long at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }

	j = scan_table[i];
        block[j] = level;
        i++;
        if (last)
            break;
    }
 not_coded:
    if (s->mb_intra) {
        mpeg4_pred_ac(s, block, n, dc_pred_dir);
        if (s->ac_pred) {
            i = 64; /* XXX: not optimal */
        }
    }
    if(s->msmpeg4_version==4 && i>1) i=64; //FIXME/XXX optimize
    s->block_last_index[n] = i - 1;

    return 0;
}

static int msmpeg4_decode_dc(MpegEncContext * s, int n, int *dir_ptr)
{
    int level, pred;

    if(s->msmpeg4_version<=2){
        if (n < 4) {
            level = get_vlc(&s->gb, &v2_dc_lum_vlc);
        } else {
            level = get_vlc(&s->gb, &v2_dc_chroma_vlc);
        }
        if (level < 0)
            return -1;
        level-=256;
    }else{  //FIXME optimize use unified tables & index
    if (n < 4) {
        level = get_vlc(&s->gb, &dc_lum_vlc[s->dc_table_index]);
    } else {
        level = get_vlc(&s->gb, &dc_chroma_vlc[s->dc_table_index]);
    }
        if (level < 0){
            fprintf(stderr, "illegal dc vlc\n");
        return -1;
        }

    if (level == DC_MAX) {
        level = get_bits(&s->gb, 8);
        if (get_bits1(&s->gb))
            level = -level;
    } else if (level != 0) {
        if (get_bits1(&s->gb))
            level = -level;
    }
    }

    if(s->msmpeg4_version==1){
        INT32 *dc_val;
        pred = msmpeg4v1_pred_dc(s, n, &dc_val);
        level += pred;
        
        /* update predictor */
        *dc_val= level;
    }else{
        UINT16 *dc_val;
    pred = msmpeg4_pred_dc(s, n, &dc_val, dir_ptr);
    level += pred;

    /* update predictor */
    if (n < 4) {
        *dc_val = level * s->y_dc_scale;
    } else {
        *dc_val = level * s->c_dc_scale;
    }
    }

    return level;
}

static int msmpeg4_decode_motion(MpegEncContext * s, 
                                 int *mx_ptr, int *my_ptr)
{
    MVTable *mv;
    int code, mx, my;

    mv = &mv_tables[s->mv_table_index];

    code = get_vlc(&s->gb, &mv->vlc);
    if (code < 0){
        fprintf(stderr, "illegal MV code at %d %d\n", s->mb_x, s->mb_y);
        return -1;
    }
    if (code == mv->n) {
//printf("MV ESC %X at %d %d\n", show_bits(&s->gb, 24), s->mb_x, s->mb_y);
        mx = get_bits(&s->gb, 6);
        my = get_bits(&s->gb, 6);
    } else {
        mx = mv->table_mvx[code];
        my = mv->table_mvy[code];
    }

    mx += *mx_ptr - 32;
    my += *my_ptr - 32;
    /* WARNING : they do not do exactly modulo encoding */
    if (mx <= -64)
        mx += 64;
    else if (mx >= 64)
        mx -= 64;

    if (my <= -64)
        my += 64;
    else if (my >= 64)
        my -= 64;
    *mx_ptr = mx;
    *my_ptr = my;
    return 0;
}
