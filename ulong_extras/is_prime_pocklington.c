/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2008 Peter Shrimpton
    Copyright (C) 2009 William Hart
    Copyright (C) 2015 Kushagra Singh

******************************************************************************/
#include <gmp.h>
#define ulong ulongxx /* interferes with system includes */
#include <math.h>
#include <stdlib.h>
#undef ulong
#define ulong mp_limb_t
#include "flint.h"
#include "ulong_extras.h"

int
n_is_prime_pocklington(mp_limb_t n, ulong iterations)
{
    int i, j, pass;
    mp_limb_t n1, cofactor, b, c = 0, ninv, limit, F, Fsq, det, rootn;
    mp_limb_t c1, c2;
    n_factor_t factors;

    if (n == 1)
        return 0;

    if (n % 2 == 0)
        return (n == UWORD(2));

    rootn = n_sqrt(n);                      /* floor(sqrt(n)) */

    if (n == rootn*rootn)
        return 0;

    n1 = n - 1;
    n_factor_init(&factors);
    limit = (mp_limb_t) cbrt((double) n1);

    if (n>9007199254740992)                 /* if n exceeds 2^53, limit is increased by 1 as */    
        limit+=1;                           /* the mantissa of n will be smaller than the actual value */
                                            /* as double used in cbrt has only 53 bits of mantissa */

    cofactor = n_factor_partial(&factors, n1, limit, 1);

    if (cofactor != 1)                      /* check that cofactor is coprime to factors found */
    {
        for (i = 0; i < factors.num; i++)
        {
            if (factors.p[i] > FLINT_FACTOR_TRIAL_PRIMES_PRIME)
            {
                while (cofactor >= factors.p[i] && (cofactor % factors.p[i]) == 0)
                {
                    factors.exp[i]++;
                    cofactor /= factors.p[i];
                }
            }
        }
    }
    F = n1/cofactor;                        /* n1 = F*cofactor */
    Fsq = F*F;

    if (F <= rootn)                         /* cube root method applicable only if n^1/3 <= F < n^1/2 */
    {   
        c2 = n1/(Fsq);                      /* expressing n as c2*F^2 + c1*F + 1  */
        c1 = (n1 - c2*Fsq )/F;
        det = c1*c1 - 4*c2; 
        if (n_is_square(det))               /* BSL's test for (n^1/3 <= F < n^1/2) */
            return 0;
    }
    ninv = n_preinvert_limb(n);
    c = 1;
    for (i = factors.num - 1; i >= 0; i--)
    {
        mp_limb_t exp = n1 / factors.p[i];
        pass = 0;

        for (j = 2; j < iterations && pass == 0; j++)
        {
            b = n_powmod2_preinv(j, exp, n, ninv);
            if (n_powmod2_ui_preinv(b, factors.p[i], n, ninv) != UWORD(1))
                return 0;

            b = n_submod(b, UWORD(1), n);
            if (b != UWORD(0))
            {
                c = n_mulmod2_preinv(c, b, n, ninv);
                pass = 1;
            }
            if (c == 0)
                return 0;
        }
        if (j == iterations)
            return -1;
    }
    return (n_gcd(n, c) == UWORD(1));
}
