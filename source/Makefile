SRC = align.c alloc.c arith.c box.c boxlist.c chr.c \
	cmd.c cond.c def.c dvi.c error.c eval.c expand.c \
	fileio.c hyph.c math.c mathlist.c mlst-hlst.c \
	pack.c page.c par.c print.c \
	scan.c str.c sym.c tex.c texext.c \
	tfm.c tok.c toklist.c

OBJ = align.o alloc.o arith.o box.o boxlist.o chr.o \
	cmd.o cond.o def.o dvi.o error.o eval.o expand.o \
	fileio.o hyph.o math.o mathlist.o mlst-hlst.o \
	pack.o page.o par.o print.o \
	scan.o str.o sym.o tex.o texext.o \
	tfm.o tok.o toklist.o

CFLAGS = -g -Bstatic


initex: ${OBJ}
	${CC} ${CFLAGS} ${OBJ} -o $@

tex: initex
	-./initex 'plain \dump'
	undump tex initex
	rm core

latex: initex
	-./initex 'lplain \dump'
	undump latex initex
	rm core

all: initex tex latex

clean::
	-rm -f *.o errs

veryclean::clean
	-rm -f initex tex latex *.log *.dvi *.aux
