
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

ptr	adjust_head;
ptr	adjust_tail;
scal	total_shrink[4];
scal	total_stretch[4];
int	last_badness;
int	pack_begin_line;

#define clr_dimens() \
	{total_stretch[FILLL] = 0; \
	total_stretch[FILL] = 0; \
	total_stretch[FIL] = 0; \
	total_stretch[NORMAL] = 0; \
	total_shrink[FILLL] = 0; \
	total_shrink[FILL] = 0; \
	total_shrink[FIL] = 0; \
	total_shrink[NORMAL] = 0;}

ptr
hpack (p, w, m)
	ptr	p;
	scal	w;
	int	m;
{
	ptr	q, g, r;
	scal	s, h, d, x;
	int	hd, o;
	fnt	f;
	qcell	i;

	last_badness = 0;
	r = new_node(BOX_NODE_SIZE);
	type(r) = HLIST_NODE;
	subtype(r) = MIN_QUARTERWORD;
	shift_amount(r) = 0;
	q = node_list(r);
	link(q) = p;
	x = h = d = 0;
	clr_dimens();

	while (p != null) {
reswitch:
		while (is_char_node(p)) {
			f = font(p);
			i = char_info(f, character(p));
			hd = height_depth(i);
			x += char_width(f, i);
			s = char_height(f, hd);
			if (s > h) {
				h = s;
			}
			s = char_depth(f, hd);
			if (s > d) {
				d = s;
			}
			p = link(p);
		}
		if (p != null) {
			switch (type(p))
			{
			case HLIST_NODE:
			case VLIST_NODE:
			case RULE_NODE:
			case UNSET_NODE:
				x += box_width(p);
				if (type(p) >= RULE_NODE) {
					s = 0;
				} else {
					s = shift_amount(p);
				}
				if (box_height(p) - s > h) {
					h = box_height(p) - s;
				}
				if (box_depth(p) + s > d) {
					d = box_depth(p) + s;
				}
				break;

			case INS_NODE:
			case MARK_NODE:
			case ADJUST_NODE:
				if (adjust_tail == null) {
					break;
				}
				while (link(q) != p) {
					q = link(q);
				}
				if (type(p) == ADJUST_NODE) {
					link(adjust_tail) = adjust_ptr(p);
					while (link(adjust_tail) != null) {
						adjust_tail =
							link(adjust_tail);
					}
					p = link(p);
					free_node(link(q), SMALL_NODE_SIZE);
				} else {
					adjust_tail = link(adjust_tail) = p;
					p = link(p);
				}
				link(q) = p;
				p = q;
				break;

			case WHATSIT_NODE:
				break;
			
			case GLUE_NODE:
				g = glue_ptr(p);
				x += glue_width(g);
				o = stretch_order(g);
				total_stretch[o] += stretch(g);
				o = shrink_order(g);
				total_shrink[o] += shrink(g);
				if (subtype(p) >= A_LEADERS) {
					g = leader_ptr(p);
					if (box_height(g) > h) {
						h = box_height(g);
					}
					if (box_depth(g) > d) {
						d = box_depth(g);
					}
				}
				break;
			
			case KERN_NODE:
			case MATH_NODE:
				x += kern_width(p);
				break;

			case LIGATURE_NODE:
				p = make_char_from_lig(p);
				goto reswitch;

			default:
				break;
			}
			p = link(p);
		}
	}
	if (adjust_tail != null) {
		link(adjust_tail) = null;
	}
	box_height(r) = h;
	box_depth(r) = d;
	if (m == ADDITIONAL) {
		w += x;
	}
	box_width(r) = w;
	x = w - x;
	if (x == 0) {
		glue_sign(r) = NORMAL;
		glue_order(r) = NORMAL;
		glue_set(r) = 0.0;
		return r;
	} else if (x > 0) {
		o = get_stretch_order(); 
		glue_order(r) = o;
		glue_sign(r) = STRETCHING;
		if (total_stretch[o] != 0) {
			glue_set(r) = (float) x / (float) total_stretch[o];
		} else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
		}
		if (o == NORMAL
		&& list_ptr(r) != null) {
			last_badness = badness(x, total_stretch[NORMAL]);
			if (last_badness > hbadness) {
				print_ln();
				if (last_badness > 100) {
					print_nl("Underfull");
				} else {
					print_nl("Loose");
				}
				print(" \\hbox (badness ");
				print_int(last_badness);
				goto common_end;
			}
		}
		return r;
	} else {
		o = get_shrink_order();
		glue_order(r) = o;
		glue_sign(r) = SHRINKING;
		if (total_shrink[o] != 0) {
			glue_set(r) = (float) -x / total_shrink[o];
		} else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
		}
		if (total_shrink[o] < -x
		&& o == NORMAL && list_ptr(r) != null) {
			last_badness = 1000000;
			glue_set(r) = 1.0;
			if (-x - total_shrink[NORMAL] > hfuzz
			|| hbadness < 100) {
				if (overfull_rule > 0
				&& -x - total_shrink[NORMAL] > hfuzz) {
					while (link(q) != null) {
						q = link(q);
					}
					link(q) = new_rule();
					rule_width(link(q)) = overfull_rule;
				}
				print_ln();
				print_nl("Overfull \\hbox ("); 
				print_scaled(-x - total_shrink[NORMAL]);
				print("pt too wide");
				goto common_end;
			}
		} else if (o == NORMAL && list_ptr(r) != null) {
			last_badness = badness(-x, total_shrink[NORMAL]);
			if (last_badness > hbadness) {
				print_ln();
				print_nl("Tight \\hbox (badness ");
				print_int(last_badness);
				goto common_end;
			}
		}
		return r;
	}

common_end:
	if (output_active) {
		print(") has occurred while \\output is active");
	} else {
		if (pack_begin_line != 0) {
			if (pack_begin_line > 0) {
				print(") in paragraph at lines ");
			} else {
				print(") in alignment at lines ");
			}
			print_int(abs(pack_begin_line));
			print("--");
		} else {
			print(") detected at line ");
		}
		print_int(line);
	}
	print_ln();
	font_in_short_display = null_font;
	short_display(list_ptr(r));
	print_ln();
	begin_diagnostic();
	show_box(r);
	end_diagnostic(TRUE);
	return r;
}

ptr
vpackage (p, h, m, l)
	ptr	p;
	scal	h;
	int	m;
	scal	l;
{
	scal	s, w, d, x;
	ptr	g, r;
	int	o;

	last_badness = 0;
	r = new_node(BOX_NODE_SIZE);
	type(r) = VLIST_NODE;
	subtype(r) = MIN_QUARTERWORD;
	shift_amount(r) = 0;
	list_ptr(r) = p;
	d = x = w = 0;
	clr_dimens();
	while (p != null) {
		if (is_char_node(p)) {
			confusion("vpack");
		}
		switch (type(p))
		{
		case HLIST_NODE:
		case VLIST_NODE:
		case RULE_NODE:
		case UNSET_NODE:
			x += d + box_height(p);
			d = box_depth(p);
			if (type(p) >= RULE_NODE) {
				s = 0;
			} else {
				s = shift_amount(p);
			}
			if (box_width(p) + s > w) {
				w = box_width(p) + s;
			}
			break;
		
		case WHATSIT_NODE:
			break;
		
		case GLUE_NODE:
			x += d;
			d = 0;
			g = glue_ptr(p);
			x += glue_width(g);
			o = stretch_order(g);
			total_stretch[o] += stretch(g);
			o = shrink_order(g);
			total_shrink[o] += shrink(g);
			if (subtype(p) >= A_LEADERS) {
				g = leader_ptr(p);
				if (box_width(g) > w) {
					w = box_width(g);
				}
			}
			break;
		
		case KERN_NODE:
			x += d + kern_width(p);
			d = 0;
			break;

		default:
			break;
		}
		p = link(p);
	}
	box_width(r) = w;
	if (d > l) {
		x += d - l;
		box_depth(r) = l;
	} else {
		box_depth(r) = d;
	}
	if (m == ADDITIONAL) {
		h += x;
	}
	box_height(r) = h;
	x = h - x;
	if (x == 0) {
		glue_sign(r) = NORMAL;
		glue_order(r) = NORMAL;
		glue_set(r) = 0.0;
		return r;
	} else if (x > 0) {
		o = get_stretch_order();
		glue_order(r) = o;
		glue_sign(r) = STRETCHING;
		if (total_stretch[o] != 0) {
			glue_set(r) = (float) x / total_stretch[o];
		} else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
		}
		if (o == NORMAL && list_ptr(r) != NULL) {
			last_badness = badness(x, total_stretch[NORMAL]);
			if (last_badness > vbadness) {
				print_ln();
				if (last_badness > 100) {
					print_nl("Underfull");
				} else {
					print_nl("Loose");
				}
				print(" \\vbox (badness ");
				print_int(last_badness);
				goto common_end;
			}
		}
		return r;
	} else {
		o = get_shrink_order();
		glue_order(r) = o;
		glue_sign(r) = SHRINKING;
		if (total_shrink[o] != 0) {
			glue_set(r) = (float) -x / total_shrink[o];
		} else {
			glue_sign(r) = NORMAL;
			glue_set(r) = 0.0;
		}
		if (total_shrink[o] < -x
		&& o == NORMAL && list_ptr(r) != NULL) {
			last_badness = 1000000;
			glue_set(r) = 1.0;
			if (-x - total_shrink[NORMAL] > vfuzz
			|| vbadness < 100) {
				print_ln();
				print_nl("Overfull \\vbox (");
				print_scaled(-x - total_shrink[NORMAL]);
				print("pt too high");
				goto common_end;
			}
		} else if (o == NORMAL && list_ptr(r) != null) {
			last_badness = badness(-x, total_shrink[NORMAL]);
			if (last_badness > vbadness) {
				print_ln();
				print_nl("Tight \\vbox (badness ");
				print_int(last_badness);
				goto common_end;
			}
		}
		return r;
	}

common_end:
	if (output_active) {
		print(") has occurred while \\output is active");
	} else {
		if (pack_begin_line != 0) {
			print(") in alignment at lines ");
			print_int(abs(pack_begin_line));
			print("--");
		} else {
			print(") detected at line ");
		}
		print_int(line);
		print_ln();
	}
	begin_diagnostic();
	show_box(r);
	end_diagnostic(TRUE);
	return r;
}

void
_pack_init ()
{
	pack_begin_line = 0;
	last_badness = 0;
	adjust_tail = null;
}

void
_pack_init_once ()
{
	adjust_head = new_avail();
}
