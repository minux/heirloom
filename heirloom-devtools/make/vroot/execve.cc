/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
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
 * Copyright 1993 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms.
 */
/*
 * @(#)execve.cc 1.4 06/12/12
 */

/*	from OpenSolaris "execve.cc	1.4	06/12/12"	*/

/*
 * Portions Copyright (c) 2007 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)execve.cc	1.2 (gritter) 01/07/07
 */

#include <unistd.h>

extern int execve (const char *path, char *const argv[], char *const envp[]);

#include <vroot/vroot.h>
#include <vroot/args.h>

static int	execve_thunk(char *path)
{
	execve(path, vroot_args.execve.argv, vroot_args.execve.environ);
	switch (errno) {
		case ETXTBSY:
		case ENOEXEC: return 1;
		default: return 0;
	}
}

int	execve_vroot(char *path, char **argv, char **environ, pathpt vroot_path, pathpt vroot_vroot)
{
	vroot_args.execve.argv= argv;
	vroot_args.execve.environ= environ;
	translate_with_thunk(path, execve_thunk, vroot_path, vroot_vroot, rw_read);
	return(-1);
}
