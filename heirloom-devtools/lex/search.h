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
/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef SEARCH_H
#define	SEARCH_H

/*	from OpenSolaris "search.h	1.19	05/06/08 SMI"	 SVr4.0 1.3.1.11 	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)search.h	1.3 (gritter) 10/22/05
 */

#include <sys/types.h>


#if 0
/* HSEARCH(3C) */
typedef enum { FIND, ENTER } ACTION;

struct qelem {
	struct qelem	*q_forw;
	struct qelem	*q_back;
};

typedef struct entry { char *key, *data; } ENTRY;

int hcreate(size_t);
void hdestroy(void);
ENTRY *hsearch(ENTRY, ACTION);
void insque(void *, void *);
void remque(void *);


/* TSEARCH(3C) */
typedef enum { preorder, postorder, endorder, leaf } VISIT;

void *tdelete(const void *_RESTRICT_KYWD, void **_RESTRICT_KYWD,
	int (*)(const void *, const void *));
void *tfind(const void *, void *const *, int (*)(const void *, const void *));
void *tsearch(const void *, void **, int (*)(const void *, const void *));
void twalk(const void *, void (*)(const void *, VISIT, int));


/* BSEARCH(3C) */
void *bsearch(const void *, const void *, size_t, size_t,
	    int (*)(const void *, const void *));
#endif


/* LSEARCH(3C) */
void *lfind(const void *, const void *, size_t *, size_t,
	    int (*)(const void *, const void *));
void *lsearch(const void *, void *, size_t *, size_t,
	    int (*)(const void *, const void *));

#endif	/* SEARCH_H */
