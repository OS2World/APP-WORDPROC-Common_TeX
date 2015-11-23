
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

void	scan_left_brace();
void	scan_optional_equals();
bool	scan_keyword();

global	int	cur_val_level;
global	int	cur_val;

void	scan_something_internal();

void	mu_error();

#define INT_VAL		0
#define DIMEN_VAL	1
#define GLUE_VAL	2
#define MU_VAL		3
#define IDENT_VAL	4
#define TOK_VAL		5

#define INPUT_LINE_NO_CODE	3
#define BADNESS_CODE		4

void	scan_eight_bit_int();
void	scan_char_num();
void	scan_four_bit_int();
void	scan_fifteen_bit_int();
void	scan_twenty_seven_bit_int();

void	scan_int();
#define INFINITY	017777777777

global	int	radix;

#define PLUS_TOKEN		(OTHER_TOKEN + '+')
#define MINUS_TOKEN		(OTHER_TOKEN + '-')
#define ZERO_TOKEN		(OTHER_TOKEN + '0')
#define A_TOKEN			(LETTER_TOKEN + 'A')
#define OTHER_A_TOKEN		(OTHER_TOKEN + 'A')
#define OCTAL_TOKEN		(OTHER_TOKEN + '\'')
#define HEX_TOKEN		(OTHER_TOKEN + '"')
#define ALPHA_TOKEN		(OTHER_TOKEN + '`')
#define POINT_TOKEN		(OTHER_TOKEN + '.')
#define EURO_POINT_TOKEN	(OTHER_TOKEN + ',')

void	scan_dimen();
#define MAX_DIMEN	07777777777

#define scan_normal_dimen()	scan_dimen(FALSE, FALSE, FALSE)

global	int	cur_order;

void	scan_glue();

#define scan_optional_space() \
	{get_x_token(); if (cur_cmd != SPACER) back_input();}

#define get_nbx_token() \
	{do get_x_token(); while (cur_cmd == SPACER);}

#define get_nbrx_token() \
	{do get_x_token(); while (cur_cmd == SPACER || cur_cmd == RELAX);}

void	scan_spec();
ptr	scan_rule_spec();

void	_scan_init();
void	_scan_init_once();
