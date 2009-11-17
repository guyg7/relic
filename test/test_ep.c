/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007, 2008, 2009 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Tests for the prime elliptic curve arithmetic module.
 *
 * @version $Id$
 * @ingroup test
 */

#include <stdio.h>

#include "relic.h"
#include "relic_test.h"

void ep_new_impl(ep_t a) {
	ep_new(a);
}

static int memory(void) {
	err_t e;
	int code = STS_ERR;
	ep_t a;

	ep_null(a);

	TRY {
		TEST_BEGIN("memory can be allocated") {
			ep_new(a);
			ep_free(a);
		} TEST_END;
	} CATCH(e) {
		switch (e) {
			case ERR_NO_MEMORY:
				util_print("FATAL ERROR!\n");
				ERROR(end);
				break;
		}
	}
	(void)a;
	code = STS_OK;
  end:
	return code;
}

int util(void) {
	int code = STS_ERR;
	ep_t a, b, c;

	ep_null(a);
	ep_null(b);
	ep_null(c);

	TRY {
		ep_new(a);
		ep_new(b);
		ep_new(c);

		TEST_BEGIN("comparison is consistent") {
			ep_rand(a);
			ep_rand(b);
			TEST_ASSERT(ep_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("copy and comparison are consistent") {
			ep_rand(a);
			ep_rand(b);
			if (ep_cmp(a, c) != CMP_EQ) {
				ep_copy(c, a);
				TEST_ASSERT(ep_cmp(c, a) == CMP_EQ, end);
			}
			if (ep_cmp(b, c) != CMP_EQ) {
				ep_copy(c, b);
				TEST_ASSERT(ep_cmp(b, c) == CMP_EQ, end);
			}
		}
		TEST_END;

		TEST_BEGIN("negation and comparison are consistent") {
			ep_rand(a);
			ep_neg(b, a);
			TEST_ASSERT(ep_cmp(a, b) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN
				("assignment to random/infinity and comparison are consistent")
		{
			ep_rand(a);
			ep_set_infty(c);
			TEST_ASSERT(ep_cmp(a, c) != CMP_EQ, end);
			TEST_ASSERT(ep_cmp(c, a) != CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to infinity and infinity test are consistent") {
			ep_set_infty(a);
			TEST_ASSERT(ep_is_infty(a), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	ep_free(a);
	ep_free(b);
	ep_free(c);
	return code;
}

int addition(void) {
	int code = STS_ERR;

	ep_t a, b, c, d, e;

	ep_null(a);
	ep_null(b);
	ep_null(c);
	ep_null(d);
	ep_null(e);

	TRY {
		ep_new(a);
		ep_new(b);
		ep_new(c);
		ep_new(d);
		ep_new(e);

		TEST_BEGIN("point addition is commutative") {
			ep_rand(a);
			ep_rand(b);
			ep_add(d, a, b);
			ep_add(e, b, a);
			ep_norm(d, d);
			ep_norm(e, e);
			TEST_ASSERT(ep_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition is associative") {
			ep_rand(a);
			ep_rand(b);
			ep_rand(c);
			ep_add(d, a, b);
			ep_add(d, d, c);
			ep_add(e, b, c);
			ep_add(e, e, a);
			ep_norm(d, d);
			ep_norm(e, e);
			TEST_ASSERT(ep_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition has identity") {
			ep_rand(a);
			ep_set_infty(d);
			ep_add(e, a, d);
			TEST_ASSERT(ep_cmp(e, a) == CMP_EQ, end);
			ep_add(e, d, a);
			TEST_ASSERT(ep_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition has inverse") {
			ep_rand(a);
			ep_neg(d, a);
			ep_add(e, a, d);
			TEST_ASSERT(ep_is_infty(e), end);
		} TEST_END;

#if EP_ADD == BASIC || !defined(STRIP)
		TEST_BEGIN("point addition in affine coordinates is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_add(d, a, b);
			ep_norm(d, d);
			ep_add_basic(e, a, b);
			TEST_ASSERT(ep_cmp(e, d) == CMP_EQ, end);
		} TEST_END;
#endif

#if EP_ADD == PROJC || !defined(STRIP)
		TEST_BEGIN("point addition in projective coordinates is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_add_projc(a, a, b);
			ep_rand(b);
			ep_rand(c);
			ep_add_projc(b, b, c);
			/* a and b in projective coordinates. */
			ep_add_projc(d, a, b);
			ep_norm(d, d);
			ep_norm(a, a);
			ep_norm(b, b);
			ep_add(e, a, b);
			ep_norm(e, e);
			TEST_ASSERT(ep_cmp(e, d) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition in mixed coordinates (z2 = 1) is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_add_projc(a, a, b);
			ep_rand(b);
			/* a and b in projective coordinates. */
			ep_add_projc(d, a, b);
			ep_norm(d, d);
			/* a in affine coordinates. */
			ep_norm(a, a);
			ep_add(e, a, b);
			ep_norm(e, e);
			TEST_ASSERT(ep_cmp(e, d) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point addition in mixed coordinates (z1,z2 = 1) is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_norm(a, a);
			ep_norm(b, b);
			/* a and b in affine coordinates. */
			ep_add(d, a, b);
			ep_norm(d, d);
			ep_add_projc(e, a, b);
			ep_norm(e, e);
			TEST_ASSERT(ep_cmp(e, d) == CMP_EQ, end);
		} TEST_END;
#endif

	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	ep_free(a);
	ep_free(b);
	ep_free(c);
	ep_free(d);
	ep_free(e);
	return code;
}

int subtraction(void) {
	int code = STS_ERR;
	ep_t a, b, c, d;

	ep_null(a);
	ep_null(b);
	ep_null(c);
	ep_null(d);

	TRY {
		ep_new(a);
		ep_new(b);
		ep_new(c);
		ep_new(d);

		TEST_BEGIN("point subtraction is anti-commutative") {
			ep_rand(a);
			ep_rand(b);
			ep_sub(c, a, b);
			ep_sub(d, b, a);
			ep_norm(c, c);
			ep_norm(d, d);
			ep_neg(d, d);
			TEST_ASSERT(ep_cmp(c, d) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("point subtraction has identity") {
			ep_rand(a);
			ep_set_infty(c);
			ep_sub(d, a, c);
			ep_norm(d, d);
			TEST_ASSERT(ep_cmp(d, a) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("point subtraction has inverse") {
			ep_rand(a);
			ep_sub(c, a, a);
			ep_norm(c, c);
			TEST_ASSERT(ep_is_infty(c), end);
		}
		TEST_END;

#if EB_ADD == BASIC || !defined(STRIP)
		TEST_BEGIN("point subtraction in affine coordinates is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_sub(c, a, b);
			ep_norm(c, c);
			ep_sub_basic(d, a, b);
			TEST_ASSERT(ep_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif

#if EB_ADD == PROJC || !defined(STRIP)
		TEST_BEGIN("point subtraction in projective coordinates is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_add_projc(a, a, b);
			ep_rand(b);
			ep_rand(c);
			ep_add_projc(b, b, c);
			/* a and b in projective coordinates. */
			ep_sub_projc(c, a, b);
			ep_norm(c, c);
			ep_norm(a, a);
			ep_norm(b, b);
			ep_sub(d, a, b);
			ep_norm(d, d);
			TEST_ASSERT(ep_cmp(c, d) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point subtraction in mixed coordinates (z2 = 1) is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_add_projc(a, a, b);
			ep_rand(b);
			/* a and b in projective coordinates. */
			ep_sub_projc(c, a, b);
			ep_norm(c, c);
			/* a in affine coordinates. */
			ep_norm(a, a);
			ep_sub(d, a, b);
			ep_norm(d, d);
			TEST_ASSERT(ep_cmp(c, d) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point subtraction in mixed coordinates (z1,z2 = 1) is correct") {
			ep_rand(a);
			ep_rand(b);
			ep_norm(a, a);
			ep_norm(b, b);
			/* a and b in affine coordinates. */
			ep_sub(c, a, b);
			ep_norm(c, c);
			ep_sub_projc(d, a, b);
			ep_norm(d, d);
			TEST_ASSERT(ep_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	ep_free(a);
	ep_free(b);
	ep_free(c);
	ep_free(d);
	return code;
}

int doubling(void) {
	int code = STS_ERR;
	ep_t a, b, c;

	ep_null(a);
	ep_null(b);
	ep_null(c);

	TRY {
		ep_new(a);
		ep_new(b);
		ep_new(c);

		TEST_BEGIN("point doubling is correct") {
			ep_rand(a);
			ep_add(b, a, a);
			ep_norm(b, b);
			ep_dbl(c, a);
			ep_norm(c, c);
			TEST_ASSERT(ep_cmp(b, c) == CMP_EQ, end);
		} TEST_END;

#if EP_ADD == BASIC || !defined(STRIP)
		TEST_BEGIN("point doubling in affine coordinates is correct") {
			ep_rand(a);
			ep_dbl(b, a);
			ep_norm(b, b);
			ep_dbl_basic(c, a);
			TEST_ASSERT(ep_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

#if EP_ADD == PROJC || !defined(STRIP)
		TEST_BEGIN("point doubling in projective coordinates is correct") {
			ep_rand(a);
			ep_dbl_projc(a, a);
			/* a in projective coordinates. */
			ep_dbl_projc(b, a);
			ep_norm(b, b);
			ep_norm(a, a);
			ep_dbl(c, a);
			ep_norm(c, c);
			TEST_ASSERT(ep_cmp(b, c) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("point doubling in mixed coordinates (z1 = 1) is correct") {
			ep_rand(a);
			ep_dbl_projc(b, a);
			ep_norm(b, b);
			ep_dbl(c, a);
			ep_norm(c, c);
			TEST_ASSERT(ep_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	ep_free(a);
	ep_free(b);
	ep_free(c);
	return code;
}

static int multiplication(void) {
	int code = STS_ERR;
	ep_t p, q, r;
	bn_t n, k;

	bn_null(n);
	bn_null(k);
	ep_null(p);
	ep_null(q);
	ep_null(r);

	TRY {
		ep_new(q);
		ep_new(r);
		bn_new(k);

		p = ep_curve_get_gen();
		n = ep_curve_get_ord();

		TEST_BEGIN("generator has the right order") {
			ep_mul(r, p, n);
			TEST_ASSERT(ep_is_infty(r) == 1, end);
		} TEST_END;

		TEST_BEGIN("generator multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_gen(r, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;

#if EP_MUL == BASIC || !defined(STRIP)
		TEST_BEGIN("binary point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_basic(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
#endif

#if EP_MUL == WTNAF || !defined(STRIP)
		TEST_BEGIN("w(t)naf point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_wtnaf(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		}
		TEST_END;
#endif

	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	ep_free(q);
	ep_free(r);
	bn_free(k);
	return code;
}

static int fixed(void) {
	int code = STS_ERR;
	ep_t p, q, r;
	ep_t t[EP_TABLE_BASIC];
	bn_t n, k;

	bn_null(n);
	bn_null(k);
	ep_null(p);
	ep_null(q);
	ep_null(r);

	for (int i = 0; i < EP_TABLE; i++) {
		ep_null(t[i]);
	}

	TRY {
		ep_new(q);
		ep_new(r);
		bn_new(k);

		p = ep_curve_get_gen();
		n = ep_curve_get_ord();

		for (int i = 0; i < EP_TABLE; i++) {
			ep_new(t[i]);
		}
		TEST_BEGIN("fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_pre(t, p);
			ep_mul_fix(q, t, k);
			ep_mul(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < EP_TABLE; i++) {
			ep_free(t[i]);
		}

#if EP_FIX == BASIC || !defined(STRIP)
		for (int i = 0; i < EP_TABLE_BASIC; i++) {
			ep_new(t[i]);
		}
		TEST_BEGIN("binary fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_pre_basic(t, p);
			ep_mul_fix_basic(q, t, k);
			ep_mul(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < EP_TABLE_BASIC; i++) {
			ep_free(t[i]);
		}
#endif

#if EP_FIX == YAOWI || !defined(STRIP)
		for (int i = 0; i < EP_TABLE_YAOWI; i++) {
			ep_new(t[i]);
		}
		TEST_BEGIN("yao windowing fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_pre_yaowi(t, p);
			ep_mul_fix_yaowi(q, t, k);
			ep_mul(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < EP_TABLE_YAOWI; i++) {
			ep_free(t[i]);
		}
#endif

#if EP_FIX == NAFWI || !defined(STRIP)
		for (int i = 0; i < EP_TABLE_NAFWI; i++) {
			ep_new(t[i]);
		}
		TEST_BEGIN("naf windowing fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_pre_nafwi(t, p);
			ep_mul_fix_nafwi(q, t, k);
			ep_mul(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < EP_TABLE_NAFWI; i++) {
			ep_free(t[i]);
		}
#endif

#if EP_FIX == COMBS || !defined(STRIP)
		for (int i = 0; i < EP_TABLE_COMBS; i++) {
			ep_new(t[i]);
		}
		TEST_BEGIN("single-table comb fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_pre_combs(t, p);
			ep_mul_fix_combs(q, t, k);
			ep_mul(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < EP_TABLE_COMBS; i++) {
			ep_free(t[i]);
		}
#endif

#if EP_FIX == COMBD || !defined(STRIP)
		for (int i = 0; i < EP_TABLE_COMBD; i++) {
			ep_new(t[i]);
		}
		TEST_BEGIN("double-table comb fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_pre_combd(t, p);
			ep_mul_fix_combd(q, t, k);
			ep_mul(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < EP_TABLE_COMBD; i++) {
			ep_free(t[i]);
		}
#endif

#if EP_FIX == WTNAF || !defined(STRIP)
		for (int i = 0; i < EP_TABLE_WTNAF; i++) {
			ep_new(t[i]);
		}
		TEST_BEGIN("w(t)naf fixed point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			ep_mul(q, p, k);
			ep_mul_pre_wtnaf(t, p);
			ep_mul_fix_wtnaf(q, t, k);
			ep_mul(r, p, k);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
		for (int i = 0; i < EP_TABLE_WTNAF; i++) {
			ep_free(t[i]);
		}
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	ep_free(q);
	ep_free(r);
	bn_free(k);
	return code;
}

static int simultaneous(void) {
	int code = STS_ERR;
	ep_t p, q, r, s;
	bn_t n, k, l;

	ep_null(p);
	ep_null(q);
	ep_null(r);
	ep_null(s);

	TRY {

		ep_new(q);
		ep_new(r);
		ep_new(s);
		bn_new(k);
		bn_new(l);

		p = ep_curve_get_gen();
		n = ep_curve_get_ord();

		TEST_BEGIN("simultaneous point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			ep_mul(q, p, k);
			ep_mul(s, q, l);
			ep_mul_sim(r, p, k, q, l);
			ep_add(q, q, s);
			ep_norm(q, q);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;

#if EP_SIM == BASIC || !defined(STRIP)
		TEST_BEGIN("basic simultaneous point multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			ep_mul_sim(r, p, k, q, l);
			ep_mul_sim_basic(q, p, k, q, l);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
#endif

#if EP_SIM == TRICK || !defined(STRIP)
		TEST_BEGIN("shamir's trick for simultaneous multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			ep_mul_sim(r, p, k, q, l);
			ep_mul_sim_trick(q, p, k, q, l);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
#endif

#if EP_SIM == INTER || !defined(STRIP)
		TEST_BEGIN("interleaving for simultaneous multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			ep_mul_sim(r, p, k, q, l);
			ep_mul_sim_inter(q, p, k, q, l);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
#endif

#if EP_SIM == JOINT || !defined(STRIP)
		TEST_BEGIN("jsf for simultaneous multiplication is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			ep_mul_sim(r, p, k, q, l);
			ep_mul_sim_joint(q, p, k, q, l);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
#endif

		TEST_BEGIN("simultaneous multiplication with generator is correct") {
			bn_rand(k, BN_POS, bn_bits(n));
			bn_mod(k, k, n);
			bn_rand(l, BN_POS, bn_bits(n));
			bn_mod(l, l, n);
			ep_mul_sim_gen(r, k, q, l);
			ep_mul_sim(q, ep_curve_get_gen(), k, q, l);
			TEST_ASSERT(ep_cmp(q, r) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	ep_free(q);
	ep_free(r);
	bn_free(k);
	return code;
}

int test(void) {
	ep_param_print();

	util_print_banner("Utilities:", 1);

	if (util() != STS_OK) {
		return STS_ERR;
	}

	util_print_banner("Arithmetic:", 1);

	if (addition() != STS_OK) {
		return STS_ERR;
	}

	if (subtraction() != STS_OK) {
		return STS_ERR;
	}

	if (doubling() != STS_OK) {
		return STS_ERR;
	}

	if (multiplication() != STS_OK) {
		return STS_ERR;
	}

	if (fixed() != STS_OK) {
		return STS_ERR;
	}

	if (simultaneous() != STS_OK) {
		return STS_ERR;
	}

	return STS_OK;
}

int main(void) {
	int r0;
	core_init();

	if (memory() != STS_OK) {
		core_clean();
		return 1;
	}
#if defined(EB_STAND) && defined(EP_ORDIN)
	r0 = ep_param_set_any_ordin();
	if (r0 == STS_OK) {
		if (test() != STS_OK) {
			core_clean();
			return 1;
		}
	}
#endif

	core_clean();
	return 0;
}
