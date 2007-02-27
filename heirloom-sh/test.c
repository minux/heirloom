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
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */
/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)test.c	1.3 (gritter) 6/14/05
 */
/* from OpenSolaris "test.c	1.15	05/06/08 SMI" */

/*
 *      test expression
 *      [ expression ]
 */

#include	"defs.h"
#include <sys/types.h>
#include <sys/stat.h>

extern	int lstat();
int	ap, ac;
unsigned char **av;

test(argn, com)
unsigned char *com[];
int	argn;
{
	ac = argn;
	av = com;
	ap = 1;
	if (eq(com[0],"["))
	{
		if (!eq(com[--ac], "]"))
			failed("test", "] missing");
	}
	com[ac] = 0;
	if (ac <= 1)
		return(1);
	return(sexp() ? 0 : 1);
}

unsigned char *
nxtarg(mt)
{
	if (ap >= ac)
	{
		if (mt)
		{
			ap++;
			return(0);
		}
		failed("test", "argument expected");
	}
	return(av[ap++]);
}

sexp()
{
	int	p1;
	unsigned char	*p2;

	p1 = e1();
	p2 = nxtarg(1);
	if (p2 != 0)
	{
		if (eq(p2, "-o"))
			return(p1 | sexp());

		/* if (!eq(p2, ")"))
			failed("test", synmsg); */
	}
	ap--;
	return(p1);
}

e1()
{
	int	p1;
	unsigned char	*p2;

	p1 = e2();
	p2 = nxtarg(1);

	if ((p2 != 0) && eq(p2, "-a"))
		return(p1 & e1());
	ap--;
	return(p1);
}

e2()
{
	if (eq(nxtarg(0), "!"))
		return(!e3());
	ap--;
	return(e3());
}

e3()
{
	int	p1;
	register unsigned char	*a;
	unsigned char	*p2;
	long long	ll_1, ll_2;

	a = nxtarg(0);
	if (eq(a, "("))
	{
		p1 = sexp();
		if (!eq(nxtarg(0), ")"))
			failed("test",") expected");
		return(p1);
	}
	p2 = nxtarg(1);
	ap--;
	if ((p2 == 0) || (!eq(p2, "=") && !eq(p2, "!=")))
	{
		if (eq(a, "-r"))
			return(chk_access(nxtarg(0), S_IREAD, 0) == 0);
		if (eq(a, "-w"))
			return(chk_access(nxtarg(0), S_IWRITE, 0) == 0);
		if (eq(a, "-x"))
			return(chk_access(nxtarg(0), S_IEXEC, 0) == 0);
		if (eq(a, "-d"))
			return(filtyp(nxtarg(0), S_IFDIR));
		if (eq(a, "-c"))
			return(filtyp(nxtarg(0), S_IFCHR));
		if (eq(a, "-b"))
			return(filtyp(nxtarg(0), S_IFBLK));
		if (eq(a, "-f"))
			if (ucb_builtins) {
				struct stat statb;
			
				return(stat((char *)nxtarg(0), &statb) >= 0 &&
					(statb.st_mode & S_IFMT) != S_IFDIR);
			}
			else
				return(filtyp(nxtarg(0), S_IFREG));
		if (eq(a, "-u"))
			return(ftype(nxtarg(0), S_ISUID));
		if (eq(a, "-g"))
			return(ftype(nxtarg(0), S_ISGID));
		if (eq(a, "-k"))
			return(ftype(nxtarg(0), S_ISVTX));
		if (eq(a, "-p"))
			return(filtyp(nxtarg(0), S_IFIFO));
		if (eq(a, "-h") || eq(a, "-L"))
			return(filtyp(nxtarg(0), S_IFLNK));
   		if (eq(a, "-s"))
			return(fsizep(nxtarg(0)));
		if (eq(a, "-t"))
		{
			if (ap >= ac)		/* no args */
				return(isatty(1));
			else if (eq((a = nxtarg(0)), "-a") || eq(a, "-o"))
			{
				ap--;
				return(isatty(1));
			}
			else
				return(isatty(atoi((char *)a)));
		}
		if (eq(a, "-n"))
			return(!eq(nxtarg(0), ""));
		if (eq(a, "-z"))
			return(eq(nxtarg(0), ""));
	}

	p2 = nxtarg(1);
	if (p2 == 0)
		return(!eq(a, ""));
	if (eq(p2, "-a") || eq(p2, "-o"))
	{
		ap--;
		return(!eq(a, ""));
	}
	if (eq(p2, "="))
		return(eq(nxtarg(0), a));
	if (eq(p2, "!="))
		return(!eq(nxtarg(0), a));
	ll_1 = strtoll((char *)a, NULL, 10);
	ll_2 = strtoll((char *)nxtarg(0), NULL, 10);
	if (eq(p2, "-eq"))
		return (ll_1 == ll_2);
	if (eq(p2, "-ne"))
		return (ll_1 != ll_2);
	if (eq(p2, "-gt"))
		return (ll_1 > ll_2);
	if (eq(p2, "-lt"))
		return (ll_1 < ll_2);
	if (eq(p2, "-ge"))
		return (ll_1 >= ll_2);
	if (eq(p2, "-le"))
		return (ll_1 <= ll_2);

	bfailed(btest, badop, p2);
/* NOTREACHED */
}


ftype(f, field)
unsigned char	*f;
int	field;
{
	struct stat statb;

	if (stat((char *)f, &statb) < 0)
		return(0);
	if ((statb.st_mode & field) == field)
		return(1);
	return(0);
}

filtyp(f,field)
unsigned char	*f;
int field;
{
	struct stat statb;
	int (*statf)() = (field == S_IFLNK) ? lstat : stat;

	if ((*statf)(f, &statb) < 0)
		return(0);
	if ((statb.st_mode & S_IFMT) == field)
		return(1);
	else
		return(0);
}



fsizep(f)
unsigned char *f;
{
	struct stat statb;

	if (stat((char *)f, &statb) < 0)
		return(0);
	return(statb.st_size > 0);
}

/*
 * fake diagnostics to continue to look like original
 * test(1) diagnostics
 */
bfailed(s1, s2, s3) 
unsigned char *s1;
unsigned char *s2;
unsigned char *s3;
{
	prp();
	prs(s1);
	if (s2)
	{
		prs(colon);
		prs(s2);
		prs(s3);
	}
	newline();
	exitsh(ERROR);
}
