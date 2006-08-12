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


/*	from OpenSolaris "n5.c	1.10	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n5.c	1.81 (gritter) 8/12/06
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#if defined (EUC) && defined (NROFF)
#include <stddef.h>
#ifdef	__sun
#include <widec.h>
#else
#include <wchar.h>
#endif
#endif	/* EUC && NROFF */
#include <string.h>
#include <unistd.h>
#include "tdef.h"
#include "ext.h"
#ifdef	NROFF
#include "tw.h"
#endif
#include "pt.h"

extern void mchbits(void);

/*
 * troff5.c
 * 
 * misc processing requests
 */

static int	*iflist;
static int	ifx;

static void
growiflist(void)
{
	int	nnif = NIF + 15;

	if ((iflist = realloc(iflist, nnif * sizeof *iflist)) == NULL) {
		errprint("if-else overflow.");
		ifx = 0;
		edone(040);
	}
	memset(&iflist[NIF], 0, (nnif-NIF) * sizeof *iflist);
	NIF = nnif;
}

void
casead(void)
{
	register int i;

	ad = 1;
	/*leave admod alone*/
	if (skip(0))
		return;
	switch (i = cbits(getch())) {
	case 'r':	/*right adj, left ragged*/
		admod = 2;
		break;
	case 'l':	/*left adj, right ragged*/
		admod = ad = 0;	/*same as casena*/
		break;
	case 'c':	/*centered adj*/
		admod = 1;
		break;
	case 'b': 
	case 'n':
		admod = 0;
		break;
	case '0': 
	case '2': 
	case '4':
		ad = 0;
	case '1': 
	case '3': 
	case '5':
		admod = (i - '0') / 2;
	}
}


void
casena(void)
{
	ad = 0;
}


void
casefi(void)
{
	tbreak();
	fi++;
	pendnf = 0;
}


void
casenf(void)
{
	tbreak();
	fi = 0;
}


void
casers(void)
{
	dip->nls = 0;
}


void
casens(void)
{
	dip->nls++;
}


void
casespreadwarn(void)
{
	if (skip(0))
		spreadwarn = !spreadwarn;
	else {
		dfact = EM;
		spreadlimit = inumb(&spreadlimit);
		spreadwarn = 1;
	}
}


int 
chget(int c)
{
	tchar i = 0;

	charf++;
	if (skip(0) || ismot(i = getch()) || cbits(i) == ' ' || cbits(i) == '\n') {
		ch = i;
		return(c);
	} else 
		return(cbits(i));
}


void
casecc(void)
{
	cc = chget('.');
}


void
casec2(void)
{
	c2 = chget('\'');
}


void
casehc(void)
{
	ohc = chget(OHC);
}


void
casetc(void)
{
	tabc = chget(0);
}


void
caselc(void)
{
	dotc = chget(0);
}


void
casehy(void)
{
	register int i;

	hyf = 1;
	if (skip(0))
		return;
	noscale++;
	i = atoi();
	noscale = 0;
	if (nonumb)
		return;
	hyf = max(i, 0);
}


void
casenh(void)
{
	hyf = 0;
}


void
casehlm(void)
{
	int	i;

	if (!skip(0)) {
		noscale++;
		i = atoi();
		noscale = 0;
		if (!nonumb)
			hlm = i;
	} else
		hlm = -1;
}

void
casehcode(void)
{
	tchar	c, d;
	int	k;

	lgf++;
	if (skip(1))
		return;
	do {
		c = getch();
		if (skip(1))
			break;
		d = getch();
		if (c && d && !ismot(c) && !ismot(d)) {
			if ((k = cbits(c)) >= nhcode) {
				hcode = realloc(hcode, (k+1) * sizeof *hcode);
				memset(&hcode[nhcode], 0,
					(k+1-nhcode) * sizeof *hcode);
				nhcode = k+1;
			}
			hcode[k] = cbits(d);
		}
	} while (!skip(0));
}

void
caseshc(void)
{
	shc = skip(0) ? 0 : getch();
}

int 
max(int aa, int bb)
{
	if (aa > bb)
		return(aa);
	else 
		return(bb);
}

int 
min(int aa, int bb)
{
	if (aa < bb)
		return(aa);
	else 
		return(bb);
}


static void
cerj(int dorj)
{
	register int i;

	noscale++;
	skip(0);
	i = max(atoi(), 0);
	if (nonumb)
		i = 1;
	tbreak();
	if (dorj) {
		rj = i;
		ce = 0;
	} else {
		ce = i;
		rj = 0;
	}
	noscale = 0;
}


void
casece(void)
{
	cerj(0);
}


void
caserj(void)
{
	if (xflag)
		cerj(1);
}


void
casein(void)
{
	register int i;

	if (skip(0))
		i = in1;
	else 
		i = max(hnumb(&in), 0);
	tbreak();
	in1 = in;
	in = i;
	if (!nc) {
		un = in;
		setnel();
	}
}


void
casell(void)
{
	register int i;

	if (skip(0))
		i = ll1;
	else 
		i = max(hnumb(&ll), INCH / 10);
	ll1 = ll;
	ll = i;
	setnel();
}


void
caselt(void)
{
	register int i;

	if (skip(0))
		i = lt1;
	else 
		i = max(hnumb(&lt), 0);
	lt1 = lt;
	lt = i;
}


void
caseti(void)
{
	register int i;

	if (skip(1))
		return;
	i = max(hnumb(&in), 0);
	tbreak();
	un1 = i;
	setnel();
}


void
casels(void)
{
	register int i;

	noscale++;
	if (skip(0))
		i = ls1;
	else 
		i = max(inumb(&ls), 1);
	ls1 = ls;
	ls = i;
	noscale = 0;
}


void
casepo(void)
{
	register int i;

	if (skip(0))
		i = po1;
	else 
		i = max(hnumb(&po), 0);
	po1 = po;
	po = i;
#ifndef NROFF
	if (!ascii)
		esc += po - po1;
#endif
}


void
casepl(void)
{
	register int i;

	skip(0);
	if ((i = vnumb(&pl)) == 0)
		pl = defaultpl ? defaultpl : 11 * INCH; /*11in*/
	else 
		pl = i;
	if (numtab[NL].val > pl) {
		numtab[NL].val = pl;
		prwatchn(NL);
	}
}


void
casewh(void)
{
	register int i, j, k;

	lgf++;
	skip(1);
	i = vnumb((int *)0);
	if (nonumb)
		return;
	skip(0);
	j = getrq(1);
	if ((k = findn(i)) != NTRAP) {
		mlist[k] = j;
		return;
	}
	for (k = 0; k < NTRAP; k++)
		if (mlist[k] == 0)
			break;
	if (k == NTRAP) {
		flusho();
		errprint("cannot plant trap.");
		return;
	}
	mlist[k] = j;
	nlist[k] = i;
}


void
casech(void)
{
	register int i, j, k;

	lgf++;
	skip(1);
	if (!(j = getrq(0)))
		return;
	else  {
		for (k = 0; k < NTRAP; k++)
			if (mlist[k] == j)
				break;
	}
	if (k == NTRAP)
		return;
	skip(0);
	i = vnumb((int *)0);
	if (nonumb)
		mlist[k] = 0;
	nlist[k] = i;
}


void
casevpt(void)
{
	if (skip(1))
		return;
	vpt = atoi() != 0;
}


tchar
setolt(void)
{
	storerq(getsn(1));
	return mkxfunc(OLT, 0);
}


int 
findn(int i)
{
	register int k;

	for (k = 0; k < NTRAP; k++)
		if ((nlist[k] == i) && (mlist[k] != 0))
			break;
	return(k);
}


void
casepn(void)
{
	register int i;

	skip(1);
	noscale++;
	i = max(inumb(&numtab[PN].val), 0);
	prwatchn(PN);
	noscale = 0;
	if (!nonumb) {
		npn = i;
		npnflg++;
	}
}


void
casebp(void)
{
	register int i;
	register struct s *savframe;

	if (dip != d)
		return;
	savframe = frame;
	skip(0);
	if ((i = inumb(&numtab[PN].val)) < 0)
		i = 0;
	tbreak();
	if (!nonumb) {
		npn = i;
		npnflg++;
	} else if (dip->nls)
		return;
	eject(savframe);
}


static void
tmtmcwr(int ab, int tmc, int wr, int ep)
{
	const char tmtab[] = {
		'a',000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		'{','}','&',000,'%','c','e',' ',
		'!',000,000,000,000,000,000,'~',
		000
	};
	register int i, j;
	tchar	c;
	char	tmbuf[NTM];

	lgf++;
	copyf++;
	if (skip(0) && ab)
		errprint("User Abort");
	for (i = 0; i < NTM - 5 - mb_cur_max; ) {
		if ((c = getch()) == '\n') {
			tmbuf[i++] = '\n';
			break;
		}
	c:	j = cbits(c);
#if !defined (NROFF) && defined (EUC)
		if (iscopy(c)) {
			int	n;
			if ((n = wctomb(&tmbuf[i], j)) > 0) {
				i += n;
				continue;
			}
		}
#endif	/* !NROFF && EUC */
		if (xflag == 0) {
			tmbuf[i++] = c;
			continue;
		}
		if (ismot(c))
			continue;
		tmbuf[i++] = '\\';
		if (c == (OHC|BLBIT))
			j = ':';
		else if (j >= 0 && j < sizeof tmtab && tmtab[j])
			j = tmtab[j];
		else if (j == ACUTE)
			j = '\'';
		else if (j == GRAVE)
			j = '`';
		else if (j == UNDERLINE)
			j = '_';
		else if (j == MINUS)
			j = '-';
		else
			i--;
		if (j == XFUNC)
			switch (fbits(c)) {
			case CHAR:
				c = charout[sbits(c)].ch;
				goto c;
			default:
				continue;
			}
		tmbuf[i++] = j;
	}
	if (i == NTM - 2)
		tmbuf[i++] = '\n';
	if (tmc)
		i--;
	tmbuf[i] = 0;
	if (ab)	/* truncate output */
		obufp = obuf;	/* should be a function in n2.c */
	if (ep) {
		flusho();
		errprint("%s", tmbuf);
	} else if (wr < 0) {
		flusho();
		fdprintf(stderr, "%s", tmbuf);
	} else if (i)
		write(wr, tmbuf, i);
	copyf--;
	lgf--;
}

void
casetm(int ab)
{
	tmtmcwr(ab, 0, -1, 0);
}

void
casetmc(void)
{
	tmtmcwr(0, 1, -1, 0);
}

void
caseerrprint(void)
{
	tmtmcwr(0, 1, -1, 1);
}

static struct stream {
	char	*name;
	int	fd;
} *streams;
static int	nstreams;

static void
open1(int flags)
{
	int	ns = nstreams;

	lgf++;
	if (skip(1) || !getname() || skip(1))
		return;
	streams = realloc(streams, sizeof *streams * ++nstreams);
	streams[ns].name = malloc(NS);
	strcpy(streams[ns].name, nextf);
	getname();
	if ((streams[ns].fd = open(nextf, flags, 0666)) < 0) {
		errprint("can't open file %s", nextf);
		done(02);
	}
}

void
caseopen(void)
{
	open1(O_WRONLY|O_CREAT|O_TRUNC);
}

void
caseopena(void)
{
	open1(O_WRONLY|O_CREAT|O_APPEND);
}

static int
getstream(const char *name)
{
	int	i;

	for (i = 0; i < nstreams; i++)
		if (strcmp(streams[i].name, name) == 0)
			return i;
	errprint("no such stream %s", name);
	return -1;
}

static void
write1(int writec)
{
	int	i;

	lgf++;
	if (skip(1) || !getname())
		return;
	if ((i = getstream(nextf)) < 0)
		return;
	tmtmcwr(0, writec, streams[i].fd, 0);
}

void
casewrite(void)
{
	write1(0);
}

void
casewritec(void)
{
	write1(1);
}

void
caseclose(void)
{
	int	i;

	lgf++;
	if (skip(1) || !getname())
		return;
	if ((i = getstream(nextf)) < 0)
		return;
	free(streams[i].name);
	memmove(&streams[i], &streams[i+1], (nstreams-i-1) * sizeof *streams);
	nstreams--;
}


void
casesp(int a)
{
	register int i, j, savlss;

	tbreak();
	if (dip->nls || trap)
		return;
	i = findt1();
	if (!a) {
		skip(0);
		j = vnumb((int *)0);
		if (nonumb)
			j = lss;
	} else 
		j = a;
	if (j == 0)
		return;
	if (i < j)
		j = i;
	savlss = lss;
	if (dip != d)
		i = dip->dnl; 
	else 
		i = numtab[NL].val;
	if ((i + j) < 0)
		j = -i;
	lss = j;
	newline(0);
	lss = savlss;
}


void
casebrp(void)
{
	if (nc) {
		spread++;
		flushi();
		pendt++;
		text();
	} else
		tbreak();
}


void
caseblm(void)
{
	if (!skip(0))
		blmac = getrq(1);
	else
		blmac = 0;
}


void
casert(void)
{
	register int a, *p;

	skip(0);
	if (dip != d)
		p = &dip->dnl; 
	else 
		p = &numtab[NL].val;
	a = vnumb(p);
	if (nonumb)
		a = dip->mkline;
	if ((a < 0) || (a >= *p))
		return;
	nb++;
	casesp(a - *p);
}


void
caseem(void)
{
	lgf++;
	skip(1);
	em = getrq(1);
}


void
casefl(void)
{
	tbreak();
	flusho();
}


static struct evnames {
	int	number;
	char	*name;
} *evnames;
static struct env	*evp;
static int	*evlist;
static int	evi;
static int	evlsz;
static int	Nev = NEV;

static struct env *
findev(int *number, char *name)
{
	int	i;

	if (*number < 0)
		return &evp[-1 - (*number)];
	else if (name) {
		for (i = 0; i < Nev-NEV; i++)
			if (evnames[i].name != NULL &&
					strcmp(evnames[i].name, name) == 0) {
				*number = -1 - i;
				return &evp[i];
			}
		*number = -1 - i;
		return NULL;
	} else if (*number >= NEV) {
		for (i = 0; i < Nev-NEV; i++)
			if (evnames[i].name == NULL &&
					evnames[i].number == *number)
				return &evp[i];
		*number = -1 - i;
		return NULL;
	} else {
		extern tchar *corebuf;
		return &((struct env *)corebuf)[*number];
	}
}

static int
getev(int *nxevp, char **namep)
{
	char	*name = NULL;
	int nxev = 0;
	char	c;
	int	i = 0, sz = 0;

	*namep = NULL;
	*nxevp = 0;
	if (skip(0))
		return 0;
	c = cbits(ch);
	if (xflag == 0 || isdigit(c) || c == '(') {
		noscale++;
		nxev = atoi();
		noscale = 0;
		if (nonumb) {
			flushi();
			return 0;
		}
	} else {
		do {
			c = getach();
			if (i >= sz)
				name = realloc(name, (sz += 8) * sizeof *name);
			name[i++] = c;
		} while (c);
	}
	flushi();
	*namep = name;
	*nxevp = nxev;
	return 1;
}

void
caseev(void)
{
	char	*name;
	int nxev;
	struct env	*np, *op;

	if (getev(&nxev, &name) == 0) {
		if (evi == 0)
			return;
		nxev =  evlist[--evi];
		goto e1;
	}
	if (xflag == 0 && ((nxev >= NEV) || (nxev < 0) || (evi >= EVLSZ)))
		goto cannot;
	if (evi >= evlsz) {
		evlsz = evi + 1;
		if ((evlist = realloc(evlist, evlsz * sizeof *evlist)) == NULL)
			goto cannot;
	}
	if (name && findev(&nxev, name) == NULL || nxev >= Nev) {
		if ((evp = realloc(evp, (Nev-NEV+1) * sizeof *evp)) == NULL ||
				(evnames = realloc(evnames,
				   (Nev-NEV+1) * sizeof *evnames)) == NULL)
			goto cannot;
		evnames[Nev-NEV].number = nxev;
		evnames[Nev-NEV].name = name;
		evp[Nev-NEV] = initenv;
		Nev++;
	}
	if (name == NULL && nxev < 0) {
		flusho();
	cannot:	errprint("cannot do ev.");
		if (error)
			done2(040);
		else 
			edone(040);
		return;
	}
	evlist[evi++] = ev;
e1:
	if (ev == nxev)
		return;
	if ((np = findev(&nxev, name)) == NULL ||
			(op = findev(&ev, NULL)) == NULL)
		goto cannot;
	*op = env;
	env = *np;
	ev = nxev;
	if (evname == NULL)
		if (name)
			evname = name;
		else {
			evname = malloc(20);
			roff_sprintf(evname, "%d", ev);
		}
}

void
caseevc(void)
{
	char	*name;
	int	nxev;
	struct env	*ep;

	if (getev(&nxev, &name) == 0 || (ep = findev(&nxev, name)) == NULL)
		return;
	free(env._hcode);
	free(env._line);
	free(env._word);
	evc(&env, ep);
}

void
evc(struct env *dp, struct env *sp)
{
	char	*name;

	if (dp != sp) {
		name = dp->_evname;
		memcpy(dp, sp, sizeof *dp);
		dp->_hcode = malloc(dp->_nhcode * sizeof *dp->_hcode);
		memcpy(dp->_hcode, sp->_hcode, dp->_nhcode * sizeof *dp->_hcode);
		dp->_evname = name;
	}
	dp->_pendnf = 0;
	dp->_pendw = 0;
	dp->_pendt = 0;
	dp->_wch = 0;
	dp->_wne = 0;
	dp->_wdstart = 0;
	dp->_wdend = 0;
	dp->_lnsize = 0;
	dp->_line = NULL;
	dp->_linep = NULL;
	dp->_wdsize = 0;
	dp->_word = 0;
	dp->_wordp = 0;
	dp->_spflg = 0;
	dp->_ce = 0;
	dp->_rj = 0;
	dp->_nn = 0;
	dp->_ndf = 0;
	dp->_nms = 0;
	dp->_ni = 0;
	dp->_ul = 0;
	dp->_cu = 0;
	dp->_it = 0;
	dp->_itc = 0;
	dp->_itmac = 0;
	dp->_pendnf = 0;
	dp->_nc = 0;
	dp->_un = 0;
	dp->_un1 = -1;
	dp->_nwd = 0;
	dp->_hyoff = 0;
	dp->_nb = 0;
	dp->_spread = 0;
	dp->_lnmod = 0;
	dp->_hlc = 0;
	dp->_cht = 0;
	dp->_cdp = 0;
	dp->_maxcht = 0;
	dp->_maxcdp = 0;
	setnel();
}

void
evcline(struct env *dp, struct env *sp)
{
	if (dp == sp)
		return;
#ifndef	NROFF
	dp->_lspnc = sp->_lspnc;
	dp->_lsplow = sp->_lsplow;
	dp->_lsphigh = sp->_lsphigh;
	dp->_lspcur = sp->_lspcur;
	dp->_lsplast = sp->_lsplast;
	dp->_lshwid = sp->_lshwid;
	dp->_lshlow = sp->_lshlow;
	dp->_lshhigh = sp->_lshhigh;
	dp->_lshcur = sp->_lshcur;
#endif
	dp->_fldcnt = sp->_fldcnt;
	dp->_hyoff = sp->_hyoff;
	dp->_hlc = sp->_hlc;
	dp->_adflg = sp->_adflg;
	dp->_adspc = sp->_adspc;
	dp->_wne = sp->_wne;
	dp->_ne = sp->_ne;
	dp->_nc = sp->_nc;
	dp->_nwd = sp->_nwd;
	dp->_un = sp->_un;
	dp->_wch = sp->_wch;
	dp->_rhang = sp->_rhang;
	dp->_cht = sp->_cht;
	dp->_cdp = sp->_cdp;
	dp->_maxcht = sp->_maxcht;
	dp->_maxcdp = sp->_maxcdp;
	if (icf == 0)
		dp->_ic = sp->_ic;
	memcpy(dp->_hyptr, sp->_hyptr, NHYP * sizeof *sp->_hyptr);
	dp->_line = malloc((dp->_lnsize = sp->_lnsize) * sizeof *dp->_line);
	memcpy(dp->_line, sp->_line, sp->_lnsize * sizeof *sp->_line);
	dp->_word = malloc((dp->_wdsize = sp->_wdsize) * sizeof *dp->_word);
	memcpy(dp->_word, sp->_word, sp->_wdsize * sizeof *sp->_word);
	dp->_linep = sp->_linep + (dp->_line - sp->_line);
	dp->_wordp = sp->_wordp + (dp->_word - sp->_word);
	dp->_wdend = sp->_wdend + (dp->_word - sp->_word);
	dp->_wdstart = sp->_wdstart + (dp->_word - sp->_word);
}

void
caseel(void)
{
	if (--ifx < 0) {
		ifx = 0;
		if (NIF == 0)
			growiflist();
		iflist[0] = 0;
		if (warn & WARN_EL)
			errprint(".el without matching .ie");
	}
	caseif(2);
}


void
caseie(void)
{
	if (ifx >= NIF)
		growiflist();
	caseif(1);
	ifx++;
}

int	tryglf;

void
caseif(int x)
{
	extern int falsef;
	register int notflag, true;
	tchar i, j;
	enum warn w = warn;
	int	flt = 0;

	if (x == 3)
		goto i2;
	if (x == 2) {
		notflag = 0;
		true = iflist ? iflist[ifx] : 0;
		goto i1;
	}
	true = 0;
	skip(1);
	if ((cbits(i = getch())) == '!') {
		notflag = 1;
		if (xflag && (cbits(i = getch())) == 'f')
			flt = 1;
		else
			ch = i;
	} else if (xflag && cbits(i) == 'f') {
		flt = 1;
		notflag = 0;
	} else {
		notflag = 0;
		ch = i;
	}
	if (flt)
		i = atof0() > 0;
	else
		i = (int)atoi0();
	if (!nonumb) {
		if (i > 0)
			true++;
		goto i1;
	}
	i = getch();
	switch (cbits(i)) {
	case 'e':
		if (!(numtab[PN].val & 01))
			true++;
		break;
	case 'o':
		if (numtab[PN].val & 01)
			true++;
		break;
#ifdef NROFF
	case 'n':
		true++;
	case 't':
#endif
#ifndef NROFF
	case 't':
		true++;
	case 'n':
#endif
		break;
	case 'c':
		if (xflag == 0)
			goto dfl;
		warn &= ~WARN_CHAR;
		tryglf++;
		if (!skip(1)) {
			j = getch();
#if defined (NROFF) && defined (EUC)
			if (multi_locale && j & MBMASK) {
				while ((j & MBMASK) != LASTOFMB)
					j = getch();
				true++;
			} else
#endif	/* NROFF && EUC */
			true = !ismot(j) && cbits(j) && cbits(j) != ' ';
		}
		tryglf--;
		warn = w;
		break;
	case 'r':
	case 'd':
		if (xflag == 0)
			goto dfl;
		warn &= ~(WARN_MAC|WARN_SPACE|WARN_REG);
		if (!skip(1)) {
			j = getrq(2);
			true = (cbits(i) == 'r' ? usedr(j) : findmn(j)) >= 0;
		}
		warn = w;
		break;
	case ' ':
		break;
	default:
	dfl:	true = cmpstr(i);
	}
i1:
	true ^= notflag;
	if (x == 1) {
		if (ifx >= NIF)
			growiflist();
		iflist[ifx] = !true;
	}
	if (true) {
		if (frame->loopf & LOOP_EVAL) {
			if (nonumb)
				goto i3;
			frame->loopf &= ~LOOP_EVAL;
			frame->loopf |= LOOP_NEXT;
		}
i2:
		while ((cbits(i = getch())) == ' ')
			;
		if (cbits(i) == LEFT)
			goto i2;
		ch = i;
		nflush++;
	} else {
i3:
		if (frame->loopf & LOOP_EVAL)
			frame->loopf = LOOP_FREE;
		copyf++;
		falsef++;
		eatblk(0);
		copyf--;
		falsef--;
	}
}

void
casenop(void)
{
	caseif(3);
}

void
casereturn(void)
{
	flushi();
	nflush++;
	while (frame->loopf) {
		frame->loopf = LOOP_FREE;
		popi();
	}
	popi();
}

void
casewhile(void)
{
	tchar	c;
	int	k, level, nl;
	filep	newip;

	if (dip != d)
		wbfl();
	if ((nextb = alloc()) == 0) {
		errprint("out of space");
		edone(04);
		return;
	}
	newip = offset = nextb;
	wbf(mkxfunc(CC, 0));
	wbf(XFUNC);	/* caseif */
	wbf(' ');
	copyf++, clonef++;
	nl = level = 0;
	do {
		nlflg = 0;
		k = cbits(c = getch());
		switch (k) {
		case LEFT:
			level++;
			break;
		case RIGHT:
			level--;
			break;
		}
		wbf(c);
	} while (!nlflg || level > 0);
	if (level < 0 && warn & WARN_DELIM)
		errprint("%d excess delimiter(s)", -level);
	wbt(0);
	copyf--, clonef--;
	pushi(newip, LOOP);
	offset = dip->op;
}

void
casebreak(void)
{
	casecontinue(1);
}

void
casecontinue(int _break)
{
	int	i, j;
	struct s	*s;

	if (skip(0))
		i = 1;
	else {
		noscale++;
		i = atoi();
		noscale--;
	}
	j = 0;
	for (s = frame; s != stk; s = s->pframe)
		if (s->loopf && ++j >= i)
			break;
	if (j != i) {
		if (warn & WARN_RANGE)
			errprint("%d loop levels requested but only %d current",
					i, j);
		return;
	}
	flushi();
	nflush++;
	while (i > 1 || _break && i > 0) {
		if (frame->loopf) {
			frame->loopf = LOOP_FREE;
			i--;
		}
		popi();
	}
	if (i == 1) {
		while (frame->loopf == 0)
			popi();
		popi();
	}
}

void
eatblk(int inblk)
{	register int cnt, i;
	tchar	ii;

	cnt = 0;
	do {
		if (ch)	{
			i = cbits(ii = ch);
			ch = 0;
		} else
			i = cbits(ii = getch0());
		if (i == ESC)
			cnt++;
		else {
			if (cnt == 1)
				switch (i) {
				case '{':  i = LEFT; break;
				case '}':  i = RIGHT; break;
				case '\n': i = 'x'; break;
				}
			cnt = 0;
		}
		if (i == LEFT) eatblk(1);
	} while ((!inblk && (i != '\n')) || (inblk && (i != RIGHT)));
	if (i == '\n') {
		nlflg++;
		tailflg = istail(ii);
	}
}


int 
cmpstr(tchar c)
{
	register int j, delim;
	register tchar i;
	register int val;
	int savapts, savapts1, savfont, savfont1, savpts, savpts1;
	tchar string[1280];
	register tchar *sp;

	if (ismot(c))
		return(0);
	delim = cbits(c);
	savapts = apts;
	savapts1 = apts1;
	savfont = font;
	savfont1 = font1;
	savpts = pts;
	savpts1 = pts1;
	sp = string;
	while ((j = cbits(i = getch()))!=delim && j!='\n' && sp<&string[1280-1])
		*sp++ = i;
	if (j != delim)
		nodelim(delim);
	if (sp >= string + 1280) {
		errprint("too-long string compare.");
		edone(0100);
	}
	if (nlflg) {
		val = sp==string;
		goto rtn;
	}
	*sp++ = 0;
	apts = savapts;
	apts1 = savapts1;
	font = savfont;
	font1 = savfont1;
	pts = savpts;
	pts1 = savpts1;
	mchbits();
	val = 1;
	sp = string;
	while ((j = cbits(i = getch())) != delim && j != '\n') {
		if (*sp != i) {
			eat(delim);
			val = 0;
			goto rtn;
		}
		sp++;
	}
	if (j != delim)
		nodelim(delim);
	if (*sp)
		val = 0;
rtn:
	apts = savapts;
	apts1 = savapts1;
	font = savfont;
	font1 = savfont1;
	pts = savpts;
	pts1 = savpts1;
	mchbits();
	return(val);
}


void
caserd(void)
{

	lgf++;
	skip(0);
	getname();
	if (!iflg) {
		if (quiet) {
#ifdef	NROFF
			echo_off();
			flusho();
#endif	/* NROFF */
			fdprintf(stderr, "\007"); /*bell*/
		} else {
			if (nextf[0]) {
				fdprintf(stderr, "%s:", nextf);
			} else {
				fdprintf(stderr, "\007"); /*bell*/
			}
		}
	}
	collect();
	tty++;
	pushi(XBLIST*BLK, PAIR('r','d'));
}


int 
rdtty(void)
{
	char	onechar;
#if defined (EUC) && defined (NROFF)
	int	i, n, col_index;
#endif /* EUC && NROFF */

	onechar = 0;
	if (read(0, &onechar, 1) == 1) {
		if (onechar == '\n')
			tty++;
		else 
			tty = 1;
#if !defined (EUC) || !defined (NROFF)
		if (tty != 3)
			return(onechar);
#else	/* EUC && NROFF */
		if (tty != 3) {
			if (!multi_locale)
				return(onechar);
			i = onechar & 0377;
			*mbbuf1p++ = i;
			*mbbuf1p = 0;
			if ((*mbbuf1&~(wchar_t)0177) == 0) {
				twc = *mbbuf1;
				i |= (BYTE_CHR);
				setcsbits(i, 0);
				twc = 0;
				mbbuf1p = mbbuf1;
			}
			else if ((n = mbtowc(&twc, mbbuf1, mb_cur_max)) <= 0) {
				if (mbbuf1p >= mbbuf1 + mb_cur_max) {
					i &= ~(MBMASK | CSMASK);
					twc = 0;
					mbbuf1p = mbbuf1;
					*mbbuf1p = 0;
				} else {
					i |= (MIDDLEOFMB);
				}
			} else {
				if (n > 1)
					i |= (LASTOFMB);
				else
					i |= (BYTE_CHR);
				if ((twc & ~(wchar_t)0177) == 0) {
					col_index = 0;
				} else {
					if ((col_index = wcwidth(twc)) < 0)
						col_index = 0;
				}
				setcsbits(i, col_index);
				twc = 0;
				mbbuf1p = mbbuf1;
			}
			return(i);
		}
#endif /* EUC && NROFF */
	}
	popi();
	tty = 0;
#ifdef	NROFF
	if (quiet)
		echo_on();
#endif	/* NROFF */
	return(0);
}


void
caseec(void)
{
	eschar = chget('\\');
}


void
caseeo(void)
{
	eschar = 0;
}


void
caseecs(void)
{
	ecs = eschar;
}


void
caseecr(void)
{
	eschar = ecs;
}


void
caseta(void)
{
	register int i;

	tabtab[0] = nonumb = 0;
	for (i = 0; ((i < (NTAB - 1)) && !nonumb); i++) {
		if (skip(0))
			break;
		tabtab[i] = max(hnumb(&tabtab[max(i-1,0)]), 0) & TABMASK;
		if (!nonumb) 
			switch (cbits(ch)) {
			case 'C':
				tabtab[i] |= CTAB;
				break;
			case 'R':
				tabtab[i] |= RTAB;
				break;
			default: /*includes L*/
				break;
			}
		nonumb = ch = 0;
	}
	tabtab[i] = 0;
}


void
casene(void)
{
	register int i, j;

	skip(0);
	i = vnumb((int *)0);
	if (nonumb)
		i = lss;
	if (i > (j = findt1())) {
		i = lss;
		lss = j;
		dip->nls = 0;
		newline(0);
		lss = i;
	}
}


void
casetr(void)
{
	register int i, j;
	tchar k;

	lgf++;
	skip(1);
	while ((i = cbits(k=getch())) != '\n') {
		if (ismot(k))
			return;
		if (ismot(k = getch()))
			return;
		if ((j = cbits(k)) == '\n')
			j = ' ';
		trtab[i] = j;
	}
}


void
casecu(void)
{
	cu++;
	caseul();
}


void
caseul(void)
{
	register int i;

	noscale++;
	if (skip(0))
		i = 1;
	else 
		i = atoi();
	if (ul && (i == 0)) {
		font = sfont;
		ul = cu = 0;
	}
	if (i) {
		if (!ul) {
			sfont = font;
			font = ulfont;
		}
		ul = i;
	}
	noscale = 0;
	mchbits();
}


void
caseuf(void)
{
	register int i, j;
	extern int findft(int, int);

	if (skip(0) || !(i = getrq(2)) || i == 'S' || (j = findft(i, 1))  == -1)
		ulfont = ULFONT; /*default underline position*/
	else 
		ulfont = j;
#ifdef NROFF
	if (ulfont == FT)
		ulfont = ULFONT;
#endif
}


void
caseit(int cflag)
{
	register int i;

	lgf++;
	it = itc = itmac = 0;
	noscale++;
	skip(0);
	i = atoi();
	skip(0);
	if (!nonumb && (itmac = getrq(1))) {
		it = i;
		itc = cflag;
	}
	noscale = 0;
}


void
caseitc(void)
{
	caseit(1);
}


void
casemc(void)
{
	register int i;

	if (icf > 1)
		ic = 0;
	icf = 0;
	if (skip(0))
		return;
	ic = getch();
	icf = 1;
	skip(0);
	i = max(hnumb((int *)0), 0);
	if (!nonumb)
		ics = i;
}


static void
propchar(int *tp)
{
	int	c, *tpp;
	tchar	i;

	if (skip(0)) {
		*tp = IMP;
		return;
	}
	tpp = tp;
	do {
		while (!ismot(c = cbits(i = getch())) &&
				c != ' ' && c != '\n')
			if (tpp < &tp[NSENT])
				*tpp++ = c;
	} while (!skip(0));
}

void
casesentchar(void)
{
	propchar(sentch);
}

void
casetranschar(void)
{
	propchar(transch);
}

void
casebreakchar(void)
{
	propchar(breakch);
}

void
casenhychar(void)
{
	propchar(nhych);
}

void
casemk(void)
{
	register int i, j, k;

	if (dip != d)
		j = dip->dnl; 
	else 
		j = numtab[NL].val;
	if (skip(0)) {
		dip->mkline = j;
		return;
	}
	if ((i = getrq(1)) == 0)
		return;
	numtab[k = findr(i)].val = j;
	prwatchn(k);
}


void
casesv(void)
{
	register int i;

	skip(0);
	if ((i = vnumb((int *)0)) < 0)
		return;
	if (nonumb)
		i = 1;
	sv += i;
	caseos();
}


void
caseos(void)
{
	register int savlss;

	if (sv <= findt1()) {
		savlss = lss;
		lss = sv;
		newline(0);
		lss = savlss;
		sv = 0;
	}
}


void
casenm(void)
{
	register int i;

	lnmod = nn = 0;
	if (skip(0))
		return;
	lnmod++;
	noscale++;
	i = inumb(&numtab[LN].val);
	if (!nonumb)
		numtab[LN].val = max(i, 0);
	prwatchn(LN);
	getnm(&ndf, 1);
	getnm(&nms, 0);
	getnm(&ni, 0);
	noscale = 0;
	nmbits = chbits;
}


void
getnm(int *p, int min)
{
	register int i;

	eat(' ');
	if (skip(0))
		return;
	i = atoi0();
	if (nonumb)
		return;
	*p = max(i, min);
}


void
casenn(void)
{
	noscale++;
	skip(0);
	nn = max(atoi(), 1);
	noscale = 0;
}


void
caseab(void)
{
	casetm(1);
	done3(0);
}


#ifdef	NROFF
/*
 * The following routines are concerned with setting terminal options.
 *	The manner of doing this differs between research/Berkeley systems
 *	and UNIX System V systems (i.e. DOCUMENTER'S WORKBENCH)
 *	The distinction is controlled by the #define'd variable USG,
 *	which must be set by System V users.
 */


#ifdef	USG
#include <termios.h>
#define	ECHO_USG (ECHO | ECHOE | ECHOK | ECHONL)
struct termios	ttys;
#else
#include <sgtty.h>
struct	sgttyb	ttys[2];
#endif	/* USG */

int	ttysave[2] = {-1, -1};

void
save_tty(void)			/*save any tty settings that may be changed*/
{

#ifdef	USG
	if (tcgetattr(0, &ttys) >= 0)
		ttysave[0] = ttys.c_lflag;
#else
	if (gtty(0, &ttys[0]) >= 0)
		ttysave[0] = ttys[0].sg_flags;
	if (gtty(1, &ttys[1]) >= 0)
		ttysave[1] = ttys[1].sg_flags;
#endif	/* USG */

}


void 
restore_tty (void)			/*restore tty settings from beginning*/
{

	if (ttysave[0] != -1) {
#ifdef	USG
		ttys.c_lflag = ttysave[0];
		tcsetattr(0, TCSADRAIN, &ttys);
#else
		ttys[0].sg_flags = ttysave[0];
		stty(0, &ttys[0]);
	}
	if (ttysave[1] != -1) {
		ttys[1].sg_flags = ttysave[1];
		stty(1, &ttys[1]);
#endif	/* USG */
	}
}


void 
set_tty (void)			/*this replaces the use of bset and breset*/
{

#ifndef	USG			/*for research/BSD only, reset CRMOD*/
	if (ttysave[1] == -1)
		save_tty();
	if (ttysave[1] != -1) {
		ttys[1].sg_flags &= ~CRMOD;
		stty(1, &ttys[1]);
	}
#endif	/* USG */

}


void 
echo_off (void)			/*turn off ECHO for .rd in "-q" mode*/
{
	if (ttysave[0] == -1)
		return;

#ifdef	USG
	ttys.c_lflag &= ~ECHO_USG;
	tcsetattr(0, TCSADRAIN, &ttys);
#else
	ttys[0].sg_flags &= ~ECHO;
	stty(0, &ttys[0]);
#endif	/* USG */

}


void 
echo_on (void)			/*restore ECHO after .rd in "-q" mode*/
{
	if (ttysave[0] == -1)
		return;

#ifdef	USG
	ttys.c_lflag |= ECHO_USG;
	tcsetattr(0, TCSADRAIN, &ttys);
#else
	ttys[0].sg_flags |= ECHO;
	stty(0, &ttys[0]);
#endif	/* USG */

}
#endif	/* NROFF */
