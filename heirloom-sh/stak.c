/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 1992 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)stak.c	1.3 (gritter) 6/14/05
 */
/* from OpenSolaris "stak.c	1.10	05/06/08 SMI" */
/*
 * UNIX shell
 */

#include	"defs.h"


/* ========	storage allocation	======== */

unsigned char *
getstak(asize)			/* allocate requested stack */
int	asize;
{
	register unsigned char	*oldstak;
	register int	size;

	size = round(asize, BYTESPERWORD);
	oldstak = stakbot;
	staktop = stakbot += size;
	if (staktop >= brkend)
		growstak(staktop);
	return(oldstak);
}

/*
 * set up stack for local use
 * should be followed by `endstak'
 */
unsigned char *
locstak()
{
	if (brkend - stakbot < BRKINCR)
	{
		if (setbrk(brkincr) == -1)
			error(nostack);
		if (brkincr < BRKMAX)
			brkincr += 256;
	}
	return(stakbot);
}

void
growstak(newtop)
unsigned char	*newtop;
{
	register unsigned	incr;

	incr = (unsigned)round(newtop - brkend + 1, BYTESPERWORD);
	if (brkincr > incr)
		incr = brkincr;
	if (setbrk(incr) == -1)
		error(nospace);
}

unsigned char *
savstak()
{
	assert(staktop == stakbot);
	return(stakbot);
}

unsigned char *
endstak(argp)		/* tidy up after `locstak' */
register unsigned char	*argp;
{
	register unsigned char	*oldstak;

	if (argp >= brkend)
		growstak(argp);
	*argp++ = 0;
	oldstak = stakbot;
	stakbot = staktop = (unsigned char *)round(argp, BYTESPERWORD);
	if (staktop >= brkend)
		growstak(staktop);
	return(oldstak);
}

tdystak(x)		/* try to bring stack back to x */
register unsigned char	*x;
{
	while ((unsigned char *)stakbsy > x)
	{
		free(stakbsy);
		stakbsy = stakbsy->word;
	}
	staktop = stakbot = max(x, stakbas);
	rmtemp(x);
}

stakchk()
{
	if ((brkend - stakbas) > BRKINCR + BRKINCR)
		setbrk(-BRKINCR);
}

unsigned char *
cpystak(x)
unsigned char	*x;
{
	return(endstak(movstrstak(x, locstak())));
}

unsigned char *
movstrstak(a, b)
register unsigned char	*a, *b;
{
	do
	{
		if (b >= brkend)
			growstak(b);
	}
	while (*b++ = *a++);
	return(--b);
}

/*
 * Copy s2 to s1, always copy n bytes.
 * Return s1
 */
unsigned char *
memcpystak(s1, s2, n)
register unsigned char *s1, *s2;
register int n;
{
	register unsigned char *os1 = s1;

	while (--n >= 0) {
		if (s1 >= brkend)
			growstak(s1);
		*s1++ = *s2++;
	}
	return (os1);
}
