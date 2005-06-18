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

/*	from OpenSolaris "copystream.c	1.8	05/06/08 SMI"	 SVr4.0 1.2   	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)copystream.c	1.3 (gritter) 6/18/05
 */
/*LINTLIBRARY*/

/*
    NAME
	copystream - copy one FILE stream to another

    SYNOPSIS
	int copystream(FILE *infp, FILE *outfp)

    DESCRIPTION
	copystream() copies one stream to another. The stream
	infp must be opened for reading and the stream outfp
	must be opened for writing.

	It returns true if the stream is successively copied;
	false if any writes fail.
*/

#include "libmail.h"
#include <sys/types.h>

int
copystream(FILE *infp, FILE *outfp)
{
	char buffer[BUFSIZ];
	size_t nread;

	while ((nread = fread(buffer, sizeof (char), sizeof (buffer),
	    infp)) > 0)
		if (fwrite(buffer, sizeof (char), nread, outfp) != nread)
			return (0);
	return (1);
}
