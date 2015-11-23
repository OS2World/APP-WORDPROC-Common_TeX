
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

void	do_extension();

void	show_whatsit();
ptr	copy_whatsit();
void	free_whatsit();
void	out_whatsit();
void	line_break_whatsit();
void	try_hyph_whatsit();
void	append_whatsit();

#define OPEN_NODE		0

struct _open_t {
	mcell	node_field;
	int	open_stream_field;
	str	open_name_field;
	str	open_area_field;
	str	open_ext_field;
};
typedef struct _open_t _open_t;

#define OPEN_NODE_SIZE		sizeof(_open_t)
#define open_stream(P)		((_open_t *) (P))->open_stream_field
#define open_name(P)		((_open_t *) (P))->open_name_field
#define open_area(P)		((_open_t *) (P))->open_area_field
#define open_ext(P)		((_open_t *) (P))->open_ext_field

void	do_open();
void	show_open();
ptr	copy_open();
void	free_open();
void	out_open();

#define WRITE_NODE		1

struct _write_t {
	mcell	node_field;
	int	write_stream_field;
	ptr	write_toks_field;
};
typedef struct _write_t _write_t;

#define WRITE_NODE_SIZE		sizeof(_write_t)
#define write_stream(P)		((_write_t *) (P))->write_stream_field
#define write_toks(P)		((_write_t *) (P))->write_toks_field

#define END_WRITE_TOKEN		sym2tok(FROZEN_END_WRITE)

global	file	write_file[];
global	bool	write_open[];
global	sym	write_cs;

void	do_write();
void	show_write();
ptr	copy_write();
void	free_write();
void	out_write();

#define CLOSE_NODE		2

struct _close_t {
	mcell	node_field;
	int	close_stream_field;
	ptr	fill_field;
};
typedef struct _close_t _close_t;

#define CLOSE_NODE_SIZE		sizeof(_close_t)
#define close_stream(P)		((_close_t *) (P))->close_stream_field

void	do_close();
void	show_close();
ptr	copy_close();
void	free_close();
void	out_close();

#define SPECIAL_NODE		3

struct _special_t {
	mcell	node_field;
	ptr	special_toks_field;
	ptr	fill_field;
};
typedef struct _special_t _special_t;

#define SPECIAL_NODE_SIZE	sizeof(_special_t)
#define special_toks(P)		((_special_t *) (P))->special_toks_field

void	do_special();
void	show_special();
ptr	copy_special();
void	free_special();
void	out_special();

#define IMMEDIATE_CODE		4

void	do_immediate();

#define SET_LANGUAGE_CODE	5
#define LANGUAGE_NODE		4

struct _language_t {
	mcell	node_field;
	int	lang_field;
	short	lhm_field;
	short	rhm_field;
};
typedef struct _language_t _language_t;

#define LANGUAGE_NODE_SIZE	sizeof(_language_t)
#define what_lang(P)		((_language_t *)(P))->lang_field
#define what_lhm(P)		((_language_t *)(P))->lhm_field
#define what_rhm(P)		((_language_t *)(P))->rhm_field

void	do_set_language();
void	show_language();
ptr	copy_language();
void	free_language();
void	out_language();
void	fix_language();

void	_texext_init();
void	_texext_init_once();
