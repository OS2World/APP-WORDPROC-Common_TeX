
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

#ifndef lint
static char *sccsid = "%A%";
#endif

#include "tex.h"

char	*banner = "This is Common TeX, Version 3.3";
str	format_ident;
int	ready_already;

main (argc, argv)
	int	argc;
	str 	argv[];
{
	signal(SIGINT, handle_int);
	set_paths();
	if (ready_already != 314159)  {
		initialize_once();
	}
	ready_already = 314159;
	initialize();
	if (!(decode_args(argc, argv) || init_terminal())) {
		exit(history);
	}
	if (end_line_char_active) {
		*++limit = end_line_char;
	}
	if (interaction == BATCH_MODE) {
		selector = NO_PRINT;
	} else {
		selector = TERM_ONLY;
	}
	fix_date_and_time();
	if (next <= limit && cat_code(*next) != ESCAPE) {
		start_input();
	}
	history = SPOTLESS;
	main_control();
	final_cleanup();
	/*NOTREACHED*/
}

#define USAGE	"usage: %s [ TeX string ]\n"

bool
decode_args (argc, argv)
	int	argc;
	str	argv[];
{
	int	i, j;

	if (argc == 1) {
		return FALSE;
	}
	for (i = 1; i < argc; i++) {
		j = str_length(argv[i]);
		argv[i][j] = ' ';
		if (limit + j + 1 >= buffer + BUF_SIZE) {
			return FALSE;
		}
		(void) strncpy(limit + 1, argv[i], j + 1);
		limit += j + 1;
		argv[i][j] = '\0';
	}
	return TRUE;
}

void
fix_date_and_time ()
{
	long	clock, begintime();
	struct tm	*tmptr, *localtime();

	clock = begintime();
	tmptr = localtime(&clock);
	time = 60 * tmptr->tm_hour + tmptr->tm_min;
	day = tmptr->tm_mday;
	month = tmptr->tm_mon + 1;
	year = tmptr->tm_year + 1900;
}

#undef time
long
begintime()
{
	return (time((char *) 0));
}

void
handle_int ()
{	
	signal(SIGINT, handle_int);
	interrupted = 1;
}

void
initialize ()
{
	_alloc_init();
	_chr_init();
	_str_init();
	_arith_init();
	_error_init();
	_sym_init();
	_def_init();
	_scan_init();
	_expand_init();
	_fileio_init();
	_cond_init();
	_toklist_init();
	_eval_init();
	_box_init();
	_boxlist_init();
	_math_init();
	_mathlist_init();
	_hyph_init();
	_pack_init();
	_page_init();
	_par_init();
	_mlst_hlst_init();
	_align_init();
	_dvi_init();
	_cmd_init();
	_tfm_init();
	_texext_init();
	_tex_init();
	_print_init();
	_tok_init();
}

void
initialize_once ()
{
	_chr_init_once();
	_print_init_once();
	_error_init_once();
	_alloc_init_once();
	_str_init_once();
	_arith_init_once();
	_sym_init_once();
	_tok_init_once();
	_def_init_once();
	_scan_init_once();
	_expand_init_once();
	_fileio_init_once();
	_cond_init_once();
	_toklist_init_once();
	_eval_init_once();
	_box_init_once();
	_boxlist_init_once();
	_math_init_once();
	_mathlist_init_once();
	_hyph_init_once();
	_pack_init_once();
	_page_init_once();
	_par_init_once();
	_mlst_hlst_init_once();
	_align_init_once();
	_dvi_init_once();
	_cmd_init_once();
	_tfm_init_once();
	_texext_init_once();
	_tex_init_once();
}

void
final_cleanup ()
{
	bool dump = cur_chr == 1;

#define SEE_TRANS "(see the transcript file for additional information)"

	if (job_name == null_str) {
		open_log_file();
	}
	while (open_parens > 0) {
		print(" )");
		decr(open_parens);
	}
	if (cur_level > LEVEL_ONE) {
		print_nl("(");
		print_esc("end occurred ");
		print("inside a group at level ");
		print_int(cur_level - LEVEL_ONE);
		print(")");
	}
	while (cond_ptr != null) {
		print_nl("(");
		print_esc("end occurred ");
		print("when ");
		print_cmd_chr(IF_TEST, cur_if);
		if (if_line != 0) {
			print(" on line ");
			print_int(if_line);
		}
		print(" was incomplete)");
		if_line = if_line_field(cond_ptr); 
		cur_if = subtype(cond_ptr);
		cond_ptr = link(cond_ptr);
	}
	if (history != SPOTLESS
	&& (history == WARNING_ISSUED || interaction < ERROR_STOP_MODE)
	&& selector == TERM_AND_LOG) {
		selector = TERM_ONLY;
		print_nl(SEE_TRANS);
		selector = TERM_AND_LOG;
	}
	close_files_and_terminate(FALSE, dump);
}

void
close_files_and_terminate (edit, dump)
	bool	edit;
	bool	dump;
{
	int	k;
	
	for (k = 0; k < 16; incr(k)) {
		if (write_open[k]) {
			a_close(write_file[k]);
		}
	}
	wake_up_terminal();
	fin_dvi();
	if (log_name != null_str) {
		wlog_cr();
		a_close(log_file);
		selector -= 2;
		if (selector == TERM_ONLY) {
			print_nl("Transcript written on ");
			print(log_name);
			print(".");
		}
	}
	print_ln();
	if (edit) {
		call_edit();
	}
	fflush(stdout);
	fflush(stderr);
	if (dump) {
		if (trie_not_ready) {
			init_trie();
		}
		kill(getpid(), SIGQUIT);
	}
	exit(history);
}

void
call_edit ()
{
	str	envedit;
	char	edit[MAX_STR_SIZE + 17];
	str	texedit = "vi +%d %s";
	str	getenv();

#define LONG_FORMAT "Edit format must be of reasonable length"
#define EDIT_FORMAT "Edit format consists of 1 filename and 1 linenumber"

	if ((envedit = getenv("TEXEDIT")) != null) {
		texedit = envedit;
		if (strlen(texedit) > MAX_STR_SIZE) {
			print_err(LONG_FORMAT);
			return;
		}
	}
	if (!one("%d", texedit) || !one("%s", texedit)) {
		print_err(EDIT_FORMAT);
		return;
	}
	cur_input = *base_ptr;
	sprintf(edit, texedit, line, file_name);
	system(edit);
}

one (s, t)
	str	s;
	str	t;
{
	int	i;
	int	j;
	int	one;

	one = 0;
	j = strlen(t) - 1;
	for (i = 0; i < j; incr(i)) {
		if (strncmp(s, &t[i], 2) == 0) {
			if (++one > 1) {
				return FALSE;
			}
		}
	}
	if (one == 0) {
		return FALSE;
	}
	return TRUE;
}

void
_tex_init ()
{
}

void
_tex_init_once ()
{
	format_ident = " (INITEX)";
}
