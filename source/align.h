
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

struct align_stack_t {
	struct align_stack_t *align_stack_field;
	ptr	preamble_field;
	ptr	cur_align_field;
	ptr	cur_span_field;
	ptr	cur_loop_field;
	ptr	cur_tail_field;
	ptr	cur_head_field;
	int	align_state_field;
};
typedef struct align_stack_t align_stack_t;

#define ALIGN_STACK_NODE_SIZE	sizeof(align_stack_t)

global	align_stack_t *align_stack;

void	push_alignment();
void	pop_alignment();

global	ptr	align_head;

#define preamble	link(align_head)

global	ptr	cur_align;
global	ptr	cur_span;
global	ptr	cur_loop;
global	ptr	cur_head;
global	ptr	cur_tail;

struct span_t {
	int	span_count_field;
	ptr	span_link_field;
	scal	span_width_field;
	int	fill_field;
};
typedef struct span_t span_t;

#define SPAN_NODE_SIZE		sizeof(span_t)
#define span_count(P)		((span_t *) (P))->span_count_field
#define span_link(P)		((span_t *) (P))->span_link_field
#define span_width(P)		((span_t *) (P))->span_width_field

global	ptr	end_span;

#define SPAN_CODE		256
#define CR_CODE			257
#define CR_CR_CODE		(CR_CODE + 1)

void	init_align();
void	get_preamble_token();
void	align_peek();
void	init_row();
void	init_span();
void	init_col();
bool	fin_col();
void	fin_row();
void	fin_align();

void	_align_init();
void	_align_init_once();
