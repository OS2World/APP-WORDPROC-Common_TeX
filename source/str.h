
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

global	str	null_str;

global	str	ASCII[];

global	str	cur_str;
global	str	cur_str_end;
global	str	cur_str_ptr;

#define MAX_STR_SIZE		4096

str	new_str();
void	free_str();
void	str_room();

#define append_char(C) 		(*cur_str_ptr++ = C)
#define flush_char()		(decr(cur_str_ptr))
#define cur_length()		(cur_str_ptr - cur_str)
#define flush_str()		(cur_str_ptr = cur_str)
#define print_str()		{*cur_str_ptr = 0; print(cur_str);}

str	make_str();

#define str_length(S)		(strlen(S))
#define str_eq(S1,S2)		(strcmp(S1,S2)==0)
#define str_eqn(S1,S2,N)	(memcmp(S1,S2,N)==0)
#define make_str_given(S)	(strcpy(new_str(str_length(S)), S))

void	_str_init();
void	_str_init_once();
