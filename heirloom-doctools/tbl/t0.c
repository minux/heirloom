/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved. The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
     
/*
 * Copyright (c) 1983, 1984 1985, 1986, 1987, 1988, Sun Microsystems, Inc.
 * All Rights Reserved.
 */
  
/*	from OpenSolaris "t0.c	1.3	05/06/02 SMI"	 SVr4.0 1.1		*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)t0.c	1.5 (gritter) 9/15/05
 */

 /* t0.c: storage allocation */
#
# include "t..c"
int expflg = 0;
int ctrflg = 0;
int boxflg = 0;
int dboxflg = 0;
int decimalpoint = '.';
int tab = '\t';
int linsize;
int pr1403;
int delim1, delim2;
int evenup[MAXCOL], evenflg;
int F1 = 0;
int F2 = 0;
int allflg = 0;
char *leftover = 0;
int textflg = 0;
int left1flg = 0;
int rightl = 0;
char *cstore, *cspace;
char *last;
struct colstr *table[MAXLIN];
int style[MAXHEAD][MAXCOL];
int ctop[MAXHEAD][MAXCOL];
char font[MAXHEAD][MAXCOL][2];
char csize[MAXHEAD][MAXCOL][4];
char vsize[MAXHEAD][MAXCOL][4];
int lefline[MAXHEAD][MAXCOL];
char cll[MAXCOL][CLLEN];
/*char *rpt[MAXHEAD][MAXCOL];*/
/*char rpttx[MAXRPT];*/
int stynum[MAXLIN+1];
int nslin, nclin;
int sep[MAXCOL];
int fullbot[MAXLIN];
char *instead[MAXLIN];
int used[MAXCOL], lused[MAXCOL], rused[MAXCOL];
int linestop[MAXLIN];
int nlin, ncol;
int iline = 1;
char *ifile = "Input";
int texname = 'a';
int texct = 0;
char texstr[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWYXZ0123456789";
int linstart;
char *exstore, *exlim;
const char *progname;
FILE *tabin  /*= stdin */;
FILE *tabout  /* = stdout */;
