
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

global	ptr	cond_ptr;
global	int	cur_if;
global	int	if_limit;
global	int	if_line;

struct if_t {
	mcell	node;
	int	line_field;
	ptr	fill_field;
};
typedef struct if_t if_t;

#define IF_NODE_SIZE		(sizeof(if_t))
#define if_line_field(I)	((if_t *) (I))->line_field

global	int	skip_line;

#define IF_CODE			1
#define FI_CODE			2
#define ELSE_CODE		3
#define OR_CODE			4

#define IF_CHAR_CODE		0
#define IF_CAT_CODE		1
#define IF_INT_CODE		2
#define IF_DIM_CODE		3
#define IF_ODD_CODE		4
#define IF_VMODE_CODE		5
#define IF_HMODE_CODE		6
#define IF_MMODE_CODE		7
#define IF_INNER_CODE		8
#define IF_VOID_CODE		9
#define IF_HBOX_CODE		10
#define IF_VBOX_CODE		11
#define IFX_CODE		12
#define IF_EOF_CODE		13
#define IF_TRUE_CODE		14
#define IF_FALSE_CODE		15
#define IF_CASE_CODE		16

void	conditional();

void	push_cond();
void	pop_cond();
void	pass_text();
void	change_if_limit();

void	_cond_init();
void	_cond_init_once();
