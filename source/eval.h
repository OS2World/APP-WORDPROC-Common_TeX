
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

#define VMODE		1
#define HMODE		(VMODE + MAX_COMMAND + 1)
#define MMODE		(HMODE + MAX_COMMAND + 1)

global	ptr	lig_stack;
global	int	cur_l;
global	int	cur_r;
global	ptr	cur_q;
global	bool	lft_hit;
global	bool	rt_hit;
global	bool	ligature_present;

global	void	main_control();
global	void	app_space();

global void	push_nest();
global void	pop_nest();
global bool	realloc_nest();
global void	print_mode();

#define IGNORE_DEPTH	-65536000

struct list {
	int	mode_field;
	ptr	head_field;
	ptr	tail_field;
	int	pg_field;
	int	aux_field;
	int	ml_field;
	int	clang_field;
	short	lhm_field;
	short	rhm_field;
};
typedef struct list list;

global	list	cur_list;

global	list	*nest;
global	list	*nest_end;
global	list	*nest_ptr;
global	list	*max_nest_stack;

#define mode			cur_list.mode_field
#define head			cur_list.head_field
#define tail			cur_list.tail_field
#define prev_graf		cur_list.pg_field
#define aux			cur_list.aux_field
#define prev_depth		aux
#define space_factor		aux
#define incompleat_noad		aux
#define mode_line		cur_list.ml_field
#define clang			cur_list.clang_field
#define lhmin			cur_list.lhm_field
#define rhmin			cur_list.rhm_field

global	int		shown_mode;

global void	show_activities();

#define tail_append(N)		{tail = link(tail) = N;}

global void	insert_dollar_sign();
global void	you_cant();
global void	report_illegal_case();
global bool	privileged();
global void	missing_font();
global bool	its_all_over();

global void	_eval_init();
global void	_eval_init_once();
