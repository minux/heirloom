OBJ = arcgen.o blockgen.o boxgen.o circgen.o for.o input.o linegen.o \
	main.o misc.o movegen.o picl.o picy.o pltroff.o print.o symtab.o \
	textgen.o version.o

FLAGS =

YFLAGS = -d

.c.o:
	$(CC) $(CFLAGS) $(WARN) $(FLAGS) $(CPPFLAGS) -c $<

all: picy.c picl.c pic

pic: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -lm -o pic

y.tab.h: picy.c

install:
	$(INSTALL) -c pic $(ROOT)$(BINDIR)/pic
	$(STRIP) $(ROOT)$(BINDIR)/pic
	$(INSTALL) -c -m 644 pic.1b $(ROOT)$(MANDIR)/man1b/pic.1b

clean:
	rm -f $(OBJ) picl.c picy.c y.tab.h pic core log *~

mrproper: clean

arcgen.o: arcgen.c pic.h y.tab.h
blockgen.o: blockgen.c pic.h y.tab.h
boxgen.o: boxgen.c pic.h y.tab.h
circgen.o: circgen.c pic.h y.tab.h
for.o: for.c pic.h y.tab.h
input.o: input.c pic.h y.tab.h
linegen.o: linegen.c pic.h y.tab.h
main.o: main.c pic.h y.tab.h
misc.o: misc.c pic.h y.tab.h
movegen.o: movegen.c pic.h y.tab.h
picy.o: picy.c pic.h
pltroff.o: pltroff.c pic.h
print.o: print.c pic.h y.tab.h
symtab.o: symtab.c pic.h y.tab.h
textgen.o: textgen.c pic.h y.tab.h
