/*	from Unix 7th Edition /usr/src/cmd/spell/spellout.c	*/
/*
 * Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   Redistributions of source code and documentation must retain the
 *    above copyright notice, this list of conditions and the following
 *    disclaimer.
 *   Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *   All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed or owned by Caldera
 *      International, Inc.
 *   Neither the name of Caldera International, Inc. nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
 * INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)/usr/5lib/spell/spellout.sl	1.5 (gritter) 10/7/04";

#include "spell.h"

int
main(int argc, char **argv)
{
	register int i, j;
	long h;
	register long *lp;
	char word[NW];
	int dflag = 0;
	int indict;
	register char *wp;

	if (argc>1 && argv[1][0]=='-' && argv[1][1]=='d') {
		dflag = 1;
		argc--;
		argv++;
	}
	if(argc<=1) {
		fprintf(stderr,"spellout: arg count\n");
		exit(1);
	}
	if(!prime(argc,argv)) {
		fprintf(stderr,
		    "spellout: cannot initialize hash table\n");
		exit(1);
	}
	while (fgets(word, sizeof(word), stdin)) {
		indict = 1;
		for (i=0; i<NP; i++) {
			for (wp = word, h = 0, lp = pow2[i];
				(j = *wp) != '\0'; ++wp, ++lp)
				h += j * *lp;
			h %= p[i];
			if (get(h)==0) {
				indict = 0;
				break;
			}
		}
		if (dflag == indict)
			fputs(word, stdout);
	}
	return 0;
}
