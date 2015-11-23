
/*
 * %Y%:%M%:%I%:%Q%
 *
 * Copyright 1987,1988,1991,1992 Pat J Monardo
 *
 * Redistribution of this file is permitted through
 * the specifications in the file COPYING.
 *
 *
 */

global	int	cur_c;
global	fnt	cur_f;
global	qcell	cur_i;
global	scal	cur_mu;
global	int	cur_size;

ptr	mlist_to_hlist();
void	make_over();
void	make_under();
void	make_vcenter();
void	make_radical();
void	make_math_accent();
void	make_fraction();
scal	make_op();
void	make_ord();
void	make_scripts();
int	make_left_right();
void	fetch();
ptr	clean_box();

void	_mlst_hlst_init();
void	_mlst_hlst_init_once();
