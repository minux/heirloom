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
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "n4.c	1.8	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n4.c	1.14 (gritter) 12/4/05
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

#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include "tdef.h"
#ifdef NROFF
#include "tw.h"
#endif
#include "proto.h"
#include "ext.h"
/*
 * troff4.c
 * 
 * number registers, conversion, arithmetic
 */


int	regcnt = NNAMES;
int	falsef	= 0;	/* on if inside false branch of if */
#define	NHASH(i)	((i>>6)^i)&0177
struct	numtab	*nhash[128];	/* 128 == the 0177 on line above */

void *
grownumtab(void)
{
	int	i, inc = 20;
	struct numtab	*onc;

	onc = numtab;
	if ((numtab = realloc(numtab, (NN+inc) * sizeof *numtab)) == NULL)
		return NULL;
	memset(&numtab[NN], 0, inc * sizeof *numtab);
	if (NN == 0) {
		for (i = 0; initnumtab[i].r; i++)
			numtab[i] = initnumtab[i];
	} else {
		for (i = 0; i < sizeof nhash / sizeof *nhash; i++)
			if (nhash[i])
				nhash[i] += numtab - onc;
		for (i = 0; i < NN; i++)
			if (numtab[i].link)
				numtab[i].link += numtab - onc;
	}
	NN += inc;
	return numtab;
}

void
setn(void)
{
	register int i, j;
	register tchar ii;
	int	f;
	double	fl;

	f = nform = 0;
	if ((i = cbits(ii = getach())) == '+')
		f = 1;
	else if (i == '-')
		f = -1;
	else 
		ch = ii;
	if (falsef)
		f = 0;
	if ((i = getsn()) == 0)
		return;
	if ((i & 0177) == '.')
		switch (i >> BYTE) {
		case 's': 
			i = fl = u2pts(pts);
			if (i != fl) {
				char	tb[20];
				roff_sprintf(tb, "%f", fl);
				cpushback(tb);
				return;
			}
			break;
		case 'v': 
			i = lss;		
			break;
		case 'f': 
			i = font;	
			break;
		case 'p': 
			i = pl;		
			break;
		case 't':  
			i = findt1();	
			break;
		case 'o': 
			i = po;		
			break;
		case 'l': 
			i = ll;		
			break;
		case 'i': 
			i = in;		
			break;
		case '$': 
			i = frame->nargs;		
			break;
		case 'A': 
			i = ascii;		
			break;
		case 'c': 
			i = numtab[CD].val;		
			break;
		case 'n': 
			i = lastl;		
			break;
		case 'a': 
			i = ralss;		
			break;
		case 'h': 
			i = dip->hnl;	
			break;
		case 'd':
			if (dip != d)
				i = dip->dnl; 
			else 
				i = numtab[NL].val;
			break;
		case 'u': 
			i = fi;		
			break;
		case 'j': 
			i = ad + 2 * admod;	
			break;
		case 'w': 
			i = widthp;
			break;
		case 'x': 
			i = nel;	
			break;
		case 'y': 
			i = un;		
			break;
		case 'T': 
			i = dotT;		
			break; /*-Tterm used in nroff*/
		case 'V': 
			i = VERT;		
			break;
		case 'H': 
			i = HOR;		
			break;
		case 'k': 
			i = ne;		
			break;
		case 'P': 
			i = print;		
			break;
		case 'L': 
			i = ls;		
			break;
		case 'R': 
			i = NN - regcnt;	
			break;
		case 'z': 
			i = dip->curd;
			pbbuf[pbp++] = (i >> BYTE) & BYTEMASK;
			pbbuf[pbp++] = i & BYTEMASK;
			return;
		case 'b': 
			i = bdtab[font];
			break;
		case 'F':
			cpushback(cfname[ifi] ? cfname[ifi] : "");
			return;

		default:
			goto s0;
		}
	else {
s0:
		if ((j = findr(i)) == -1)
			i = 0;
		else {
			i = numtab[j].val = (numtab[j].val+numtab[j].inc*f);
			nform = numtab[j].fmt;
		}
	}
	setn1(i, nform, (tchar) 0);
}

tchar	numbuf[17];
tchar	*numbufp;

int 
wrc(tchar i)
{
	if (numbufp >= &numbuf[16])
		return(0);
	*numbufp++ = i;
	return(1);
}



/* insert into input number i, in format form, with size-font bits bits */
void
setn1(int i, int form, tchar bits)
{
	numbufp = numbuf;
	nrbits = bits;
	nform = form;
	fnumb(i, wrc);
	*numbufp = 0;
	pushback(numbuf);
}


void
nrehash(void)
{
	register struct numtab *p;
	register int i;

	for (i=0; i<128; i++)
		nhash[i] = 0;
	for (p=numtab; p < &numtab[NN]; p++)
		p->link = 0;
	for (p=numtab; p < &numtab[NN]; p++) {
		if (p->r == 0)
			continue;
		i = NHASH(p->r);
		p->link = nhash[i];
		nhash[i] = p;
	}
}

void
nunhash(register struct numtab *rp)
{	
	register struct numtab *p;
	register struct numtab **lp;

	if (rp->r == 0)
		return;
	lp = &nhash[NHASH(rp->r)];
	p = *lp;
	while (p) {
		if (p == rp) {
			*lp = p->link;
			p->link = 0;
			return;
		}
		lp = &p->link;
		p = p->link;
	}
}

int 
findr(register int i)
{
	register struct numtab *p;
	register int h = NHASH(i);

	if (i == 0)
		return(-1);
	for (p = nhash[h]; p; p = p->link)
		if (i == p->r)
			return(p - numtab);
	do {
		for (p = numtab; p < &numtab[NN]; p++) {
			if (p->r == 0) {
				p->r = i;
				p->link = nhash[h];
				nhash[h] = p;
				regcnt++;
				return(p - numtab);
			}
		}
	} while (p == &numtab[NN] && grownumtab() != NULL);
	errprint("too many number registers (%d).", NN);
	done2(04); 
	/* NOTREACHED */
	return 0;
}

int 
usedr (	/* returns -1 if nr i has never been used */
    register int i
)
{
	register struct numtab *p;

	if (i == 0)
		return(-1);
	for (p = nhash[NHASH(i)]; p; p = p->link)
		if (i == p->r)
			return(p - numtab);
	return -1;
}


int 
fnumb(register int i, register int (*f)(tchar))
{
	register int j;

	j = 0;
	if (i < 0) {
		j = (*f)('-' | nrbits);
		i = -i;
	}
	switch (nform) {
	default:
	case '1':
	case 0: 
		return decml(i, f) + j;
		break;
	case 'i':
	case 'I': 
		return roman(i, f) + j;
		break;
	case 'a':
	case 'A': 
		return abc(i, f) + j;
		break;
	}
}


int 
decml(register int i, register int (*f)(tchar))
{
	register int j, k;

	k = 0;
	nform--;
	if ((j = i / 10) || (nform > 0))
		k = decml(j, f);
	return(k + (*f)((i % 10 + '0') | nrbits));
}


int 
roman(int i, int (*f)(tchar))
{

	if (!i)
		return((*f)('0' | nrbits));
	if (nform == 'i')
		return(roman0(i, f, "ixcmz", "vldw"));
	else 
		return(roman0(i, f, "IXCMZ", "VLDW"));
}


int 
roman0(int i, int (*f)(tchar), char *onesp, char *fivesp)
{
	register int q, rem, k;

	k = 0;
	if (!i)
		return(0);
	k = roman0(i / 10, f, onesp + 1, fivesp + 1);
	q = (i = i % 10) / 5;
	rem = i % 5;
	if (rem == 4) {
		k += (*f)(*onesp | nrbits);
		if (q)
			i = *(onesp + 1);
		else 
			i = *fivesp;
		return(k += (*f)(i | nrbits));
	}
	if (q)
		k += (*f)(*fivesp | nrbits);
	while (--rem >= 0)
		k += (*f)(*onesp | nrbits);
	return(k);
}


int 
abc(int i, int (*f)(tchar))
{
	if (!i)
		return((*f)('0' | nrbits));
	else 
		return(abc0(i - 1, f));
}


int 
abc0(int i, int (*f)(tchar))
{
	register int j, k;

	k = 0;
	if (j = i / 26)
		k = abc0(j - 1, f);
	return(k + (*f)((i % 26 + nform) | nrbits));
}

long 
atoi0(void)
{
	register int c, k, cnt;
	register tchar ii;
	long	i, acc;

	i = 0; 
	acc = 0;
	nonumb = 0;
	cnt = -1;
a0:
	cnt++;
	ii = getch();
	c = cbits(ii);
	switch (c) {
	default:
		ch = ii;
		if (cnt)
			break;
	case '+':
		i = ckph();
		if (nonumb)
			break;
		acc += i;
		goto a0;
	case '-':
		i = ckph();
		if (nonumb)
			break;
		acc -= i;
		goto a0;
	case '*':
		i = ckph();
		if (nonumb)
			break;
		acc *= i;
		goto a0;
	case '/':
		i = ckph();
		if (nonumb)
			break;
		if (i == 0) {
			flusho();
			errprint("divide by zero.");
			acc = 0;
		} else 
			acc /= i;
		goto a0;
	case '%':
		i = ckph();
		if (nonumb)
			break;
		acc %= i;
		goto a0;
	case '&':	/*and*/
		i = ckph();
		if (nonumb)
			break;
		if ((acc > 0) && (i > 0))
			acc = 1; 
		else 
			acc = 0;
		goto a0;
	case ':':	/*or*/
		i = ckph();
		if (nonumb)
			break;
		if ((acc > 0) || (i > 0))
			acc = 1; 
		else 
			acc = 0;
		goto a0;
	case '=':
		if (cbits(ii = getch()) != '=')
			ch = ii;
		i = ckph();
		if (nonumb) {
			acc = 0; 
			break;
		}
		if (i == acc)
			acc = 1;
		else 
			acc = 0;
		goto a0;
	case '>':
		k = 0;
		if (cbits(ii = getch()) == '=')
			k++; 
		else 
			ch = ii;
		i = ckph();
		if (nonumb) {
			acc = 0; 
			break;
		}
		if (acc > (i - k))
			acc = 1; 
		else 
			acc = 0;
		goto a0;
	case '<':
		k = 0;
		if (cbits(ii = getch()) == '=')
			k++; 
		else 
			ch = ii;
		i = ckph();
		if (nonumb) {
			acc = 0; 
			break;
		}
		if (acc < (i + k))
			acc = 1; 
		else 
			acc = 0;
		goto a0;
	case ')': 
		break;
	case '(':
		acc = atoi0();
		goto a0;
	}
	return(acc);
}


long 
ckph(void)
{
	register tchar i;
	register long	j;

	if (cbits(i = getch()) == '(')
		j = atoi0();
	else {
		j = atoi1(i);
	}
	return(j);
}


long 
atoi1(register tchar ii)
{
	register int i, j, digits;
	register long long	acc;
	int	neg, abs, field;

	neg = abs = field = digits = 0;
	acc = 0;
	for (;;) {
		i = cbits(ii);
		switch (i) {
		default:
			break;
		case '+':
			ii = getch();
			continue;
		case '-':
			neg = 1;
			ii = getch();
			continue;
		case '|':
			abs = 1 + neg;
			neg = 0;
			ii = getch();
			continue;
		}
		break;
	}
a1:
	while (i >= '0' && i <= '9') {
		field++;
		digits++;
		acc = 10 * acc + i - '0';
		ii = getch();
		i = cbits(ii);
	}
	if (i == '.') {
		field++;
		digits = 0;
		ii = getch();
		i = cbits(ii);
		goto a1;
	}
	if (!field) {
		ch = ii;
		goto a2;
	}
	switch (i) {
	case 'u':
		i = j = 1;	/* should this be related to HOR?? */
		break;
	case 'v':	/*VSs - vert spacing*/
		j = lss;
		i = 1;
		break;
	case 'm':	/*Ems*/
		j = EM;
		i = 1;
		break;
	case 'n':	/*Ens*/
		j = EM;
#ifndef NROFF
		i = 2;
#endif
#ifdef NROFF
		i = 1;	/*Same as Ems in NROFF*/
#endif
		break;
	case 'p':	/*Points*/
		j = INCH;
		i = 72;
		break;
	case 'i':	/*Inches*/
		j = INCH;
		i = 1;
		break;
	case 'c':	/*Centimeters*/
		/* if INCH is too big, this will overflow */
		j = INCH * 50;
		i = 127;
		break;
	case 'P':	/*Picas*/
		j = INCH;
		i = 6;
		break;
	default:
		j = dfact;
		ch = ii;
		i = dfactd;
	}
	if (neg) 
		acc = -acc;
	if (!noscale) {
		acc = (acc * j) / i;
	}
	if ((field != digits) && (digits > 0))
		while (digits--)
			acc /= 10;
	if (abs) {
		if (dip != d)
			j = dip->dnl; 
		else 
			j = numtab[NL].val;
		if (!vflag) {
			j = numtab[HP].val;
		}
		if (abs == 2)
			j = -j;
		acc -= j;
	}
a2:
	nonumb = !field;
	return(acc);
}


void
setnr(const char *name, int val, int inc)
{
	int	i, j;

	if ((j = makerq(name)) < 0)
		return;
	if ((i = findr(j)) < 0)
		return;
	numtab[i].val = val;
	numtab[i].inc = inc;
}


void
caserr(void)
{
	register int i, j;
	register struct numtab *p;

	lgf++;
	while (!skip() && (i = getrq()) ) {
		if (i >= 256)
			i = maybemore(i, 0);
		j = usedr(i);
		if (j < 0)
			continue;
		p = &numtab[j];
		nunhash(p);
		p->r = p->val = p->inc = p->fmt = 0;
		regcnt--;
	}
}


void
casenr(void)
{
	register int i, j;

	lgf++;
	skip();
	if ((j = getrq()) >= 256)
		j = maybemore(j, 1);
	if ((i = findr(j)) == -1)
		goto rtn;
	skip();
	j = inumb(&numtab[i].val);
	if (nonumb)
		goto rtn;
	numtab[i].val = j;
	skip();
	j = atoi();
	if (nonumb)
		goto rtn;
	numtab[i].inc = j;
rtn:
	return;
}


void
caseaf(void)
{
	register int i, k;
	register tchar j, jj;

	lgf++;
	if (skip() || !(i = getrq()) || skip())
		return;
	if (i >= 256)
		i = maybemore(i, 1);
	k = 0;
	j = getch();
	if (!ischar(jj = cbits(j)) || !isalpha(jj)) {
		ch = j;
		while ((j = cbits(getch())) >= '0' &&  j <= '9')
			k++;
	}
	if (!k)
		k = j;
	numtab[findr(i)].fmt = k & BYTEMASK;
}

void
setaf (void)	/* return format of number register */
{
	register int i, j;

	i = usedr(getsn());
	if (i == -1)
		return;
	if (numtab[i].fmt > 20)	/* it was probably a, A, i or I */
		pbbuf[pbp++] = numtab[i].fmt;
	else {
		for (j = (numtab[i].fmt ? numtab[i].fmt : 1); j; j--) {
			if (pbp >= pbsize-3)
				if (growpbbuf() == NULL) {
					errprint("no space for .af");
					done(2);
				}
			pbbuf[pbp++] = '0';
		}
	}
}


int 
vnumb(int *i)
{
	vflag++;
	dfact = lss;
	res = VERT;
	return(inumb(i));
}


int 
hnumb(int *i)
{
	dfact = EM;
	res = HOR;
	return(inumb(i));
}


int 
inumb(int *n)
{
	register int i, j, f;
	register tchar ii;

	f = 0;
	if (n) {
		if ((j = cbits(ii = getch())) == '+')
			f = 1;
		else if (j == '-')
			f = -1;
		else 
			ch = ii;
	}
	i = atoi();
	if (n && f)
		i = *n + f * i;
	i = quant(i, res);
	vflag = 0;
	res = dfactd = dfact = 1;
	if (nonumb)
		i = 0;
	return(i);
}


int 
quant(int n, int m)
{
	register int i, neg;

	neg = 0;
	if (n < 0) {
		neg++;
		n = -n;
	}
	/* better as i = ((n + (m/2))/m)*m */
	i = n / m;
	if ((n - m * i) > (m / 2))
		i += 1;
	i *= m;
	if (neg)
		i = -i;
	return(i);
}


