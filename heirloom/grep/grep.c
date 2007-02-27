/*
 * grep - search a file for a pattern
 *
 * Gunnar Ritter, Freiburg i. Br., Germany, April 2001.
 */
/*
 * Copyright (c) 2003 Gunnar Ritter
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

/*	Sccsid @(#)grep.c	1.44 (gritter) 12/2/04>	*/

/*
 * Code common to all grep flavors.
 */

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<libgen.h>
#include	<locale.h>
#include	<limits.h>
#include	<ctype.h>
#include	<dirent.h>

#include	"grep.h"
#include	"alloc.h"

/*
 * Generic flags and the like.
 */
int		Eflag;			/* use EREs */
int		Fflag;			/* use fixed strings */
int		bflag;			/* print buffer count */
int		cflag;			/* print count only */
int		fflag;			/* had pattern file argument */
int		hflag;			/* do not print filenames */
int		iflag;			/* ignore case */
int		lflag;			/* print filenames only */
int		nflag;			/* print line numbers */
int		qflag;			/* no output at all */
int		rflag;			/* operate recursively */
int		sflag;			/* avoid error messages */
int		vflag;			/* inverse selection */
int		xflag;			/* match entire line */
int		mb_cur_max;		/* avoid multiple calls to MB_CUR_MAX */
unsigned	status = 1;		/* exit status */
off_t		lmatch;			/* count of line matches */
off_t		lineno;			/* current line number */
char		*progname;		/* argv[0] to main() */
char		*filename;		/* name of current file */
char		*options;		/* for getopt() */
void		(*build)(void);		/* compile function */
int		(*match)(const char *, size_t); /* comparison function */
int		(*range)(struct iblok *, char *); /* grep range of lines */

/*
 * Regexp variables.
 */
struct expr	*e0;			/* start of expression list */
enum matchflags	matchflags;		/* matcher flags */

/*
 * Lower-case a character string.
 */
void
loconv(register char *dst, register char *src, size_t sz)
{
	if (mbcode) {
		char	mb[MB_LEN_MAX];
		wchar_t wc;
		int len, i;

		while (sz > 0) {
			if ((*src & 0200) == 0) {
				*dst++ = tolower(*src);
				src++;
				sz--;
			} else if ((len = mbtowc(&wc, src, sz)) <= 0 ||
					len > sz) {
				*dst++ = *src++;
				sz--;
			} else {
				wc = towlower(wc);
				if (len >= mb_cur_max) {
					if (wctomb(dst, wc) == len) {
						dst += len;
						src += len;
						sz -= len;
					} else {
						*dst++ = *src++;
						sz--;
					}
				} else {
					if (wctomb(mb, wc) == len) {
						sz -= len;
						src += len;
						for (i = 0; i < len; i++)
							*dst++ = mb[i];
					} else {
						*dst++ = *src++;
						sz--;
					}
				}
			}
		}
	} else
		while (sz--) {
			*dst++ = tolower(*src & 0377);
			src++;
		}
}

/*
 * Report a matching line.
 */
void
report(const char *line, size_t llen, off_t bcnt, int addnl)
{
	if (filename && !hflag)
		printf("%s:", filename);
#ifdef	LONGLONG
	if (bflag)
		printf("%llu:", (long long)bcnt);
	if (nflag)
		printf("%llu:", (long long)lineno);
#else	/* !LONGLONG */
	if (bflag)
		printf("%lu:", (long)bcnt);
	if (nflag)
		printf("%lu:", (long)lineno);
#endif	/* !LONGLONG */
	if (line && llen)
		fwrite(line, sizeof *line, llen, stdout);
	if (addnl)
		putchar('\n');
}

/*
 * Check line for match. If necessary, the line gets NUL-terminated (so
 * its address range must be writable then). When ignoring character case,
 * a lower-case-only copy of the line is made instead. If a match is found,
 * statistics are printed. Returns 1 if main loop shall terminate, 0 else.
 */
static int
matchline(char *line, size_t sz, int putnl, struct iblok *ip)
{
	int terminate = 0;
	char lbuf[512], *abuf = NULL, *cline = line;

	if (iflag && (matchflags & MF_LOCONV)) {
		if (sz >= sizeof lbuf - 1) {
			abuf = smalloc(sz + 1);
			cline = abuf;
		} else
			cline = lbuf;
		loconv(cline, line, sz);
		cline[sz] = '\0';
	} else if (matchflags & MF_NULTERM)
		cline[sz] = '\0';
	lineno++;
	if (match(cline, sz) ^ vflag) {
		lmatch++;
		if (qflag == 0) {
			if (status == 1)
				status = 0;
			if (lflag) {
				puts(filename ? filename : stdinmsg);
			} else if (!cflag)
				report(line, sz, (ib_offs(ip)-1) / BSZ, putnl);
		} else
			exit(0);
		if (qflag || lflag)
			terminate = 1;
	}
	if (abuf)
		free(abuf);
	return terminate;
}

/*
 * Check all lines within ip->ib_cur and last which contains the last
 * newline. If the main loop shall terminate, 1 is returned.
 */
static int
gn_range(struct iblok *ip, char *last)
{
	char *nl;

	while ((nl = memchr(ip->ib_cur, '\n', last + 1 - ip->ib_cur)) != NULL) {
		if (matchline(ip->ib_cur, nl - ip->ib_cur,  1, ip))
			return 1;
		if (nl == last)
			return 0;
		ip->ib_cur = nl + 1;
	}
	return 0;
}

/*
 * Main grep routine. The line buffer herein is only used for overlaps
 * between file buffer fills.
 */
static int
grep(struct iblok *ip)
{
	char *line = NULL;		/* line buffer */
	register char *lastnl;		/* last newline in file buffer */
	size_t sz = 0;			/* length of line in line buffer */
	char *cp;
	int hadnl;			/* lastnl points to newline char */
	int	oom = 0;		/* got out of memory */

	lineno = lmatch = 0;
	if (ib_read(ip) == EOF)
		goto endgrep;
	ip->ib_cur--;
	for (;;) {
		for (lastnl = ip->ib_end - 1;
				*lastnl != '\n' && lastnl > ip->ib_cur;
				lastnl--);
		if (hadnl = (ip->ib_cur < ip->ib_end && *lastnl == '\n'))
			if (range(ip, lastnl))
				break;
		if (lastnl < ip->ib_end - 1) {
			/*
			 * Copy the partial line from file buffer to line
			 * buffer. Allocate enough space to zero-terminate
			 * the line later if necessary.
			 */
			sz = ip->ib_end - lastnl - hadnl;
			line = smalloc(sz + 1);
			memcpy(line, lastnl + hadnl, sz);
			ip->ib_cur = lastnl + hadnl;
		} else
			line = NULL;
nextbuf:
		if (ib_read(ip) == EOF) {
			if (line) {
				matchline(line, sz, sus, ip);
				free(line);
				line = NULL;
				sz = 0;
			}
			break;
		}
		ip->ib_cur--;
		if (line) {
			/*
			 * Append the partial line at the beginning of the
			 * file buffer to the line buffer.
			 */
			size_t oldsz = sz;
			if ((cp = memchr(ip->ib_cur, '\n',
					ip->ib_end - ip->ib_cur)) == NULL) {
				char	*nline;
				/*
				 * Ugh. This is really a huge line. Store the
				 * entire file buffer in the line buffer and
				 * read the next part of the file.
				 */
				sz += ip->ib_end - ip->ib_cur;
				if ((nline = realloc(line, sz + 1)) == NULL) {
					sz = oldsz;
					cp = &ip->ib_end[-1];
					oom++;
				} else {
					line = nline;
					memcpy(line + oldsz, ip->ib_cur,
						ip->ib_end - ip->ib_cur);
					goto nextbuf;
				}
			}
			if ((sz = cp - ip->ib_cur) > 0) {
				char	*nline;
				sz += oldsz;
				if ((nline = realloc(line, sz + 1)) == NULL) {
					sz = oldsz;
					oom++;
				} else {
					line = nline;
					memcpy(line + oldsz, ip->ib_cur,
							cp - ip->ib_cur);
				}
			} else
				sz = oldsz;
			if (matchline(line, sz, 1, ip))
				break;
			free(line);
			line = NULL;
			sz = 0;
			ip->ib_cur = cp + (oom == 0);
			oom = 0;
		}
	}
endgrep:
	if (!qflag && cflag) {
		if (filename && !hflag)
			printf("%s:", filename);
#ifdef	LONGLONG
		printf("%llu\n", (long long)lmatch);
#else
		printf("%lu\n", (long)lmatch);
#endif
	}
	return 0;
}

/*
 * Grep a named file.
 */
static void
fngrep(const char *fn, int level)
{
	struct iblok	*ip;
#ifdef	ADDONS
	struct stat	st;

	if (rflag && fn && stat(fn, &st) == 0) {
		switch (st.st_mode&S_IFMT) {
#define	ignoring(t, s)	fprintf(stderr, "%s: ignoring %s %s\n", progname, t, s)
		case S_IFIFO:
			ignoring("named pipe", fn);
			return;
		case S_IFBLK:
			ignoring("block device", fn);
			return;
		case S_IFCHR:
			ignoring("block device", fn);
			return;
#ifdef	S_IFSOCK
		case S_IFSOCK:
			ignoring("socket", fn);
			return;
#endif	/* S_IFSOCK */
		default:
			break;
		case S_IFDIR: {
			char	*path;
			int	pend, psize, i;
			DIR	*df;
			struct dirent	*dp;

			if (hflag == 2)
				hflag = 0;
			if ((df = opendir(fn)) == NULL) {
				if (sflag == 0)
					fprintf(stderr, "%s: can't open "
							"directory %s\n",
							progname, fn);
				if (!qflag || status == 1)
					status = 2;
				return;
			}
			pend = strlen(fn);
			path = malloc(psize = pend + 2);
			strcpy(path, fn);
			path[pend++] = '/';
			while ((dp = readdir(df)) != NULL) {
				if (dp->d_name[0] == '.' &&
						(dp->d_name[1] == '\0' ||
					 	dp->d_name[1] == '.' &&
					 	dp->d_name[2] == '\0'))
					continue;
				i = 0;
				do {
					if (pend + i >= psize)
						path = srealloc(path,
								psize += 14);
					path[pend+i] = dp->d_name[i];
				} while (dp->d_name[i++]);
				filename = path;
				fngrep(path, level+1);
			}
			free(path);
			closedir(df);
			return;
		    }
		}
	}
#endif	/* ADDONS */
	if (fn) {
		if ((ip = ib_open(fn, 0)) == NULL) {
			if (sflag == 0)
				fprintf(stderr, "%s: can't open %s\n",
						progname, fn);
			if (!qflag || status == 1)
				status = 2;
			return;
		}
	} else
		ip = ib_alloc(0, 0);
	grep(ip);
	if (ip->ib_fd)
		ib_close(ip);
	else
		ib_free(ip);
}

int
main(int argc, char **argv)
{
	int i, hadpat = 0;

#ifdef	__GLIBC__
	putenv("POSIXLY_CORRECT=1");
#endif
	progname = basename(argv[0]);
	setlocale(LC_CTYPE, "");
	setlocale(LC_COLLATE, "");
	mb_cur_max = MB_CUR_MAX;
	range = gn_range;
	init();
	while ((i = getopt(argc, argv, options)) != EOF) {
		switch (i) {
		case 'E':
			Eflag |= 1;
			rc_select();
			break;
		case 'F':
			if (Eflag&2)
				Eflag = 0;
			Fflag |= 1;
			ac_select();
			break;
		case 'b':
			bflag = 1;
			break;
		case 'c':
			cflag = 1;
			break;
		case 'e':
			patstring(optarg);
			hadpat++;
			break;
		case 'f':
			fflag++;
			patfile(optarg);
			hadpat++;
			break;
		case 'h':
			hflag = 1;
			break;
		case 'i':
		case 'y':
			iflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'n':
			nflag = 1;
			break;
		case 'q':
			qflag = 1;
			break;
#ifdef	ADDONS
		case 'r':
			rflag = 1;
			break;
#endif	/* ADDONS */
		case 's':
			sflag = 1;
			break;
		case 'v':
			vflag = 1;
			break;
		case 'x':
			xflag = 1;
			break;
		default:
			if (!(Fflag&2))
				usage();
			status = 2;
		}
	}
	if (sus) {
		if (Fflag == 2) {
			if (sflag) {
				optind = 1;
				argv[1] = "-s";
				getopt(argc, argv, "");
				usage();
			}
			if (qflag) {
				optind = 1;
				argv[1] = "-q";
				getopt(argc, argv, "");
				usage();
			}
		}
		if (Fflag && status == 2)
			usage();
		if (Eflag == 1 && Fflag == 1 || cflag + lflag + qflag > 1)
			usage();
	}
	if (cflag)
		lflag = 0;
	if (hadpat == 0) {
		if (optind >= argc)
			misop();
		patstring(argv[optind++]);
	} else if (e0 == NULL)
		patstring(NULL);
	build();
	if (optind != argc) {
		if (optind + 1 == argc)
			hflag = 2;
		do {
			if (sus && argv[optind][0] == '-' &&
					argv[optind][1] == '\0') {
				filename = NULL;
				fngrep(NULL, 0);
			} else {
				filename = argv[optind];
				fngrep(argv[optind], 0);
			}
		} while (++optind < argc);
	} else {
		if (lflag && !sus && (Eflag || Fflag))
			exit(1);
		fngrep(NULL, 0);
	}
	return status;
}
