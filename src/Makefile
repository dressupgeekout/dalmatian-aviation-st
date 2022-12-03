# DALMATIAN AVIATION ST
# Makefile
#

CROSS?=		m68k-atari-mint
CC=		$(CROSS)-gcc
LD=		$(CROSS)-ld
RANLIB=		$(CROSS)-ranlib
STRIP=		$(CROSS)-strip

CFLAGS?=	-Os -Wall
LDFLAGS?=	-nostdlib -L$(CURDIR)/lib lib/crt0.o
LIBS+=		-lgem -lcmini -lgcc

SRCS=	# defined
SRCS+=	dalmatianlib.c
SRCS+=	main.c

OBJS=	$(patsubst %.c,%.o,$(SRCS))
PROG=	DALMATIA.PRG

$(PROG): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)
	$(STRIP) $@
	@echo ==============================
	wc -c $@
	@echo ==============================

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS)
