
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

#ifndef lint
static char *sccsid = "%A%";
#endif

#include "tex.h"

file	write_file[16];
bool	write_open[18];

sym	write_cs;

void
do_extension ()
{
	switch (cur_chr) {
	case OPEN_NODE: do_open(); break;
	case WRITE_NODE: do_write(); break;
	case CLOSE_NODE: do_close(); break;
	case SPECIAL_NODE: do_special(); break;
	case IMMEDIATE_CODE: do_immediate(); break;
	case SET_LANGUAGE_CODE: do_set_language(); break;
	default: confusion("ext1");
	}
}

void
do_immediate ()
{
	ptr	p;

	get_x_token();
	if (cur_cmd == EXTENSION && cur_chr <= CLOSE_NODE) {
		p = tail;
		do_extension();
		out_whatsit(tail);
		flush_node_list(tail);
		tail = p;
		link(p) = null;
	} else {
		back_input();
	}
}

void
show_whatsit (p)
	ptr	p;
{
	switch (subtype(p)) {
	case OPEN_NODE: show_open(p); break;
	case WRITE_NODE: show_write(p); break;
	case CLOSE_NODE: show_close(p); break;
	case SPECIAL_NODE: show_special(p); break;
	case LANGUAGE_NODE: show_language(p); break;
	default:print("whatsit?");break;
	}
}

ptr
copy_whatsit (p)
	ptr	p;
{
	switch (subtype(p)) {
	case OPEN_NODE: return(copy_open(p));
	case WRITE_NODE: return(copy_write(p));
	case CLOSE_NODE: return(copy_close(p));
	case SPECIAL_NODE: return(copy_special(p));
	case LANGUAGE_NODE: return(copy_language(p));
	default:confusion("ext2");
	}
	/*NOTREACHED*/
}

void
free_whatsit (p)
	ptr	p;
{
	switch (subtype(p)) {
	case OPEN_NODE: free_open(p); break;
	case WRITE_NODE: free_write(p); break;
	case CLOSE_NODE: free_close(p); break;
	case SPECIAL_NODE: free_special(p); break;
	case LANGUAGE_NODE: free_language(p); break;
	default:confusion("ext3");
	}
}

#define adv_past(P) \
{	if (subtype(P) == LANGUAGE_NODE) { \
		cur_lang = what_lang(P); \
		l_hyf = what_lhm(P); \
		r_hyf = what_rhm(P); \
	} \
}

void
line_break_whatsit (p)
	ptr	p;
{
	adv_past(p);
}

void
try_hyph_whatsit (p)
	ptr	p;
{
	adv_past(p);
}

void
out_whatsit (p)
	ptr	p;
{
	switch (subtype(p)) {
	case OPEN_NODE: out_open(p); break;
	case WRITE_NODE: out_write(p); break;
	case CLOSE_NODE: out_close(p); break;
	case SPECIAL_NODE: out_special(p); break;
	case LANGUAGE_NODE:  out_language(p); break;
	default: confusion("ext4");
	}
}

void
append_whatsit (s, w)
	int	s;
	int	w;
{
	ptr	p;

	p = new_node(w);
	type(p) = WHATSIT_NODE;
	subtype(p) = s;
	tail_append(p);
}

void
do_open ()
{
	append_whatsit(cur_chr, OPEN_NODE_SIZE);
	scan_four_bit_int();
	open_stream(tail) = cur_val;
	scan_optional_equals();
	scan_file_name();
	open_name(tail) = cur_name;
	open_area(tail) = cur_area;
	open_ext(tail) = cur_ext;
}

void
show_open (p)
	ptr	p;
{
	print_esc("openout");
	if (open_stream(p) < 16) {
		print_int(open_stream(p));
	} else if (open_stream(p) == 16) {
		print("*");
	} else {
		print("-");
	}
	print("=");
	print_file_name(open_name(p), open_area(p), open_ext(p));
}

ptr
copy_open (p)
	ptr	p;
{
	ptr	q;

	q = new_node(OPEN_NODE_SIZE);
	memcpy((void *) q, (void *) p, OPEN_NODE_SIZE);

	return q;
}

void
free_open (p)
{
	free_node(p, OPEN_NODE_SIZE);
}

void
out_open (p)
	ptr	p;
{
	int	j;

	if (doing_leaders)
		return;
	j = open_stream(p);
	cur_name = open_name(p);
	cur_area = open_area(p);
	cur_ext = open_ext(p);
	if (cur_ext == null_str) {
		cur_ext = str_tex;
	}
	if (cur_area == null_str) {
		cur_area = job_area;
	}
	pack_cur_name();
	if ((write_file[j] = a_open_out()) == null_file) {
		cur_area = null_str;
		pack_cur_name();
		while ((write_file[j] = a_open_out()) == null_file) {
			prompt_file_name("output file name", str_tex);
		}
	}
	write_open[j] = TRUE;
}

void
do_write ()
{
	sym	s;

	s = cur_cs;
	append_whatsit(cur_chr, WRITE_NODE_SIZE);
	scan_int();
	if (cur_val < 0) {
		cur_val = 17;
	} else if (cur_val > 15) {
		cur_val = 16;
	}
	write_stream(tail) = cur_val;
	cur_cs = s;
	scan_toks(FALSE, FALSE);
	write_toks(tail) = def_ref;
}

void
show_write (p)
	ptr	p;
{
	print_esc("write");
	if (write_stream(p) < 16) {
		print_int(write_stream(p));
	} else if (write_stream(p) == 16) {
		print("*");
	} else {
		print("-");
	}
	print_mark(write_toks(p));
}

ptr
copy_write (p)
{
	ptr	q;

	q = new_node(WRITE_NODE_SIZE);
	memcpy((void *) q, (void *) p, WRITE_NODE_SIZE);
	add_token_ref(write_toks(p));

	return	q;
}

void
free_write (p)
{
	delete_token_ref(write_toks(p));
	free_node(p, WRITE_NODE_SIZE);
}

void
out_write (p)
	ptr	p;
{
	int	j;
	ptr	q;
	ptr	r;
	int	old_mode;
	int	old_setting;

	if (doing_leaders) {
		return;
	}
	q = new_token();
	token(q) = RIGHT_BRACE_TOKEN + '}';
	r = new_token();
	token_link(q) = r;
	token(r) = END_WRITE_TOKEN;
	ins_list(q);
	begin_token_list(write_toks(p), WRITE_TEXT);
	q = new_token();
	token(q) = LEFT_BRACE_TOKEN + '{';
	ins_list(q);
	old_mode = mode;
	mode = 0;
	cur_cs = write_cs;
	scan_toks(FALSE, TRUE);
	get_token();
	if (cur_tok != END_WRITE_TOKEN) {
		print_err("Unbalanced write command");
		help_unbal_write();
		error();
		do get_token();
		while (cur_tok != END_WRITE_TOKEN);
	}
	mode = old_mode;
	end_token_list();
	old_setting = selector;
	j = write_stream(p);
	if (write_open[j]) {
		selector = j;
	} else {
		if (j == 17 && selector == TERM_AND_LOG)
			selector = LOG_ONLY;
		print_nl(null_str);
	}
	token_show(def_ref);
	print_ln();
	flush_list(def_ref);
	selector = old_setting;
}

void
do_close ()
{
	append_whatsit(cur_chr, CLOSE_NODE_SIZE);
	scan_int();
	if (cur_val < 0) {
		cur_val = 17;
	} else if (cur_val > 15) {
		cur_val = 16;
	}
	write_stream(tail) = cur_val;
}

void
show_close (p)
	ptr	p;
{
	print_esc("closeout");
	if (write_stream(p) < 16) {
		print_int(write_stream(p));
	} else if (write_stream(p) == 16) {
		print("*");
	} else {
		print("-");
	}
}

ptr
copy_close (p)
	ptr	p;
{
	ptr	q;

	q = new_node(CLOSE_NODE_SIZE);
	memcpy((void *) q, (void *) p, CLOSE_NODE_SIZE);

	return	q;
}

void
free_close (p)
	ptr	p;
{
	free_node(p, CLOSE_NODE_SIZE);
}

void
out_close (p)
	ptr	p;
{
	int	j;

	if (!doing_leaders) {
		j = write_stream(p);
		if (write_open[j]) {
			a_close(write_file[j]);
			write_open[j] = FALSE;
		}
	}
}

void
do_special ()
{
	append_whatsit(SPECIAL_NODE, SPECIAL_NODE_SIZE);
	scan_toks(FALSE, TRUE);
	special_toks(tail) = def_ref;
}

void
show_special (p)
	ptr	p;
{
	print_esc("special");
	print_mark(special_toks(p));
}

ptr
copy_special (p)
	ptr	p;
{
	ptr	q;

	q = new_node(SPECIAL_NODE_SIZE);
	memcpy((void *)q, (void *)p, SPECIAL_NODE_SIZE);
	add_token_ref(special_toks(p));

	return q;
}

void
free_special (p)
	ptr	p;
{
	delete_token_ref(special_toks(p));
	free_node(p, SPECIAL_NODE_SIZE);
}

void
out_special (p)
	ptr	p;
{
	str	s;
	int	old_setting;

	synch_h();
	synch_v();
	old_setting = selector;
	selector = NEW_STRING;
	show_token_list(token_link(special_toks(p)), null, MAX_STR_SIZE);
	selector = old_setting;
	if (cur_length() < 256) {
		dvi_out(XXX1);
		dvi_out(cur_length());
	} else {
		dvi_out(XXX4);
		dvi_four(cur_length());
	}
	for (s = cur_str; s < cur_str_ptr; incr(s)) {
		dvi_out(*s);
	}
	flush_str();
}

void
do_set_language ()
{
	if (abs(mode) != HMODE) {
		report_illegal_case();
		return;
	}
	scan_int();
	if (cur_val <= 0 || cur_val > 255) {
		clang = 0;
	} else {
		clang = cur_val;
	}
	append_whatsit(LANGUAGE_NODE, LANGUAGE_NODE_SIZE);
	what_lang(tail) = clang;
	what_lhm(tail) = norm_min(left_hyphen_min);
	what_rhm(tail) = norm_min(right_hyphen_min);
}		

void
show_language (p)
	ptr	p;
{
	print_esc("setlanguage");
	print_int(what_lang(p));
	print(" (hyphenmin ");
	print_int(what_lhm(p));
	print(",");
	print_int(what_rhm(p));
	print(")");
}

ptr
copy_language (p)
	ptr	p;
{
	ptr	q;

	q = new_node(SMALL_NODE_SIZE);
	memcpy((void *) q, (void *) p, SMALL_NODE_SIZE);

	return q;
}

void
free_language (p)
{
	free_node(p, SMALL_NODE_SIZE);
}

void
out_language (p)
{
	return;
}

void
fix_language ()
{
	int	l;

	if (language <= 0 || language > 255) {
		l = 0;
	} else {
		l = language;
	}
	if (l != clang) {
		append_whatsit(LANGUAGE_NODE, SMALL_NODE_SIZE);
		what_lang(tail) = clang = l;
		what_lhm(tail) = norm_min(left_hyphen_min);
		what_rhm(tail) = norm_min(right_hyphen_min);
	}
}

void
_texext_init ()
{
}

void
_texext_init_once ()
{
}

/*
**	Help text
*/

help_unbal_write ()
{
	help2("On this page there's a \\write with fewer real {'s than }'s.",
	"I can't handle that very well; good luck.");
}
