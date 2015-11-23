
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

ptr	str_toks();
ptr	the_toks();
void	conv_toks();

#define NUMBER_CODE		0
#define ROMAN_NUMERAL_CODE	1
#define STRING_CODE		2
#define MEANING_CODE		3
#define FONT_NAME_CODE		4
#define JOB_NAME_CODE		5

void	scan_toks();
void	read_toks();
void	show_token_list();
void	token_show();
void	print_meaning();
void	flush_list();

#define token_ref_count(T)	token(T)
#define add_token_ref(T)	incr(token_ref_count(T))
#define delete_token_ref(T) \
	{if (token_ref_count(T) == 0) \
		flush_list(T); \
	else decr(token_ref_count(T));}

void	_toklist_init();
void	_toklist_init_once();
