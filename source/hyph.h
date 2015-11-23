
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

global	ptr	ha;
global	ptr	hb;
global	fnt	hf;
global	int	hn;
global	int	hc[];
global	int	hu[];

global	int	l_hyf;
global	int	r_hyf;
global	int	cur_lang;

global	ptr	init_list;
global	bool	init_lig;
global	bool	init_lft;

struct trie_t {
	short	s;
	union {
		short	s;
		struct {
			byte	b0;
			byte	b1;
		} bb;
	} u_s;
};
typedef struct trie_t trie_t;

#define trie_op(T)		trie[T].u_s.bb.b0
#define trie_char(T)		trie[T].u_s.bb.b1
#define trie_back(T)		trie[T].u_s.s
#define trie_link(T)		trie[T].s

global	trie_t	*trie;

global	int	*op_start;
global	int	*hyf_distance;
global	int	*hyf_num;
global	int	*hyf_next;

global	int	hyph_count;
global	str	*hyph_word;
global	ptr	*hyph_list;

void	new_hyph_exceptions();
void	enter_hyph_exception();

void	init_hyph();
void	try_hyph();
void	hyphenate();

global	int	hyf[];
global	int	hyf_char;

global	int	hyphen_passed;

int	reconstitute();

global	int	trie_op_ptr;
global	int	*trie_op_hash;
global	int	*trie_op_val;
global	int	*trie_op_lang;
global	int	*trie_op_used;

int	new_trie_op();

#define trie_root	trie_l[0]

global	int	trie_ptr;
global	int	trie_max;
global	int	*trie_c;
global	int	*trie_o;
global	int	*trie_l;
global	int	*trie_r;
global	int	*trie_hash;
global	int	*trie_min;

int	trie_compress();
int	trie_node();

#define trie_ref	trie_hash

global	bool	*trie_taken;

void	first_fit();
void	trie_pack();
void	trie_fix();

void	new_patterns();
void	init_trie();

void	init_pattern_memory();
void	free_pattern_memory();

global	bool	trie_not_ready;

void	_hyph_init();
void	_hyph_init_once();
