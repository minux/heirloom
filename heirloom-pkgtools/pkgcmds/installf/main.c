/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/* Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T */
/* All Rights Reserved */

/*	from OpenSolaris "main.c	1.46	06/11/17 SMI"	*/

/*
 * Portions Copyright (c) 2007 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)main.c	1.6 (gritter) 2/26/07
 */

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pkginfo.h>
#include <pkgstrct.h>
#include <pkglocs.h>
#include <locale.h>
#include <libintl.h>
#include <spmizones_api.h>
#include <pkglib.h>
#include <install.h>
#include <libadm.h>
#include <libinst.h>
#include "installf.h"

#define	BASEDIR	"/BASEDIR/"

#define	INSTALF	(*prog == 'i')
#define	REMOVEF	(*prog == 'r')

#define	MSG_MANMOUNT	"Assuming mounts were provided."

#define	ERR_PKGNAME_TOO_LONG	\
"The package name specified on the command line\n" \
"exceeds the maximum package name length: a package name may contain a\n" \
"maximum of <%d> characters; however, the package name specified on\n" \
"the command line contains <%d> characters, which exceeds the maximum\n" \
"package name length by <%d> characters. Please specify a package name\n" \
"that contains no more than <%d> characters."

#define	ERR_DB_GET "unable to retrieve entries from the database."
#define	ERR_DB_PUT "unable to update the package database."
#define	ERR_ROOT_SET	"Could not set install root from the environment."
#define	ERR_ROOT_CMD	"Command line install root contends with environment."
#define	ERR_CLASSLONG	"classname argument too long"
#define	ERR_CLASSCHAR	"bad character in classname"
#define	ERR_INVAL	"package instance <%s> is invalid"
#define	ERR_NOTINST	"package instance <%s> is not installed"
#define	ERR_MERG	"unable to merge contents file"
#define	ERR_SORT	"unable to sort contents file"
#define	ERR_I_FAIL	"installf did not complete successfully"
#define	ERR_R_FAIL	"removef did not complete successfully"
#define	ERR_NOTROOT	"You must be \"root\" for %s to execute properly."
#define	ERR_USAGE0	"usage:\n" \
	"\t%s host_path] pkginst path " \
	"[path ...]\n" \
	"\t%s host_path] pkginst path\n"

#define	ERR_USAGE1	"usage:\n" \
	"\t%s [-c class] <pkginst> " \
	"<path>\n" \
	"\t%s [-c class] <pkginst> " \
	"<path> <specs>\n" \
	"\t   where <specs> may be defined as:\n" \
	"\t\tf <mode> <owner> <group>\n" \
	"\t\tv <mode> <owner> <group>\n" \
	"\t\te <mode> <owner> <group>\n" \
	"\t\td <mode> <owner> <group>\n" \
	"\t\tx <mode> <owner> <group>\n" \
	"\t\tp <mode> <owner> <group>\n" \
	"\t\tc <major> <minor> <mode> <owner> <group>\n" \
	"\t\tb <major> <minor> <mode> <owner> <group>\n" \
	"\t\ts <path>=<srcpath>\n" \
	"\t\tl <path>=<srcpath>\n" \
	"\t%s [-c class] -f pkginst\n"

#define	CMD_SORT	"sort +0 -1"

#define	LINK	1

extern char	dbst; 			/* libinst/pkgdbmerg.c */

struct cfextra **extlist;
struct pinfo **eptlist;

char	*classname = NULL;
char	*pkginst;
char	*uniTmp;
char 	*abi_sym_ptr;
char 	*ulim;
char 	*script;

int	eptnum;
int	sortflag;
int	nosetuid;
int	nocnflct;
int	warnflag = 0;

/* libadm/pkgparam.c */
extern void	set_PKGADM(char *newpath);
extern void	set_PKGLOC(char *newpath);

extern void set_limit(void);

int
main(int argc, char **argv)
{
	FILE		*pp;
	VFP_T		*cfTmpVfp;
	VFP_T		*cfVfp;
	char		*cmd;
	char		*tp;
	char		*prog;
	char		*pt;
	char		*vfstab_file = NULL;
	char		line[1024];
	char		outbuf[PATH_MAX];
	int		c;
	int		dbchg;
	int		err;
	int		fflag = 0;
	int		map_client = 1;
	int		n;
	int		pkgrmremote = 0;	/* don't remove remote files */
	struct cfent	*ept;

	/* hookup signals */

	(void) signal(SIGHUP, exit);
	(void) signal(SIGINT, exit);
	(void) signal(SIGQUIT, exit);

	/* initialize locale mechanism */

#if	!defined(TEXT_DOMAIN)	/* Should be defined by cc -D */
#define	TEXT_DOMAIN "SYS_TEST"
#endif	/* !defined(TEXT_DOMAIN) */

	(void) textdomain(TEXT_DOMAIN);

	/* determine program name */

	prog = set_prog_name(argv[0]);

	/* tell spmi zones interface how to access package output functions */

	z_set_output_functions(echo, echoDebug, progerr);

#if 0
	/* only allow root to run this program */

	if (getuid() != 0) {
		progerr(gettext(ERR_NOTROOT), prog);
		exit(1);
	}
#endif

	ulim = getenv("PKG_ULIMIT");
	script = getenv("PKG_PROC_SCRIPT");

	if (ulim && script) {
		set_limit();
		clr_ulimit();
	}

	/* bug id 4244631, not ABI compliant */
	abi_sym_ptr = getenv("PKG_NONABI_SYMLINKS");
	if (abi_sym_ptr && strncasecmp(abi_sym_ptr, "TRUE", 4) == 0)
		set_nonABI_symlinks();

	/* bugId 4012147 */
	if ((uniTmp = getenv("PKG_NO_UNIFIED")) != NULL)
		map_client = 0;
	if (!set_inst_root(getenv("PKG_INSTALL_ROOT"))) {
		progerr(gettext(ERR_ROOT_SET));
		exit(1);
	}

	while ((c = getopt(argc, argv, "c:V:fAMR:?")) != EOF) {
		switch (c) {
			case 'f':
			fflag++;
			break;

			case 'c':
			classname = optarg;
			/* validate that classname is acceptable */
			if (strlen(classname) > (size_t)CLSSIZ) {
				progerr(gettext(ERR_CLASSLONG));
				exit(1);
			}
			for (pt = classname; *pt; pt++) {
				if (!isalpha(*pt) && !isdigit(*pt)) {
					progerr(gettext(ERR_CLASSCHAR));
					exit(1);
				}
			}
			break;

		/*
		 * Don't map the client filesystem onto the server's. Assume
		 * the mounts have been made for us.
		 */
			case 'M':
			map_client = 0;
			break;

		/*
		 * Allow admin to establish the client filesystem using a
		 * vfstab-like file of stable format.
		 */
			case 'V':
			vfstab_file = flex_device(optarg, 2);
			map_client = 1;
			break;

			case 'A':
			pkgrmremote++;
			break;

			case 'R':	/* added for newroot option */
			if (!set_inst_root(optarg)) {
				progerr(gettext(ERR_ROOT_CMD));
				exit(1);
			}
			break;

			default:
			usage();
			/*NOTREACHED*/
		}
	}

	if (pkgrmremote && (!is_an_inst_root() || fflag || INSTALF)) {
		usage();
		/*NOTREACHED*/
	}

	/*
	 * Get the mount table info and store internally.
	 */
	if (get_mntinfo(map_client, vfstab_file))
		exit(1);

	/*
	 * This function defines the standard /var/... directories used later
	 * to construct the paths to the various databases.
	 */
	(void) set_PKGpaths(get_inst_root());

	/*
	 * If this is being installed on a client whose /var filesystem is
	 * mounted in some odd way, remap the administrative paths to the
	 * real filesystem. This could be avoided by simply mounting up the
	 * client now; but we aren't yet to the point in the process where
	 * modification of the filesystem is permitted.
	 */
	if (is_an_inst_root()) {
		int fsys_value;

		fsys_value = fsys(get_PKGLOC());
		if (use_srvr_map_n(fsys_value))
			set_PKGLOC(server_map(get_PKGLOC(), fsys_value));

		fsys_value = fsys(get_PKGADM());
		if (use_srvr_map_n(fsys_value))
			set_PKGADM(server_map(get_PKGADM(), fsys_value));
	}

	sortflag = 0;

	/*
	 * get the package name and verify length is not too long
	 */

	pkginst = argv[optind++];
	if (pkginst == NULL) {
		usage();
		/*NOTREACHED*/

	}

	n = strlen(pkginst);
	if (n > PKGSIZ) {
		progerr(gettext(ERR_PKGNAME_TOO_LONG), PKGSIZ, n, n-PKGSIZ,
		    PKGSIZ);
		usage();
		/*NOTREACHED*/
	}

	/*
	 * The following is used to setup the environment. Note that the
	 * variable 'BASEDIR' is only meaningful for this utility if there
	 * is an install root, recorded in PKG_INSTALL_ROOT. Otherwise, this
	 * utility can create a file or directory anywhere unfettered by
	 * the basedir associated with the package instance.
	 */
	if ((err = set_basedirs(0, NULL, pkginst, 1)) != 0)
		exit(err);

	if (INSTALF)
		mkbasedir(0, get_basedir());

	if (fflag) {
		/* installf and removef must only have pkginst */
		if (optind != argc) {
			usage();
			/*NOTREACHED*/
		}
	} else {
		/*
		 * installf and removef must have at minimum
		 * pkginst & pathname specified on command line
		 */
		if (optind >= argc) {
			usage();
			/*NOTREACHED*/
		}
	}
	if (REMOVEF) {
		if (classname) {
			usage();
		}
	}
	if (pkgnmchk(pkginst, "all", 0)) {
		progerr(gettext(ERR_INVAL), pkginst);
		exit(1);
	}
	if (fpkginst(pkginst, NULL, NULL) == NULL) {
		progerr(gettext(ERR_NOTINST), pkginst);
		exit(1);
	}

#ifdef	ALLOW_EXCEPTION_PKG_LIST
	/*
	 * *********************************************************************
	 * this feature is removed starting with Solaris 10 - there is no built
	 * in list of packages that should be run "the old way"
	 * *********************************************************************
	 */
	/* Until 2.9, set it from the execption list */
	if (pkginst && exception_pkg(pkginst, LINK))
		set_nonABI_symlinks();
#endif
	/*
	 * This maps the client filesystems into the server's space.
	 */
	if (map_client && !mount_client())
		logerr(gettext(MSG_MANMOUNT));

	/* open the package database (contents) file */

	if (!ocfile(&cfVfp, &cfTmpVfp, 0L)) {
		quit(1);
	}

	if (fflag) {
		dbchg = dofinal(cfVfp, cfTmpVfp, REMOVEF, classname, prog);
	} else {
		if (INSTALF) {
			dbst = INST_RDY;
			if (installf(argc-optind, &argv[optind]))
				quit(1);
		} else {
			dbst = RM_RDY;
			removef(argc-optind, &argv[optind]);
		}

		dbchg = pkgdbmerg(cfVfp, cfTmpVfp, extlist, 0);
		if (dbchg < 0) {
			progerr(gettext(ERR_MERG));
			quit(99);
		}
	}

	if (dbchg) {
		if ((n = swapcfile(&cfVfp, &cfTmpVfp, pkginst, 1))
			== RESULT_WRN) {
		    warnflag++;
		} else if (n == RESULT_ERR) {
		    quit(99);
		}
	}

	relslock();

	if (REMOVEF && !fflag) {
		for (n = 0; extlist[n]; n++) {
			ept = &(extlist[n]->cf_ent);

			if (!extlist[n]->mstat.shared) {
				/*
				 * Only output paths that can be deleted.
				 * so need to skip if the object is owned
				 * by a remote server and removal is not
				 * being forced.
				 */
				if (ept->pinfo &&
					(ept->pinfo->status == SERVED_FILE) &&
					!pkgrmremote)
					continue;

				c = 0;
				if (is_a_cl_basedir() && !is_an_inst_root()) {
					c = strlen(get_client_basedir());
					(void) snprintf(outbuf, sizeof (outbuf),
						"%s/%s\n", get_basedir(),
						&(ept->path[c]));
				} else if (is_an_inst_root()) {
					(void) snprintf(outbuf, sizeof (outbuf),
						"%s/%s\n", get_inst_root(),
						&(ept->path[c]));
				} else {
					(void) snprintf(outbuf, sizeof (outbuf),
						"%s\n", &(ept->path[c]));
				}
				canonize(outbuf);
				(void) printf("%s", outbuf);
			}
		}
	} else if (INSTALF && !fflag) {
		for (n = 0; extlist[n]; n++) {
			ept = &(extlist[n]->cf_ent);

			if (strchr("dxcbp", ept->ftype)) {
				tp = fixpath(ept->path);
				(void) averify(1, &ept->ftype,
					tp, &ept->ainfo);
			}
		}
	}

	/* Sort the contents files if needed */
	if (sortflag) {
		int n;

		warnflag += (ocfile(&cfVfp, &cfTmpVfp, 0L)) ? 0 : 1;
		if (!warnflag) {
			size_t	len;

			len = strlen(CMD_SORT) + strlen(get_PKGADM()) +
				strlen("/contents") + 5;
			cmd = (char *)malloc(len);
			(void) snprintf(cmd, len, "%s %s/contents",
				CMD_SORT, get_PKGADM());
			pp = popen(cmd, "r");
			if (pp == NULL) {
				(void) vfpClose(&cfVfp);
				(void) vfpClose(&cfTmpVfp);
				free(cmd);
				progerr(gettext(ERR_SORT));
				quit(1);
			}
			while (fgets(line, 1024, pp) != NULL) {
				if (line[0] != DUP_ENTRY) {
					vfpPuts(cfTmpVfp, line);
				}
			}
			free(cmd);
			(void) pclose(pp);
			n = swapcfile(&cfVfp, &cfTmpVfp, pkginst, 1);
			if (n == RESULT_WRN) {
				warnflag++;
			} else if (n == RESULT_ERR) {
				quit(99);
			}

			relslock();	/* Unlock the database. */
		}
	}

	z_destroyMountTable();

	quit(warnflag ? 1 : 0);
	/*NOTREACHED*/
	/*LINTED warning: Function has no return statement : main */
	return 0;
}

void
quit(int n)
{
	char *prog = get_prog_name();

	unmount_client();

	if (ulim && script) {
		if (REMOVEF) {
			set_ulimit(script, gettext(ERR_R_FAIL));
		} else {
			set_ulimit(script, gettext(ERR_I_FAIL));
		}
	}

	exit(n);
}

void
usage(void)
{
	char *prog = get_prog_name();

	if (REMOVEF) {
		(void) fprintf(stderr, gettext(ERR_USAGE0), prog, prog);
	} else {
		(void) fprintf(stderr, gettext(ERR_USAGE1), prog, prog, prog);
	}
	exit(1);
}
