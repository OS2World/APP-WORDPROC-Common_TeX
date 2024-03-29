
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

sym	cur_cs;
sym	warning_cs;

sym	par_cs;
sym	par_shape_cs;
sym	undefined_cs;
sym	null_cs;

#define HASH_SIZE	1231

sym	*active_base;
sym	*single_base;
sym	*hash_base;
sym	*frozen_cs_base;
sym	*font_id_base;

reg	*box_reg;
reg	*mu_skip_reg;
reg	*skip_reg;
reg	*dimen_reg;
reg	*int_reg;
reg	*fnt_reg;
reg	*toks_reg;
reg	*cat_code_reg;
reg	*lc_code_reg;
reg	*uc_code_reg;
reg	*sf_code_reg;
reg	*math_code_reg;
reg	*del_code_reg;


ptr	hold_head;
ptr	zero_glue;
ptr	fil_glue;
ptr 	fill_glue;
ptr	ss_glue;
ptr	fil_neg_glue;

bool	no_new_control_sequence;

#ifdef STAT
int	cs_count;
#endif

int	cur_level;
int	cur_group;

#define SAVE_SIZE	1000
#define SAVE_INC	300

int	nsaves;
reg	save_stack;
reg	save_end;
reg	save_ptr;
reg	max_save_stack;

sym
id_lookup (s, l)
	str	s;
	int	l;
{
	int	h;
	int	k;
	sym	c;
	str	t;

	t = s;
	h = *t++;
	k = l - 1;
	while (k--) {
		h = h + h + *t++;
		while (h >= HASH_SIZE) {
			h -= HASH_SIZE;
		}
	}
	for (c = hash_base[h]; c; c = hash(c)) {
		if (length(c) == l && str_eqn(text(c), s, l)) {
			return c;
		}
		if (hash(c) == null_sym) {
			if (no_new_control_sequence) {
				return undefined_cs;
			}
			k = l;
			str_room(k);
			while (k--) {
				append_char(*s++);
			}
			c = hash(c) = new_sym(make_str(), l);
#ifdef STAT
			incr(cs_count);
#endif
			return c;
		}
	}
	k = l;
	str_room(k);
	while (k--) {
		append_char(*s++);
	}
#ifdef STAT
	incr(cs_count);
#endif
	return (hash_base[h] = new_sym(make_str(), l));
}

bool
isactive (s)
	sym	s;
{
	return (active_base[0] <= s && s <= active_base[255]);
}

bool
issingle (s)
	sym	s;
{
	return (single_base[0] <= s && s <= single_base[255]);
}

bool
isfrozen (s)
	sym	s;
{
	int	i;

	for (i = 1; i < FROZEN_CONTROLS; incr(i)) {
		if (s == FROZEN_CONTROL_SEQUENCE[i]) {
			return 1;
		}
	}
	return 0;
}

sym
new_sym (s, l)
	str	s;
	int	l;
{
	sym	c;
	
	c = (sym) new_node(sizeof(sym_t));
	eq_type(c) = UNDEFINED_CS;
	eq_level(c) = LEVEL_ONE;
	equiv(c) = 0;
	text(c) = s;
	length(c) = l;
	hash(c) = null_sym;

	return  c;
}

void
print_cs (s)
	sym	s;
{	
	str 	t;
	int	i;

	if (s == null_cs) {
		print_esc("csname");
		print_esc("endcsname");
	} else if (isactive(s)) {
		print_ASCII(textbyte(s));
	} else if (issingle(s)) {
		print_esc("");
		print_ASCII(textbyte(s));
		if (cat_code(textbyte(s)) == LETTER)
			print(" ");
	} else {
		t = text(s);
		print_esc(null_str);
		for (i = 0; i < length(s); incr(i)) {
			print_ASCII(t[i]);
		}
		print(" ");
	}
}

void
sprint_cs (s)
	sym	s;
{
	str 	t;
	int	i;

	if (s == null_cs) {
		print_esc("csname");
		print_esc("endcsname");
	} else if (isactive(s)) {
		print_ASCII(textbyte(s));
	} else if (issingle(s)) {
		print_esc("");
		print_ASCII(textbyte(s));
	} else {
		t = text(s);
		print_esc(null_str);
		for (i = 0; i < length(s); incr(i)) {
			print_ASCII(t[i]);
		}
	}
}

sym
primitive (s, code, order)
	str	s;
	int	code;
	int	order;
{
	int	l;

	if (s[1] == NUL) {
		cur_cs = single_base[s[0]];
	} else {
		l = str_length(s);
		cur_cs = id_lookup(s, l);
	}
	eq_level(cur_cs) = LEVEL_ONE;
	eq_type(cur_cs) = code;
	equiv(cur_cs) = order;

	return cur_cs;
}

sym
freeze (s)
	sym	s;
{
	sym	t;

	t = new_sym(text(s), length(s));
	eq_level(t) = eq_level(s);
	eq_type(t) = eq_type(s);
	equiv(t) = equiv(s);

	return t;
}

#define check_full_save_stack() \
{	if (save_ptr > max_save_stack) { \
		if (max_save_stack > save_end - 6 \
		&& !realloc_save_stack()) \
			overflow("save size", nsaves); \
		max_save_stack = save_ptr; \
	} \
}

bool
realloc_save_stack ()
{
	reg	rtmp;
	int	ntmp;

	nsaves += SAVE_INC;
	ntmp = save_ptr - save_stack;
	rtmp = (reg)realloc(save_stack, nsaves * sizeof(mcell));
	if (rtmp == (reg) 0) {
		overflow("save stack", nsaves);
	}
	save_stack = rtmp;
	save_end = save_stack + nsaves - 1;
	save_ptr = save_stack + ntmp;
}

void
new_save_level (c)
	int	c;
{
	check_full_save_stack();
	save_type(save_ptr) = LEVEL_BOUNDARY;
	save_level(save_ptr) = cur_group;
	if (cur_level == MAX_QUARTERWORD) {
		overflow("grouping levels", MAX_QUARTERWORD - MIN_QUARTERWORD);
	}
	cur_group = c;
	incr(cur_level);
	incr(save_ptr);
}

void
reg_destroy (r)
	reg	r;
{
	ptr	p;

	switch (reg_type(r))
	{
	case SET_SHAPE:
		p = reg_equiv(r);
		if (p != null) {
			free_node(p, sizeof(mcell) + info(p) * sizeof(shape_t));
		}
		break;

	case CALL:
	case LONG_CALL:
	case LONG_OUTER_CALL:
		delete_token_ref(reg_equiv(r));
		break;

	case SKIP_REG:
	case MU_SKIP_REG:
		delete_glue_ref(reg_equiv(r));
		break;

	case BOX_REG:
		flush_node_list(reg_equiv(r));
		break;

	case TOKS_REG:
		if (reg_equiv(r) != null) {
			delete_token_ref(reg_equiv(r));
		}
		break;
	}
}

void
reg_save (r, l)
	reg	r;
	int	l;
{
	check_full_save_stack();
	*save_ptr++ = *r;
	save_type(save_ptr) = RESTORE_OLD_VALUE;
	save_level(save_ptr) = l;
	save_index(save_ptr) = (int) r;
	incr(save_ptr);
}

void
eq_define (s, t, e)
	sym	s;
	int	t;
	int	e;
{
	if (eq_level(s) == cur_level) {
		reg_destroy(sym2reg(s));
	} else if (cur_level > LEVEL_ONE) {
		reg_save(sym2reg(s), eq_level(s));
	}
	eq_level(s) = cur_level;
	eq_type(s) = t;
	equiv(s) = e;
}

void
reg_define (r, t, e)
	reg	r;
	int	t;
	int	e;
{
	if (reg_level(r) == cur_level) {
		reg_destroy(r);
	} else if (cur_level > LEVEL_ONE) {
		reg_save(r, reg_level(r));
	}
	reg_level(r) = cur_level;
	reg_type(r) = t;
	reg_equiv(r) = e;
}

void
eq_gdefine (s, t, e)
	sym	s;
	int	t;
	int	e;
{
	reg_destroy(sym2reg(s));
	eq_level(s) = LEVEL_ONE;
	eq_type(s) = t;
	equiv(s) = e;
}

void
reg_gdefine (r, t, e)
	reg	r;
	int	t;
	int	e;
{
	reg_destroy(r);
	reg_level(r) = LEVEL_ONE;
	reg_type(r) = t;
	reg_equiv(r) = e;
}

void
save_for_after (t)
	tok	t;
{
	if (cur_level > LEVEL_ONE) {
		check_full_save_stack();
		save_type(save_ptr) = INSERT_TOKEN;
		save_level(save_ptr) = LEVEL_ZERO;
		save_index(save_ptr) = t;
		incr(save_ptr);
	}
}

void
unsave ()
{
	reg	r;
	tok	t;

	if (cur_level > LEVEL_ONE) {
		decr(cur_level);
		loop {
			decr(save_ptr);
			if (save_type(save_ptr) == LEVEL_BOUNDARY) {
				break;
			}
			r = (reg) save_index(save_ptr);
			if (save_type(save_ptr) == INSERT_TOKEN) {
				t = cur_tok;
				cur_tok = (tok) r;
				back_input();
				cur_tok = t;
			} else if (save_type(save_ptr) == RESTORE_OLD_VALUE) {
				decr(save_ptr);
				if (reg_level(r) == LEVEL_ONE) {
					reg_destroy(save_ptr);
					if (tracing_restores > 0) {
						restore_trace(r, "retaining");
					}
				} else {
					reg_destroy(r);
					*r = *save_ptr;
					if (tracing_restores > 0) {
						restore_trace(r, "restoring");
					}
				}
			}
		}
		cur_group = save_level(save_ptr);
	} else {
		confusion("curlevel");
	}
}

void
off_save ()
{	
	ptr	h;
	ptr	p;

	if (cur_group == BOTTOM_LEVEL) {
		print_err("Extra ");
		print_cmd_chr(cur_cmd, cur_chr);
		help_offsave_xtra();
		error();
	} else {
		back_input();
		h = p = new_token();
		print_err("Missing ");
		switch (cur_group) 
		{
		case SEMI_SIMPLE_GROUP:
			token(p) = sym2tok(FROZEN_END_GROUP);
			print_esc("endgroup");
			break;
		
		case MATH_SHIFT_GROUP:
			token(p) = MATH_SHIFT_TOKEN + '$';
			print("$");
			break;
		
		case MATH_LEFT_GROUP:
			token(p) = sym2tok(FROZEN_RIGHT);
			p = token_link(p) = new_token();
			token(p) = OTHER_TOKEN + '.';
			print_esc("right.");
			break;
		
		default:
			token(p) = RIGHT_BRACE_TOKEN + '}';
			print("}");
			break;
		}
		print(" inserted");
		ins_list(h);
		help_offsave_missing();
		error();
	}
}

void
restore_trace (r, s)
	reg	r;
	str	s;
{
	begin_diagnostic();
	print("{");
	print(s);
	print(" ");
	show_reg(r);
	print("}");
	end_diagnostic(FALSE);
}

void
show_reg (r)
	reg	r;
{
	int	n;

	if (reg_type(r) < BOX_REG) {
		sym s = (sym) r;
		sprint_cs(s);
		print("=");
		if (s == par_shape_cs) {
			if (par_shape_ptr == null) {
				print("0");
			} else {
				print_int(info(par_shape_ptr));
			}
		} else {
			print_cmd_chr(eq_type(s), equiv(s));
			if (eq_type(s) >= CALL) {
				print(":");
				show_token_list(token_link(equiv(s)),
					null, 32);
			}
		}
		return;
	}
	switch (reg_type(r))
	{
	case BOX_REG:
		print_esc("box");
		print_int(r - box_reg[0]);
		print("=");
		if (reg_equiv(r) == null) {
			print("void");
		} else {
			depth_threshold = 0;
			breadth_max = 1;
			show_node_list(reg_equiv(r));
		}
		break;

	case MU_SKIP_REG:
		n = r - mu_skip_reg[0];
		if (n < MU_GLUE_PARS) {
			print_mu_skip_param(n);
			print("=");
			print_spec(reg_equiv(r), "mu");
		} else {
			print_esc("muskip");
			print_int(n - MU_GLUE_PARS);
			print("=");
			print_spec(reg_equiv(r), "mu");
		}
		break;

	case SKIP_REG:
		n = r - skip_reg[0];
		if (n < GLUE_PARS) {
			print_skip_param(n);
			print("=");
			print_spec(reg_equiv(r), "pt");
		} else {
			print_esc("skip");
			print_int(n - GLUE_PARS);
			print("=");
			print_spec(reg_equiv(r), "pt");
		}
		break;

	case DIMEN_REG:
		n = r - dimen_reg[0];
		if (n < DIMEN_PARS) {
			print_length_param(n);
		} else {
			print_esc("dimen");
			print_int(n - DIMEN_PARS);
		}
		print("=");
		print_scaled(reg_equiv(r));
		print("pt");
		break;

	case INT_REG:
		n = r - int_reg[0];
		if (n < INT_PARS) {
			print_param(n);
		} else {
			print_esc("count");
			print_int(n - INT_PARS);
		}
		print("=");
		print_int(reg_equiv(r));
		break;

	case TOKS_REG:
		n = r - toks_reg[0];
		if (n < TOK_PARS) {
			print_toks_param(n);
		} else {
			print_esc("toks");
			print_int(n = TOK_PARS);
		}
		print("=");
		if (reg_equiv(r) != null)
			show_token_list(token_link(reg_equiv(r)),
				null, 32);
		break;

	case FNT_REG:
		n = r - fnt_reg[0];
		if (n == 0) {
			print("current font");
		} else if (n <= 16) {
			print_esc("textfont");
			print_int(n - 1);
		} else if (n <= 32) {
			print_esc("scriptfont");
			print_int(n - 17);
		} else {
			print_esc("scriptscriptfont");
			print_int(n - 33);
		}
		print("=");
		sprint_cs(font_id(reg_equiv(r)));
		break;

	case CAT_CODE_REG:
		print_esc("catcode");
		print_int(r - cat_code_reg[0]);
		print("=");
		print_int(reg_equiv(r));
		break;

	case LC_CODE_REG:
		print_esc("lccode");
		print_int(r - lc_code_reg[0]);
		print("=");
		print_int(reg_equiv(r));
		break;

	case UC_CODE_REG:
		print_esc("uccode");
		print_int(r - uc_code_reg[0]);
		print("=");
		print_int(reg_equiv(r));
		break;

	case SF_CODE_REG:
		print_esc("sfcode");
		print_int(r - sf_code_reg[0]);
		print("=");
		print_int(reg_equiv(r));
		break;

	case MATH_CODE_REG:
		print_esc("mathcode");
		print_int(r - math_code_reg[0]);
		print("=");
		print_int(reg_equiv(r));
		break;

	case DEL_CODE_REG:
		print_esc("delcode");
		print_int(r - del_code_reg[0]);
		print("=");
		print_int(reg_equiv(r));
		break;

	default:
		print("?");
		break;
	}
}

sym *
sym_alloc (i)
	int	i;
{
	sym	*s;

	s = (sym *)malloc(i * sizeof(sym));
	if (s == (sym *) 0) {
		overflow("sym alloc", i);
	}
	while (i--) {
		s[i] = null_sym;
	}
	return	s;
}

reg *
reg_alloc (i)
	int	i;
{
	int	ii;
	reg	r;
	reg	*p;

	p = (reg *)malloc(i * sizeof(reg));
	if (p == (reg *) 0) {
		overflow("reg alloc", i);
	}
	r = (reg) malloc(i * sizeof(reg_t));
	if (r == (reg) 0) {
		overflow("reg alloc", i);
	}
	for (ii = 0; ii < i; ii++) {
		p[ii] = r++;
	}
	return	p;
}

void
_sym_init ()
{
	cur_level = LEVEL_ONE;
	cur_group = BOTTOM_LEVEL;
	no_new_control_sequence = TRUE;
	max_save_stack = save_ptr = save_stack;
	cur_cs = null_sym;
	warning_cs = null_sym;
}

void
_sym_init_once ()
{
	int	k;
	sym	s;

	nsaves = SAVE_SIZE;
	save_stack = (reg)malloc(nsaves * sizeof(mcell));
	if (save_stack == (reg) 0) {
		overflow("save stack", nsaves);
	}
	save_end = save_stack + nsaves - 1;
#ifdef STAT
	cs_count = 0;
#endif

	active_base=sym_alloc(256);
	single_base=sym_alloc(256);
	hash_base=sym_alloc(HASH_SIZE);
	frozen_cs_base=sym_alloc(FROZEN_CONTROLS);
	font_id_base=sym_alloc(FONT_MAX);
	box_reg=reg_alloc(256);
	mu_skip_reg=reg_alloc(MU_GLUE_PARS + 256);
	skip_reg=reg_alloc(GLUE_PARS + 256);
	dimen_reg=reg_alloc(DIMEN_PARS + 256);
	int_reg=reg_alloc(INT_PARS + 256);
	toks_reg=reg_alloc(TOK_PARS + 256);
	cat_code_reg=reg_alloc(256);
	lc_code_reg=reg_alloc(256);
	uc_code_reg=reg_alloc(256);
	sf_code_reg=reg_alloc(256);
	math_code_reg=reg_alloc(256);
	del_code_reg=reg_alloc(256);
	fnt_reg=reg_alloc(49);
	hold_head=new_avail();

	zero_glue=new_node(GLUE_SPEC_SIZE);
	glue_ref_count(zero_glue) = 0;
	glue_width(zero_glue) = 0;
	stretch(zero_glue) = 0;
	stretch_order(zero_glue) = NORMAL;
	shrink(zero_glue) = 0;
	shrink_order(zero_glue) = NORMAL;

	fil_glue=new_spec(zero_glue);
	stretch(fil_glue) = UNITY;
	stretch_order(fil_glue) = FIL;

	fill_glue=new_spec(zero_glue);
	stretch(fill_glue) = UNITY;
	stretch_order(fill_glue) = FILL;

	ss_glue=new_spec(zero_glue);
	stretch(ss_glue) = UNITY;
	stretch_order(ss_glue) = FIL;
	shrink(ss_glue) = UNITY;
	shrink_order(ss_glue) = FIL;

	fil_neg_glue=new_spec(zero_glue);
	stretch(fil_neg_glue) = -UNITY;
	stretch_order(fil_neg_glue) = FIL;

	for (k = 0; k <= 48; incr(k)) {
		reg_type(fnt_reg[k]) = FNT_REG;
		reg_level(fnt_reg[k]) = LEVEL_ONE;
		reg_equiv(fnt_reg[k]) = null_font;
	}
	undefined_cs = new_sym("undefined", 9);
	null_cs = new_sym("",0);
	s = (sym)malloc(256 * sizeof(sym_t));
	if (s == (sym) 0) {
		overflow("active cs alloc", 256);
	}
	for (k = 0; k < 256; incr(k), incr(s)) {
		active_base[k] = s;
		hash(s) = null_sym;
		append_char(k);
		text(s) = make_str();
		length(s) = 1;
		eq_type(s) = UNDEFINED_CS;
		eq_level(s) = LEVEL_ONE;
		equiv(s) = 0;
	}
	s = (sym)malloc(256 * sizeof(sym_t));
	if (s == (sym) 0) {
		overflow("single cs alloc", 256);
	}
	for (k = 0; k < 256; incr(k), incr(s)) {
		single_base[k] = s;
		hash(s) = null_sym;
		append_char(k);
		text(s) = make_str();
		length(s) = 1;
		eq_type(s) = UNDEFINED_CS;
		eq_level(s) = LEVEL_ZERO;
		equiv(s) = 0;
	}
	s = (sym)malloc(FONT_MAX * sizeof(sym_t));
	if (s == (sym) 0) {
		overflow("font id cs alloc", FONT_MAX);
	}
	for (k = 0; k < FONT_MAX; incr(k), incr(s)) {
		font_id_base[k] = s;
		hash(s) = null_sym;
		text(s) = null_str;
		length(s) = 0;
		eq_type(s) = UNDEFINED_CS;
		eq_level(s) = LEVEL_ZERO;
	}
	for (k = 0; k < TOK_PARS + 256; incr(k)) {
		reg_type(toks_reg[k]) = TOKS_REG;
		reg_level(toks_reg[k]) = LEVEL_ZERO;
		reg_equiv(toks_reg[k]) = null;
	}
	for (k = 0; k < MU_GLUE_PARS + 256; incr(k)) {
		reg_type(mu_skip_reg[k]) = MU_SKIP_REG;
		reg_level(mu_skip_reg[k]) = LEVEL_ONE;
		reg_equiv(mu_skip_reg[k]) = zero_glue;
	}
	for (k = 0; k < GLUE_PARS + 256; incr(k)) {
		reg_type(skip_reg[k]) = SKIP_REG;
		reg_level(skip_reg[k]) = LEVEL_ONE;
		reg_equiv(skip_reg[k]) = zero_glue;
	}
	glue_ref_count(zero_glue) += 512 + MU_GLUE_PARS + GLUE_PARS;
	for (k = 0; k < INT_PARS + 256; incr(k)) {
		reg_type(int_reg[k]) = INT_REG;
		reg_level(int_reg[k]) = LEVEL_ONE;
		reg_equiv(int_reg[k]) = 0;
	}
	mag = 1000;
	tolerance = 10000;
	hang_after = 1;
	max_dead_cycles = 25;
	escape_char = '\\';
	end_line_char = CARRIAGE_RETURN;
	for (k = 0; k < DIMEN_PARS + 256; incr(k)) {
		reg_type(dimen_reg[k]) = DIMEN_REG;
		reg_level(dimen_reg[k]) = LEVEL_ONE;
		reg_equiv(dimen_reg[k]) = 0;
	}
	for (k = 0; k < 256; incr(k)) {
		reg_type(box_reg[k]) = BOX_REG;
		reg_level(box_reg[k]) = LEVEL_ONE;
		reg_equiv(box_reg[k]) = null;
	}	
	for (k = 0; k < 256; incr(k)) {
		reg_type(cat_code_reg[k]) = CAT_CODE_REG;
		reg_level(cat_code_reg[k]) = LEVEL_ONE;
		reg_equiv(cat_code_reg[k]) = OTHER_CHAR;
		reg_type(lc_code_reg[k]) = LC_CODE_REG;
		reg_level(lc_code_reg[k]) = LEVEL_ONE;
		reg_equiv(lc_code_reg[k]) = 0;
		reg_type(uc_code_reg[k]) = UC_CODE_REG;
		reg_level(uc_code_reg[k]) = LEVEL_ONE;
		reg_equiv(uc_code_reg[k]) = 0;
		reg_type(sf_code_reg[k]) = SF_CODE_REG;
		reg_level(sf_code_reg[k]) = LEVEL_ONE;
		reg_equiv(sf_code_reg[k]) = 1000;
		reg_type(math_code_reg[k]) = MATH_CODE_REG;
		reg_level(math_code_reg[k]) = LEVEL_ONE;
		reg_equiv(math_code_reg[k]) = k;
		reg_type(del_code_reg[k]) = DEL_CODE_REG;
		reg_level(del_code_reg[k]) = LEVEL_ONE;
		reg_equiv(del_code_reg[k]) = -1;
	}
	cat_code(CARRIAGE_RETURN) = CAR_RET;
	cat_code(' ') = SPACER;
	cat_code('^') = SUP_MARK;
	cat_code('\\') = ESCAPE;
	cat_code('%') = COMMENT;
	cat_code(INVALID_CODE) = INVALID_CHAR;
	cat_code(NULL_CODE) = IGNORE;
	for (k = '0'; k <= '9'; incr(k)) {
		math_code(k) = k + VAR_CODE;
	}
	for (k = 'A'; k <= 'Z'; incr(k)) {
		cat_code(k) = cat_code(k + 'a' - 'A') = LETTER;
		math_code(k) = k + VAR_CODE + 0x100;
		math_code(k + 'a' - 'A') = k + 'a' - 'A'+ VAR_CODE + 0x100;
		lc_code(k) = lc_code(k + 'a' - 'A') = k + 'a' - 'A';
		uc_code(k) = uc_code(k + 'a' - 'A') = k;
		sf_code(k) = 999;
	}
	del_code('.') = 0;
}

/*
**	Help text
*/

help_offsave_xtra ()
{	
	help1("Things are pretty mixed up, but I think the worst is over.");
}

help_offsave_missing ()
{
	help5("I've inserted something that you may have forgotten.",
	"(See the <inserted text> above.)",
	"With luck, this will get me unwedged. But if you",
	"really didn't forget anything, try typing `2' now; then",
	"my insertion and my current dilemma will both disappear.");
}
