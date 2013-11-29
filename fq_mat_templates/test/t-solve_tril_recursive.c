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

    Copyright (C) 2010,2011 Fredrik Johansson
    Copyright (C) 2013 Mike Hansen

******************************************************************************/


#ifdef T

#include "templates.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int
main(void)
{
    slong i;
    FLINT_TEST_INIT(state);
    
    printf("solve_tril_recursive....");
    fflush(stdout);

    for (i = 0; i < 5 * flint_test_multiplier(); i++)
    {
        TEMPLATE(T, ctx_t) ctx;
        TEMPLATE(T, mat_t) A, X, B, Y;
        
        slong rows, cols;
        int unit;

        TEMPLATE(T, ctx_randtest)(ctx, state);
        
        rows = n_randint(state, 50);
        cols = n_randint(state, 50);
        unit = n_randint(state, 2);

        TEMPLATE(T, mat_init)(A, rows, rows, ctx);
        TEMPLATE(T, mat_init)(B, rows, cols, ctx);
        TEMPLATE(T, mat_init)(X, rows, cols, ctx);
        TEMPLATE(T, mat_init)(Y, rows, cols, ctx);

        TEMPLATE(T, mat_randtril)(A, state, unit, ctx);
        TEMPLATE(T, mat_randtest)(X, state, ctx);
        TEMPLATE(T, mat_mul)(B, A, X, ctx);

        /* Check Y = A^(-1) * (A * X) = X */
        TEMPLATE(T, mat_solve_tril_recursive)(Y, A, B, unit, ctx);
        if (!TEMPLATE(T, mat_equal)(Y, X, ctx))
        {
            printf("FAIL!\n");
            printf("A:\n");
            TEMPLATE(T, mat_print_pretty)(A, ctx);
            printf("X:\n");
            TEMPLATE(T, mat_print_pretty)(X, ctx);
            printf("B:\n");
            TEMPLATE(T, mat_print_pretty)(B, ctx);
            printf("Y:\n");
            TEMPLATE(T, mat_print_pretty)(Y, ctx);
            abort();
        }

        /* Check aliasing */
        TEMPLATE(T, mat_solve_tril_recursive)(B, A, B, unit, ctx);
        if (!TEMPLATE(T, mat_equal)(B, X, ctx))
        {
            printf("FAIL!\n");
            printf("aliasing test failed");
            printf("A:\n");
            TEMPLATE(T, mat_print_pretty)(A, ctx);
            printf("B:\n");
            TEMPLATE(T, mat_print_pretty)(B, ctx);
            abort();
        }

        TEMPLATE(T, mat_clear)(A, ctx);
        TEMPLATE(T, mat_clear)(B, ctx);
        TEMPLATE(T, mat_clear)(X, ctx);
        TEMPLATE(T, mat_clear)(Y, ctx);

        TEMPLATE(T, ctx_clear)(ctx);
    }

    FLINT_TEST_CLEANUP(state);
    printf("PASS\n");
    return 0;
}

#endif