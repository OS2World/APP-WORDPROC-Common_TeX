
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

void	get_x_token();
void	expand();
void	insert_relax();

#define TOP_MARK_CODE		0
#define FIRST_MARK_CODE		1
#define BOT_MARK_CODE		2
#define SPLIT_FIRST_MARK_CODE	3
#define SPLIT_BOT_MARK_CODE	4

#define top_mark		cur_mark[TOP_MARK_CODE]
#define first_mark		cur_mark[FIRST_MARK_CODE]
#define bot_mark		cur_mark[BOT_MARK_CODE]
#define split_first_mark	cur_mark[SPLIT_FIRST_MARK_CODE]
#define split_bot_mark		cur_mark[SPLIT_BOT_MARK_CODE]

global	ptr	cur_mark[];

global	int	long_state;
global	ptr	pstack[];

void	macro_call();
void	x_token();
void	runaway_arg();

void	_expand_init();
void	_expand_init_once();
