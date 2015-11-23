
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

#include	<stdio.h>
#include	<string.h>
#include	<signal.h>
#include	<time.h>
#include	<malloc.h>

/*
 * constants in the outer block
 */

#define NUL			'\0'
#define EOLN			'\n'
#define FALSE			0
#define TRUE			1
#define EMPTY			0

#define MIN_QUARTERWORD		0
#define MAX_QUARTERWORD		255
#define MIN_HALFWORD		0
#define MAX_HALFWORD		65535

/*
 *	types in the outer block
 */

#define global		extern

typedef int		bool;
typedef unsigned char	byte;
typedef int		tok;
typedef int		fnt;
typedef float		ratio;
typedef int 		scal;
typedef char		*str;
typedef FILE		*file;

typedef int		ptr;

struct qcell {
	short	b0;
	short	b1;
	short	b2;
	short	b3;
};
typedef struct qcell qcell;

union hcell { 
	int	i;   
	ptr	p;
	struct {
		short	b0;
		short	b1;
	} qq;
};
typedef union hcell hcell;

struct mcell { 
	hcell	h1;
	hcell	h2;
};
typedef struct mcell mcell;

#define rinfo(P)	((mcell *) (P))->h1.i
#define link(P)		((mcell *) (P))->h1.p
#define info(P)		((mcell *) (P))->h2.i
#define llink(P)	((mcell *) (P))->h2.p
#define type(P)		((mcell *) (P))->h2.qq.b0
#define subtype(P)	((mcell *) (P))->h2.qq.b1
#define	q0(P)		((mcell *) (P))->h1.qq.b0
#define	q1(P)		((mcell *) (P))->h1.qq.b1
#define	q2(P)		((mcell *) (P))->h2.qq.b0
#define	q3(P)		((mcell *) (P))->h2.qq.b1

#ifdef NOLCASTS
#define mcopy(x,y)	memcpy((void *)x, (void *)y, sizeof(mcell))
#else
#define mcopy(x,y)	*(mcell *)x = *(mcell *)y
#endif
#define mzero(m)	memset(m,0,sizeof(mcell))

/*
 *	variables in the outer block
 */

global	char	*banner;
global	str	format_ident;
global	int	ready_already;

/*
 *	functions in the outer block
 */

void	initialize();
void	initialize_once();
bool	decode_args();
void	fix_date_and_time();
void	handle_int();
void	final_cleanup();
void	close_files_and_terminate();
void	call_edit();

void	_tex_init ();
void	_tex_init_once ();

/*
 * some common programming idioms
 */

#define incr(i)		++(i)
#define decr(i)	 	--(i)
#define odd(i)		((i) & 1)
#define abs(i)		((i) >= 0 ? (i) : -(i))
#define round(x)	(int) ((x) > 0.0 ? ((x) + 0.5) : ((x) - 0.5))
#define negate(x)	(x) = -(x)
#define loop		while (1)

#include "align.h"
#include "alloc.h"
#include "arith.h"
#include "box.h"
#include "boxlist.h"
#include "chr.h"
#include "cmd.h"
#include "cond.h"
#include "def.h"
#include "dvi.h"
#include "sym.h"
#include "error.h"
#include "eval.h"
#include "expand.h"
#include "fileio.h"
#include "hyph.h"
#include "math.h"
#include "mathlist.h"
#include "mlst-hlst.h"
#include "pack.h"
#include "page.h"
#include "par.h"
#include "print.h"
#include "scan.h"
#include "str.h"
#include "tfm.h"
#include "tok.h"
#include "toklist.h"
#include "texext.h"
