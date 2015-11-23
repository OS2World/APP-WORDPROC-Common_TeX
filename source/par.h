
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

struct passive_t {
	mcell	node_field;
	ptr	cur_break_field;
	ptr	prev_break_field;
};
typedef struct passive_t passive_t;

#define PASSIVE_NODE_SIZE	sizeof(passive_t)
#define serial			info
#define cur_break(P)		((passive_t *) (P))->cur_break_field
#define prev_break(P)		((passive_t *) (P))->prev_break_field
#define next_break		prev_break

global	ptr	passive;

struct active_t {
	mcell	node_field;
	ptr	break_node_field;
	int	line_number_field;
	int	total_demerits_field;
};
typedef struct active_t active_t;

#define ACTIVE_NODE_SIZE	sizeof(active_t)
#define UNHYPHENATED		0
#define HYPHENATED		1
#define fitness			subtype
#define VERY_LOOSE_FIT		0
#define LOOSE_FIT		1
#define DECENT_FIT		2
#define TIGHT_FIT		3
#define break_node(P)		((active_t *) (P))->break_node_field
#define line_number(P)		((active_t *) (P))->line_number_field
#define total_demerits(P)	((active_t *) (P))->total_demerits_field

global	ptr	active;
#define last_active		active

#define DELTA_NODE		2

struct delta_t {
	mcell	node_field;
	scal	delta_field[7];
};
typedef struct delta_t delta_t;

#define DELTA_NODE_SIZE		sizeof(delta_t)
#define deltas(P)		((delta_t *) (P))->delta_field

#define do_all_six(F) \
	{F(1); F(2); F(3); F(4); F(5); F(6);}

global	ptr	just_box;

void	line_break();

global	ptr	printed_node;
global	int	pass_number;

global	scal	active_width[];
global	scal	cur_active_width[];
global	scal	background[];
global	scal	break_width[];
global	bool	no_shrink_error_yet;

ptr	finite_shrink();

global	ptr	cur_p;
global	bool	second_pass;
global	bool	final_pass;
global	int	threshold;

void	try_break();

#define AWFUL_BAD		07777777777

global	int	fit_class;
global	int	minimal_demerits[];
global	int	minimum_demerits;
global	ptr	best_place[];
global	int	best_pl_line[];

global	int	easy_line;
global	int	last_special_line;
global	scal	first_width;
global	scal	second_width;
global	scal	first_indent;
global	scal	second_indent;
global	scal	line_width;
global	scal	disc_width;
global	ptr	best_bet;
global	int	fewest_demerits;
global	int	best_line;
global	int	line_diff;
global	int	actual_looseness;

void	post_line_break();

void	set_disc_width();
void	set_act_width();
void	set_break_width();
int	get_best_bet();
int	get_badness();
void	get_active_nodes();
ptr	get_break_node();
void	show_break_node();
void	show_break_status();
void	update_printed_node();

void	_par_init();
void	_par_init_once();
