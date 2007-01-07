SHELL = /sbin/sh

SUBDIRS = build libwchar libcommon libuxre _install \
	banner basename bc bdiff bfs \
	cal calendar cat chmod chown \
	cksum cmp col comm copy cp cpio csplit cut \
	date dc dd deroff diff diff3 dircmp dirname df du \
	echo ed env expand expr \
	factor file find fmt fmtmsg fold \
	getconf getopt grep groups hd head hostname id join \
	kill line listusers ln logins logname ls \
	mail man mesg mkdir mkfifo mknod more mvdir \
	nawk news nice nl nohup oawk od \
	paste pathchk pg pgrep pr printenv printf priocntl ps psrinfo pwd \
	random renice rm rmdir \
	sdiff sed setpgrp shl sleep sort spell split stty su sum sync \
	tabs tail tapecntl tar tcopy tee test time touch tr true tsort tty \
	ul uname uniq units users wc what who whoami whodo xargs yes

dummy: makefiles all

.DEFAULT:
	+ for i in $(SUBDIRS) ;\
	do	\
		(cd "$$i" && $(MAKE) $@) || exit ; \
	done
	$(MAKE) -f Makefile $@

.SUFFIXES: .mk
.mk:
	cat build/mk.head build/mk.config $< build/mk.tail >$@

makefiles: Makefile $(SUBDIRS:=/Makefile)

install:
	$(MAKE) -f Makefile directories
	+ for i in $(SUBDIRS) ;\
	do	\
		(cd "$$i" && $(MAKE) $@) || exit ; \
	done
	$(MAKE) -f Makefile links

mrproper:
	rm -f .foo .FOO Makefile
	+ for i in $(SUBDIRS) ;\
	do	\
		(cd "$$i" && $(MAKE) $@) || exit ; \
	done

casecheck: .foo .Foo
	cmp -s .foo .Foo && exit 1 || exit 0

.foo .Foo:
	echo $@ > $@

PKGFLAGS =	CC=cc CFLAGS2=-O CFLAGSS=-O CFLAGSS=-O CFLAGSU=-O \
		CFLAGS=-O CPPFLAGS='-D__EXTENSIONS__' \
		TTYGRP= LCURS=-lcurses LIBGEN=-lgen \
		LSOCKET='-lsocket -lnsl' STRIP=: \
		LIBZ= USE_ZLIB=0 LIBBZ2= USE_BZLIB=0 \
		XO5FL= XO6FL= GNUFL= \
		DEFBIN=/usr/5bin SV3BIN=/usr/5bin S42BIN=/usr/5bin/s42 \
		SUSBIN=/usr/5bin/posix SU3BIN=/usr/5bin/posix2001 \
		UCBBIN=/usr/ucb/heirloom \
		DEFLIB=/usr/5lib DEFSBIN=/usr/5bin MANDIR=/usr/share/man/5man \
		DFLDIR=/etc/default/heirloom SPELLHIST=/var/adm/spellhist \
		SULOG=/var/log/sulog MAGIC=/usr/5lib/magic YACC=yacc
PKGROOT =	/var/tmp/heirloom-root
PKGTEMP =	/var/tmp
PKGPROTO =	prototype

pkgbuild:
	$(MAKE) $(PKGFLAGS)

ou8:
	$(MAKE) pkgbuild LEX=flex

pkg: pkgbuild
	rm -rf $(PKGROOT)
	mkdir $(PKGROOT)
	$(MAKE) $(PKGFLAGS) ROOT=$(PKGROOT) install
	mkdir -p $(PKGROOT)/usr/share/doc/heirloom
	(echo README; find * -name NOTES -print; \
			find LICENSE/* -print) | \
		cpio -pdm $(PKGROOT)/usr/share/doc/heirloom
	rm -f $(PKGPROTO)
	echo 'i pkginfo' >$(PKGPROTO)
	(cd $(PKGROOT) && find . -print | pkgproto) | >>$(PKGPROTO) sed '\
		s:^\([df] [^ ]* [^ ]* [^ ]*\) .*:\1 root root:; \
		s:^\(f [^ ]* [^ ]*/ps \).*:\14755 root root:; \
		s:^\(f [^ ]* [^ ]*/shl \).*:\12755 root adm:; \
		s:^\(f [^ ]* [^ ]*/su \).*:\14755 root root:; \
		s:^f\( [^ ]* etc/\):v \1:; \
		s:^f\( [^ ]* var/\):v \1:; \
		s:^\(s [^ ]* [^ ]*=\)\([^/]\):\1./\2:'
	rm -rf $(PKGTEMP)/heirloom
	pkgmk -a `uname -m` -d $(PKGTEMP) -r $(PKGROOT) -f $(PKGPROTO) heirloom
	pkgtrans -o -s $(PKGTEMP) `pwd`/heirloom.pkg heirloom
	rm -rf $(PKGROOT)
	rm -rf $(PKGPROTO) $(PKGTEMP)/heirloom

DIETFLAGS =	CC="$(HOME)/src/diet gcc" HOSTCC="$(HOME)/src/diet gcc" \
		CFLAGS="-Os -fomit-frame-pointer" \
		CFLAGSS="-Os -fomit-frame-pointer" \
		CFLAGS2="-Os -fomit-frame-pointer" \
		CFLAGSU="-Os -fomit-frame-pointer" \
		LCRYPT= \
		IWCHAR=-I../libwchar LWCHAR="-L../libwchar -lwchar" \
		DEFBIN=/5bin SV3BIN=/5bin S42BIN=/5bin/s42 \
		SUSBIN=/5bin/posix SU3BIN=/5bin/posix2001 UCBBIN=/5bin/ucb \
		DEFLIB=/5bin/lib DEFSBIN=/5bin MANDIR=/tmp/__man__ \
		DFLDIR=/etc/default SPELLHIST=/var/adm/spellhist \
		SULOG=/var/log/sulog MAGIC=/5bin/lib/magic

diet:
	$(MAKE) $(DIETFLAGS)

dietinstall:
	$(MAKE) $(DIETFLAGS) install
	rm -rf /tmp/__man__

world:
	make mrproper
	make diet
	sudo make dietinstall
	make mrproper
	make
	sudo make install
	make clean

freebsd:
	$(MAKE) LKVM=-lkvm \
	XO5FL= XO6FL= GNUFL= YACC=yacc

netbsd:
	$(MAKE) LKVM=-lkvm WERROR= \
	XO5FL= XO6FL= GNUFL= YACC=yacc LCURS=-ltermcap CPPFLAGS=-DUSE_TERMCAP

pie:
	$(MAKE) \
		CFLAGS="-Os -fomit-frame-pointer -fPIE" \
		CFLAGSS="-Os -fomit-frame-pointer -fPIE" \
		CFLAGS2="-Os -fomit-frame-pointer -fPIE" \
		LDFLAGS="-pie"

ps2:
	$(MAKE) WARN="-Wchar-subscripts -Wimplicit \
			-Wmissing-braces -Wreturn-type -Wtrigraphs \
			-Wuninitialized -Wmultichar -Wpointer-arith -Werror"

ps2diet:
	$(MAKE) CC="$(HOME)/src/diet gcc" \
		CFLAGS="-Os -fomit-frame-pointer" \
		CFLAGSS="-Os -fomit-frame-pointer" \
		CFLAGS2="-Os -fomit-frame-pointer" \
		CFLAGSU="-Os -fomit-frame-pointer" \
		LCRYPT= \
		IWCHAR=-I../libwchar LWCHAR="-L../libwchar -lwchar" \
		WARN="-Wchar-subscripts -Wimplicit \
			-Wmissing-braces -Wreturn-type -Wtrigraphs \
			-Wuninitialized -Wmultichar -Wpointer-arith -Werror"
