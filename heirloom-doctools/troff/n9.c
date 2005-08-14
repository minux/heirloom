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
 * Copyright 1989 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "n9.c	1.11	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n9.c	1.7 (gritter) 8/13/05
 */

/*
 * University Copyright- Copyright (c) 1982, 1986, 1988
 * The Regents of the University of California
 * All Rights Reserved
 *
 * University Acknowledgment- Portions of this document are derived from
 * software developed by the University of California, Berkeley, and its
 * contributors.
 */

#include <stdlib.h>
#include <string.h>

#include "tdef.h"
#ifdef NROFF
#include "tw.h"
#endif
#include "proto.h"
#include "ext.h"
#ifdef EUC
#include <locale.h>
#include <wctype.h>
#include <langinfo.h>

#define	ISO646	"646"

int	multi_locale;
int	(*wdbdg)(wchar_t, wchar_t, int);
wchar_t	*(*wddlm)(wchar_t, wchar_t, int);

int	csi_width[4] = {
	1,
	1,
	2,
	3,
};
#endif /* EUC */

/*
 * troff9.c
 * 
 * misc functions
 */

tchar 
setz(void)
{
	tchar i;

	if (!ismot(i = getch()))
		i |= ZBIT;
	return(i);
}

void
setline(void)
{
	register tchar *i;
	tchar c;
	int	length;
	int	w, cnt, delim, rem, temp;
	tchar linebuf[NC];

	if (ismot(c = getch()))
		return;
	delim = cbits(c);
	vflag = 0;
	dfact = EM;
	length = quant(atoi(), HOR);
	dfact = 1;
	if (!length) {
		eat(delim);
		return;
	}
s0:
	if ((cbits(c = getch())) == delim) {
		ch = c;
		c = RULE | chbits;
	} else if (cbits(c) == FILLER)
		goto s0;
	w = width(c);
	i = linebuf;
	if (length < 0) {
		*i++ = makem(length);
		length = -length;
	}
	if (!(cnt = length / w)) {
		*i++ = makem(-(temp = ((w - length) / 2)));
		*i++ = c;
		*i++ = makem(-(w - length - temp));
		goto s1;
	}
	if (rem = length % w) {
		if (cbits(c) == RULE || cbits(c) == UNDERLINE || cbits(c) == ROOTEN)
			*i++ = c | ZBIT;
		*i++ = makem(rem);
	}
	if (cnt) {
		*i++ = RPT;
		*i++ = cnt;
		*i++ = c;
	}
s1:
	*i++ = 0;
	eat(delim);
	pushback(linebuf);
}


int 
eat(register int c)
{
	register int i;

	while ((i = cbits(getch())) != c &&  (i != '\n'))
		;
	return(i);
}


void
setov(void)
{
	register int j, k;
	tchar i, o[NOV];
	int delim, w[NOV];

	if (ismot(i = getch()))
		return;
	delim = cbits(i);
	for (k = 0; (k < NOV) && ((j = cbits(i = getch())) != delim) &&  (j != '\n'); k++) {
		o[k] = i;
		w[k] = width(i);
	}
	o[k] = w[k] = 0;
	if (o[0])
		for (j = 1; j; ) {
			j = 0;
			for (k = 1; o[k] ; k++) {
				if (w[k-1] < w[k]) {
					j++;
					i = w[k];
					w[k] = w[k-1];
					w[k-1] = i;
					i = o[k];
					o[k] = o[k-1];
					o[k-1] = i;
				}
			}
		}
	else 
		return;
	*pbp++ = makem(w[0] / 2);
	for (k = 0; o[k]; k++)
		;
	while (k>0) {
		k--;
		*pbp++ = makem(-((w[k] + w[k+1]) / 2));
		*pbp++ = o[k];
	}
}


void
setbra(void)
{
	register int k;
	tchar i, *j, dwn;
	int	cnt, delim;
	tchar brabuf[NC];

	if (ismot(i = getch()))
		return;
	delim = cbits(i);
	j = brabuf + 1;
	cnt = 0;
#ifdef NROFF
	dwn = (2 * t.Halfline) | MOT | VMOT;
#endif
#ifndef NROFF
	dwn = EM | MOT | VMOT;
#endif
	while (((k = cbits(i = getch())) != delim) && (k != '\n') &&  (j <= (brabuf + NC - 4))) {
		*j++ = i | ZBIT;
		*j++ = dwn;
		cnt++;
	}
	if (--cnt < 0)
		return;
	else if (!cnt) {
		ch = *(j - 2);
		return;
	}
	*j = 0;
#ifdef NROFF
	*--j = *brabuf = (cnt * t.Halfline) | MOT | NMOT | VMOT;
#endif
#ifndef NROFF
	*--j = *brabuf = (cnt * EM) / 2 | MOT | NMOT | VMOT;
#endif
	*--j &= ~ZBIT;
	pushback(brabuf);
}


void
setvline(void)
{
	register int i;
	tchar c, rem, ver, neg;
	int	cnt, delim, v;
	tchar vlbuf[NC];
	register tchar *vlp;

	if (ismot(c = getch()))
		return;
	delim = cbits(c);
	dfact = lss;
	vflag++;
	i = quant(atoi(), VERT);
	dfact = 1;
	if (!i) {
		eat(delim);
		vflag = 0;
		return;
	}
	if ((cbits(c = getch())) == delim) {
		c = BOXRULE | chbits;	/*default box rule*/
	} else 
		getch();
	c |= ZBIT;
	neg = 0;
	if (i < 0) {
		i = -i;
		neg = NMOT;
	}
#ifdef NROFF
	v = 2 * t.Halfline;
#endif
#ifndef NROFF
	v = EM;
#endif
	cnt = i / v;
	rem = makem(i % v) | neg;
	ver = makem(v) | neg;
	vlp = vlbuf;
	if (!neg)
		*vlp++ = ver;
	if (absmot(rem) != 0) {
		*vlp++ = c;
		*vlp++ = rem;
	}
	while ((vlp < (vlbuf + NC - 3)) && cnt--) {
		*vlp++ = c;
		*vlp++ = ver;
	}
	*(vlp - 2) &= ~ZBIT;
	if (!neg)
		vlp--;
	*vlp++ = 0;
	pushback(vlbuf);
	vflag = 0;
}

#define	NPAIR	(NC/2-6)	/* max pairs in spline, etc. */

void
setdraw (void)	/* generate internal cookies for a drawing function */
{
	int i, dx[NPAIR], dy[NPAIR], delim, type;
	tchar c;
#ifndef	NROFF
	int j, k;
	tchar drawbuf[NC];
#endif	/* NROFF */

	/* input is \D'f dx dy dx dy ... c' (or at least it had better be) */
	/* this does drawing function f with character c and the */
	/* specified dx,dy pairs interpreted as appropriate */
	/* pairs are deltas from last point, except for radii */

	/* l dx dy:	line from here by dx,dy */
	/* c x:		circle of diameter x, left side here */
	/* e x y:	ellipse of diameters x,y, left side here */
	/* a dx1 dy1 dx2 dy2:
			ccw arc: ctr at dx1,dy1, then end at dx2,dy2 from there */
	/* ~ dx1 dy1 dx2 dy2...:
			spline to dx1,dy1 to dx2,dy2 ... */
	/* f dx dy ...:	f is any other char:  like spline */

	if (ismot(c = getch()))
		return;
	delim = cbits(c);
	type = cbits(getch());
	for (i = 0; i < NPAIR ; i++) {
		c = getch();
		if (cbits(c) == delim)
			break;
	/* ought to pick up optional drawing character */
		if (cbits(c) != ' ')
			ch = c;
		vflag = 0;
		dfact = EM;
		dx[i] = quant(atoi(), HOR);
		if (dx[i] > MAXMOT)
			dx[i] = MAXMOT;
		else if (dx[i] < -MAXMOT)
			dx[i] = -MAXMOT;
		if (cbits((c = getch())) == delim) {	/* spacer */
			dy[i++] = 0;
			break;
		}
		vflag = 1;
		dfact = lss;
		dy[i] = quant(atoi(), VERT);
		if (dy[i] > MAXMOT)
			dy[i] = MAXMOT;
		else if (dy[i] < -MAXMOT)
			dy[i] = -MAXMOT;
	}
	dfact = 1;
	vflag = 0;
#ifndef NROFF
	drawbuf[0] = DRAWFCN | chbits | ZBIT;
	drawbuf[1] = type | chbits | ZBIT;
	drawbuf[2] = '.' | chbits | ZBIT;	/* use default drawing character */
	for (k = 0, j = 3; k < i; k++) {
		drawbuf[j++] = MOT | ((dx[k] >= 0) ? dx[k] : (NMOT | -dx[k]));
		drawbuf[j++] = MOT | VMOT | ((dy[k] >= 0) ? dy[k] : (NMOT | -dy[k]));
	}
	if (type == DRAWELLIPSE) {
		drawbuf[5] = drawbuf[4] | NMOT;	/* so the net vertical is zero */
		j = 6;
	}
	drawbuf[j++] = DRAWFCN | chbits | ZBIT;	/* marks end for ptout */
	drawbuf[j] = 0;
	pushback(drawbuf);
#endif
}


void
casefc(void)
{
	register int i;
	tchar j;

	gchtab[fc] &= ~FCBIT;
	fc = IMP;
	padc = ' ';
	if (skip() || ismot(j = getch()) || (i = cbits(j)) == '\n')
		return;
	fc = i;
	gchtab[fc] |= FCBIT;
	if (skip() || ismot(ch) || (ch = cbits(ch)) == fc)
		return;
	padc = ch;
}


tchar 
setfield(int x)
{
	register tchar ii, jj, *fp;
	register int i, j;
	int length, ws, npad, temp, type;
	tchar **pp, *padptr[NPP];
	tchar fbuf[FBUFSZ];
	int savfc, savtc, savlc;
	tchar rchar = 0;
	int savepos;

	if (x == tabch) 
		rchar = tabc | chbits;
	else if (x ==  ldrch) 
		rchar = dotc | chbits;
	temp = npad = ws = 0;
	savfc = fc;
	savtc = tabch;
	savlc = ldrch;
	tabch = ldrch = fc = IMP;
	savepos = numtab[HP].val;
	gchtab[tabch] &= ~TABBIT;
	gchtab[ldrch] &= ~LDRBIT;
	gchtab[fc] &= ~FCBIT;
	gchtab[IMP] |= TABBIT|LDRBIT|FCBIT;
	for (j = 0; ; j++) {
		if ((tabtab[j] & TABMASK) == 0) {
			if (x == savfc)
				errprint("zero field width.");
			jj = 0;
			goto rtn;
		}
		if ((length = ((tabtab[j] & TABMASK) - numtab[HP].val)) > 0 )
			break;
	}
	type = tabtab[j] & (~TABMASK);
	fp = fbuf;
	pp = padptr;
	if (x == savfc) {
		while (1) {
			j = cbits(ii = getch());
			jj = width(ii);
			widthp = jj;
			numtab[HP].val += jj;
			if (j == padc) {
				npad++;
				*pp++ = fp;
				if (pp > (padptr + NPP - 1))
					break;
				goto s1;
			} else if (j == savfc) 
				break;
			else if (j == '\n') {
				temp = j;
				nlflg = 0;
				break;
			}
			ws += jj;
s1:
			*fp++ = ii;
			if (fp > (fbuf + FBUFSZ - 3))
				break;
		}
		if (!npad) {
			npad++;
			*pp++ = fp;
			*fp++ = 0;
		}
		*fp++ = temp;
		*fp++ = 0;
		temp = i = (j = length - ws) / npad;
		i = (i / HOR) * HOR;
		if ((j -= i * npad) < 0)
			j = -j;
		ii = makem(i);
		if (temp < 0)
			ii |= NMOT;
		for (; npad > 0; npad--) {
			*(*--pp) = ii;
			if (j) {
				j -= HOR;
				(*(*pp)) += HOR;
			}
		}
		pushback(fbuf);
		jj = 0;
	} else if (type == 0) {
		/*plain tab or leader*/
		if ((j = width(rchar)) > 0) {
			int nchar = length / j;
			while (nchar-->0 && pbp < &pbbuf[NC-3]) {
				numtab[HP].val += j;
				widthp = j;
				*pbp++ = rchar;
			}
			length %= j;
		}
		if (length)
			jj = length | MOT;
		else 
			jj = getch0();
	} else {
		/*center tab*/
		/*right tab*/
		while (((j = cbits(ii = getch())) != savtc) &&  (j != '\n') && (j != savlc)) {
			jj = width(ii);
			ws += jj;
			numtab[HP].val += jj;
			widthp = jj;
			*fp++ = ii;
			if (fp > (fbuf + FBUFSZ - 3)) 
				break;
		}
		*fp++ = ii;
		*fp++ = 0;
		if (type == RTAB)
			length -= ws;
		else 
			length -= ws / 2; /*CTAB*/
		pushback(fbuf);
		if ((j = width(rchar)) != 0 && length > 0) {
			int nchar = length / j;
			while (nchar-- > 0 && pbp < &pbbuf[NC-3])
				*pbp++ = rchar;
			length %= j;
		}
		length = (length / HOR) * HOR;
		jj = makem(length);
		nlflg = 0;
	}
rtn:
	gchtab[fc] &= ~FCBIT;
	gchtab[tabch] &= ~TABBIT;
	gchtab[ldrch] &= ~LDRBIT;
	fc = savfc;
	tabch = savtc;
	ldrch = savlc;
	gchtab[fc] |= FCBIT;
	gchtab[tabch] = TABBIT;
	gchtab[ldrch] |= LDRBIT;
	numtab[HP].val = savepos;
	return(jj);
}


#ifdef EUC
#ifdef NROFF
/* locale specific initialization */
void
localize(void)
{
	extern int	wdbindf();
	extern wchar_t	*wddelim();
	char	*codeset;

	codeset = nl_langinfo(CODESET);

	if (MB_CUR_MAX > 1)
		multi_locale = 1;
	else {
		if (*codeset == '\0' ||
			(strcmp(codeset, ISO646) == 0)) {
			/*
			 * if codeset is an empty string
			 * assumes this is C locale (7-bit) locale.
			 * This happens in 2.5, 2.5.1, and 2.6 system
			 * Or, if codeset is "646"
			 * this is 7-bit locale.
			 */
			multi_locale = 0;
		} else {
			/* 8-bit locale */
			multi_locale = 1;
		}

	}
	wdbdg = wdbindf;
	wddlm = wddelim;
}
#endif /* EUC */
#endif /* NROFF */

#ifndef	__sun
int
wdbindf(wchar_t wc1, wchar_t wc2, int type)
{
	return 6;
}

wchar_t *
wddelim(wchar_t wc1, wchar_t wc2, int type)
{
	return L" ";
}
#endif	/* !__sun */