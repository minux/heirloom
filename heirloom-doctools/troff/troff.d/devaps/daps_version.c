#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)/usr/ucb/daps.sl	1.2 (gritter) 8/13/05";
/* SLIST */
/*
aps.h: * Sccsid @(#)aps.h	1.3 (gritter) 8/9/05
build.c: * Sccsid @(#)build.c	1.4 (gritter) 8/13/05
daps.c: * Sccsid @(#)daps.c	1.6 (gritter) 8/13/05
daps.h: * Sccsid @(#)daps.h	1.3 (gritter) 8/9/05
getopt.c: * Sccsid @(#)getopt.c	1.8 (gritter) 8/2/05
makedev.c: * Sccsid @(#)makedev.c	1.3 (gritter) 8/9/05
*/
