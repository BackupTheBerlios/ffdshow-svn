/*
    libfame - Fast Assembly MPEG Encoder Library
    Copyright (C) 2000-2001 Vivien Chappelier

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/*************************** floating point DCT/iDCT *************************/

#define IDCT1 1.414213562
#define IDCT2 2.613125930
#define IDCT3 1.082392200
#define IDCT4 0.765366865

/*  idct_aan_row                                                             */
/*                                                                           */
/*  Description:                                                             */
/*    Perform the second step a 1-D AA&N iDCT on a row of a block            */
/*                                                                           */
/*  Arguments:                                                               */
/*    dct_t *block: a 8x8 block                                              */
/*    int row: which row to perform the 1-D iDCT on                          */
/*                                                                           */
/*  Return value:                                                            */
/*    None.                                                                  */
typedef short dct_t;
static void __inline idct_aan_row(dct_t * block, int row)
{
   dct_t v0, v1, v2, v3, v4, v5, v6, v7;
   dct_t v04,v05;
   dct_t v11,v12,v13,v15;
   dct_t v22,v23,v24,v25;
   dct_t v31,v32,v35;
   dct_t v44,v45;
   dct_t v51,v53,v54,v55;
   dct_t v62,v64,v65;
   dct_t v71,v73,v74,v75;
   dct_t va2, va3;

   v0 = block[row*8+0];
   v1 = block[row*8+1];
   v2 = block[row*8+2];
   v3 = block[row*8+3];
   v4 = block[row*8+4];
   v5 = block[row*8+5];
   v6 = block[row*8+6];
   v7 = block[row*8+7];

   /* even part */
   v22 = v2-v6;
   v62 = v2+v6;
   v23 = v22*IDCT1;
   v04 = v0+v4;
   v44 = v0-v4;
   v24 = v23-v62;
   v64 = v62;
   v05 = v04+v64;
   v45 = v44+v24;
   v25 = v44-v24;
   v65 = v04-v64;

   /* odd part */
   v51 = v5-v3;
   v11 = v1+v7;
   v71 = v1-v7;
   v31 = v5+v3;

   v12 = v11-v31;
   v32 = v11+v31;
   va2 = v51-v71;

   v53 = v51*IDCT2;
   v13 = v12*IDCT1;
   v73 = v71*IDCT3;
   va3 = va2*IDCT4;

   v54 = va3-v53;
   v74 = v73-va3;

   v35 = v32;
   v75 = v74-v35;
   v15 = v13-v75;
   v55 = (v54+v15);

   /* output butterfly */      					
   block[row*8+0] = v05+v35;
   block[row*8+1] = v45+v75;
   block[row*8+2] = v25+v15;
   block[row*8+3] = v65-v55;
   block[row*8+4] = v65+v55;
   block[row*8+5] = v25-v15;
   block[row*8+6] = v45-v75;
   block[row*8+7] = v05-v35;
}

/*  idct_aan_col                                                             */
/*                                                                           */
/*  Description:                                                             */
/*    Perform the second step a 1-D AA&N iDCT on a column of a block         */
/*                                                                           */
/*  Arguments:                                                               */
/*    dct_t *block: a 8x8 block                                              */
/*    int col: which column to perform the 1-D iDCT on                       */
/*                                                                           */
/*  Return value:                                                            */
/*    None.                                                                  */

static void __inline idct_aan_col(dct_t * block, int col)
{
   dct_t v0, v1, v2, v3, v4, v5, v6, v7;
   dct_t v04,v05;
   dct_t v11,v12,v13,v15;
   dct_t v22,v23,v24,v25;
   dct_t v31,v32,v35;
   dct_t v44,v45;
   dct_t v51,v53,v54,v55;
   dct_t v62,v64,v65;
   dct_t v71,v73,v74,v75;
   dct_t va2, va3;

   v0 = block[0*8+col];
   v1 = block[1*8+col];
   v2 = block[2*8+col];
   v3 = block[3*8+col];
   v4 = block[4*8+col];
   v5 = block[5*8+col];
   v6 = block[6*8+col];
   v7 = block[7*8+col];

   /* even part */
   v22 = v2-v6;
   v62 = v2+v6;
   v23 = v22*IDCT1;
   v04 = v0+v4;
   v44 = v0-v4;
   v24 = v23-v62;
   v64 = v62;
   v05 = v04+v64;
   v45 = v44+v24;
   v25 = v44-v24;
   v65 = v04-v64;

   /* odd part */
   v51 = v5-v3;
   v11 = v1+v7;
   v71 = v1-v7;
   v31 = v5+v3;

   v12 = v11-v31;
   v32 = v11+v31;
   va2 = v51-v71;

   v53 = v51*IDCT2;
   v13 = v12*IDCT1;
   v73 = v71*IDCT3;
   va3 = va2*IDCT4;

   v54 = va3-v53;
   v74 = v73-va3;

   v35 = v32;
   v75 = v74-v35;
   v15 = v13-v75;
   v55 = (v54+v15);

   /* output butterfly */      					
   block[0*8+col] = v05+v35;
   block[1*8+col] = v45+v75;
   block[2*8+col] = v25+v15;
   block[3*8+col] = v65-v55;
   block[4*8+col] = v65+v55;
   block[5*8+col] = v25-v15;
   block[6*8+col] = v45-v75;
   block[7*8+col] = v05-v35;
}


/*  idct                                                                     */
/*                                                                           */
/*  Description:                                                             */
/*    Perform the second step of a 2-D AA&N iDCT on a 8x8 block              */
/*                                                                           */
/*  Arguments:                                                               */
/*    dct_t *block: a 8x8 block                                              */
/*                                                                           */
/*  Return value:                                                            */
/*    None.                                                                  */

void idct_famefloat(dct_t *block)
{
   int i;

   /* TODO : clipping (-2048,2048) */
   for(i = 0; i < 8; i++)
     idct_aan_row(block, i);
   for(i = 0; i < 8; i++)
     idct_aan_col(block, i);
 __asm {emms};

}
