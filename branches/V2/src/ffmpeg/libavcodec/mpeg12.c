/*
 * MPEG1 encoder / MPEG2 decoder
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
 */
//#define DEBUG
#include "avcodec.h"
#include "dsputil.h"
#include "mpegvideo.h"

#include "mpeg12data.h"

/* Start codes. */
#define SEQ_END_CODE		0x000001b7
#define SEQ_START_CODE		0x000001b3
#define GOP_START_CODE		0x000001b8
#define PICTURE_START_CODE	0x00000100
#define SLICE_MIN_START_CODE	0x00000101
#define SLICE_MAX_START_CODE	0x000001af
#define EXT_START_CODE		0x000001b5
#define USER_START_CODE		0x000001b2

static int mpeg1_decode_block(MpegEncContext *s, 
                              DCTELEM *block, 
                              int n);
static int mpeg2_decode_block_non_intra(MpegEncContext *s, 
                                        DCTELEM *block, 
                                        int n);
static int mpeg2_decode_block_intra(MpegEncContext *s, 
                                    DCTELEM *block, 
                                    int n);
static int mpeg_decode_motion(MpegEncContext *s, int fcode, int pred);

static void common_init(MpegEncContext *s)
{
    s->y_dc_scale_table=
    s->c_dc_scale_table= ff_mpeg1_dc_scale_table;
}

/******************************************/
/* decoding */

static VLC dc_lum_vlc;
static VLC dc_chroma_vlc;
static VLC mv_vlc;
static VLC mbincr_vlc;
static VLC mb_ptype_vlc;
static VLC mb_btype_vlc;
static VLC mb_pat_vlc;

void mpeg1_init_vlc(MpegEncContext *s)
{
    static int done = 0;

    if (!done) {
        done = 1;

        init_vlc(&dc_lum_vlc, 9, 12, 
                 vlc_dc_lum_bits, 1, 1,
                 vlc_dc_lum_code, 2, 2);
        init_vlc(&dc_chroma_vlc, 9, 12, 
                 vlc_dc_chroma_bits, 1, 1,
                 vlc_dc_chroma_code, 2, 2);
        init_vlc(&mv_vlc, 9, 17, 
                 &mbMotionVectorTable[0][1], 2, 1,
                 &mbMotionVectorTable[0][0], 2, 1);
        init_vlc(&mbincr_vlc, 9, 35, 
                 &mbAddrIncrTable[0][1], 2, 1,
                 &mbAddrIncrTable[0][0], 2, 1);
        init_vlc(&mb_pat_vlc, 9, 63, 
                 &mbPatTable[0][1], 2, 1,
                 &mbPatTable[0][0], 2, 1);
        
        init_vlc(&mb_ptype_vlc, 6, 32, 
                 &table_mb_ptype[0][1], 2, 1,
                 &table_mb_ptype[0][0], 2, 1);
        init_vlc(&mb_btype_vlc, 6, 32, 
                 &table_mb_btype[0][1], 2, 1,
                 &table_mb_btype[0][0], 2, 1);
        init_rl(&rl_mpeg1);
        init_rl(&rl_mpeg2);
        /* cannot use generic init because we must add the EOB code */
        init_vlc(&rl_mpeg1.vlc, 9, rl_mpeg1.n + 2, 
                 &rl_mpeg1.table_vlc[0][1], 4, 2,
                 &rl_mpeg1.table_vlc[0][0], 4, 2);
        init_vlc(&rl_mpeg2.vlc, 9, rl_mpeg2.n + 2, 
                 &rl_mpeg2.table_vlc[0][1], 4, 2,
                 &rl_mpeg2.table_vlc[0][0], 4, 2);
    }
}

static inline int get_dmv(MpegEncContext *s)
{
    if(get_bits1(&s->gb)) 
        return 1 - (get_bits1(&s->gb) << 1);
    else
        return 0;
}

static inline int get_qscale(MpegEncContext *s)
{
    int qscale;
    if (s->mpeg2) {
        if (s->q_scale_type) {
            qscale = non_linear_qscale[get_bits(&s->gb, 5)];
        } else {
            qscale = get_bits(&s->gb, 5) << 1;
        }
    } else {
        /* for mpeg1, we use the generic unquant code */
        qscale = get_bits(&s->gb, 5);
    }
    return qscale;
}

/* motion type (for mpeg2) */
#define MT_FIELD 1
#define MT_FRAME 2
#define MT_16X8  2
#define MT_DMV   3

static int mpeg_decode_mb(MpegEncContext *s,
                          DCTELEM block[6][64])
{
    int i, j, k, cbp, val, code, mb_type, motion_type;
    
    /* skip mb handling */
    if (s->mb_incr == 0) {
        /* read again increment */
        s->mb_incr = 1;
        for(;;) {
            code = get_vlc(&s->gb, &mbincr_vlc);
            if (code < 0)
                return 1; /* error = end of slice */
            if (code >= 33) {
                if (code == 33) {
                    s->mb_incr += 33;
                }
                /* otherwise, stuffing, nothing to do */
            } else {
                s->mb_incr += code;
                break;
            }
        }
    }
    if (++s->mb_x >= s->mb_width) {
        s->mb_x = 0;
        if (s->mb_y >= (s->mb_height - 1))
            return -1;
        s->mb_y++;
    }
    dprintf("decode_mb: x=%d y=%d\n", s->mb_x, s->mb_y);

    if (--s->mb_incr != 0) {
        /* skip mb */
        s->mb_intra = 0;
        for(i=0;i<6;i++)
            s->block_last_index[i] = -1;
        s->mv_type = MV_TYPE_16X16;
        if (s->pict_type == P_TYPE) {
            /* if P type, zero motion vector is implied */
            s->mv_dir = MV_DIR_FORWARD;
            s->mv[0][0][0] = s->mv[0][0][1] = 0;
            s->last_mv[0][0][0] = s->last_mv[0][0][1] = 0;
            s->last_mv[0][1][0] = s->last_mv[0][1][1] = 0;
        } else {
            /* if B type, reuse previous vectors and directions */
            s->mv[0][0][0] = s->last_mv[0][0][0];
            s->mv[0][0][1] = s->last_mv[0][0][1];
            s->mv[1][0][0] = s->last_mv[1][0][0];
            s->mv[1][0][1] = s->last_mv[1][0][1];
        }
        s->mb_skiped = 1;
        return 0;
    }

    switch(s->pict_type) {
    default:
    case I_TYPE:
        if (get_bits1(&s->gb) == 0) {
            if (get_bits1(&s->gb) == 0)
                return -1;
            mb_type = MB_QUANT | MB_INTRA;
        } else {
            mb_type = MB_INTRA;
        }
        break;
    case P_TYPE:
        mb_type = get_vlc(&s->gb, &mb_ptype_vlc);
        if (mb_type < 0)
            return -1;
        break;
    case B_TYPE:
        mb_type = get_vlc(&s->gb, &mb_btype_vlc);
        if (mb_type < 0)
            return -1;
        break;
    }
    dprintf("mb_type=%x\n", mb_type);
    motion_type = 0; /* avoid warning */
    if (mb_type & (MB_FOR|MB_BACK)) {
        /* get additionnal motion vector type */
        if (s->picture_structure == PICT_FRAME && s->frame_pred_frame_dct) 
            motion_type = MT_FRAME;
        else
            motion_type = get_bits(&s->gb, 2);
    }
    /* compute dct type */
    if (s->picture_structure == PICT_FRAME && 
        !s->frame_pred_frame_dct &&
        (mb_type & (MB_PAT | MB_INTRA))) {
        s->interlaced_dct = get_bits1(&s->gb);
#ifdef DEBUG
        if (s->interlaced_dct)
            printf("interlaced_dct\n");
#endif
    } else {
        s->interlaced_dct = 0; /* frame based */
    }

    if (mb_type & MB_QUANT) {
        s->qscale = get_qscale(s);
    }
    if (mb_type & MB_INTRA) {
        if (s->concealment_motion_vectors) {
            /* just parse them */
            if (s->picture_structure != PICT_FRAME) 
                skip_bits1(&s->gb); /* field select */
            mpeg_decode_motion(s, s->mpeg_f_code[0][0], 0);
            mpeg_decode_motion(s, s->mpeg_f_code[0][1], 0);
        }
        s->mb_intra = 1;
        cbp = 0x3f;
        memset(s->last_mv, 0, sizeof(s->last_mv)); /* reset mv prediction */
    } else {
        s->mb_intra = 0;
        cbp = 0;
    }
    /* special case of implicit zero motion vector */
    if (s->pict_type == P_TYPE && !(mb_type & MB_FOR)) {
        s->mv_dir = MV_DIR_FORWARD;
        s->mv_type = MV_TYPE_16X16;
        s->last_mv[0][0][0] = 0;
        s->last_mv[0][0][1] = 0;
        s->last_mv[0][1][0] = 0;
        s->last_mv[0][1][1] = 0;
        s->mv[0][0][0] = 0;
        s->mv[0][0][1] = 0;
    } else if (mb_type & (MB_FOR | MB_BACK)) {
        /* motion vectors */
        s->mv_dir = 0;
        for(i=0;i<2;i++) {
            if (mb_type & (MB_FOR >> i)) {
                s->mv_dir |= (MV_DIR_FORWARD >> i);
                dprintf("motion_type=%d\n", motion_type);
                switch(motion_type) {
                case MT_FRAME: /* or MT_16X8 */
                    if (s->picture_structure == PICT_FRAME) {
                        /* MT_FRAME */
                        s->mv_type = MV_TYPE_16X16;
                        for(k=0;k<2;k++) {
                            val = mpeg_decode_motion(s, s->mpeg_f_code[i][k], 
                                                     s->last_mv[i][0][k]);
                            s->last_mv[i][0][k] = val;
                            s->last_mv[i][1][k] = val;
                            /* full_pel: only for mpeg1 */
                            if (s->full_pel[i])
                                val = val << 1;
                            s->mv[i][0][k] = val;
                            dprintf("mv%d: %d\n", k, val);
                        }
                    } else {
                        /* MT_16X8 */
                        s->mv_type = MV_TYPE_16X8;
                        for(j=0;j<2;j++) {
                            s->field_select[i][j] = get_bits1(&s->gb);
                            for(k=0;k<2;k++) {
                                val = mpeg_decode_motion(s, s->mpeg_f_code[i][k],
                                                         s->last_mv[i][j][k]);
                                s->last_mv[i][j][k] = val;
                                s->mv[i][j][k] = val;
                            }
                        }
                    }
                    break;
                case MT_FIELD:
                    if (s->picture_structure == PICT_FRAME) {
                        s->mv_type = MV_TYPE_FIELD;
                        for(j=0;j<2;j++) {
                            s->field_select[i][j] = get_bits1(&s->gb);
                            val = mpeg_decode_motion(s, s->mpeg_f_code[i][0],
                                                     s->last_mv[i][j][0]);
                            s->last_mv[i][j][0] = val;
                            s->mv[i][j][0] = val;
                            dprintf("fmx=%d\n", val);
                            val = mpeg_decode_motion(s, s->mpeg_f_code[i][1],
                                                     s->last_mv[i][j][1] >> 1);
                            s->last_mv[i][j][1] = val << 1;
                            s->mv[i][j][1] = val;
                            dprintf("fmy=%d\n", val);
                        }
                    } else {
                        s->mv_type = MV_TYPE_16X16;
                        s->field_select[i][0] = get_bits1(&s->gb);
                        for(k=0;k<2;k++) {
                            val = mpeg_decode_motion(s, s->mpeg_f_code[i][k],
                                                     s->last_mv[i][0][k]);
                            s->last_mv[i][0][k] = val;
                            s->last_mv[i][1][k] = val;
                            s->mv[i][0][k] = val;
                        }
                    }
                    break;
                case MT_DMV:
                    {
                        int dmx, dmy, mx, my, m;

                        mx = mpeg_decode_motion(s, s->mpeg_f_code[i][0], 
                                                s->last_mv[i][0][0]);
                        s->last_mv[i][0][0] = mx;
                        s->last_mv[i][1][0] = mx;
                        dmx = get_dmv(s);
                        my = mpeg_decode_motion(s, s->mpeg_f_code[i][1], 
                                                s->last_mv[i][0][1] >> 1);
                        dmy = get_dmv(s);
                        s->mv_type = MV_TYPE_DMV;
                        /* XXX: totally broken */
                        if (s->picture_structure == PICT_FRAME) {
                            s->last_mv[i][0][1] = my << 1;
                            s->last_mv[i][1][1] = my << 1;

                            m = s->top_field_first ? 1 : 3;
                            /* top -> top pred */
                            s->mv[i][0][0] = mx; 
                            s->mv[i][0][1] = my << 1;
                            s->mv[i][1][0] = ((mx * m + (mx > 0)) >> 1) + dmx;
                            s->mv[i][1][1] = ((my * m + (my > 0)) >> 1) + dmy - 1;
                            m = 4 - m;
                            s->mv[i][2][0] = mx;
                            s->mv[i][2][1] = my << 1;
                            s->mv[i][3][0] = ((mx * m + (mx > 0)) >> 1) + dmx;
                            s->mv[i][3][1] = ((my * m + (my > 0)) >> 1) + dmy + 1;
                        } else {
                            s->last_mv[i][0][1] = my;
                            s->last_mv[i][1][1] = my;
                            s->mv[i][0][0] = mx;
                            s->mv[i][0][1] = my;
                            s->mv[i][1][0] = ((mx + (mx > 0)) >> 1) + dmx;
                            s->mv[i][1][1] = ((my + (my > 0)) >> 1) + dmy - 1 
                                /* + 2 * cur_field */;
                        }
                    }
                    break;
                }
            }
        }
    }

    if ((mb_type & MB_INTRA) && s->concealment_motion_vectors) {
        skip_bits1(&s->gb); /* marker */
    }
    
    if (mb_type & MB_PAT) {
        cbp = get_vlc(&s->gb, &mb_pat_vlc);
        if (cbp < 0)
            return -1;
        cbp++;
    }
    dprintf("cbp=%x\n", cbp);

    if (s->mpeg2) {
        if (s->mb_intra) {
            for(i=0;i<6;i++) {
                if (cbp & (1 << (5 - i))) {
                    if (mpeg2_decode_block_intra(s, block[i], i) < 0)
                        return -1;
                } else {
                    s->block_last_index[i] = -1;
                }
            }
        } else {
            for(i=0;i<6;i++) {
                if (cbp & (1 << (5 - i))) {
                    if (mpeg2_decode_block_non_intra(s, block[i], i) < 0)
                        return -1;
                } else {
                    s->block_last_index[i] = -1;
                }
            }
        }
    } else {
        for(i=0;i<6;i++) {
            if (cbp & (1 << (5 - i))) {
                if (mpeg1_decode_block(s, block[i], i) < 0)
                    return -1;
            } else {
                s->block_last_index[i] = -1;
            }
        }
    }
    return 0;
}

/* as h263, but only 17 codes */
static int mpeg_decode_motion(MpegEncContext *s, int fcode, int pred)
{
    int code, sign, val, m, l, shift;

    code = get_vlc(&s->gb, &mv_vlc);
    if (code < 0) {
        return 0xffff;
    }
    if (code == 0) {
        return pred;
    }
    sign = get_bits1(&s->gb);
    shift = fcode - 1;
    val = (code - 1) << shift;
    if (shift > 0)
        val |= get_bits(&s->gb, shift);
    val++;
    if (sign)
        val = -val;
    val += pred;
    
    /* modulo decoding */
    l = (1 << shift) * 16;
    m = 2 * l;
    if (val < -l) {
        val += m;
    } else if (val >= l) {
        val -= m;
    }
    return val;
}

static inline int decode_dc(MpegEncContext *s, int component)
{
    int code, diff;

    if (component == 0) {
        code = get_vlc(&s->gb, &dc_lum_vlc);
    } else {
        code = get_vlc(&s->gb, &dc_chroma_vlc);
    }
    if (code < 0)
        return 0xffff;
    if (code == 0) {
        diff = 0;
    } else {
        diff = get_bits(&s->gb, code);
        if ((diff & (1 << (code - 1))) == 0) 
            diff = (-1 << code) | (diff + 1);
    }
    return diff;
}

static int mpeg1_decode_block(MpegEncContext *s, 
                               DCTELEM *block, 
                               int n)
{
    int level, dc, diff, i, j, run;
    int code, component;
    RLTable *rl = &rl_mpeg1;

    if (s->mb_intra) {
        /* DC coef */
        component = (n <= 3 ? 0 : n - 4 + 1);
        diff = decode_dc(s, component);
        if (diff >= 0xffff)
            return -1;
        dc = s->last_dc[component];
        dc += diff;
        s->last_dc[component] = dc;
        block[0] = dc;
        dprintf("dc=%d diff=%d\n", dc, diff);
        i = 1;
    } else {
        int bit_cnt, v;
        UINT32 bit_buf;
        UINT8 *buf_ptr;
        i = 0;
        /* special case for the first coef. no need to add a second vlc table */
        SAVE_BITS(&s->gb);
        SHOW_BITS(&s->gb, v, 2);
        if (v & 2) {
            run = 0;
            level = 1 - ((v & 1) << 1);
            FLUSH_BITS(2);
            RESTORE_BITS(&s->gb);
            goto add_coef;
        }
        RESTORE_BITS(&s->gb);
    }

    /* now quantify & encode AC coefs */
    for(;;) {
        code = get_vlc(&s->gb, &rl->vlc);
        if (code < 0) {
            return -1;
        }
        if (code == 112) {
            break;
        } else if (code == 111) {
            /* escape */
            run = get_bits(&s->gb, 6);
            level = get_bits(&s->gb, 8);
            level = (level << 24) >> 24;
            if (level == -128) {
                level = get_bits(&s->gb, 8) - 256;
            } else if (level == 0) {
                level = get_bits(&s->gb, 8);
            }
        } else {
            run = rl->table_run[code];
            level = rl->table_level[code];
            if (get_bits1(&s->gb))
                level = -level;
        }
        i += run;
        if (i >= 64)
            return -1;
    add_coef:
        dprintf("%d: run=%d level=%d\n", n, run, level);
	j = zigzag_direct[i];
        block[j] = level;
        i++;
    }
    s->block_last_index[n] = i-1;
    return 0;
}

/* Also does unquantization here, since I will never support mpeg2
   encoding */
static int mpeg2_decode_block_non_intra(MpegEncContext *s, 
                                        DCTELEM *block, 
                                        int n)
{
    int level, i, j, run;
    int code;
    RLTable *rl = &rl_mpeg1;
    const UINT8 *scan_table;
    const UINT16 *matrix;
    int mismatch;

    if (s->alternate_scan)
        scan_table = ff_alternate_vertical_scan;
    else
        scan_table = zigzag_direct;
    mismatch = 1;

    {
        int bit_cnt, v;
        UINT32 bit_buf;
        UINT8 *buf_ptr;
        i = 0;
        if (n < 4) 
            matrix = s->inter_matrix;
        else
            matrix = s->chroma_inter_matrix;
            
        /* special case for the first coef. no need to add a second vlc table */
        SAVE_BITS(&s->gb);
        SHOW_BITS(&s->gb, v, 2);
        if (v & 2) {
            run = 0;
            level = 1 - ((v & 1) << 1);
            FLUSH_BITS(2);
            RESTORE_BITS(&s->gb);
            goto add_coef;
        }
        RESTORE_BITS(&s->gb);
    }

    /* now quantify & encode AC coefs */
    for(;;) {
        code = get_vlc(&s->gb, &rl->vlc);
        if (code < 0)
            return -1;
        if (code == 112) {
            break;
        } else if (code == 111) {
            /* escape */
            run = get_bits(&s->gb, 6);
            level = get_bits(&s->gb, 12);
            level = (level << 20) >> 20;
        } else {
            run = rl->table_run[code];
            level = rl->table_level[code];
            if (get_bits1(&s->gb))
                level = -level;
        }
        i += run;
        if (i >= 64)
            return -1;
    add_coef:
	j = scan_table[i];
        dprintf("%d: run=%d level=%d\n", n, run, level);
        /* XXX: optimize */
        if (level > 0) {
            level = ((level * 2 + 1) * s->qscale * matrix[j]) >> 5;
        } else {
            level = ((-level * 2 + 1) * s->qscale * matrix[j]) >> 5;
            level = -level;
        }
        /* XXX: is it really necessary to saturate since the encoder
           knows whats going on ? */
        mismatch ^= level;
        block[j] = level;
        i++;
    }
    block[63] ^= (mismatch & 1);
    s->block_last_index[n] = i;
    return 0;
}

static int mpeg2_decode_block_intra(MpegEncContext *s, 
                                    DCTELEM *block, 
                                    int n)
{
    int level, dc, diff, i, j, run;
    int code, component;
    RLTable *rl;
    const UINT8 *scan_table;
    const UINT16 *matrix;
    int mismatch;

    if (s->alternate_scan)
        scan_table = ff_alternate_vertical_scan;
    else
        scan_table = zigzag_direct;

    /* DC coef */
    component = (n <= 3 ? 0 : n - 4 + 1);
    diff = decode_dc(s, component);
    if (diff >= 0xffff)
        return -1;
    dc = s->last_dc[component];
    dc += diff;
    s->last_dc[component] = dc;
    block[0] = dc << (3 - s->intra_dc_precision);
    dprintf("dc=%d\n", block[0]);
    mismatch = block[0] ^ 1;
    i = 1;
    if (s->intra_vlc_format)
        rl = &rl_mpeg2;
    else
        rl = &rl_mpeg1;
    if (n < 4) 
        matrix = s->intra_matrix;
    else
        matrix = s->chroma_intra_matrix;

    /* now quantify & encode AC coefs */
    for(;;) {
        code = get_vlc(&s->gb, &rl->vlc);
        if (code < 0)
            return -1;
        if (code == 112) {
            break;
        } else if (code == 111) {
            /* escape */
            run = get_bits(&s->gb, 6);
            level = get_bits(&s->gb, 12);
            level = (level << 20) >> 20;
        } else {
            run = rl->table_run[code];
            level = rl->table_level[code];
            if (get_bits1(&s->gb))
                level = -level;
        }
        i += run;
        if (i >= 64)
            return -1;
	j = scan_table[i];
        dprintf("%d: run=%d level=%d\n", n, run, level);
        level = (level * s->qscale * matrix[j]) / 16;
        /* XXX: is it really necessary to saturate since the encoder
           knows whats going on ? */
        mismatch ^= level;
        block[j] = level;
        i++;
    }
    block[63] ^= (mismatch & 1);
    s->block_last_index[n] = i;
    return 0;
}

/* compressed picture size */
#define PICTURE_BUFFER_SIZE 100000

typedef struct Mpeg1Context {
    MpegEncContext mpeg_enc_ctx;
    UINT32 header_state;
    int start_code; /* current start code */
    UINT8 buffer[PICTURE_BUFFER_SIZE]; 
    UINT8 *buf_ptr;
    int buffer_size;
    int mpeg_enc_ctx_allocated; /* true if decoding context allocated */
    int repeat_field; /* true if we must repeat the field */
} Mpeg1Context;

static int mpeg_decode_init(AVCodecContext *avctx)
{
    Mpeg1Context *s = avctx->priv_data;

    common_init(&s->mpeg_enc_ctx);

    s->header_state = 0xff;
    s->mpeg_enc_ctx_allocated = 0;
    s->buffer_size = PICTURE_BUFFER_SIZE;
    s->start_code = -1;
    s->buf_ptr = s->buffer;
    s->mpeg_enc_ctx.picture_number = 0;
    s->repeat_field = 0;
    s->mpeg_enc_ctx.codec_id= avctx->codec->id;
    avctx->mbskip_table= s->mpeg_enc_ctx.mbskip_table;
    s->mpeg_enc_ctx.flags= avctx->flags;
    return 0;
}

/* return the 8 bit start code value and update the search
   state. Return -1 if no start code found */
static int find_start_code(UINT8 **pbuf_ptr, UINT8 *buf_end, 
                           UINT32 *header_state)
{
    UINT8 *buf_ptr;
    unsigned int state, v;
    int val;

    state = *header_state;
    buf_ptr = *pbuf_ptr;
    while (buf_ptr < buf_end) {
        v = *buf_ptr++;
        if (state == 0x000001) {
            state = ((state << 8) | v) & 0xffffff;
            val = state;
            goto found;
        }
        state = ((state << 8) | v) & 0xffffff;
    }
    val = -1;
 found:
    *pbuf_ptr = buf_ptr;
    *header_state = state;
    return val;
}

static int mpeg1_decode_picture(AVCodecContext *avctx, 
                                UINT8 *buf, int buf_size)
{
    Mpeg1Context *s1 = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int ref, f_code;

    init_get_bits(&s->gb, buf, buf_size);

    ref = get_bits(&s->gb, 10); /* temporal ref */
    s->pict_type = get_bits(&s->gb, 3);
    dprintf("pict_type=%d number=%d\n", s->pict_type, s->picture_number);
    skip_bits(&s->gb, 16);
    if (s->pict_type == P_TYPE || s->pict_type == B_TYPE) {
        s->full_pel[0] = get_bits1(&s->gb);
        f_code = get_bits(&s->gb, 3);
        if (f_code == 0)
            return -1;
        s->mpeg_f_code[0][0] = f_code;
        s->mpeg_f_code[0][1] = f_code;
    }
    if (s->pict_type == B_TYPE) {
        s->full_pel[1] = get_bits1(&s->gb);
        f_code = get_bits(&s->gb, 3);
        if (f_code == 0)
            return -1;
        s->mpeg_f_code[1][0] = f_code;
        s->mpeg_f_code[1][1] = f_code;
    }
    s->y_dc_scale = 8;
    s->c_dc_scale = 8;
    s->first_slice = 1;
    return 0;
}

static void mpeg_decode_sequence_extension(MpegEncContext *s)
{
    int horiz_size_ext, vert_size_ext;
    int bit_rate_ext, vbv_buf_ext, low_delay;
    int frame_rate_ext_n, frame_rate_ext_d;

    skip_bits(&s->gb, 8); /* profil and level */
    s->progressive_sequence = get_bits1(&s->gb); /* progressive_sequence */
    skip_bits(&s->gb, 2); /* chroma_format */
    horiz_size_ext = get_bits(&s->gb, 2);
    vert_size_ext = get_bits(&s->gb, 2);
    s->width |= (horiz_size_ext << 12);
    s->height |= (vert_size_ext << 12);
    bit_rate_ext = get_bits(&s->gb, 12);  /* XXX: handle it */
    s->bit_rate = ((s->bit_rate / 400) | (bit_rate_ext << 12)) * 400;
    skip_bits1(&s->gb); /* marker */
    vbv_buf_ext = get_bits(&s->gb, 8);
    low_delay = get_bits1(&s->gb);
    frame_rate_ext_n = get_bits(&s->gb, 2);
    frame_rate_ext_d = get_bits(&s->gb, 5);
    if (frame_rate_ext_d >= 1)
        s->frame_rate = (s->frame_rate * frame_rate_ext_n) / frame_rate_ext_d;
    dprintf("sequence extension\n");
    s->mpeg2 = 1;
    s->avctx->sub_id = 2; /* indicates mpeg2 found */
}

static void mpeg_decode_quant_matrix_extension(MpegEncContext *s)
{
    int i, v, j;

    dprintf("matrix extension\n");

    if (get_bits1(&s->gb)) {
        for(i=0;i<64;i++) {
            v = get_bits(&s->gb, 8);
            j = zigzag_direct[i];
            s->intra_matrix[j] = v;
            s->chroma_intra_matrix[j] = v;
        }
    }
    if (get_bits1(&s->gb)) {
        for(i=0;i<64;i++) {
            v = get_bits(&s->gb, 8);
            j = zigzag_direct[i];
            s->inter_matrix[j] = v;
            s->chroma_inter_matrix[j] = v;
        }
    }
    if (get_bits1(&s->gb)) {
        for(i=0;i<64;i++) {
            v = get_bits(&s->gb, 8);
            j = zigzag_direct[i];
            s->chroma_intra_matrix[j] = v;
        }
    }
    if (get_bits1(&s->gb)) {
        for(i=0;i<64;i++) {
            v = get_bits(&s->gb, 8);
            j = zigzag_direct[i];
            s->chroma_inter_matrix[j] = v;
        }
    }
}

static void mpeg_decode_picture_coding_extension(MpegEncContext *s)
{
    s->full_pel[0] = s->full_pel[1] = 0;
    s->mpeg_f_code[0][0] = get_bits(&s->gb, 4);
    s->mpeg_f_code[0][1] = get_bits(&s->gb, 4);
    s->mpeg_f_code[1][0] = get_bits(&s->gb, 4);
    s->mpeg_f_code[1][1] = get_bits(&s->gb, 4);
    s->intra_dc_precision = get_bits(&s->gb, 2);
    s->picture_structure = get_bits(&s->gb, 2);
    s->top_field_first = get_bits1(&s->gb);
    s->frame_pred_frame_dct = get_bits1(&s->gb);
    s->concealment_motion_vectors = get_bits1(&s->gb);
    s->q_scale_type = get_bits1(&s->gb);
    s->intra_vlc_format = get_bits1(&s->gb);
    s->alternate_scan = get_bits1(&s->gb);
    s->repeat_first_field = get_bits1(&s->gb);
    s->chroma_420_type = get_bits1(&s->gb);
    s->progressive_frame = get_bits1(&s->gb);
    /* composite display not parsed */
    dprintf("intra_dc_precision=%d\n", s->intra_dc_precision);
    dprintf("picture_structure=%d\n", s->picture_structure);
    dprintf("top field first=%d\n", s->top_field_first);
    dprintf("repeat first field=%d\n", s->repeat_first_field);
    dprintf("conceal=%d\n", s->concealment_motion_vectors);
    dprintf("intra_vlc_format=%d\n", s->intra_vlc_format);
    dprintf("alternate_scan=%d\n", s->alternate_scan);
    dprintf("frame_pred_frame_dct=%d\n", s->frame_pred_frame_dct);
    dprintf("progressive_frame=%d\n", s->progressive_frame);
}

static void mpeg_decode_extension(AVCodecContext *avctx, 
                                  UINT8 *buf, int buf_size)
{
    Mpeg1Context *s1 = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int ext_type;

    init_get_bits(&s->gb, buf, buf_size);
    
    ext_type = get_bits(&s->gb, 4);
    switch(ext_type) {
    case 0x1:
        /* sequence ext */
        mpeg_decode_sequence_extension(s);
        break;
    case 0x3:
        /* quant matrix extension */
        mpeg_decode_quant_matrix_extension(s);
        break;
    case 0x8:
        /* picture extension */
        mpeg_decode_picture_coding_extension(s);
        break;
    }
}

/* return 1 if end of frame */
static int mpeg_decode_slice(AVCodecContext *avctx, 
                              AVPicture *pict,
                              int start_code,
                              UINT8 *buf, int buf_size)
{
    Mpeg1Context *s1 = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int ret;

    start_code = (start_code - 1) & 0xff;
    if (start_code >= s->mb_height)
        return -1;
    s->last_dc[0] = 1 << (7 + s->intra_dc_precision);
    s->last_dc[1] = s->last_dc[0];
    s->last_dc[2] = s->last_dc[0];
    memset(s->last_mv, 0, sizeof(s->last_mv));
    s->mb_x = -1;
    s->mb_y = start_code;
    s->mb_incr = 0;
    /* start frame decoding */
    if (s->first_slice) {
        s->first_slice = 0;
        MPV_frame_start(s);
    }

    init_get_bits(&s->gb, buf, buf_size);

    s->qscale = get_qscale(s);
    /* extra slice info */
    while (get_bits1(&s->gb) != 0) {
        skip_bits(&s->gb, 8);
    }

    for(;;) {
        clear_blocks(s->block[0]);
        emms_c();
        ret = mpeg_decode_mb(s, s->block);
        dprintf("ret=%d\n", ret);
        if (ret < 0)
            return -1;
        if (ret == 1)
            break;
        MPV_decode_mb(s, s->block);
    }
    emms_c();
    
    /* end of slice reached */
    if (s->mb_x == (s->mb_width - 1) &&
        s->mb_y == (s->mb_height - 1)) {
        /* end of image */
        UINT8 **picture;

        MPV_frame_end(s);

        /* XXX: incorrect reported qscale for mpeg2 */
        if (s->pict_type == B_TYPE) {
            picture = s->current_picture;
            avctx->quality = s->qscale;
        } else {
            /* latency of 1 frame for I and P frames */
            /* XXX: use another variable than picture_number */
            if (s->picture_number == 0) {
                picture = NULL;
            } else {
                picture = s->last_picture;
                avctx->quality = s->last_qscale;
            }
            s->last_qscale = s->qscale;
            s->picture_number++;
        }
        if (picture) {
            pict->data[0] = picture[0];
            pict->data[1] = picture[1];
            pict->data[2] = picture[2];
            pict->linesize[0] = s->linesize;
            pict->linesize[1] = s->linesize / 2;
            pict->linesize[2] = s->linesize / 2;
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static int mpeg1_decode_sequence(AVCodecContext *avctx, 
                                 UINT8 *buf, int buf_size)
{
    Mpeg1Context *s1 = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int width, height, i, v, j;
    
    init_get_bits(&s->gb, buf, buf_size);

    width = get_bits(&s->gb, 12);
    height = get_bits(&s->gb, 12);
    skip_bits(&s->gb, 4);
    s->frame_rate_index = get_bits(&s->gb, 4);
    if (s->frame_rate_index == 0)
        return -1;
    s->bit_rate = get_bits(&s->gb, 18) * 400;
    if (get_bits1(&s->gb) == 0) /* marker */
        return -1;
    if (width <= 0 || height <= 0 ||
        (width % 2) != 0 || (height % 2) != 0)
        return -1;
    if (width != s->width ||
        height != s->height) {
        /* start new mpeg1 context decoding */
        s->out_format = FMT_MPEG1;
        if (s1->mpeg_enc_ctx_allocated) {
            MPV_common_end(s);
        }
        s->width = width;
        s->height = height;
        s->has_b_frames = 1;
        s->avctx = avctx;
        avctx->width = width;
        avctx->height = height;
        if (s->frame_rate_index >= 9) {
            /* at least give a valid frame rate (some old mpeg1 have this) */
            avctx->frame_rate = 25 * FRAME_RATE_BASE;
        } else {
        avctx->frame_rate = frame_rate_tab[s->frame_rate_index];
        }
        s->frame_rate = avctx->frame_rate;
        avctx->bit_rate = s->bit_rate;
        
        if (MPV_common_init(s) < 0)
            return -1;
        mpeg1_init_vlc(s);
        s1->mpeg_enc_ctx_allocated = 1;
    }

    skip_bits(&s->gb, 10); /* vbv_buffer_size */
    skip_bits(&s->gb, 1);

    /* get matrix */
    if (get_bits1(&s->gb)) {
        for(i=0;i<64;i++) {
            v = get_bits(&s->gb, 8);
            j = zigzag_direct[i];
            s->intra_matrix[j] = v;
            s->chroma_intra_matrix[j] = v;
        }
#ifdef DEBUG
        dprintf("intra matrix present\n");
        for(i=0;i<64;i++)
            dprintf(" %d", s->intra_matrix[zigzag_direct[i]]);
        printf("\n");
#endif
    } else {
        for(i=0;i<64;i++) {
            v = default_intra_matrix[i];
            s->intra_matrix[i] = v;
            s->chroma_intra_matrix[i] = v;
        }
    }
    if (get_bits1(&s->gb)) {
        for(i=0;i<64;i++) {
            v = get_bits(&s->gb, 8);
            j = zigzag_direct[i];
            s->inter_matrix[j] = v;
            s->chroma_inter_matrix[j] = v;
        }
#ifdef DEBUG
        dprintf("non intra matrix present\n");
        for(i=0;i<64;i++)
            dprintf(" %d", s->inter_matrix[zigzag_direct[i]]);
        printf("\n");
#endif
    } else {
        for(i=0;i<64;i++) {
            v = default_non_intra_matrix[i];
            s->inter_matrix[i] = v;
            s->chroma_inter_matrix[i] = v;
        }
    }

    /* we set mpeg2 parameters so that it emulates mpeg1 */
    s->progressive_sequence = 1;
    s->progressive_frame = 1;
    s->picture_structure = PICT_FRAME;
    s->frame_pred_frame_dct = 1;
    s->mpeg2 = 0;
    avctx->sub_id = 1; /* indicates mpeg1 */
    return 0;
}

/* handle buffering and image synchronisation */
static int mpeg_decode_frame(AVCodecContext *avctx, 
                             void *data, int *data_size,
                             UINT8 *buf, int buf_size)
{
    Mpeg1Context *s = avctx->priv_data;
    UINT8 *buf_end, *buf_ptr, *buf_start;
    int len, start_code_found, ret, code, start_code, input_size;
    AVPicture *picture = data;
    MpegEncContext *s2 = &s->mpeg_enc_ctx;
            
    dprintf("fill_buffer\n");

    *data_size = 0;
    
    /* special case for last picture */
    if (buf_size == 0) {
        if (s2->picture_number > 0) {
            picture->data[0] = s2->next_picture[0];
            picture->data[1] = s2->next_picture[1];
            picture->data[2] = s2->next_picture[2];
            picture->linesize[0] = s2->linesize;
            picture->linesize[1] = s2->linesize / 2;
            picture->linesize[2] = s2->linesize / 2;
            *data_size = sizeof(AVPicture);
        }
        return 0;
    }

    buf_ptr = buf;
    buf_end = buf + buf_size;
    
#if 0    
    if (s->repeat_field % 2 == 1) { 
        s->repeat_field++;
        //fprintf(stderr,"\nRepeating last frame: %d -> %d! pict: %d %d", avctx->frame_number-1, avctx->frame_number,
        //                                                         s2->picture_number, s->repeat_field);
        if (avctx->flags & CODEC_FLAG_REPEAT_FIELD) {
        *data_size = sizeof(AVPicture);
        goto the_end;
    }
    }
#endif
    while (buf_ptr < buf_end) {
        buf_start = buf_ptr;
        /* find start next code */
        code = find_start_code(&buf_ptr, buf_end, &s->header_state);
        if (code >= 0) {
            start_code_found = 1;
        } else {
            start_code_found = 0;
        }
        /* copy to buffer */
        len = buf_ptr - buf_start;
        if (len + (s->buf_ptr - s->buffer) > s->buffer_size) {
            /* data too big : flush */
            s->buf_ptr = s->buffer;
            if (start_code_found)
                s->start_code = code;
        } else {
            memcpy(s->buf_ptr, buf_start, len);
            s->buf_ptr += len;
            
            if (start_code_found) {
                /* prepare data for next start code */
                input_size = s->buf_ptr - s->buffer;
                start_code = s->start_code;
                s->buf_ptr = s->buffer;
                s->start_code = code;
                switch(start_code) {
                case SEQ_START_CODE:
                    mpeg1_decode_sequence(avctx, s->buffer, 
                                          input_size);
                    break;
                            
                case PICTURE_START_CODE:
                    /* we have a complete image : we try to decompress it */
                    mpeg1_decode_picture(avctx, 
                                         s->buffer, input_size);
                    break;
                case EXT_START_CODE:
                    mpeg_decode_extension(avctx,
                                          s->buffer, input_size);
                    break;
                default:
                    if (start_code >= SLICE_MIN_START_CODE &&
                        start_code <= SLICE_MAX_START_CODE) {
                        ret = mpeg_decode_slice(avctx, picture,
                                                start_code, s->buffer, input_size);
                        if (ret == 1) {
                            /* got a picture: exit */
                            /* first check if we must repeat the frame */
                            avctx->repeat_pict = 0;
#if 0
                            if (s2->progressive_frame && s2->repeat_first_field) {
                                //fprintf(stderr,"\nRepeat this frame: %d! pict: %d",avctx->frame_number,s2->picture_number);
                                //s2->repeat_first_field = 0;
                                //s2->progressive_frame = 0;
                                if (++s->repeat_field > 2)
                                    s->repeat_field = 0;
                                avctx->repeat_pict = 1;
                            }
#endif                      
                            if (s2->repeat_first_field) {
                                if (s2->progressive_sequence) {
                                    if (s2->top_field_first)
                                        avctx->repeat_pict = 4;
                                    else
                                        avctx->repeat_pict = 2;
                                } else if (s2->progressive_frame) {
                                    avctx->repeat_pict = 1;
                                }
                            }
                            *data_size = sizeof(AVPicture);
                            goto the_end;
                        }
                    }
                    break;
                }
            }
        }
    }
 the_end:
    return buf_ptr - buf;
}

static int mpeg_decode_end(AVCodecContext *avctx)
{
    Mpeg1Context *s = avctx->priv_data;

    if (s->mpeg_enc_ctx_allocated)
        MPV_common_end(&s->mpeg_enc_ctx);
    return 0;
}

AVCodec mpeg_decoder = {
    "mpegvideo",
    CODEC_TYPE_VIDEO,
    CODEC_ID_MPEG1VIDEO,
    sizeof(Mpeg1Context),
    mpeg_decode_init,
    NULL,
    mpeg_decode_end,
    mpeg_decode_frame,
};
