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
 * @(#)chroot.cc 1.4 06/12/12
 */

/*	from OpenSolaris "chroot.cc	1.4	06/12/12"	*/

/*
 * Portions Copyright (c) 2007 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)chroot.cc	1.2 (gritter) 01/07/07
 */

#include <unistd.h>

extern int chroot(const char *path);

#include <vroot/vroot.h>
#include <vroot/args.h>

static int	chroot_thunk(char *path)
{
	vroot_result= chroot(path);
	return(vroot_result == 0);
}

int	chroot_vroot(char *path, pathpt vroot_path, pathpt vroot_vroot)
{
	translate_with_thunk(path, chroot_thunk, vroot_path, vroot_vroot, rw_read);
	return(vroot_result);
}
