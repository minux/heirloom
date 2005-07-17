all: intro.1

intro.1: intro.1.in
	$(SHELL) build/genintro intro.1.in >intro.1

clean:
	rm -f heirloom.pkg core log *~

MRPROPER = intro.1

install: links directories

directories:
	test -d $(ROOT)$(DEFBIN) || mkdir -p $(ROOT)$(DEFBIN)
	test -d $(ROOT)$(DEFLIB) || mkdir -p $(ROOT)$(DEFLIB)
	test -d $(ROOT)$(SV3BIN) || mkdir -p $(ROOT)$(SV3BIN)
	test -d $(ROOT)$(SV3BIN) || mkdir -p $(ROOT)$(SV3BIN)
	test -d $(ROOT)$(S42BIN) || mkdir -p $(ROOT)$(S42BIN)
	test -d $(ROOT)$(SUSBIN) || mkdir -p $(ROOT)$(SUSBIN)
	test -d $(ROOT)$(SU3BIN) || mkdir -p $(ROOT)$(SU3BIN)
	test -d $(ROOT)$(DEFSBIN) || mkdir -p $(ROOT)$(DEFSBIN)
	test -d $(ROOT)$(UCBBIN) || mkdir -p $(ROOT)$(UCBBIN)
	for i in 1 1b 1m 2 3 4 5 6 7 8; \
	do \
		test -d $(ROOT)$(MANDIR)/man$$i || \
			mkdir -p $(ROOT)$(MANDIR)/man$$i; \
	done
	test -d $(ROOT)$(DFLDIR) || mkdir -p $(ROOT)$(DFLDIR)

links:
	for i in install ; \
	do \
		sh build/crossln $(ROOT)$(UCBBIN)/$$i $(ROOT)$(DEFBIN)/$$i $(ROOT); \
	done
	for i in basename chmod cp date du echo file id ln mkdir mv nohup pr ps rm rmdir sort test touch tr wc who; \
	do \
		sh build/crossln $(ROOT)$(SUSBIN)/$$i $(ROOT)$(SU3BIN)/$$i $(ROOT); \
	done
	for i in csplit date egrep fgrep find getconf grep id mkdir nawk pg; \
	do \
		sh build/crossln $(ROOT)$(SUSBIN)/$$i $(ROOT)$(S42BIN)/$$i $(ROOT); \
	done
	for i in basename chmod du echo file lc ls more nohup od page pax pr rm rmdir sort test touch tr who; \
	do \
		sh build/crossln $(ROOT)$(SV3BIN)/$$i $(ROOT)$(S42BIN)/$$i $(ROOT); \
	done
	for i in pax; \
	do \
		sh build/crossln $(ROOT)$(SV3BIN)/$$i $(ROOT)$(SUSBIN)/$$i $(ROOT); \
	done
	for i in getconf; \
	do \
		sh build/crossln $(ROOT)$(SUSBIN)/$$i $(ROOT)$(SV3BIN)/$$i $(ROOT); \
	done
	for i in od; \
	do \
		sh build/crossln $(ROOT)$(SV3BIN)/$$i $(ROOT)$(SU3BIN)/$$i $(ROOT); \
	done
	for i in apropos expand hostname man printenv renice tcopy ul unexpand uptime users w whoami whatis; \
	do \
		sh build/crossln $(ROOT)$(DEFBIN)/$$i $(ROOT)$(UCBBIN)/$$i $(ROOT); \
	done
	sh build/crossln $(ROOT)$(SV3BIN)/more $(ROOT)$(UCBBIN)/more $(ROOT)
	sh build/crossln $(ROOT)$(DEFSBIN)/catman $(ROOT)$(UCBBIN)/catman $(ROOT)
	sh build/crossln $(ROOT)$(DEFBIN)/oawk $(ROOT)$(SV3BIN)/awk $(ROOT)
	sh build/crossln $(ROOT)$(S42BIN)/nawk $(ROOT)$(S42BIN)/awk $(ROOT)
	sh build/crossln $(ROOT)$(SUSBIN)/nawk $(ROOT)$(SUSBIN)/awk $(ROOT)
	sh build/crossln $(ROOT)$(SU3BIN)/nawk $(ROOT)$(SU3BIN)/awk $(ROOT)
	rm -f $(ROOT)$(MANDIR)/man1/awk.1
	if test $(DEFBIN) = $(S42BIN) -o $(DEFBIN) = $(SUSBIN) \
		-o $(DEFBIN) = $(SU3BIN) ; \
	then \
		sh build/crossln $(ROOT)$(MANDIR)/man1/nawk.1 $(ROOT)$(MANDIR)/man1/awk.1 $(ROOT); \
	else \
		sh build/crossln $(ROOT)$(MANDIR)/man1/oawk.1 $(ROOT)$(MANDIR)/man1/awk.1 $(ROOT); \
	fi
	$(SHELL) build/maninst -c -m 644 intro.1 $(ROOT)$(MANDIR)/man1/intro.1
