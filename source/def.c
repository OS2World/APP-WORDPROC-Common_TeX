
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

tok	after_token;
int	mag_set;
bool	long_help_seen;

#define glob	(a >= 4)

#define def(eq, type, value) \
	if (glob) \
		eq_gdefine(eq, type, value); \
	else eq_define(eq, type, value)

#define reg_def(reg, type, value) \
	if (glob) \
		reg_gdefine(reg, type, value); \
	else reg_define(reg, type, value)

void
get_r_token ()
{
restart:
	do get_token();
	while (cur_tok == SPACE_TOKEN);
	if (cur_cs == null_sym || isfrozen(cur_cs)) {
		print_err("Missing control sequence inserted");
		help_missing_cs();
		if (cur_cs == null_sym)
			back_input();
		cur_tok = sym2tok(FROZEN_PROTECTION);
		ins_error();
		goto restart;
	}
}

void
prefixed_command ()
{
	int	a = 0;

	while (cur_cmd == PREFIX) {
		if (!odd(a / cur_chr))
			a += cur_chr;
		get_nbrx_token();
		if (cur_cmd <= MAX_NON_PREFIXED_COMMAND) {
			print_err("You can't use a prefix with `");
			print_cmd_chr(cur_cmd, cur_chr);
			print("'");
			help_prefix();
			back_error();
			return;
		}
	}
	if (cur_cmd != DEF && a % 4 != 0) {
		print_err("You can't use `");
		print_esc("long");
		print("' or `");
		print_esc("outer");
		print("' with `");
		print_cmd_chr(cur_cmd, cur_chr);
		print("'");
		help_pref();
		error();
	}
	if (global_defs != 0) {
		if (global_defs < 0) {
			if (glob) a -= 4;
		} else {
			if (!glob) a += 4;
		}
	}
	switch (cur_cmd) 
	{
	case SET_FONT:
		reg_def(cur_font_ptr, FNT_REG, cur_chr);
		break;

	case DEF: {
		bool	e;
		sym	s;

		if (odd(cur_chr) && !glob && global_defs >= 0)
			a += 4;
		e = cur_chr >= 2;
		get_r_token();
		s = cur_cs;
		scan_toks(TRUE, e);
		def(s, CALL + (a % 4), def_ref);
		break;
	}
	case LET: {
		int	n;
		sym	s;
		tok	t;

		n = cur_chr;
		get_r_token();
		s = cur_cs;
		if (n == NORMAL) {
			do get_token();
			while (cur_cmd == SPACER);
			if (cur_tok == OTHER_TOKEN + '=') {
				get_token();
				if (cur_cmd == SPACER)
					get_token();
			}
		} else {
			get_token();
			t = cur_tok;
			get_token();
			back_input();
			cur_tok = t;
			back_input();
		}
		if (cur_cmd >= CALL)
			add_token_ref(cur_chr);
		def(s, cur_cmd, cur_chr);
		break;
	}
	case SHORTHAND_DEF: {
		int	n;
		sym	s;

		n = cur_chr;
		get_r_token();
		s = cur_cs; 
		def(s, RELAX, 256);
		scan_optional_equals();
		switch (n)
		{
		case CHAR_DEF_CODE:
			scan_char_num();
			def(s, CHAR_GIVEN, cur_val);
			break;

		case MATH_CHAR_DEF_CODE:
			scan_fifteen_bit_int();
			def(s, MATH_GIVEN, cur_val);
			break;

		default:
			scan_eight_bit_int();
			switch (n) {
			case MU_SKIP_DEF_CODE:
				def(s, ASSIGN_MU_GLUE, MU_GLUE_PARS + cur_val);
				break;
			case SKIP_DEF_CODE:
				def(s, ASSIGN_GLUE, GLUE_PARS + cur_val);
				break;
			case DIMEN_DEF_CODE:
				def(s, ASSIGN_DIMEN, DIMEN_PARS + cur_val);
				break;
			case COUNT_DEF_CODE:
				def(s, ASSIGN_INT, INT_PARS + cur_val);
				break;
			case TOKS_DEF_CODE:
				def(s, ASSIGN_TOKS, TOK_PARS + cur_val);
				break;
			}
			break;
		}
		break;
	}
	case READ_TO_CS: {
		int	n;
		sym	s;

		scan_int();
		n = cur_val;
		if (!scan_keyword("to")) {
			print_err("Missing `to' inserted");
			help_read_to();
			error();
		}
		get_r_token();
		s = cur_cs;
		read_toks(n, s);
		def(s, CALL, cur_val);
		break;
	}
	case TOKS_REGISTER:
	case ASSIGN_TOKS: {
		ptr	p;
		reg	r;
		sym	s;

		s = cur_cs;
		if (cur_cmd == TOKS_REGISTER) {
			scan_eight_bit_int();
			r = toks_reg[TOK_PARS + cur_val];
		} else {
			r = toks_reg[cur_chr];
		}
		scan_optional_equals();
		get_nbrx_token();
		if (cur_cmd != LEFT_BRACE) {
			if (cur_cmd == TOKS_REGISTER) {
				scan_eight_bit_int();
				cur_cmd = ASSIGN_TOKS;
				cur_chr = TOK_PARS + cur_val;
			}
			if (cur_cmd == ASSIGN_TOKS) {
				p = reg_equiv(toks_reg[cur_chr]);
				if (p == null) {
					reg_def(r, TOKS_REG, null);
				} else {    
					add_token_ref(p);
					reg_def(r, TOKS_REG, p);
				}
				break;
			}
		}
		back_input();
		cur_cs = s;
		scan_toks(FALSE, FALSE);
		if (token_link(def_ref) == null) {
			reg_def(r, TOKS_REG, null);
			free_token(def_ref);
		} else {
			if (r == output_routine_reg) {
				p = token_link(def_ref);
				while (token_link(p) != null)
					p = token_link(p);
				p = token_link(p) = new_token();
				token(p) = RIGHT_BRACE_TOKEN + '}';
				p = new_token();
				token(p) = LEFT_BRACE_TOKEN + '{';
				token_link(p) = token_link(def_ref);
				token_link(def_ref) = p;
			}
			reg_def(r, TOKS_REG, def_ref);
		}
		break;
	}
	case ASSIGN_INT: {
		reg		r;

		r = int_reg[cur_chr];
		scan_optional_equals();
		scan_int();
		reg_def(r, INT_REG, cur_val); 
		break;
	}
	case ASSIGN_DIMEN: {
		reg		r;

		r = dimen_reg[cur_chr];
		scan_optional_equals();
		scan_normal_dimen();
		reg_def(r, DIMEN_REG, cur_val); 
		break;
	}
	case ASSIGN_GLUE: {
		reg		r;

		r = skip_reg[cur_chr];
		scan_optional_equals();
		scan_glue(GLUE_VAL);
		trap_zero_glue();
		reg_def(r, SKIP_REG, cur_val); 
		break;
	}
	case ASSIGN_MU_GLUE: {
		reg		r;

		r = mu_skip_reg[cur_chr];
		scan_optional_equals();
		scan_glue(MU_VAL);
		trap_zero_glue();
		reg_def(r, MU_SKIP_REG, cur_val); 
		break;
	}
	case DEF_CODE: {
		int	m;
		int	n;
		int	o;
		reg		r;

		m = cur_chr;
		if (m == CAT_CODE) {
			n = MAX_CHAR_CODE;
			o = CAT_CODE_REG;
		} else if (m == LC_CODE) {
			n = 255;
			o = LC_CODE_REG;
		} else if (m == UC_CODE) {
			n = 255;
			o = UC_CODE_REG;
		} else if (m == SF_CODE) {
			n = 077777;
			o = SF_CODE_REG;
		} else if (m == MATH_CODE) {
			n = 0100000;
			o = MATH_CODE_REG;
		} else if (m == DEL_CODE) {
			n = 077777777;
			o = DEL_CODE_REG;
		} else {
			confusion("confusing code");
		}
		scan_char_num();
		switch (m) {
		case CAT_CODE: r = cat_code_reg[cur_val]; break;
		case UC_CODE: r = uc_code_reg[cur_val]; break;
		case LC_CODE: r = lc_code_reg[cur_val]; break;
		case SF_CODE: r = sf_code_reg[cur_val]; break;
		case MATH_CODE: r = math_code_reg[cur_val]; break;
		case DEL_CODE: r = del_code_reg[cur_val]; break;
		}
		scan_optional_equals();
		scan_int(); 
		if (cur_val < 0 && m != DEL_CODE || cur_val > n) {
			print_err("Invalid code (");
			print_int(cur_val);
			if (m != DEL_CODE) {
				print("), should be in the range 0..");
			} else {
				print("), should be at most ");
			}
			print_int(n);
			help_code();
			error();
			cur_val = 0;
		}
		reg_def(r, o, cur_val);
		break;
	}
	case DEF_FAMILY: {
		int	i;
		reg	r;

		i = cur_chr;
		scan_four_bit_int();
		i += cur_val;
		scan_optional_equals();
		scan_font_ident();
		r = fnt_reg[1 + i];
		reg_def(r, FNT_REG, cur_val);
		break;
	}
	case REGISTER:
	case ADVANCE:
	case MULTIPLY:
	case DIVIDE:
		do_register_command(a);
		break;

	case SET_BOX: {
		int	n;

		scan_eight_bit_int();
		if (glob) {
			n = 256 + cur_val;
		} else {
			n = cur_val;
		}
		scan_optional_equals();
		scan_box(BOX_FLAG + n);
		break;
	}
	case SET_AUX:
		alter_aux();
		break;

	case SET_PREV_GRAF:
		alter_prev_graf();
		break;

	case SET_PAGE_DIMEN:
		alter_page_so_far();
		break;

	case SET_PAGE_INT:
		alter_integer();
		break;

	case SET_BOX_DIMEN:
		alter_box_dimen();
		break;

	case SET_SHAPE: {
		int	i;
		int	n;
		ptr	p;

		scan_optional_equals();
		scan_int();
		n = cur_val;
		if (n <= 0) {
			def(par_shape_cs, SET_SHAPE, null);
		} else {
			p = new_node(sizeof(mcell) + n * sizeof(shape_t));
			def(par_shape_cs, SET_SHAPE, p);
			info(p) = n;
			for (i = 1; i <= n; incr(i)) {
				scan_normal_dimen();
				par_shape_indent(i) = cur_val;
				scan_normal_dimen();
				par_shape_width(i) = cur_val;
			}
		}
		break;
	}
	case HYPH_DATA:
		if (cur_chr == 1) 
			new_patterns();
		else new_hyph_exceptions();
		break;

	case ASSIGN_FONT_DIMEN: {
		scal	*d;

		d = find_font_dimen(TRUE);
		scan_optional_equals();
		scan_normal_dimen();
		if (d != (scal *) 0)
			*d = cur_val;
		break;
	}
	case ASSIGN_FONT_INT: {
		int	f;
		int	n;

		n = cur_chr;
		scan_font_ident();
		f = cur_val;
		scan_optional_equals();
		scan_int(); 
		if (n == 0)
			hyphen_char(f) = cur_val;
		else skew_char(f) = cur_val;
		break;
	}
	case DEF_FONT:
		new_font(a);
		break;

	case SET_INTERACTION:
		new_interaction();
		break;

	default:
		confusion("prefix");
		break;
	}
	if (after_token != 0) {
		cur_tok = after_token;
		back_input();
		after_token = 0;
	}
} 

void
trap_zero_glue()
{
	if (glue_width(cur_val) == 0
	&& stretch(cur_val) == 0
	&& shrink(cur_val) == 0) {
		add_glue_ref(zero_glue);
		delete_glue_ref(cur_val);
		cur_val = zero_glue;
	}
}

void
do_register_command (a)
	int	a;
{
	int	b;
	int	c;
	ptr	p;
	reg	r;
	int	v;

	c = cur_cmd;
	if (c != REGISTER) {
		get_x_token();
		if (cur_cmd >= ASSIGN_INT && cur_cmd <= ASSIGN_MU_GLUE) {
			b = cur_cmd - ASSIGN_INT;
			switch (b) {
			case INT_VAL: r = int_reg[cur_chr]; break;
			case DIMEN_VAL: r = dimen_reg[cur_chr]; break; 
			case GLUE_VAL: r = skip_reg[cur_chr]; break; 
			case MU_VAL: r = mu_skip_reg[cur_chr]; break;
			}
			goto found;
		}
		if (cur_cmd != REGISTER) {
			print_err("You can't use `");
			print_cmd_chr(cur_cmd, cur_chr);
			print("' after ");
			print_cmd_chr(c, 0);
			help_register();
			error();
			return;
		}
	}

	b = cur_chr;
	scan_eight_bit_int();
	switch (b) {
	case INT_VAL: r = int_reg[INT_PARS + cur_val]; break;
	case DIMEN_VAL: r = dimen_reg[DIMEN_PARS + cur_val]; break; 
	case GLUE_VAL: r = skip_reg[GLUE_PARS + cur_val]; break; 
	case MU_VAL: r = mu_skip_reg[MU_GLUE_PARS + cur_val]; break;
	}

found:
	v = reg_equiv(r);
	if (c == REGISTER)
		scan_optional_equals();
	else scan_keyword("by");
	arith_error = FALSE;
	if (c < MULTIPLY)  {
		if (b < GLUE_VAL) {
			if (b == INT_VAL)
				scan_int();
			else scan_normal_dimen();
			if (c == ADVANCE)
				cur_val += v;
		} else {
			scan_glue(b);
			if (c == ADVANCE) {
				p = new_spec(cur_val);
				delete_glue_ref(cur_val);
				glue_width(p) += glue_width(v);
				if (stretch(p) == 0)
					stretch_order(p) = NORMAL;
				if (stretch_order(p) == stretch_order(v)) {
					stretch(p) += stretch(v);
				} else if (stretch_order(p) < stretch_order(v)
					&& stretch(v)) {
					stretch(p) = stretch(v);
					stretch_order(p) = stretch_order(v);
				}
				if (shrink(p) == 0)
					shrink_order(p) = NORMAL;
				if (shrink_order(p) == shrink_order(v)) {
					shrink(p) += shrink(v);
				} else if (shrink_order(p) < shrink_order(v)
					&& shrink(v)) {
					shrink(p) = shrink(v); 
					shrink_order(p) = shrink_order(v);
				}
				cur_val = p;
			}
		}
	} else {
		scan_int();
		if (b < GLUE_VAL) {
			if (c == MULTIPLY) {
				if (b == INT_VAL) {
					cur_val = mult_integers(v, cur_val);
				} else {
					cur_val = nx_plus_y(v, cur_val, 0);
				}
			} else {
				cur_val = x_over_n(v, cur_val);
			}
		} else {
			p = new_spec(v);
			if (c == MULTIPLY) {
				glue_width(p) =
					nx_plus_y(glue_width(v), cur_val, 0);
				stretch(p) = nx_plus_y(stretch(v), cur_val, 0);
				shrink(p) = nx_plus_y(shrink(v), cur_val, 0);
			} else {
				glue_width(p) =
					x_over_n(glue_width(v), cur_val);
				stretch(p) = x_over_n(stretch(v), cur_val);
				shrink(p) = x_over_n(shrink(v), cur_val);
			}
			cur_val = p;
		}
	}
	if (arith_error) {
		print_err("Arithmetic overflow");
		help_overflow();
		error();
		return;
	}
	switch (b) {
	case INT_VAL: reg_def(r, INT_REG, cur_val); break;
	case DIMEN_VAL: reg_def(r, DIMEN_REG, cur_val); break;
	case GLUE_VAL: trap_zero_glue(); reg_def(r, SKIP_REG, cur_val); break;
	case MU_VAL: trap_zero_glue(); reg_def(r, MU_SKIP_REG, cur_val); break;
	}
}

void
alter_aux ()
{
	int	c;

	if (cur_chr != abs(mode)) {
		report_illegal_case();
	} else {
		c = cur_chr;
		scan_optional_equals();
		if (c == VMODE)  {
			scan_normal_dimen();
			prev_depth = cur_val;
		} else {
			scan_int();
			if (cur_val <= 0 || cur_val > 32767) {
				print_err("Bad space factor");
				help_space_factor();
				int_error(cur_val);
			} else {
				space_factor = cur_val;
			}
		}
	}
}

void
alter_prev_graf ()
{
	list	*p;

	*nest_ptr = cur_list;
	p = nest_ptr;
	while (abs(p->mode_field) != VMODE)
		decr(p);
	scan_optional_equals();
	scan_int();
	if (cur_val < 0) {
		print_err("Bad ");
		print_esc("prevgraf");
		help_prevgraf();
		int_error(cur_val);
	} else {
		p->pg_field = cur_val;
		cur_list = *nest_ptr;
	}
}

void
alter_page_so_far ()
{
	int	c;

	c = cur_chr;
	scan_optional_equals(); 
	scan_normal_dimen();
	page_so_far[c] = cur_val;
}

void
alter_integer ()
{
	int	c;
	
	c = cur_chr;
	scan_optional_equals();
	scan_int();
	if (c == 0) {
		dead_cycles = cur_val;
	} else {
		insert_penalties = cur_val;
	}
}

void
alter_box_dimen ()
{
	int	b;
	int	c;
	
	c = cur_chr;
	scan_eight_bit_int();
	b = cur_val; 
	scan_optional_equals();
	scan_normal_dimen();
	if (box(b) != null) {
		switch (c) {
		case WD_CODE: box_width(box(b)) = cur_val; break;
		case HT_CODE: box_height(box(b)) = cur_val; break;
		case DP_CODE: box_depth(box(b)) = cur_val; break;
		}
	}
}

void
new_font (a)
	int	a;
{
	fnt	f;
	scal	s;
	str	t;
	int	l;
	sym	u;

#define ILL_MAG "Illegal magnification has been changed to 1000"

	if (job_name == null_str)
		open_log_file();
	get_r_token();
	u = cur_cs;
	switch (length(u))
	{
	case 0:
		t = make_str_given("FONT");
		l = 0;
		break;

	case 1:
		if (isactive(u)) {
			int old_setting = selector;
			selector = NEW_STRING;
			print("FONT");
			print_ASCII(textbyte(u));
			selector = old_setting;
			l = cur_length();
			t = make_str();
		} else {
			l = 1;
			t = text(u);
		}
		break;

	default:
		l = length(u);
		t = text(u);
		break;
	}
	def(u, SET_FONT, null_font);
	scan_optional_equals();
	scan_file_name();
	name_in_progress = TRUE;
	if (scan_keyword("at")) {
		scan_normal_dimen();
		s = cur_val;
		if (s <= 0 || s >= 01000000000) {
			print_err("Improper `at' size (");
			print_scaled(s);
			print("pt), replaced by 10pt");
			help_font_at();
			error();
			s = 10 * UNITY;
		}
	} else if (scan_keyword("scaled")) {
		scan_int();
		s = -cur_val;
		if (cur_val <= 0 || cur_val > 32768) {
			print_err(ILL_MAG);
			help_font_magnification();
			int_error(cur_val);
			s = -1000;
		}
	} else {
		s = -1000;
	}
	name_in_progress = FALSE;
	for (f = null_font + 1; f < null_font + font_ptr - font_info; f++) {
		if (str_eq(font_name(f), cur_name) &&
			str_eq(font_area(f), cur_area)) {
			if (s > 0) {
				if (s == font_size(f))
					goto common_end;
			} else if (font_size(f) ==
				xn_over_d(font_dsize(f), -s, 1000)) {
				goto common_end;
			}
		}
	}
	f = read_font_info(u, cur_name, cur_area, s);

common_end:
	equiv(u) = f;
	*font_id_base[f] = *u;
	font_id_text(f) = t;
	font_id_length(f) = l;
}

void
prepare_mag ()
{
	if (mag_set > 0 && mag != mag_set) {
		print_err("Incompatible magnification (");
		print_int(mag);
		print(");");
		print_nl(" the previous value will be retained");
		help_mag();
		int_error(mag_set);
		reg_gdefine(int_reg[MAG_CODE], INT_REG, mag_set);
	}
	if (mag <= 0 || mag > 32768) {
		print_err("Illegal magnification has been changed to 1000");
		help_ill_mag();
		int_error(mag);
		reg_gdefine(int_reg[MAG_CODE], INT_REG, 1000);
	}
	mag_set = mag;
}

void
new_interaction ()
{
	print_ln();
	interaction = cur_chr;
	if (interaction == BATCH_MODE)
		selector = NO_PRINT;
	else selector = TERM_ONLY;
	if (job_name != null_str)
		selector += 2;
}

void
do_assignments ()
{
	loop {
		get_nbrx_token();
		if (cur_cmd <= MAX_NON_PREFIXED_COMMAND)
			break;
		prefixed_command();
	}
}

void
clopen_stream ()
{	
	int	c;
	int	n;

	c = cur_chr;
	scan_four_bit_int();
	n = cur_val; 
	if (read_open[n] != CLOSED) {
		a_close(read_file[n]);
		read_open[n] = CLOSED;
	}
	if (c != 0) {
		scan_optional_equals();
		scan_file_name();
		if (cur_ext == null_str) {
			cur_ext = str_tex;
		}
		pack_cur_name();
		read_file[n] = a_open_in();
		if (read_file[n]) {
			read_open[n] = OPENED;
		}
	}
}

void
issue_message ()
{
	int	c;
	str	s;
	int	old_setting;

	c = cur_chr;
	scan_toks(FALSE, TRUE);
	old_setting = selector;
	selector = NEW_STRING;
	token_show(def_ref);
	selector = old_setting;
	flush_list(def_ref);
	s = make_str();
	if (c == 0) {
		if (term_offset + str_length(s) > MAX_PRINT_LINE - 2) {
			print_ln();
		} else if (term_offset > 0 || file_offset > 0) {
			print(" ");
		}
		print(s);
		update_terminal();
	} else {
		print_err(null_str);
		print(s);
		if (err_help != null) {
			use_err_help = TRUE;
		} else if (long_help_seen) {
			help_err_msg();
		} else {
			if (interaction < ERROR_STOP_MODE) {
				long_help_seen = TRUE;
			}
			help_poirot();
		}
		error();
		use_err_help = FALSE;
	}
	flush_str();
}

void
give_err_help ()
{
	token_show(err_help);
}

void
shift_case ()
{
	reg	*r;
	int	c;
	int	i;
	ptr	p;
	sym	s;
	tok	t;

	r = (cur_chr == LC_CODE) ? lc_code_reg : uc_code_reg;
	scan_toks(FALSE, FALSE);
	for (p = token_link(def_ref); p != null; p = token_link(p)) {
		t = token(p);
		if (is_sym(t)) {
			s = tok2sym(t);
			if (!isactive(s)) {
				continue;
			}
			c = textbyte(s);
			if (reg_equiv(r[c]) != 0) {
				i = reg_equiv(r[c]);
				token(p) = sym2tok(active_base[i]);
			}
		} else {
			c = t % 0400;
			if (reg_equiv(r[c]) != 0) {
				i = reg_equiv(r[c]);
				token(p) = (t / 0400) * 256 + i;
			}
		}
	}
	back_list(token_link(def_ref));
	free_token(def_ref);
}

void
show_whatever ()
{
	ptr	t;
	
	switch (cur_chr)
	{
	case SHOW_LISTS:
		begin_diagnostic();
		show_activities();
		break;

	case SHOW_BOX_CODE:
		scan_eight_bit_int();
		begin_diagnostic();
		print_nl("> \\box");
		print_int(cur_val);
		print("=");
		if (box(cur_val) == null) {
			print("void");
		} else {
			show_box(box(cur_val));
		}
		break;
	
	case SHOW_CODE:
		get_token();
		print_nl("> ");
		if (cur_cs != null_sym) {
			sprint_cs(cur_cs);
			print("=");
		}
		print_meaning();
		goto common_end;
	
	default:
		t = the_toks();
		print_nl("> ");
		token_show(t);
		flush_list(t);
		goto common_end;
	}
	end_diagnostic(TRUE);
	print_err("OK");
	if (selector == TERM_AND_LOG && tracing_online <= 0) {
		selector = TERM_ONLY;
		print(" (see the transcript file)");
		selector = TERM_AND_LOG;
	}

common_end:
	if (interaction < ERROR_STOP_MODE) {
		help0();
		decr(error_cnt);
	} else if (tracing_online > 0) {
		help_show_online();
	} else {
		help_show();
	}
	error();
}

void
_def_init ()
{
	long_help_seen = FALSE;
	mag_set = 0;
}

void
_def_init_once ()
{
}

/*
**	Help text
*/

help_missing_cs ()
{
	help5("Please don't say `\\def cs{...}', say `\\def\\cs{...}'.",
	"I've inserted an inaccessible control sequence so that your",
	"definition will be completed without mixing me up too badly.",
	"You can recover graciously from this error, if you're",
	"careful; see exercise 27.2 in The TeXbook.");
}

help_prefix ()
{
	help1("I'll pretend you didn't say \\long or \\outer or \\global.");
}

help_pref ()
{
	help1("I'll pretend you didn't say \\long or \\outer here.");
}

help_read_to ()
{
	help2("You should have said `\\read<number> to \\cs'.",
	"I'm going to look for the \\cs now.");
}

help_code ()
{
	help1("I'm going to use 0 instead of that illegal code value.");
}

help_register ()
{
	help1("I'm forgetting what you said and not changing anything.");
}

help_space_factor ()
{
	help1("I allow only values in the range 1..32767 here.");
}

help_prevgraf ()
{
	help1("I allow only nonnegative values here.");
}

help_overflow ()
{
	help2("I can't carry out that multiplication or division,",
	"since the result is out of range.");
}

help_font_at ()
{
	help2("I can only handle fonts at positive sizes that are",
	"less than 2048pt, so I've changed what you said to 10pt.");
}

help_font_magnification ()
{
	help1("The magnification ratio must be between 1 and 32768.");
}

help_mag()
{
	help2("I can handle only one magnification ratio per job. So I've",
	"reverted to the magnification you used earlier on this run.");
}

help_ill_mag ()
{
	help1("The magnification ratio must be between 1 and 32768.");
}

help_err_msg ()
{
	help1("(That was another \\errmessage.)");
}

help_poirot ()
{
	help4("This error message was generated by an \\errmessage",
	"command, so I can't give any explicit help.",
	"Pretend that you're Hercule Poirot: Examine all clues,",
	"and deduce the truth by order and method.");
}

help_show_online ()
{
	help3("This isn't an error message; I'm just \\showing something.",
	"Type `I\\show...' to show more (e.g., \\show\\cs,",
	"\\showthe\\count10, \\showbox255, \\showlists).");
}

help_show ()
{
	help5("This isn't an error message; I'm just \\showing something.",
	"Type `I\\show...' to show more (e.g., \\show\\cs,",
	"\\showthe\\count10, \\showbox255, \\showlists).",
	"And type `I\\tracingonline=1\\show...' to show boxes and",
	"lists on your terminal as well as in the transcript file.");
}
