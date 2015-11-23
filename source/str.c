
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

str	null_str;
str	ASCII[256];

str	cur_str;
str	cur_str_end;
str	cur_str_ptr;

str
make_str ()
{
	int	n;
	str	s;

	n = cur_length();
	s = new_str(n);
	memcpy(s, cur_str, n);
	cur_str_ptr = cur_str;

	return s;
}

void
str_room (i)
	int	i;
{
	if (cur_str_ptr + i >= cur_str_end) {
		overflow("str pool", MAX_STR_SIZE);
	}
}

str
new_str (i)
	int	i;
{
	str	s;

	s = (str) malloc(i+1);
	if (s == (str) 0) {
		overflow("new str", i);
	}
	s[i] = NUL;

	return	s;
}

void
free_str (s)
	str	s;
{
	free(s);
}

void
_str_init ()
{
}

void
_str_init_once ()
{
	int 	k;

#define app_lc_hex(L) append_char(((L)<10) ? (L)+'0' : (L)-10+'a')

	cur_str = new_str(MAX_STR_SIZE);
	cur_str_end = cur_str + MAX_STR_SIZE;
	cur_str_ptr = cur_str;
	for (k = 0; k <= 255; incr(k)) {
		if (k < ' ' || k > '~') {
			append_char('^');
			append_char('^');
			if (k < 0100) {
				append_char(k + 0100);
			} else if (k < 0200) {
				append_char(k - 0100);
			} else {
				app_lc_hex(k / 16);
				app_lc_hex(k % 16);
			}
		} else {
			append_char(k);
		}
		ASCII[k] = make_str();
	}
	null_str = "";
}
