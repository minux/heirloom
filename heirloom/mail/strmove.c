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

/*	from OpenSolaris "strmove.c	1.8	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)strmove.c	1.3 (gritter) 6/18/05
 */
/*LINTLIBRARY*/

#include <sys/types.h>
#include "libmail.h"

/*
 *  NAME
 *	strmove - copy a string, permitting overlaps
 *
 *  SYNOPSIS
 *	void strmove(char *to, char *from)
 *
 *  DESCRIPTION
 *	strmove() acts exactly like strcpy() with the additional
 *	guarantee that it will work with overlapping strings.
 *	It does it left-to-right, a byte at a time.
 */

void
strmove(char *to, char *from)
{
	while ((*to++ = *from++) != 0)
		;
}
