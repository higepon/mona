/* Minimal values gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2006, 2008 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

/* The values in this file are not currently minimal.
   Trimming them further would be good.  */

#define MUL_KARATSUBA_THRESHOLD           8
#define MUL_TOOM3_THRESHOLD              20
#define MUL_TOOM44_THRESHOLD             24

#define SQR_BASECASE_THRESHOLD            0
#define SQR_KARATSUBA_THRESHOLD           8
#define SQR_TOOM3_THRESHOLD              20
#define SQR_TOOM4_THRESHOLD              24

#define MULLOW_BASECASE_THRESHOLD         0
#define MULLOW_DC_THRESHOLD               2
#define MULLOW_MUL_N_THRESHOLD            4

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                  6
#define POWM_THRESHOLD                    4

#define HGCD_THRESHOLD                   10
#define GCD_DC_THRESHOLD                 20
#define GCDEXT_SCHOENHAGE_THRESHOLD      20
#define JACOBI_BASE_METHOD                1

#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define USE_PREINV_DIVREM_1               1  /* native */
#define USE_PREINV_MOD_1                  1
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always (native) */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always (native) */

#define GET_STR_DC_THRESHOLD              4
#define GET_STR_PRECOMPUTE_THRESHOLD     10
#define SET_STR_THRESHOLD                64
#define SET_STR_PRECOMPUTE_THRESHOLD    100

#define MUL_FFT_TABLE  {64-1, 256-1, 1024-1, 4096-1, 8192-1, 65536-1, 0}
#define MUL_FFT_MODF_THRESHOLD  65
#define MUL_FFT_THRESHOLD      200

#define SQR_FFT_TABLE  {64-1, 256-1, 1024-1, 4096-1, 8192-1, 65536-1, 0}
#define SQR_FFT_MODF_THRESHOLD  65
#define SQR_FFT_THRESHOLD      200
