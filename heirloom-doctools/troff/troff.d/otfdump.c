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
 * Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)otfdump.c	1.5 (gritter) 10/2/05
 */

static enum show {
	SHOW_CHARS		= 001,
	SHOW_KERNPAIRS		= 002,
	SHOW_SUBSTITUTIONS	= 004,
	SHOW_NAME		= 010
} show;

#include <stdarg.h>

static void	print(enum show, const char *, ...);

#define	DUMP
#include <stdio.h>
#undef	stderr
#define	stderr	troff_stderr
#include "otf.c"
#include "afm.c"
#undef	stderr
#include "dpost.d/getopt.c"
#include "../version.c"

#include <libgen.h>

static const char	*progname;
struct dev	dev;
char	*chname;
short	*chtab;
int	nchtab;

static int	prname;

static void
print(enum show s, const char *fmt, ...)
{
	va_list	ap;

	if (show & s) {
		if (prname)
			printf("%s: ", filename);
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
		putchar('\n');
	}
}

void
verrprint(const char *s, va_list ap)
{
	fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, s, ap);
	putc('\n', stderr);
}

void
errprint(const char *s, ...)
{
	va_list	ap;

	va_start(ap, s);
	verrprint(s, ap);
	va_end(ap);
}

static void
devinit(void)
{
	dev.res = 72000;
	dev.hor = dev.vert = 1;
	dev.unitwidth = 1;
	dev.afmfonts = 1;
}

static void
usage(void)
{
	errprint("usage: %s [-ckns] font ...", progname);
	exit(2);
}

static int
dump(const char *name)
{
	struct afmtab	A;
	struct stat	st;
	FILE	*fp;

	if ((fp = fopen(filename = name, "r")) == NULL) {
		errprint("%s: cannot open", filename);
		return 1;
	}
	memset(&A, 0, sizeof A);
	a = &A;
	a->file = a->path = (char *)filename;
	a->base = malloc(strlen(filename) + 1);
	strcpy(a->base, filename);
	a->base = basename(a->base);
	if (fstat(fileno(fp), &st) < 0) {
		errprint("%s: cannot stat", filename);
		return 1;
	}
	size = st.st_size;
	contents = malloc(size);
	if (fread(contents, 1, size, fp) != size) {
		errprint("%s: cannot read", filename);
		return 1;
	}
	fclose(fp);
	return otfget(a, contents, size) != 0;
}

int
main(int argc, char **argv)
{
	int	i, e = 0;

	progname = basename(argv[0]);
	devinit();
	while ((i = getopt(argc, argv, "ckns")) != EOF) {
		switch (i) {
		case 'c':
			show |= SHOW_CHARS;
			break;
		case 'k':
			show |= SHOW_KERNPAIRS;
			break;
		case 'n':
			show |= SHOW_NAME;
			break;
		case 's':
			show |= SHOW_SUBSTITUTIONS;
			break;
		default:
			usage();
		}
	}
	if (show == 0)
		show = 0xFFFFFFFF;
	if (argc < optind + 1)
		usage();
	prname = argc > optind + 1;
	for (i = optind; i < argc; i++)
		e |= dump(argv[i]);
	return e;
}

void
afmaddchar(struct afmtab *a, int C, int tp, int cl, int WX, int B[4], char *N,
		int isS, int isS1, int gid)
{
	print(SHOW_CHARS, "char %s width %d", N, unitconv(WX));
}

void
afmalloc(struct afmtab *a, int n)
{
}

struct kernpair *
afmkernlook(struct afmtab *a, int ch1, int ch2)
{
	static struct kernpair	k;

	return &k;
}

static void
kernpair(int first, int second, int x)
{
	char	*s1, *s2;

	if (x) {
		nkerntmp++;
		s1 = GID2SID(first);
		s2 = GID2SID(second);
		if (s1 && s2)
			print(SHOW_KERNPAIRS, "kernpair %s %s width %d",
				s1, s2, unitconv(x));
	}
}

static void
kernfinish(void)
{
}
