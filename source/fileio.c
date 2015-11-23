
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

#include <sys/types.h>
#ifdef quad
#undef quad
#endif
#include <sys/stat.h>

#include "tex.h"

int	name_length;
str	name_of_file;
str	name_str;
str	area_str;
str	ext_str;

str	cur_name;
str	cur_area;
str	cur_ext;

str	job_area;
str	job_name;

bool    name_in_progress;
bool	log_opened;

int	area_delimiter;
int	ext_delimiter;
int	path_delimiter;

file	read_file[16];

int	read_open[17]; 

str	str_dvi;
str	str_log;
str	str_tex;
str	str_tfm;
str	str_texput;

void
begin_name ()
{
	name_str = name_of_file;
	area_str = null_str;
	ext_str = null_str;
}
		
#define append_to_name(C) \
	{if (name_str - name_of_file == MAX_STR_SIZE) \
		overflow("file name", MAX_STR_SIZE); \
	*name_str++ = C;}

bool
more_name (c)
	int	c;
{
	if (c == ' ') {
		return FALSE;
	} else {
		append_to_name(c);
		if (c == area_delimiter) {
			area_str = name_str;
			ext_str = null_str;
		} else if (c == ext_delimiter && ext_str == null_str) {
			ext_str = name_str - 1;
		}
		return TRUE;
	}
}

void
end_name ()
{
	int	n;

	if (area_str == null_str) {
		cur_area = null_str;
		area_str = name_of_file;
	} else {
		n = area_str - name_of_file;
		cur_area = new_str(n);
		strncpy(cur_area, name_of_file, n);
	}
	if (ext_str == null_str) {
		cur_ext = null_str;
		ext_str = name_str;
	} else {
		n = name_str - ext_str;
		cur_ext = new_str(n);
		strncpy(cur_ext, ext_str, n);
	}
	n = ext_str - area_str;
	if (n == 0) {
		cur_name = null_str;
	} else {
		cur_name = new_str(n);
		strncpy(cur_name, area_str, n);
	}	
}

void
scan_file_name ()
{
	name_in_progress = TRUE;
	get_nbx_token();
	begin_name();
	loop {
		if (cur_cmd > OTHER_CHAR || cur_chr > 255) {
			back_input();
			break;
		}
		if (!more_name(cur_chr)) {
			break;
		}
		get_x_token();
	}
	end_name();
	name_in_progress = FALSE;
}


void
pack_file_name (n, a, e)
	str	n;
	str	a;
	str	e;
{
	str	s;

	name_str = name_of_file;
	for (s = a; *s; incr(s)) {
		append_to_name(*s);
	}
	for (s = n; *s; incr(s)) {
		append_to_name(*s);
	}
	for (s = e; *s; incr(s)) {
		append_to_name(*s);
	}
	append_to_name(NUL);
	name_length = name_str - name_of_file;
}

void
pack_job_name (s)
	str	s;
{
	cur_area = job_area;
	cur_name = job_name;
	cur_ext = s;
	pack_cur_name();
}

void
print_file_name (n, a, e)
	str	n;
	str	a;
	str	e;
{
	print(a);
	print(n);
	print(e);
}

str
make_name_str ()
{
	str	s;

	s = new_str(name_length);
	strcpy(s, name_of_file);

	return (s);
}

void
prompt_file_name (s, e)
	str	s;
	str	e;
{
	str	t;

	if (s[0] == 'i' && s[1] == 'n') {
		print_nl("! I can't find file `");
	} else {
		print_nl("! I can't write on file `");
	}
	print_file_name(cur_name, cur_area, cur_ext);
	print("'.");
	if (e == str_tex) {
		show_context();
	}
	print_nl("Please type another ");
	print(s);
	if (interaction < SCROLL_MODE) {
		fatal_error("*** (job aborted, file error in nonstop mode)");
	}
	clear_terminal();
	prompt_input(": ");
	begin_name();
	t = cur_str;
	while (*t == ' ' && t < cur_str_ptr) {
		incr(t);
	}
	while (t < cur_str_ptr && more_name(*t)) {
		incr(t);
	}
	end_name();
	if (cur_ext == null_str) {
		cur_ext = e;
	}
	pack_cur_name();
	flush_str();
}

void
start_input ()
{
	int	save_selector;

	scan_file_name();
	if (cur_ext == null_str) {
		cur_ext = str_tex;
	}
	pack_cur_name();
	loop {
		begin_file_reading();
		if (cur_file = a_open_in()) {
			break;
		}
		end_file_reading();
		if (cur_ext == str_tex) {
			cur_ext = null_str;
			pack_cur_name();
			begin_file_reading();
			if (cur_file = a_open_in()) {
				break;
			}
			end_file_reading();
		}
		prompt_file_name("input file name", str_tex);
	}
	name = make_name_str();
	if (job_name == null_str) {
		job_area = cur_area;
		job_name = cur_name;
		open_log_file();
		save_selector = selector;
		selector = NEW_STRING;
		print(" (preloaded format=");
		print(job_name);
		print(" ");
		print_int(year % 100);
		print(".");
		print_int(month);
		print(".");
		print_int(day);
		print(")");
		format_ident = make_str();
		selector = save_selector;
	}
	if (term_offset + str_length(name) > MAX_PRINT_LINE - 2) {
		print_ln();
	} else if (term_offset > 0 || file_offset > 0) {
		print(" ");
	}
	incr(open_parens);
	print("(");
	print(name);
	update_terminal(); 
	state = NEW_LINE;
	line = 1;
	index = 18;
	input_ln(cur_file);
	firm_up_the_line();
	if (end_line_char_active) {
		*++limit = end_line_char;
	}
	next = buffer;
}

FILE *
a_open_in ()
{
	if (test_access(READ_ACCESS, INPUT_FILE_PATH)) {
		return (fopen(name_of_file, "r"));
	}
	return null_file;
}

FILE *
a_open_out ()
{
	if (test_access(WRITE_ACCESS, NO_FILE_PATH)) {
		return (fopen(name_of_file, "w"));
	}
	return null_file;
}

FILE *
b_open_in ()
{
	if (test_access(READ_ACCESS, FONT_FILE_PATH)) {
		return (fopen(name_of_file, "rb"));
	}
	return null_file;
}

FILE *
b_open_out ()
{
	if (test_access(WRITE_ACCESS, NO_FILE_PATH)) {
		return (fopen(name_of_file, "wb"));
	}
	return null_file;
}

bool 
input_ln (f)
	file	f;
{
	int	c;

	next = buffer;
	limit = next - 1;
	c = getc(f);
	if (feof(f)) {
		return FALSE;
	}
	loop {
		if (c == EOLN || c == EOF) {
			break;
		}
		if ((*next = xord[c]) != ' ') {
			limit = next;
		}
		if (++next >= buffer + BUF_SIZE) {
			overflow("buffer size", BUF_SIZE);
		}
		c = getc(f);
	}
	next = buffer;
	return TRUE;
}

void
term_input ()
{
	int	c;
	str	s;

	update_terminal();
	flush_str();
	loop {
		c = getc(term_in);
		if (c == EOLN) {
			break;
		}
		if (c == EOF) {
			fatal_error("! End of file on the terminal");
		}
		append_char(xord[c]);
		if (cur_str_ptr >= cur_str_end) {
			overflow("str size", MAX_STR_SIZE);
		}
	}
	term_offset = 0;
	decr(selector);
	if (cur_str_ptr != cur_str) {
		for (s = cur_str; s < cur_str_ptr; incr(s)) {
			print_char(*s);
		}
	}
	print_ln();
	incr(selector);
}

bool
init_terminal ()
{
	loop {
		fputs("**", term_out);
		update_terminal();
		if (!input_ln(term_in)) {
			fputs("\n! End of file on the terminal...why?\n",
				term_out);
			return FALSE;
		}
		next = buffer;
		while (next <= limit && *next == ' ') {
			incr(next);
		}
		if (next <= limit) {
			return TRUE;
		}
		fputs("Please type the name of your input file.\n", term_out);
	}
}

void
open_log_file ()
{
	int	k;
	byte	*s;
	byte	*t;
	char	*months; 
	int	old_setting;

	old_setting = selector;
	if (job_name == null_str) {
		job_area = null_str;
		job_name = str_texput;
	}
	pack_job_name(str_log);
	while ((log_file = a_open_out()) == null_file) {
		if (interaction < SCROLL_MODE) {
			print_err("I can't write on file `");
			print_file_name(cur_name, cur_area, cur_ext);
			print("'.");
			job_name = null_str;
			history = FATAL_ERROR_STOP;
			jump_out();
		}
		prompt_file_name("transcript file name", str_log);
	}
	log_name = make_name_str();
	selector = LOG_ONLY;
	log_opened = TRUE;
	fputs(banner, log_file);
	if (format_ident == null_str) {
		print(" (no format preloaded)");
	} else {
		print(format_ident);
	}
	print(" ");
	print_int(day);
	print(" ");
	months = "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";
	for (k = 3 * month - 3; k < 3 * month; incr(k)) {
		putc(months[k], log_file);
	}
	print(" ");
	print_int(year);
	print(" ");
	print_two(time / 60);
	print(":");
	print_two(time % 60);
	*input_ptr = cur_input;
	print_nl("**");
	s = input_stack[0].obj_field.f.buf_field;
	t = input_stack[0].obj_field.f.limit_field;
	if (*t == end_line_char) {
		decr(t);
	}
	while (s <= t) {
		print_char(*s++);
	}
	print_ln();
	selector = old_setting + 2; 
}

str	cur_path;
str	input_path = default_input_path;
str 	font_path = default_font_path;

void
set_paths ()
{
	str	env_path;
	str	getenv();

	if (env_path = getenv("TEXINPUTS")) {
		input_path = env_path;
	}
	if (env_path = getenv("TEXFONTS")) {
		font_path = env_path;
	}
}

/*
**	test_access(amode, file_path)
**
**  Test whether or not the file whose name is in the global name_of_file
**  can be opened for reading according to access mode.
**
**  If the filename given in name_of_file does not begin with '/', we try 
**  prepending all the ':'-separated areanames in the appropriate path to the
**  filename until access can be made.
*/

bool
test_access (amode, file_path)
	int	amode;
	int	file_path;
{
	int	nl;
	bool	ok;
	char	original_name[MAX_STR_SIZE];

	strcpy(original_name, name_of_file);
	nl = name_length;
	switch (file_path)
	{
	case NO_FILE_PATH:
		cur_path = null_str;
		break;

	case INPUT_FILE_PATH: 
		cur_path = input_path;
		break;

	case FONT_FILE_PATH: 
		cur_path = font_path;
		break;

	}
#ifdef	OS2
        /*
	 * Watch for file names that begin with a '/', './' or
	 * have the form 'x:', where "x" is any alphabetic character
	 */
	if (name_of_file[0] == '/' 
	    || (name_of_file[0] == '.' && name_of_file[1] == '/')
	    || (isalpha(name_of_file[0]) && name_of_file[1] == ':')) {
#else
	if (name_of_file[0] == '/' ||
		name_of_file[0] == '.' && name_of_file[1] == '/') {
#endif
		cur_path = null_str;
	}
	do {
		strcpy(name_of_file, original_name);
		name_length = nl;
		if (get_real_name()) {
			switch (amode)
			{
			case READ_ACCESS: {
				FILE *fp = fopen(name_of_file, "r");
				ok = fp != (FILE *) 0;
				if (ok) {
					struct stat st;
					fstat(fileno(fp), &st);
					fclose(fp);
					ok = (st.st_mode & S_IFMT) == S_IFREG;
				}
				break;
				}

			case WRITE_ACCESS: {
				FILE *fp = fopen(name_of_file, "w");
				ok = fp != (FILE *) 0;
				if (ok)
					fclose(fp);
				break;
				}
			}
		} else {
			ok = FALSE;
		}
	} while (!ok && cur_path != null_str);
	return ok;
}

#define append_to_real_name(C) \
	{if (k == MAX_STR_SIZE) \
		overflow("real_name", MAX_STR_SIZE); \
	real_name[k++] = C;}

get_real_name ()
{
	int	k;
	str	s;
	char	real_name[MAX_STR_SIZE];
	
	real_name[k = 0] = NUL;
	s = cur_path;
	while (*s && *s != path_delimiter) {
		append_to_real_name(*s);
		incr(s);
	}
	if (*s == NUL) {
		cur_path = null_str;
	} else {
		cur_path = ++s;
	}
	if (k && real_name[k - 1] != area_delimiter) {
		append_to_real_name(area_delimiter);
	}
	if (*(s = name_of_file)) {
		while (*s) {
			append_to_real_name(*s);
			incr(s);
		}
	}
	name_length = k;
	append_to_real_name(NUL);
	strcpy(name_of_file, real_name);
	return k;
}

void
_fileio_init ()
{
	int	i;

	job_name = null_str;
	name_in_progress = FALSE;
	log_opened = FALSE;
	for (i = 0; i <= 16; incr(i)) {
		read_open[i] = CLOSED;
	}
}

void
_fileio_init_once ()
{
	name_of_file = new_str(MAX_STR_SIZE);
	area_delimiter = '/';
	ext_delimiter = '.';
#ifdef	OS2
	path_delimiter = ';';
#else	
	path_delimiter = ':';
#endif
	str_texput = "texput";
	str_tex = ".tex";
	str_dvi = ".dvi";
	str_log = ".log";
	str_tfm = ".tfm";
}
