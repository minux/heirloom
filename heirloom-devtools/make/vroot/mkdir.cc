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
 * @(#)mkdir.cc 1.4 06/12/12
 */

/*	from OpenSolaris "mkdir.cc	1.4	06/12/12"	*/

/*
 * Portions Copyright (c) 2007 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)mkdir.cc	1.2 (gritter) 01/07/07
 */

#include <sys/types.h>
#include <sys/stat.h>

extern int mkdir(const char *path, mode_t mode);

#include <vroot/vroot.h>
#include <vroot/args.h>

static int	mkdir_thunk(char *path)
{
	vroot_result= mkdir(path, vroot_args.mkdir.mode);
	return(vroot_result == 0);
}

int	mkdir_vroot(char *path, int mode, pathpt vroot_path, pathpt vroot_vroot)
{
	vroot_args.mkdir.mode= mode;
	translate_with_thunk(path, mkdir_thunk, vroot_path, vroot_vroot, rw_write);
	return(vroot_result);
}
