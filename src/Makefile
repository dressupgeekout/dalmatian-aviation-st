# DALMATIAN AVIATION ST
# Makefile
#

CROSS?=		m68k-atari-mint
CC=		$(CROSS)-gcc
LD=		$(CROSS)-ld
RANLIB=		$(CROSS)-ranlib
STRIP=		$(CROSS)-strip

CFLAGS?=	-DATARI -Wall
CFLAGS+=	-Os
LDFLAGS?=	-nostdlib -L$(CURDIR)/lib lib/crt0.o
LIBS+=		-lgem -lm -lcmini -lgcc

SRCS=	# defined
SRCS+=	daljan.c
SRCS+=	dalmatianlib.c
SRCS+=	janet.c
SRCS+=	main.c
SRCS+=	yb.c

OBJS=	$(patsubst %.c,%.o,$(SRCS))
PROG=	DALMATIA.PRG

.PHONY: all
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	$(STRIP) $@
	@echo ==============================
	wc -c $@
	@echo ==============================

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS)
