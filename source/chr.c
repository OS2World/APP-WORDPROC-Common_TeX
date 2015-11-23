
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

byte	xord[256];
byte	xchr[256];

void
_chr_init ()
{
	int	i;

	for (i = 0; i <= 0377; incr(i)) {
		xchr[i] = i;
	}
	for (i = 0; i <= 0377; incr(i)) {
		xord[xchr[i]] = i;
	}
}

void
_chr_init_once ()
{
}
