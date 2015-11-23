
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

#define UNITY	0200000
#define TWO	0400000

int	half();
scal	round_decimals();
void	print_scaled();

global	bool	arith_error;
global	scal	remainder;

scal	mult_and_add();

#define nx_plus_y(N,X,Y)	mult_and_add(N,X,Y,07777777777)
#define mult_integers(N,X)	mult_and_add(N,X,0,017777777777)

scal	x_over_n();
scal	xn_over_d();

#define INF_BAD	10000 

int	badness();

void	_arith_init();
void	_arith_init_once();
