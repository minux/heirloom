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
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright (c) 1999, by Sun Microsystems, Inc.
 * All rights reserved.
 */

/*	from OpenSolaris "delempty.c	1.8	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)delempty.c	1.4 (gritter) 6/22/05
 */
/*LINTLIBRARY*/

/*
 *  NAME
 *	delempty - delete an empty mail box
 *
 *  SYNOPSIS
 *	int delempty(mode_t mode, char *mailname)
 *
 *  DESCRIPTION
 *	Delete an empty mail box if it's allowed. Check
 *	the value of xgetenv("DEL_EMPTY_MFILE") for
 *	"yes" [always], "no" [never] or the default [based
 *	on the mode].
 */

#include "libmail.h"
#include <sys/types.h>
#include <unistd.h>

int
delempty(mode_t mode, char *mailname)
{
	char *del_empty = Xgetenv("DEL_EMPTY_MFILE");
	size_t del_len;
	int do_del = 0;

	del_len = strlen(del_empty);
	/* "yes" means always remove the mailfile */
	if (casncmp(del_empty, "yes", (ssize_t)del_len))
		do_del = 1;
	/* "no" means never remove the mailfile */
	else if (!casncmp(del_empty, "no", (ssize_t)del_len)) {
		/* check for mode 0660 */
		if ((mode & 07777) == MFMODE)
			do_del = 1;
	}

	if (do_del)
		unlink(mailname);
	return (do_del);
}
