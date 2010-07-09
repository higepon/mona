/* Test mpz_perfect_power_p.

Copyright 2008 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

struct
{
  char *num_as_str;
  char want;
} tests[] =
  {
    { "0", 1},
    { "1", 1},
    {"-1", 1},
    { "2", 0},
    {"-2", 0},
    { "3", 0},
    {"-3", 0},
    { "4", 1},
    {"-4", 0},
    { "64", 1},
    {"-64", 1},
    { "128", 1},
    {"-128", 1},
    { "256", 1},
    {"-256", 0},
    { "512", 1},
    {"-512", 1},
    { "0x4000000", 1},
    {"-0x4000000", 1},
    { "0x3cab640", 1},
    {"-0x3cab640", 0},
    { "0x3e23840", 1},
    {"-0x3e23840", 0},
    { "0x3d3a7ed1", 1},
    {"-0x3d3a7ed1", 1},
    { "0x30a7a6000", 1},
    {"-0x30a7a6000", 1},
    { "0xf33e5a5a59", 1},
    {"-0xf33e5a5a59", 0},
    { "0xed1b1182118135d", 1},
    {"-0xed1b1182118135d", 1},
    { "0xe71f6eb7689cc276b2f1", 1},
    {"-0xe71f6eb7689cc276b2f1", 0},
    { "0x12644507fe78cf563a4b342c92e7da9fe5e99cb75a01", 1},
    {"-0x12644507fe78cf563a4b342c92e7da9fe5e99cb75a01", 0},
    { "0x1ff2e7c581bb0951df644885bd33f50e472b0b73a204e13cbe98fdb424d66561e4000000", 1},
    {"-0x1ff2e7c581bb0951df644885bd33f50e472b0b73a204e13cbe98fdb424d66561e4000000", 1},
    { "0x2b9b44db2d91a6f8165c8c7339ef73633228ea29e388592e80354e4380004aad84000000", 1},
    {"-0x2b9b44db2d91a6f8165c8c7339ef73633228ea29e388592e80354e4380004aad84000000", 1},
    { "0x28d5a2b8f330910a9d3cda06036ae0546442e5b1a83b26a436efea5b727bf1bcbe7e12b47d81", 1},
    {"-0x28d5a2b8f330910a9d3cda06036ae0546442e5b1a83b26a436efea5b727bf1bcbe7e12b47d81", 1},
    {NULL, 0}
  };


void
check_tests ()
{
  mpz_t x;
  int i;
  int got, want;

  mpz_init (x);

  for (i = 0; tests[i].num_as_str != NULL; i++)
    {
      mpz_set_str (x, tests[i].num_as_str, 0);
      got = mpz_perfect_power_p (x);
      want = tests[i].want;
      if (got != want)
	{
	  fprintf (stderr, "mpz_perfect_power_p returns %d when %d was expected\n", got, want);
	  fprintf (stderr, "fault operand: %s\n", tests[i].num_as_str);
	  abort ();
	}
    }

  mpz_clear (x);
}

void
check_random (int n_tests)
{
  int test;
  mpz_t bs, op1, op2, x;
  gmp_randstate_ptr rands;
  unsigned long ex;

  rands = RANDS;

  mpz_init (bs);
  mpz_init (op1);
  mpz_init (op2);
  mpz_init (x);

  for (test = 0; test < n_tests; test++)
    {
      mpz_urandomb (bs, rands, 32);
      mpz_rrandomb (op1, rands, mpz_get_ui (bs) % 32);
      if (test & 1)
	mpz_neg (op1, op1);
      mpz_rrandomb (op2, rands, (mpz_get_ui (bs) >> 5) % 8);

      ex = mpz_get_ui (op2) + 2;
      mpz_pow_ui (x, op1, ex);

      if (! mpz_perfect_power_p (x))
	{
	  gmp_fprintf (stderr, "mpz_perfect_power_p rejects perfect power %Zd^%ld\n", op1, ex);
	  gmp_fprintf (stderr, "fault operand: %Zx\n", x);
	  abort ();
	}
    }

  mpz_clear (bs);
  mpz_clear (op1);
  mpz_clear (op2);
  mpz_clear (x);
}

int
main (int argc, char **argv)
{
  int n_tests;

  tests_start ();
  mp_trace_base = -16;

  check_tests ();

  n_tests = 100000;
  if (argc == 2)
    n_tests = atoi (argv[1]);
  check_random (n_tests);

  tests_end ();
  exit (0);
}
