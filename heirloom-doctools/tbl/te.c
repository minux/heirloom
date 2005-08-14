/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved. The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
     
/*
 * Copyright 1983-1988,2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */
  
/*	from OpenSolaris "te.c	1.6	05/06/02 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)te.c	1.9 (gritter) 8/13/05
 */

 /* te.c: error message control, input line count */
# include "t..c"
# include <errno.h>
# include <string.h>
# include <stdlib.h>
void
error(char *s)
{
fprintf(stderr, "\n%s: line %d: %s\n", ifile, iline, s);
# ifndef gcos
fprintf(stderr, "%s quits\n", progname);
exit(1);
# endif
# ifdef gcos
fprintf(stderr, "run terminated due to error condition detected by tbl preprocessor\n");
exit(0);
# endif
}
char *
errmsg(int errnum)
{
	return (strerror(errnum));
}
char *
gets1(char **sp, size_t *zp)
{
char *s, *p = 0;
int c, n = 0;
int nbl;
for (;;)
	{
	iline++;
	for (;;)
		{
		if ((c = getc(tabin))==EOF)
			{
			if (ferror(tabin))
				error(errmsg(errno));
			if (swapin()==0)
				return(0);
			}
		if (n + MAXCHS >= *zp)
			{
			*zp = n + MAXCHS + 128;
			if ((p = realloc(*sp, *zp))==NULL)
				error("Line too long");
			updspace(*sp, p);
			*sp = p;
			}
		if (c=='\n')
			{
			p = *sp;
			s = n ? &(*sp)[n-1] : *sp;
			(*sp)[n] = '\0';
			break;
			}
		(*sp)[n++] = c;
		}
	for(nbl=0; *s == '\\' && s>p; s--)
		nbl++;
	if (linstart && nbl % 2) /* fold escaped nl if in table */
		{
		s++;
		continue;
		}
	break;
	}

return(p);
}
# define BACKMAX 500
char backup[BACKMAX];
char *backp = backup;
void
un1getc(int c)
{
if (c=='\n')
	iline--;
*backp++ = c;
if (backp >= backup+BACKMAX)
	error("too much backup");
}
int
get1char(void)
{
int c;
if (backp>backup)
	c = *--backp;
else
	c=getc(tabin);
if (c== EOF) /* EOF */
	{
	if (swapin() ==0)
		error("unexpected EOF");
	c = getc(tabin);
	}
if (c== '\n')
	iline++;
return(c);
}
