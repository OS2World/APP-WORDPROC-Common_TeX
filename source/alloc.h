
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

#define null 0

global	ptr	avail;

ptr	new_avail();

#ifdef STAT
#define fast_new_avail(M) \
    {M = avail; if (M == NULL) M = new_avail(); \
    else {avail = link(M); link(M) = NULL; incr(dyn_used);}}
#define free_avail(M) \
    {link(M) = avail; avail = M; decr(dyn_used);}
#else
#define fast_new_avail(M) \
    {M = avail; if (M == NULL) M = new_avail(); \
    else {avail = link(M); link(M) = NULL;}}
#define free_avail(M) \
    {link(M) = avail; avail = M;}
#endif

global	ptr	rover;

ptr	new_node();
void	free_node();

#ifdef STAT
global	int	dyn_used;
global	int	var_used;
#endif

void	_alloc_init();
void	_alloc_init_once();
