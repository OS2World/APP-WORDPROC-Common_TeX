
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

global	file	dvi_file;
global	str	dvi_name;

#define SET_CHAR_0	0
#define SET1		128
#define SET_RULE	132
#define PUT1		133
#define PUT_RULE	137
#define NOP		138
#define BOP		139
#define EOP		140
#define PUSH		141
#define POP		142
#define RIGHT1		143
#define W0		147
#define W1		148
#define X0		152
#define X1		153
#define DOWN1		157
#define Y0		161
#define Y1		162
#define Z0		166
#define Z1		167
#define FNT_NUM_0	171
#define FNT1		235
#define XXX1		239
#define XXX4		242
#define FNT_DEF1	243
#define PRE		247
#define POST		248
#define POST_POST	249
#define ID_BYTE		2

global	int	dead_cycles;
global	bool	doing_leaders;
global	int	last_bop;
global	scal	max_h;
global	scal	max_v;
global	int	max_push;
global	scal	rule_ht;
global	scal	rule_dp;
global	scal	rule_wd;

global	fnt	dvi_f;
global	scal	dvi_h;
global	scal	dvi_v;
global	scal	cur_h;
global	scal	cur_v;
global	int	cur_s;

global	ptr	down_ptr;
global	ptr	right_ptr;

global	byte	*dvi_buf;
global	ptr	dvi_limit; 
global	ptr	dvi_ptr;
global	int	dvi_offset;
global	int	dvi_gone;
global	bool	output_active;
global	int	total_pages;

struct move_t {
	mcell	node_field;
	scal	movement_field;
	int	location_field;
};
typedef struct move_t move_t;

#define MOVEMENT_NODE_SIZE	sizeof(move_t)
#define move_amount(P)		((move_t *) (P))->movement_field
#define location(P)		((move_t *) (P))->location_field

#define synch_h() \
	{if (cur_h != dvi_h) {movement(cur_h - dvi_h, RIGHT1); dvi_h = cur_h;}}

#define synch_v() \
	{if (cur_v != dvi_v) {movement(cur_v - dvi_v, DOWN1); dvi_v = cur_v;}}

#define dvi_out(B) \
	{dvi_buf[dvi_ptr++] = B; if (dvi_ptr == dvi_limit) dvi_swap();}

#define write_dvi(a, b) \
	{fwrite((char *)&dvi_buf[a], sizeof(dvi_buf[0]), b - a, dvi_file);}

void	ship_out();
void	hlist_out();
void	hskip_out();
void	hrule_out();
void	vlist_out();
void	vskip_out();
void	vrule_out();
void	fnt_out();
void	dvi_font_def();
void	dvi_swap();
void	dvi_four();
void	dvi_pop();
void	movement();
void	prune_movements();
void	start_dvi();
void	fin_dvi();

void	_dvi_init();
void	_dvi_init_once();
