
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

global	int	name_length;
global	str	name_of_file;

global	str	cur_name;
global	str	cur_area;
global	str	cur_ext;

global	int	area_delimiter;
global	int	ext_delimiter;
global	int	path_delimiter;

void	print_file_name();
void	pack_file_name();

str	make_name_str();

global	bool	name_in_progress;

void	prompt_file_name();
void	scan_file_name();

#define pack_cur_name() \
	{pack_file_name(cur_name, cur_area, cur_ext);}

void	pack_job_name();
void	prompt_job_name();

global	str	job_name;
global	str	job_area;

global	bool	log_opened;

void	start_input();

#define null_file ((FILE *) 0)

FILE	*a_open_in();
FILE	*a_open_out();

FILE	*b_open_in();
FILE	*b_open_out();

#define a_close(FP)		{fflush(FP);fclose(FP);}
#define b_close(FP)		{fflush(FP);fclose(FP);}

bool 	init_terminal();
bool	input_ln();
void	term_input();
#define prompt_input(S)		{print(S); term_input();}

#define term_in			stdin
#define term_out		stdout
#define t_open_in()
#define t_open_out()
#define update_terminal()	fflush(stdout)
#define clear_terminal()

void	open_log_file();

global	file	read_file[];
global	int	read_open[];

#define OPENED			1
#define CLOSED			2

bool	test_access();

#define READ_ACCESS		4
#define WRITE_ACCESS 		2

global	char	*input_path;
global	char	*format_path;
global	char	*font_path;

void	set_paths();

#define default_font_path	":/usr/local/lib/tex/fonts/tfm"
#define default_input_path 	":/usr/local/lib/tex/inputs"

#define NO_FILE_PATH 		0
#define INPUT_FILE_PATH 	1
#define FONT_FILE_PATH 		2

global	str	str_dvi;
global	str	str_tex;
global	str	str_log;
global	str	str_tfm;
global	str	str_texput;

void	_fileio_init();
void	_fileio_init_once();
