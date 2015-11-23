
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

void	build_page();
void	insert_page();
ptr	vert_break();
ptr	vsplit();
ptr	prune_page_top();

global	ptr	contrib_head;

#define contrib_tail	 nest[0].tail_field

global	ptr	page_tail;
global	ptr	page_head;
global	ptr	page_ins_head;
global	int	page_contents;

#define INSERTS_ONLY	1
#define BOX_THERE	2

#define active_height	active_width
#define cur_height	active_height[1]


global	scal	best_height_plus_depth;

#define DEPLORABLE	100000

global	ptr	best_page_break;
global	scal	best_size;
global	scal	page_max_depth;
global	int	least_page_cost;

struct page_ins_t {
	mcell	node_field;
	ptr	broken_ptr_field;
	ptr	broken_ins_field;
	ptr	last_ins_ptr_field;
	ptr	best_ins_ptr_field;
	scal	page_ins_height_field;
};
typedef struct page_ins_t page_ins_t;

#define PAGE_INS_NODE_SIZE	sizeof(page_ins_t)
#define INSERTING		0
#define SPLIT_UP		1
#define broken_ptr(P)		((page_ins_t *) (P))->broken_ptr_field
#define broken_ins(P)		((page_ins_t *) (P))->broken_ins_field
#define last_ins_ptr(P)		((page_ins_t *) (P))->last_ins_ptr_field
#define best_ins_ptr(P)		((page_ins_t *) (P))->best_ins_ptr_field
#define page_ins_height(P)	((page_ins_t *) (P))->page_ins_height_field

global	scal	page_so_far[];

#define page_goal	page_so_far[0]
#define page_total	page_so_far[1]
#define page_shrink	page_so_far[6]
#define page_depth	page_so_far[7]

global	ptr	last_glue;
global	int	last_penalty;
global	scal	last_kern;
global	int	insert_penalties;

void	print_totals();

#define start_new_page() \
	{page_contents = EMPTY; \
	page_tail = page_head; \
	link(page_head) = null; \
	last_glue = null; \
	last_penalty = 0; \
	last_kern = 0; \
	page_depth = 0; \
	page_max_depth = 0;}

void	freeze_page_specs();

global	bool	output_active;

int	page_badness();
int	vert_badness();
void	ensure_vbox();
void	box_error();
void	show_page_stats();
void	show_split();
void	show_cur_page();

void	fire_up();
bool	insert_box();

void	_page_init();
void	_page_init_once();
