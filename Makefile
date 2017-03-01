#
# Sysinfo
#
# Copyright (c) 2017 Guillaume Gomez
#

#
# Please note that this Makefile only generates the c example.
#

IDIR = ./src/
CC = gcc
CFLAGS = -I$(IDIR)

ODIR = examples/src/
LDIR = ./target/debug/

LIBS = -lsysinfo

_DEPS = sysinfo.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = simple.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


simple: $(OBJ)
	@echo "Compiling sources..."
	cargo build --features=c-interface
	gcc -o $@ $^ $(CFLAGS) -L$(LDIR) $(LIBS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: simple

clean:
	@echo "Cleaning mess"
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
