
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

void	handle_right_brace();
void	extra_right_brace();

void	append_glue();

#define FIL_CODE	0
#define FILL_CODE	1
#define SS_CODE		2
#define FIL_NEG_CODE	3
#define SKIP_CODE	4
#define MSKIP_CODE	5

void	append_kern();

global	ptr	cur_box;

void	scan_box();
void	begin_box();
void	box_end();
void	package();

#define BOX_FLAG	010000000000
#define SHIP_OUT_FLAG	(BOX_FLAG + 512)
#define LEADER_FLAG	(BOX_FLAG + 513)
#define BOX_CODE	0
#define COPY_CODE	1
#define LAST_BOX_CODE	2
#define VSPLIT_CODE	3
#define VTOP_CODE	4

void	normal_paragraph();
void	indent_in_hmode();
int	norm_min();
void	new_graf();
void	end_graf();
void	head_for_vmode();
void	append_to_vlist();
void	begin_insert_or_adjust();
void	make_mark();
void	append_penalty();
void	delete_last();
void	unpackage();
void	append_italic_correction();
void	append_discretionary();
void	build_discretionary();
void	flush_discretionary();
void	make_accent();
void	align_error();
void	no_align_error();
void	omit_error();
void	do_endv();
void	cs_error();

void	_boxlist_init();
void	_boxlist_init_once();
