#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)dpost.sl	1.9 (gritter) 8/13/05";
const char creator[] = "Heirloom Documentation Tools - dpost 1.9 (gritter) 8/13/05";
/* SLIST */
/*
color.c: * Sccsid @(#)color.c	1.3 (gritter) 8/9/05
comments.h: * Sccsid @(#)comments.h	1.4 (gritter) 8/13/05
dev.h: * Sccsid @(#)dev.h	1.3 (gritter) 8/9/05
dpost.c: * Sccsid @(#)dpost.c	1.7 (gritter) 8/13/05
dpost.h: * Sccsid @(#)dpost.h	1.3 (gritter) 8/9/05
draw.c: * Sccsid @(#)draw.c	1.3 (gritter) 8/9/05
ext.h: * Sccsid @(#)ext.h	1.3 (gritter) 8/9/05
gen.h: * Sccsid @(#)gen.h	1.4 (gritter) 8/13/05
getopt.c: * Sccsid @(#)getopt.c	1.8 (gritter) 8/2/05
glob.c: * Sccsid @(#)glob.c	1.3 (gritter) 8/9/05
misc.c: * Sccsid @(#)misc.c	1.3 (gritter) 8/9/05
path.h: * Sccsid @(#)path.h	1.3 (gritter) 8/9/05
pictures.c: * Sccsid @(#)pictures.c	1.3 (gritter) 8/9/05
ps_include.c: * Sccsid @(#)ps_include.c	1.5 (gritter) 8/13/05
request.c: * Sccsid @(#)request.c	1.3 (gritter) 8/9/05
request.h: * Sccsid @(#)request.h	1.3 (gritter) 8/9/05
*/