
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

global	int	interaction;

#define BATCH_MODE		0
#define NONSTOP_MODE		1
#define SCROLL_MODE		2
#define ERROR_STOP_MODE		3

#define wake_up_terminal()

void	begin_diagnostic();
void	end_diagnostic();

void	print_err();

global	bool	deletions_allowed;
global	int	error_cnt;

#ifdef NOHELP 
#define help0()
#define help1(h0)
#define help2(h0, h1)
#define help3(h0, h1, h2)
#define help4(h0, h1, h2, h3)
#define help5(h0, h1, h2, h3, h4) 
#define help6(h0, h1, h2, h3, h4, h5)
#else
#define help0() \
	{help_cnt = 0;}

#define help1(h0) \
	{help_cnt = 1; help_line[0] = h0;}

#define help2(h0, h1) \
	{help_cnt = 2; \
	help_line[0] = h0; help_line[1] = h1;}

#define help3(h0, h1, h2) \
	{help_cnt = 3; help_line[0] = h0; \
	help_line[1] = h1; help_line[2] = h2;}

#define help4(h0, h1, h2, h3) \
	{help_cnt = 4; \
	help_line[0] = h0; help_line[1] = h1; \
	help_line[2] = h2; help_line[3] = h3;}

#define help5(h0, h1, h2, h3, h4) \
	{help_cnt = 5; help_line[0] = h0; \
	help_line[1] = h1; help_line[2] = h2; \
	help_line[3] = h3; help_line[4] = h4;}

#define help6(h0, h1, h2, h3, h4, h5) \
	{help_cnt = 6; \
	help_line[0] = h0; help_line[1] = h1; \
	help_line[2] = h2; help_line[3] = h3; \
	help_line[4] = h4; help_line[5] = h5;}
#endif

global	str	help_line[];
global	int	help_cnt;
global	bool	use_err_help;

void	jump_out();
void	error();
void	normalize_selector();

global	int	history;

#define SPOTLESS		0
#define WARNING_ISSUED		1
#define ERROR_MESSAGE_ISSUED	2
#define FATAL_ERROR_STOP	3

void	int_error();
void	fatal_error();
void	overflow();
void	confusion();
void	succumb();

global	bool	interrupted;
global	bool	OK_to_interrupt;

void	pause_for_instructions();

#define check_interrupt() \
	{if (interrupted) pause_for_instructions();}

void	_error_init();
void	_error_init_once();
