
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

int	interaction;
int	history;
int	error_cnt;

bool	interrupted;
bool	OK_to_interrupt;
bool	deletions_allowed;
bool	use_err_help;

str	help_line[6];
int	help_cnt;
int	old_setting;

void
begin_diagnostic ()
{
	old_setting = selector;
	if (tracing_online <= 0 && selector == TERM_AND_LOG) {
		decr(selector);
		if (history == SPOTLESS) {
			history = WARNING_ISSUED;
		}
	}
}

void
end_diagnostic (blank_line)
	bool	blank_line;
{
	print_nl(null_str);
	if (blank_line) {
		print_ln();
	}
	selector = old_setting;
	fflush(stdout);
}

void
print_err (msg)
	str	msg;
{
	if (interaction == ERROR_STOP_MODE) {
		wake_up_terminal(); 
	}
	print_nl("! ");
	print(msg);
}

void
error ()
{
	int	c;
	int	hx;
	int	s1, s2, s3, s4;

	if (history < ERROR_MESSAGE_ISSUED) {
		history = ERROR_MESSAGE_ISSUED;
	}
	print(".");
	show_context();
	if (interaction == ERROR_STOP_MODE) {
		loop {
			clear_for_error_prompt();
			prompt_input("? ");
			if (cur_length() == 0) {
				return;
			}
			c = *cur_str;
			if (c >= 'a') {
				c -= 'a' - 'A';
			}
			switch (c)
			{
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
				if (! deletions_allowed) {
					continue;
				}
				s1 = cur_tok;
				s2 = cur_cmd;
				s3 = cur_chr;
				s4 = align_state;
				align_state = 1000000;
				OK_to_interrupt = FALSE;
				if (cur_str_ptr > cur_str + 1
				&& cur_str[1] >= '0'
				&& cur_str[1] <= '9') {
					c = c * 10 + cur_str[1] - '0' * 11;
				} else {
					c -= '0';
				}
				while (c-- > 0) {
					get_token();
				}
				cur_tok = s1;
				cur_cmd = s2;
				cur_chr = s3;
				align_state = s4;
				OK_to_interrupt = TRUE;
				help_delete_text();
				show_context();
				continue;
			
			case 'H':
				if (use_err_help)  {
					give_err_help();
					use_err_help = FALSE;
				} else {
					if (help_cnt == 0) {
						help_no_help();
					} else for (hx = 0;
						hx < help_cnt;
							incr(hx)) {
						print(help_line[hx]);
						print_ln();
					}
					help_help();
				}
				continue;
				
			case 'I':
				begin_file_reading();
				if (cur_length() > 1) {
					*cur_str = ' ';
				} else {
					prompt_input("insert>");
				}
				next = buffer;
				limit = buffer + cur_length() - 1;
				memcpy(buffer, cur_str, cur_length());
				flush_str();
				return;
			
			case 'Q':
			case 'R':
			case 'S':
				error_cnt = 0;
				interaction = BATCH_MODE + c - 'Q';
				print("OK, entering ");
				switch (c)
				{
				case 'Q':
					print_esc("batchmode");
					decr(selector);
					break;
				
				case 'R':
					print_esc("nonstopmode");
					break;
				
				case 'S':
					print_esc("scrollmode");
					break;
				}
				print("...");
				print_ln();
				update_terminal();
				return;
			
			case 'E':
				if (base_ptr > input_stack) {
					close_files_and_terminate(TRUE, FALSE);
				}
				break;

			case 'X':
				interaction = SCROLL_MODE;
				jump_out();
				break;

			default:
				print_menu();
				continue;
			}
		}
	}
	incr(error_cnt);
	if (error_cnt == 100) {
		print_nl("(That makes 100 errors; please try again.)");
		history = FATAL_ERROR_STOP;
		jump_out();
	}
	if (interaction > BATCH_MODE) {
		decr(selector);
	}
	if (use_err_help) {
		print_ln();
		give_err_help();
	} else for (hx = 0; hx < help_cnt; incr(hx)) {
		print_nl(help_line[hx]);
	}
	help_cnt = 0;
	print_ln();
	if (interaction > BATCH_MODE) {
		incr(selector);
	}
	print_ln();
}

print_menu ()
{
	print("Type <return> to proceed, S to scroll future error messages,");
	print_nl("R to run without stopping, Q to run quietly,");
	print_nl("I to insert something, ");
	if (base_ptr > input_stack) {
		print("E to edit your file,");
	}
	if (deletions_allowed) {
		print_nl("1 or ... or 9 to ignore the next 1 to 9 tokens of input");
	}
	print_nl("H for help, X to quit.");
}

void
int_error (i)
	int	i;
{
	print(" (");
	print_int(i);
	print(")");
	error();
}

void
normalize_selector ()
{
	if (log_opened) {
		selector = TERM_AND_LOG;
	} else {
		selector = TERM_ONLY;
	}
	if (job_name == null_str) {
		open_log_file();
	}
	if (interaction == BATCH_MODE) {
		decr(selector);
	}
}

void
jump_out ()
{
	close_files_and_terminate(FALSE, FALSE);
}

void
succumb()
{
	if (interaction == ERROR_STOP_MODE) {
		interaction = SCROLL_MODE;
	}
	if (log_opened) {
		error();
	}
	history = FATAL_ERROR_STOP;
	jump_out();
}

void
fatal_error (s)
	str	s;
{
	normalize_selector();
	print_err("Emergency stop");
	help1(s);
	succumb();
}

void
overflow (s, n)
	str	s;
	int	n;
{
	normalize_selector();
	print_err("TeX capacity exceeded, sorry [");
	print(s);
	print("=");
	print_int(n);
	print("]");
	help_capacity();
	succumb();
}

void
confusion (s)
	str	s;
{
	normalize_selector();
	if (history < ERROR_MESSAGE_ISSUED) {
		print_err("This can't happen (");
		print(s);
		print(")");
		help_broken();
	} else {
		print_err("I can't go on meeting you like this");
		help_wounded();
	}
	succumb();
}

void
pause_for_instructions ()
{
	if (OK_to_interrupt) {
		interaction = ERROR_STOP_MODE;
		if (selector == LOG_ONLY || selector == NO_PRINT) {
			incr(selector);
		}
		print_err("Interruption");
		help_interrupt();
		deletions_allowed = FALSE;
		error();
		deletions_allowed = TRUE;
		interrupted = 0;
	}
}

void
_error_init ()
{
	interrupted = FALSE;
	OK_to_interrupt = TRUE;
	deletions_allowed = TRUE;
	history = FATAL_ERROR_STOP;
	use_err_help = FALSE;
	error_cnt = 0;
	help_cnt = 0;
}

void
_error_init_once ()
{
	interrupted = FALSE;
	OK_to_interrupt = TRUE;
	deletions_allowed = TRUE;
	history = FATAL_ERROR_STOP;
	interaction = ERROR_STOP_MODE;
	use_err_help = FALSE;
	error_cnt = 0;
	help_cnt = 0;
}

/*
**	Help text
*/

help_delete_text () 
{
	help2("I have just deleted some text, as you asked.", 
	"You can now delete more, or insert, or whatever.");
}

help_no_help () 
{
	help2("Sorry, I don't know how to help in this situation.",
	"Maybe you should try asking a human?");
}

help_help () 
{
	help4("Sorry, I already gave what help I could...",
	"Maybe you should try asking a human?", 
	"An error might have occurred before I noticed any problems.",
	"``If all else fails, read the instructions.''");
}

help_capacity () 
{
	help2("If you really absolutely need more capacity,",
	"you can ask a wizard to enlarge me.");
}

help_broken () 
{
	help1("I'm broken. Please show this to someone who can fix can fix");
}

help_wounded () 
{
	help2("One of your faux pas seems to have wounded me deeply...",
	"in fact, I'm barely conscious. Please fix it and try again.");
}

help_interrupt () 
{
	help3("You rang?",
	"Try to insert some instructions for me (e.g., `I\\showlists),",
	"unless you just want to quit by typing `X'.");
}
