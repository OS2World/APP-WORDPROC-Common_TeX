
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

#define EXACTLY		0
#define ADDITIONAL	1
#define NATURAL		0, ADDITIONAL

global	ptr	adjust_head;
global	ptr	adjust_tail;

global	scal	total_stretch[];
global	scal	total_shrink[];

#define get_stretch_order() \
	(total_stretch[FILLL] != 0) ? FILLL : \
	(total_stretch[FILL] != 0) ? FILL : \
	(total_stretch[FIL] != 0) ? FIL : \
	NORMAL

#define get_shrink_order() \
	(total_shrink[FILLL] != 0) ? FILLL : \
	(total_shrink[FILL] != 0) ? FILL : \
	(total_shrink[FIL] != 0) ? FIL : \
	NORMAL

global	int	last_badness;
global	int	pack_begin_line;

ptr	hpack();
#define vpack(P, H)	vpackage(P, H, MAX_DIMEN)
ptr	vpackage();

void	_pack_init();
void	_pack_init_once();
