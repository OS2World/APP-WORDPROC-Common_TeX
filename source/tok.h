
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

#define token_link(T)	link(T)
#define token(T)	info(T)

#define LEFT_BRACE_TOKEN	0400
#define LEFT_BRACE_LIMIT	01000
#define RIGHT_BRACE_TOKEN	01000
#define RIGHT_BRACE_LIMIT	01400
#define MATH_SHIFT_TOKEN	01400
#define TAB_TOKEN		02000
#define OUT_PARAM_TOKEN		02400
#define SPACE_TOKEN		05040
#define LETTER_TOKEN		05400
#define OTHER_TOKEN		06000
#define MATCH_TOKEN		06400
#define END_MATCH_TOKEN		07000
#define CS_TOKEN_FLAG		010000
#define is_sym(TOK)		((TOK) >= CS_TOKEN_FLAG)
#define sym2tok(SYM)		((tok) (CS_TOKEN_FLAG + (char *) (SYM)))
#define tok2sym(TOK)		((sym) ((TOK) - CS_TOKEN_FLAG))
#define END_TEMPLATE_TOKEN	sym2tok(FROZEN_END_TEMPLATE)

#define NO_EXPAND_FLAG		257

#define null_tok ((tok) 0)

global	tok	cur_tok;
global	tok	par_tok;

global	int	cur_cmd;
global	int	cur_chr;

void	get_token();
void	get_next();
void	get_cs();
void	check_outer_validity();
void	firm_up_the_line();

#define end_line_char_active	(end_line_char >= 0 && end_line_char <= 255)

global	bool	force_eof;
global	int	open_parens;
global	int	align_state;
global	int	scanner_status;

#define SKIPPING	1
#define DEFINING	2
#define MATCHING	3
#define ALIGNING	4
#define ABSORBING	5

global	ptr	null_list;
global	ptr	def_ref;
global	ptr	match_toks;
global	ptr	align_toks;
global	ptr	omit_template;

struct input {
	short	state_field;
	short	type_field;
	union {
		struct {
			sym	in_cs_field;
			ptr	start_field;
			ptr	loc_field;
			ptr	*param_field;
		} t;
		struct {
			ptr	in_open_field;
			byte	*buf_field;
			byte	*next_field;
			byte	*limit_field;
		} f;
	} obj_field;
};
typedef struct input input;

global	input	cur_input;
global	input	*input_stack;
global	input	*input_end;
global	input	*input_ptr;
global	input	*base_ptr;
global	input	*max_in_stack;

#define state		cur_input.state_field

#define MID_LINE	1
#define SKIP_BLANKS	(2 + MAX_CHAR_CODE)
#define NEW_LINE	(3 + MAX_CHAR_CODE + MAX_CHAR_CODE)

#define file_state	(state != TOKEN_LIST)

#define index		cur_input.type_field
#define in_file		cur_input.obj_field.f.in_open_field
#define buffer		cur_input.obj_field.f.buf_field
#define next		cur_input.obj_field.f.next_field
#define limit		cur_input.obj_field.f.limit_field

#define BUF_SIZE		4096

#define terminal_input	(index == 0)

struct infile {
	file	file_field;
	str	name_field;
	int	line_field;
};
typedef struct infile infile;

#define cur_file	((infile *) in_file)->file_field
#define file_name	((infile *) in_file)->name_field
#define file_line	((infile *) in_file)->line_field

global	str	name;
global	int	line;

global	infile	*file_stack;
global	infile	*file_end;
global	infile	*file_ptr;
global	infile	*max_file_stack;

#define TOKEN_LIST	0

#define token_type	cur_input.type_field
#define in_cs		cur_input.obj_field.t.in_cs_field
#define start		cur_input.obj_field.t.start_field
#define loc		cur_input.obj_field.t.loc_field
#define param_start	cur_input.obj_field.t.param_field

#define cs_name		text(in_cs)

#define PARAMETER		0
#define U_TEMPLATE		1
#define V_TEMPLATE		2
#define BACKED_UP		3
#define INSERTED		4
#define MACRO			5
#define OUTPUT_TEXT		6
#define EVERY_PAR_TEXT		7
#define EVERY_MATH_TEXT		8
#define EVERY_DISPLAY_TEXT	9
#define EVERY_HBOX_TEXT		10
#define EVERY_VBOX_TEXT		11
#define EVERY_JOB_TEXT		12
#define EVERY_CR_TEXT		13
#define MARK_TEXT		14
#define WRITE_TEXT		15

global	int	nparams;
global	ptr	*param_stack;
global	ptr	*param_end;
global	ptr	*param_ptr;
global	ptr	*max_param_stack;

void	push_input();
void	pop_input();
void	begin_token_list();
void	end_token_list();
void	begin_file_reading();
void	end_file_reading();
void	back_input();
void	back_error();
void	ins_error();
void	runaway();
void	show_context();
void	clear_for_error_prompt();

#define new_token	new_avail
#define free_token	free_avail

#define back_list(L)	begin_token_list(L, BACKED_UP)
#define ins_list(L)	begin_token_list(L, INSERTED)

void	_tok_init();
void	_tok_init_once();
