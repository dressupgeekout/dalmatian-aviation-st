# DALMATIAN AVIATION ST
# Makefile
#

CROSS?=		m68k-atari-mint
CC=		$(CROSS)-gcc
LD=		$(CROSS)-ld
RANLIB=		$(CROSS)-ranlib
STRIP=		$(CROSS)-strip

CFLAGS?=	-Wall
CFLAGS+=	-Os
LDFLAGS?=	-nostdlib -L$(CURDIR)/lib lib/crt0.o
LIBS+=		-lgem -lcmini -lgcc

SRCS=	# defined
SRCS+=	dalmatianlib.c
SRCS+=	main.c
SRCS+=	yb.c

OBJS=	$(patsubst %.c,%.o,$(SRCS))
PROG=	DALMATIA.PRG

.PHONY: all
all: script.h $(PROG)

$(PROG): $(OBJS) script.h
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	$(STRIP) $@
	@echo ==============================
	wc -c $@
	@echo ==============================

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

script.h: ../tools/script.janet
	janet $^ > $@

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS) script.h
