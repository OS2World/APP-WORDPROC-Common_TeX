
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

global	tok	after_token;
global	int	mag_set;
global	bool	long_help_seen;

void	get_r_token();
void	prefixed_command();

#define CHAR_DEF_CODE		0
#define MATH_CHAR_DEF_CODE	1
#define COUNT_DEF_CODE		2
#define DIMEN_DEF_CODE		3
#define SKIP_DEF_CODE		4
#define MU_SKIP_DEF_CODE	5
#define TOKS_DEF_CODE		6

void	do_register_command();
void	trap_zero_glue();
void	alter_aux();
void	alter_prev_graf();
void	alter_page_so_far();
void	alter_integer();
void	alter_box_dimen();

#define WD_CODE			0
#define HT_CODE			1
#define DP_CODE			2

void	new_font();
void	prepare_mag();
void	new_interaction();
void	do_assignments();
void	clopen_stream();
void	issue_message();
void	give_err_help();
void	shift_case();
void	show_whatever();

#define SHOW_CODE		0
#define SHOW_BOX_CODE		1
#define SHOW_THE_CODE		2
#define SHOW_LISTS		3

void	_def_init();
void	_def_init_once();
