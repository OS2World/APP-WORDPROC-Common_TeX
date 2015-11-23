
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

#define ABOVE_CODE	0
#define OVER_CODE	1
#define ATOP_CODE	2
#define DELIMITED_CODE	3
#define VAR_CODE	070000

void	push_math();
void	init_math();
void	start_eq_no();
void	scan_math();
void	set_math_char();
void	math_limit_switch();
void	scan_delimiter();
void	math_radical();
void	math_ac();
void	append_choices();
void	build_choices();
void	sub_sup();
void	math_fraction();
ptr	fin_mlist();
void	math_left_right();
void	after_math();
void	check_dollar();
bool	check_font_params();
void	resume_after_display();

void	_mathlist_init();
void	_mathlist_init_once();
