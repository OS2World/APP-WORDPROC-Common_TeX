
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

ptr	cur_p;

ptr	active;
ptr	passive;

ptr	cur_r;
ptr	prev_r;
ptr	prev_prev_r;

scal	background[7];
scal	break_width[7];
scal	active_width[7];
scal	cur_active_width[7];

int	threshold;
bool	second_pass;
bool	final_pass;
scal	first_indent;
scal	first_width;
scal	second_indent;
scal	second_width;

int	fewest_demerits;
int	minimum_demerits;
int	minimal_demerits[4];
int	best_pl_line[4];
ptr	best_place[4];
ptr	best_bet;
int	best_line;
int	fit_class;

int	easy_line;
int	last_special_line;
int	line_diff;
scal	line_width;
scal	disc_width;
int	pass_number;
ptr	printed_node;
int	actual_looseness;
bool	no_shrink_error_yet;

ptr	just_box;

#define act_width	active_width[1]

#define width_lig_char(C) \
	char_width(font(lig_char(C)), \
		char_info(font(lig_char(C)), character(lig_char(C))))

#define width_char(C) \
	char_width(font(C), char_info(font(C), character(C)))

#define check_shrinkage(G) \
	{if (shrink_order(G) != NORMAL && shrink(G) != 0) \
		G = finite_shrink(G);}

void
line_break (final_widow_penalty)
	int	final_widow_penalty;
{
	ptr	q, r, t;
	ptr	prev_p;
	bool	auto_breaking;

	pack_begin_line = mode_line;
	t = new_avail();
	link(t) = link(head);
	if (is_char_node(tail)) {
		tail_append(new_penalty(INF_PENALTY));
	} else if (type(tail) != GLUE_NODE) {
		tail_append(new_penalty(INF_PENALTY));
	} else {
		type(tail) = PENALTY_NODE;
		delete_glue_ref(glue_ptr(tail));
		flush_node_list(leader_ptr(tail));
		penalty(tail) = INF_PENALTY;
	}
	link(tail) = new_param_glue(PAR_FILL_SKIP_CODE);
	pop_nest();
	no_shrink_error_yet = TRUE;
	check_shrinkage(left_skip);
	check_shrinkage(right_skip);
	q = left_skip;
	r = right_skip;
	background[1] = glue_width(q) + glue_width(r);
	background[2] = 0;
	background[3] = 0;
	background[4] = 0;
	background[5] = 0;
	background[2 + stretch_order(q)] = stretch(q);
	background[2 + stretch_order(r)] += stretch(r);
	background[6] = shrink(q) + shrink(r);
	minimum_demerits = AWFUL_BAD;
	minimal_demerits[VERY_LOOSE_FIT] = AWFUL_BAD;
	minimal_demerits[LOOSE_FIT] = AWFUL_BAD;
	minimal_demerits[DECENT_FIT] = AWFUL_BAD;
	minimal_demerits[TIGHT_FIT] = AWFUL_BAD;
	if (par_shape_ptr == null) {
		if (hang_indent == 0) {
			last_special_line = 0;
			second_width = hsize;
			second_indent = 0;
		} else {
			last_special_line = abs(hang_after);
			if (hang_after < 0) {
				first_width = hsize - abs(hang_indent);
				first_indent = hang_indent >= 0 ?
					hang_indent : 0;
				second_width = hsize;
				second_indent = 0;
			} else {
				first_width = hsize;
				first_indent = 0;
				second_width = hsize - abs(hang_indent);
				second_indent = (hang_indent >= 0) ?
					hang_indent : 0;
			}
		}
	} else {
		last_special_line = info(par_shape_ptr) - 1;
		second_width = par_shape_width(last_special_line + 1);
		second_indent = par_shape_indent(last_special_line + 1);
	}
	easy_line = (looseness == 0) ? last_special_line : MAX_HALFWORD;
	threshold = pretolerance;
	if (threshold >= 0) {
		if (tracing_paragraphs > 0) {
			begin_diagnostic();
			print_nl("@firstpass");
		}
		second_pass = FALSE;
		final_pass = FALSE;
	} else {
		threshold = tolerance;
		second_pass = TRUE;
		final_pass = emergency_stretch <= 0;
		if (tracing_paragraphs > 0) {
			begin_diagnostic();
		}
	}

#define store_background(W) \
	(active_width[W] = background[W])

next_pass:
	if (threshold >= INF_BAD) {
		threshold = INF_BAD;
	}
	if (second_pass) {
		init_hyph();
	}
	q = new_node(ACTIVE_NODE_SIZE);
	type(q) = UNHYPHENATED;
	fitness(q) = DECENT_FIT;
	link(q) = last_active;
	break_node(q) = null;
	line_number(q) = prev_graf + 1;
	total_demerits(q) = 0;
	link(active) = q;
	do_all_six(store_background);
	passive = null;
	printed_node = t;
	pass_number = 0;
	font_in_short_display = null_font;
	prev_p = cur_p = link(t);
	auto_breaking = TRUE;
	while (cur_p != null && link(active) != last_active) {
		if (is_char_node(cur_p)) {
			prev_p = cur_p;
			while (is_char_node(cur_p)) {
				act_width += width_char(cur_p);
				cur_p = link(cur_p);
			}
		}
		switch (type(cur_p))
		{
		case HLIST_NODE:
		case VLIST_NODE:
		case RULE_NODE:
			act_width += box_width(cur_p);
			break;
		
		case GLUE_NODE:
			if (auto_breaking) {
				if (is_char_node(prev_p)
				|| precedes_break(prev_p)) {
					try_break(0, UNHYPHENATED);
				}
			}
			check_shrinkage(glue_ptr(cur_p));
			q = glue_ptr(cur_p);
			act_width += glue_width(q);
			active_width[2 + stretch_order(q)] += stretch(q);
			active_width[6] += shrink(q);
			if (second_pass && auto_breaking) {
				try_hyph();
			}
			break;
		
		case KERN_NODE:
			if (!is_char_node(link(cur_p))
			&& auto_breaking
			&& type(link(cur_p)) == GLUE_NODE) {
				try_break(0, UNHYPHENATED);
			}
			act_width += kern_width(cur_p);
			break;
		
		case LIGATURE_NODE:
			act_width += width_lig_char(cur_p);
			break;
		
		case DISC_NODE:
			disc_width = 0;
			if (pre_break(cur_p) == null) {
				try_break(ex_hyphen_penalty, HYPHENATED);
			} else {
				set_disc_width();
				act_width += disc_width;
				try_break(hyphen_penalty, HYPHENATED);
				act_width -= disc_width;
			}
			prev_p = cur_p;
			set_act_width();
			continue;
		
		case MATH_NODE:
			auto_breaking = subtype(cur_p) == AFTER;
			if (!is_char_node(link(cur_p))
			&& auto_breaking
			&& type(link(cur_p)) == GLUE_NODE) {
				try_break(0, UNHYPHENATED);
			}
			act_width += math_width(cur_p);
			break;
		
		case PENALTY_NODE:
			try_break(penalty(cur_p), UNHYPHENATED);
			break;
		
		case MARK_NODE:
		case INS_NODE:
		case ADJUST_NODE:
			break;

		case WHATSIT_NODE:
			line_break_whatsit(cur_p);
			break;

		default:
			confusion("paragraph");
			break;
		}
		prev_p = cur_p;
		cur_p = link(cur_p);
	}
	if (cur_p == null) {
		try_break(EJECT_PENALTY, HYPHENATED);
		if (link(active) != last_active) {
			if (get_best_bet()) {
				goto done;
			}
		}
	}
	for (q = link(active); q != last_active; q = cur_p) {
		cur_p = link(q);
		if (type(q) == DELTA_NODE) {
			free_node(q, DELTA_NODE_SIZE);
		} else {
			free_node(q, ACTIVE_NODE_SIZE);
		}
	}
	for (q = passive; q != null; q = cur_p) {
		cur_p = link(q);
		free_node(q, PASSIVE_NODE_SIZE);
	}
	if (!second_pass) {
		if (tracing_paragraphs > 0) {
			print_nl("@secondpass");
		}
		threshold = tolerance;
		second_pass = TRUE;
		final_pass = emergency_stretch <= 0;
	} else {
		if (tracing_paragraphs > 0) {
			print_nl("@emergencypass");
		}
		background[2] += emergency_stretch;
		final_pass = TRUE;
	}	
	goto next_pass;

done:
	if (tracing_paragraphs > 0) {
		end_diagnostic(TRUE);
		normalize_selector();
	}
	post_line_break(t, final_widow_penalty);
	for (q = link(active); q != last_active; q = cur_p) {
		cur_p = link(q);
		if (type(q) == DELTA_NODE) {
			free_node(q, DELTA_NODE_SIZE);
		} else {
			free_node(q, ACTIVE_NODE_SIZE);
		}
	}
	for (q = passive; q != null; q = cur_p) {
		cur_p = link(q);
		free_node(q, PASSIVE_NODE_SIZE);
	}
	pack_begin_line = 0;
}

void
try_break (pi, break_type)
	int	pi;
	int	break_type;
{
	int	b, d;
	int	l, old_l;
	bool	artificial_demerits;
	bool	cur_r_stays_active;
	bool	no_break_yet;

#define update_width(W) \
	(cur_active_width[W] += deltas(cur_r)[W])

#define downdate_width(W) \
	(cur_active_width[W] -= deltas(prev_r)[W])

#define copy_to_cur_active(W) \
	(cur_active_width[W] = active_width[W])

#define update_active(W) \
	(active_width[W] += deltas(cur_r)[W])

#define combine_two_deltas(W) \
	deltas(prev_r)[W] += deltas(cur_r)[W]

	if (abs(pi) >= INF_PENALTY) {
		if (pi > 0) {
			update_printed_node();
			return;
		} else {
			pi = EJECT_PENALTY;
		}
	}
	no_break_yet = TRUE;
	prev_r = active;
	old_l = 0;
	do_all_six(copy_to_cur_active);
	loop {
		cur_r = link(prev_r);
		if (type(cur_r) == DELTA_NODE) {
			do_all_six(update_width);
			prev_prev_r = prev_r;
			prev_r = cur_r;
			continue;
		}
		l = line_number(cur_r);
		if (l > old_l) {
			if (minimum_demerits < AWFUL_BAD
			&& (old_l != easy_line || cur_r == last_active)) {
				if (no_break_yet) {
					no_break_yet = FALSE;
					set_break_width(break_type);
				}
				get_active_nodes(cur_r, break_type);
			}
			if (cur_r == last_active) {
				update_printed_node();
				return;
			}
			if (l > easy_line) {
				line_width = second_width;
				old_l = MAX_HALFWORD - 1;
			} else {
				old_l = l;
				if (l > last_special_line) {
					line_width = second_width;
				} else if (par_shape_ptr == null) {
					line_width = first_width;
				} else {
					line_width = par_shape_width(l);
				}
			}
		}
		artificial_demerits = FALSE;
		b = get_badness();
		if (b > INF_BAD || pi == EJECT_PENALTY) {
			if (final_pass && minimum_demerits == AWFUL_BAD
			&& link(cur_r) == last_active && prev_r == active) {
				artificial_demerits = TRUE;
			} else if (b > threshold) {
				goto deactivate;
			}
			cur_r_stays_active = FALSE;
		} else {
			prev_r = cur_r;
			if (b > threshold) {
				continue;
			}
			cur_r_stays_active = TRUE;
		}
		if (artificial_demerits) {
			d = 0;
		} else {
			d = line_penalty + b;
			if (abs(d) >= 10000) {
				d = 100000000;
			} else {
				d = d * d;
			}
			if (pi != 0) {
				if (pi > 0) {
					d += pi * pi;
				} else if (pi > EJECT_PENALTY) {
					d -= pi * pi;
				}
			}
			if (break_type == HYPHENATED
			&& type(cur_r) == HYPHENATED) {
				if (cur_p != null) {
					d += double_hyphen_demerits;
				} else {
					d += final_hyphen_demerits;
				}
			}
			if (abs(fit_class - (int)fitness(cur_r)) > 1) {
				d += adj_demerits;
			}
		}
		if (tracing_paragraphs > 0) {
			show_break_status(cur_r, artificial_demerits, b, pi, d);
		}
		d += total_demerits(cur_r);
		if (d <= minimal_demerits[fit_class]) {
			minimal_demerits[fit_class] = d;
			best_place[fit_class] = break_node(cur_r);
			best_pl_line[fit_class] = l;
			if (d < minimum_demerits) {
				minimum_demerits = d;
			}
		}
		if (cur_r_stays_active) {
			continue;
		}

	deactivate:
		link(prev_r) = link(cur_r);
		free_node(cur_r, ACTIVE_NODE_SIZE);
		if (prev_r == active) {
			cur_r = link(active);
			if (type(cur_r) == DELTA_NODE) {
				do_all_six(update_active);
				do_all_six(copy_to_cur_active);
				link(active) = link(cur_r);
				free_node(cur_r, DELTA_NODE_SIZE);
			}
		} else if (type(prev_r) == DELTA_NODE) {
			cur_r = link(prev_r);
			if (cur_r == last_active) {
				do_all_six(downdate_width);
				link(prev_prev_r) = last_active;
				free_node(prev_r, DELTA_NODE_SIZE);
				prev_r = prev_prev_r;
			} else if (type(cur_r) == DELTA_NODE) {
				do_all_six(update_width);
				do_all_six(combine_two_deltas);
				link(prev_r) = link(cur_r);
				free_node(cur_r, DELTA_NODE_SIZE);
			}
		}
	}
}

void
post_line_break (p, final_widow_penalty)
	ptr	p;
	int	final_widow_penalty;
{
	ptr	q;
	ptr	r;
	ptr	s;
	int	t;
	int	pen;
	int	cur_line;
	scal	cur_width;
	scal	cur_indent;
	bool	disc_break;
	bool	post_disc_break;

	q = break_node(best_bet);
	cur_p = null;
	while (q != null) {
		r = q;
		q = prev_break(q);
		next_break(r) = cur_p;
		cur_p = r;
	}
	cur_line = prev_graf + 1;
	do {
		q = cur_break(cur_p);
		disc_break = FALSE;
		post_disc_break = FALSE;
		if (q != null) {
			if (type(q) == GLUE_NODE) {
				delete_glue_ref(glue_ptr(q));
				glue_ptr(q) = right_skip;
				subtype(q) = RIGHT_SKIP_CODE + 1;
				add_glue_ref(right_skip);
				goto done;
			} else if (type(q) == DISC_NODE) {
				t = replace_count(q);
				if (t == 0) {
					r = link(q);
				} else {
					r = q;
					while (t > 1) {
						r = link(r);
						decr(t);
					}
					s = link(r);
					r = link(s);
					link(s) = null;
					flush_node_list(link(q));
					replace_count(q) = 0;
				}
				if (post_break(q) != null) {
					s = post_break(q);
					while (link(s) != null) {
						s = link(s);
					}
					link(s) = r;
					r = post_break(q);
					post_break(q) = null;
					post_disc_break = TRUE;
				}
				if (pre_break(q) != null) {
					s = pre_break(q);
					link(q) = s;
					while (link(s) != null) {
						s = link(s);
					}
					pre_break(q) = null;
					q = s;
				}
				link(q) = r;
				disc_break = TRUE;
			} else if (type(q) == MATH_NODE) {
				math_width(q) = 0;
			} else if (type(q) == KERN_NODE) {
				kern_width(q) = 0;
			}
		} else {
			q = p;
			while (link(q) != null) {
				q = link(q);
			}
		}
		r = new_param_glue(RIGHT_SKIP_CODE);
		link(r) = link(q);
		q = link(q) = r;

	done:
		r = link(q);
		link(q) = null;
		q = link(p);
		link(p) = r;
		if (left_skip != zero_glue) {
			r = new_param_glue(LEFT_SKIP_CODE);
			link(r) = q;
			q = r;
		}
		if (cur_line > last_special_line) {
			cur_width = second_width;
			cur_indent = second_indent;
		} else if (par_shape_ptr == null) {
			cur_width = first_width;
			cur_indent = first_indent;
		} else {
			cur_width = par_shape_width(cur_line);
			cur_indent = par_shape_indent(cur_line);
		}
		adjust_tail = adjust_head;
		just_box = hpack(q, cur_width, EXACTLY);
		shift_amount(just_box) = cur_indent;
		append_to_vlist(just_box);
		if (adjust_head != adjust_tail) {
			link(tail) = link(adjust_head);
			tail = adjust_tail;
		}
		adjust_tail = null;
		if (cur_line + 1 != best_line) {
			pen = inter_line_penalty;
			if (cur_line == prev_graf + 1) {
				pen += club_penalty;
			}
			if (cur_line + 2 == best_line) {
				pen += final_widow_penalty;
			}
			if (disc_break) {
				pen += broken_penalty;
			}
			if (pen != 0) {
				r = new_penalty(pen);
				link(tail) = r;
				tail = r;
			}
		}
		incr(cur_line);
		cur_p = next_break(cur_p);
		if (cur_p != null && !post_disc_break) {
			r = p;
			loop {
				q = link(r);
				if (q == cur_break(cur_p)) {
					break;
				}
				if (is_char_node(q)) {
					break;
				}
				if (non_discardable(q)) {
					break;
				}
				if (subtype(q) == ACC_KERN
				&& type(q) == KERN_NODE) {
					break;
				}
				r = q;
			}
			if (r != p) {
				link(r) = null;
				flush_node_list(link(p));
				link(p) = q;
			}
		}
	} while (cur_p != null);
	if (cur_line != best_line || link(p) != null) {
		confusion("line breaking");
	}
	prev_graf = best_line - 1;
}

void
set_disc_width ()
{
	ptr	p;

	for (p = pre_break(cur_p); p != null; p = link(p)) {
		if (is_char_node(p)) {
			disc_width += width_char(p);
		} else {
			switch (type(p))
			{
			case LIGATURE_NODE:
				disc_width += width_lig_char(p);
				break;
			
			case HLIST_NODE:
			case VLIST_NODE:
				disc_width += box_width(p);
				break;

			case RULE_NODE:
				disc_width += rule_width(p);
				break;

			case KERN_NODE:
				disc_width += kern_width(p);
				break;
			
			default:
				confusion("disc3");
				break;
			}
		}
	}
}

void
set_act_width ()
{
	ptr	p;
	int	n;

	p = link(cur_p);
	for (n = replace_count(cur_p); n > 0; decr(n)) {
		if (is_char_node(p)) {
			act_width += width_char(p);
		} else {
			switch (type(p))
			{
			case LIGATURE_NODE:
				act_width += width_lig_char(p);
				break;
			
			case HLIST_NODE:
			case VLIST_NODE:
				act_width += box_width(p);
				break;

			case RULE_NODE:
				act_width += rule_width(p);
				break;

			case KERN_NODE:
				act_width += kern_width(p);
				break;
			
			default:
				confusion("disc3");
				break;
			}
		}
		p = link(p);
	}
	cur_p = p;
}

void
set_break_width (break_type)
	int     break_type;
{
	int     t;
	ptr     p, q;

#define set_break_width_to_background(W) \
	(break_width[W] = background[W])

	do_all_six(set_break_width_to_background);
	p = cur_p;
	if (break_type > UNHYPHENATED && cur_p != null) {
		t = replace_count(cur_p);
		q = cur_p;
		while (t > 0) {
			decr(t);
			q = link(q);
			if (is_char_node(q)) {
				break_width[1] -= width_char(q);
			} else {
				switch (type(q))
				{
				case LIGATURE_NODE:
					break_width[1] -= width_lig_char(q);
					break;

				case HLIST_NODE:
				case VLIST_NODE:
					break_width[1] -= box_width(q);
					break;

				case RULE_NODE:
					break_width[1] -= rule_width(q);
					break;

				case KERN_NODE:
					break_width[1] -= kern_width(q);
					break;

				default:
					confusion("disc1");
					break;
				}
			}
		}
		for (p = post_break(cur_p); p != null; p = link(p)) {
			if (is_char_node(p)) {
				break_width[1] += width_char(p);
			} else {
				switch (type(p))
				{
				case LIGATURE_NODE:
					break_width[1] += width_lig_char(p);
					break;

				case HLIST_NODE:
				case VLIST_NODE:
					break_width[1] += box_width(p);
					break;

				case RULE_NODE:
					break_width[1] += rule_width(p);
					break;

				case KERN_NODE:
					if (t == 0 && subtype(p) != ACC_KERN) {
						t = -1;
					} else {
						break_width[1] += kern_width(p);
					}
					break;

				default:
					confusion("disc2");
					break;
				}
			}
			incr(t);
		}
		break_width[1] += disc_width;
		if (t == 0) {
			p = link(q);
		}
	}
	while (p != null) {
		if (is_char_node(p)) {
			return;
		}
		switch (type(p))
		{
		case GLUE_NODE:
			q = glue_ptr(p);
			break_width[1] -= glue_width(q);
			break_width[2 + stretch_order(q)] -= stretch(q);
			break_width[6] -= shrink(q);
			break;
		
		case PENALTY_NODE:
			break;
		
		case MATH_NODE:
			break_width[1] -= math_width(p);
			break;

		case KERN_NODE:
			if (subtype(p) == ACC_KERN)
				return;
			break_width[1] -= kern_width(p);
			break;

		default:
			return;
		}
		p = link(p);
	}
}

int
get_best_bet ()
{
	ptr	p;

	fewest_demerits = AWFUL_BAD;
	for (p = link(active); p != last_active; p = link(p)) {
		if (type(p) != DELTA_NODE) {
			if (total_demerits(p) < fewest_demerits) {
				fewest_demerits = total_demerits(p);
				best_bet = p;
			}
		}
	}
	best_line = line_number(best_bet);
	if (looseness == 0) {
		return TRUE;
	}
	actual_looseness = 0;
	for (p = link(active); p != last_active; p = link(p)) {
		if (type(p) != DELTA_NODE) {
			line_diff = line_number(p) - best_line;
			if (line_diff < actual_looseness
			&& looseness <= line_diff
			|| line_diff > actual_looseness
			&& looseness >= line_diff) {
				best_bet = p;
				actual_looseness = line_diff;
				fewest_demerits = total_demerits(p);
			} else if (line_diff == actual_looseness
				&& total_demerits(p) < fewest_demerits) {
				best_bet = p;
				fewest_demerits = total_demerits(p);
			}
		}
	}
	best_line = line_number(best_bet);
	if (actual_looseness == looseness || final_pass) {
		return TRUE;
	}
	return FALSE;
}

int
get_badness ()
{
	scal	s;
	int	b;

	s = line_width - cur_active_width[1];
	if (s > 0) {
		if (cur_active_width[3] != 0
		|| cur_active_width[4] != 0
		|| cur_active_width[5] != 0) {
			fit_class = DECENT_FIT;
			return 0;
		}
		if (s > 7230584 && cur_active_width[2] < 1663497) {
			fit_class = VERY_LOOSE_FIT;
			return INF_BAD;
		}
		b = badness(s, cur_active_width[2]);
		if (b > 12) {
			if (b > 99) {
				fit_class = VERY_LOOSE_FIT;
			} else {
				fit_class = LOOSE_FIT;
			}
		} else {
			fit_class = DECENT_FIT;
		}
	} else {
		if (-s > cur_active_width[6]) {
			b = INF_BAD + 1;
		} else {
			b = badness(-s, cur_active_width[6]);
		}
		if (b > 12) {
			fit_class = TIGHT_FIT;
		} else {
			fit_class = DECENT_FIT;
		}
	}
	return b;
}

void
get_active_nodes (r, break_type)
	ptr	r;
	int	break_type;
{
	ptr	q;

#define new_delta_to_break_width(W) \
	deltas(q)[W] = break_width[W] - cur_active_width[W]

#define new_delta_from_break_width(W) \
	deltas(q)[W] = cur_active_width[W] - break_width[W]

#define convert_to_break_width(W) \
	deltas(prev_r)[W] += break_width[W] - cur_active_width[W]

#define store_break_width(W) \
	(active_width[W] = break_width[W])

	if (type(prev_r) == DELTA_NODE) {
		do_all_six(convert_to_break_width);
	} else if (prev_r == active) {
		do_all_six(store_break_width);
	} else {
		q = new_node(DELTA_NODE_SIZE);
		link(q) = r;
		type(q) = DELTA_NODE;
		subtype(q) = 0;
		do_all_six(new_delta_to_break_width);
		prev_prev_r = prev_r;
		prev_r = link(prev_r) = q;
	}
	if (abs(adj_demerits) >= AWFUL_BAD - minimum_demerits) {
		minimum_demerits = AWFUL_BAD - 1;
	} else {
		minimum_demerits += abs(adj_demerits);
	}
	for (fit_class = VERY_LOOSE_FIT;
		fit_class <= TIGHT_FIT;
			incr(fit_class)) {
		if (minimal_demerits[fit_class] <= minimum_demerits) {
			q = get_break_node(fit_class, break_type);
			link(q) = r;
			prev_r = link(prev_r) = q;
		}
		minimal_demerits[fit_class] = AWFUL_BAD;
	}
	minimum_demerits = AWFUL_BAD;
	if (r != last_active) {
		q = new_node(DELTA_NODE_SIZE);
		link(q) = r;
		type(q) = DELTA_NODE;
		subtype(q) = 0;
		do_all_six(new_delta_from_break_width);
		prev_prev_r = prev_r;
		prev_r = link(prev_r) = q;
	}
}

ptr
get_break_node (fit_class, break_type)
	int	fit_class, break_type;
{
	ptr	p;

	p = new_node(PASSIVE_NODE_SIZE);
	link(p) = passive;
	passive = p;
	cur_break(p) = cur_p;
	incr(pass_number);
	serial(p) = pass_number;
	prev_break(p) = best_place[fit_class];
	p = new_node(ACTIVE_NODE_SIZE);
	break_node(p) = passive;
	line_number(p) = best_pl_line[fit_class] + 1;
	fitness(p) = fit_class;
	type(p) = break_type;
	total_demerits(p) = minimal_demerits[fit_class];
	if (tracing_paragraphs > 0) {
		show_break_node(p, fit_class, break_type);
	}
	return p;
}

void
show_break_node (p, fit_class, break_type)
	ptr	p;
	int	fit_class;
	int	break_type;
{
	print_nl("@@");
	print_int(serial(passive));
	print(": line ");
	print_int(line_number(p) - 1);
	print(".");
	print_int(fit_class);
	if (break_type == HYPHENATED) {
		print("-");
	}
	print(" t=");
	print_int(total_demerits(p));
	print(" -> @@");
	if (prev_break(passive) == null) {
		print("0");
	} else {
		print_int(serial(prev_break(passive)));
	}
}

void
show_break_status (r, a, b, p, d)
	ptr	r;
	bool	a;
	int	b;
	int	p;
	int	d;
{
	ptr	save_link;

	if (printed_node != cur_p) {
		print_nl(null_str);
		if (cur_p == null) {
			short_display(link(printed_node));
		} else {
			save_link = link(cur_p);
			link(cur_p) = null;
			print_nl(null_str);
			short_display(link(printed_node));
			link(cur_p) = save_link;
		}
		printed_node = cur_p;
	}
	print_nl("@");
	if (cur_p == null) {
		print_esc("par");
	} else if (type(cur_p) != GLUE_NODE) {
		if (type(cur_p) == PENALTY_NODE) {
			print_esc("penalty");
		} else if (type(cur_p) == DISC_NODE) {
			print_esc("discretionary");
		} else if (type(cur_p) == KERN_NODE) {
			print_esc("kern");
		} else {
			print_esc("math");
		}
	}
	print(" via @@");
	if (break_node(r) == null) {
		print("0");
	} else {
		print_int(serial(break_node(r)));
	}
	print(" b=");
	if (b > INF_BAD) {
		print("*");
	} else {
		print_int(b);
	}
	print(" p=");
	print_int(p);
	print(" d=");
	if (a) {
		print("*");
	} else {
		print_int(d);
	}
}

void
update_printed_node ()
{
	int	t;

	if (cur_p == printed_node
	&& cur_p != null
	&& type(cur_p) == DISC_NODE) {
		for (t = replace_count(cur_p); t > 0; decr(t)) {
			printed_node = link(printed_node);
		}
	}
}

ptr
finite_shrink (p)
	ptr	p;
{
	ptr	q;

	if (no_shrink_error_yet) {
		no_shrink_error_yet = FALSE;
		print_err("Infinite glue shrinkage found in a paragraph");
		help_shrink();
		error();
	}
	q = new_spec(p);
	shrink_order(q) = NORMAL;
	delete_glue_ref(p);
	return q;
}

void
_par_init ()
{
}

void
_par_init_once ()
{
	active = new_node(ACTIVE_NODE_SIZE);
	type(active) = HYPHENATED;
	subtype(active) = 0;
	line_number(active) = MAX_HALFWORD;
}

/*
**	Help text
*/

help_shrink()
{
	help5("The paragraph just ended includes some glue that has",
	"infinite shrinkability, e.g., `\\hskip 0pt minus 1fil'.",
	"Such glue doesn't belong there---it allows a paragraph",
	"of any length to fit on one line. But it's safe to proceed,",
	"since the offensive shrinkability has been made finite.");
}
