
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

global	file	log_file;
global	str	log_name;

global	int	selector;

#define NO_PRINT	16 
#define TERM_ONLY	17
#define LOG_ONLY	18
#define TERM_AND_LOG	19
#define PSEUDO		20
#define NEW_STRING	21
#define MAX_SELECTOR	21

global	int	term_offset;
global	int	file_offset;

global	int	tally;
global	int	first_count;
global	int	trick_count;
global	int	trick_buf[];

int	begin_pseudoprint();
void	set_trick_count();

#define HALF_ERROR_LINE		32
#define ERROR_LINE		64
#define MAX_PRINT_LINE		72

global	int	dig[];

void	print();
void	print_char();
void	print_ASCII();
void	print_ln();
void	print_esc();
void	print_nl();
void	print_the_digs();
void	print_int();
void	print_two();
void	print_hex();
void	print_roman_int();

#define wterm(c)	{putchar(c);}
#define wterm_cr()	{putchar('\n');}
#define wlog(c)		{putc(c, log_file);}
#define wlog_cr()	{putc('\n', log_file);}
#define wfile(c)	{putc(c, write_file[selector]);}
#define wfile_cr()	{putc('\n', write_file[selector]);}

void	_print_init();
void	_print_init_once();
