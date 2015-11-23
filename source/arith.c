
/*
 * %Y%:%M%:%I%:%Q%
 *
 * Copyright 1987,1988,1991 Pat J Monardo
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 *
 */

#ifndef lint
static char *sccsid = "%A%";
#endif

#include "tex.h"

bool	arith_error;
scal	remainder;

int 
half (x)
	int	x;
{
	return (odd(x) ? (x + 1) / 2 : x / 2);
}


scal
round_decimals (k)
	int	k;
{
	int	a;

	a = 0;
	while (k > 0) {
		decr(k);
		a = (a + dig[k] * TWO) / 10;
	}
	return ((a + 1) / 2);
}

void
print_scaled (s)
	scal	s;
{
	scal	delta;

	if (s < 0) {
		print("-");
		s = -s;
	}
	print_int(s / UNITY);
	print(".");
	s = 10 * (s % UNITY) + 5;
	delta = 10;
	do {
		if (delta > UNITY) {
			s += 0100000 - 50000;
		}
		print_char('0' + s / UNITY);
		s = 10 * (s % UNITY);
		delta *= 10;
	} while (s > delta);
}

scal
mult_and_add (n, x, y, max_answer)
	int	n;
	scal	x;
	scal	y;
	scal	max_answer;
{
	
	if (n < 0) {
		x = -x;
		n = -n;
	}
	if (n == 0) {
		return y;
	}
	if (x <= (max_answer - y) / n
	&& -x <= (max_answer + y) / n) {
		return (n * x + y);
	}
	arith_error = TRUE;
	return 0;
}

scal
x_over_n (x, n)
	scal	x;
	int	n;
{
	bool	negative;
	scal	quotient;

	negative = FALSE;
	if (n == 0) {
		arith_error = TRUE;
		remainder = x;
		return 0;
	}
	if (n < 0) {
		x = -x;
		n = -n;
		negative = TRUE;
	}
	if (x >= 0) {
		quotient = x / n;
		remainder = x % n;
	} else {
		quotient = -(-x / n);
		remainder = -(-x % n);
	}
	if (negative) {
		negate(remainder);
	}
	return quotient;
}

scal
xn_over_d (x, n, d)
	scal	x;
	int	n;
	int	d;
{
	int	t;
	int	u;
	int	v;
	bool	positive;

	if (x >= 0) {
		positive = TRUE;
	} else {
		x = -x;
		positive = FALSE;
	}
	t = x % 0100000 * n;
	u = x / 0100000 * n + t / 0100000;
	v = u % d * 0100000 + t % 0100000;
	if (u / d >= 0100000) {
		arith_error = TRUE;
	} else {
		u = 0100000 * (u / d) + v / d;
	}
	if (positive) {
		remainder = v % d;
		return u;
	} else {
		remainder = - (v % d);
		return -u;
	}
}

int
badness (t, s)
	scal	t;
	scal	s;
{
	int	r;

	if (t == 0) {
		return 0;
	} else if (s <= 0) {
		return INF_BAD;
	} else {
		if (t <= 7230584) {
			r = t * 297 / s;
		} else if (s >= 1663497) {
			r = t / (s / 297);
		} else {
			r = t;
		}
		if (r > 1290) {
			return INF_BAD;
		} else {
			return ((r * r * r + 0400000) / 01000000);
		}
	}
}

void
_arith_init ()
{
}

void
_arith_init_once ()
{
}
