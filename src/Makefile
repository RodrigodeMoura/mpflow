#
#	Makefile	WJ109
#
#	mpflow Copyright (c) 2009 by Walter de Jong <walter@heiho.net>
#

PREFIX=/usr
BINDIR=$(PREFIX)/bin

INSTALL=install

Q = @
OBJDIR = obj

CC = gcc

###
#
#	Debug version
#
##CFLAGS = -g -Wall -Wstrict-prototypes
##LFLAGS = -lSDL -lSDL_image -lSDL_gfx -lGL -lGLU

###
#
#	Production version
#
CFLAGS = -s -O2
LFLAGS = -lSDL -lSDL_image -lSDL_gfx -lGL -lGLU

INCLUDE = include -I/usr/include/SDL -I/usr/include/GL

RM = /bin/rm
RM_F = $(RM) -f
TOUCH = /usr/bin/touch

CFILES = $(wildcard *.c)
_OBJS = $(CFILES:.c=.o)
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

TARGET = mpflow

all: .depend $(TARGET)

include .depend

$(OBJDIR)/%.o: %.c
	@echo '  CC       ' $@
	$(Q)$(CC) -I$(INCLUDE) $(CFLAGS) -c $< -o $@

#
#	Targets
#

mpflow: $(OBJS)
	@echo '  BUILD     mpflow'
	$(Q)$(CC) $(OBJS) -o mpflow $(LFLAGS)

neat:
	@echo '  NEAT'
	$(Q)$(RM_F) ../core core *~

clean:
	@echo '  CLEAN'
	$(Q)$(RM_F) core ../core $(TARGET) $(OBJS) *~

mrproper: clean
	@echo '  MRPROPER'
	$(Q)$(RM_F) .depend config.status config.cache config.log
	$(Q)$(TOUCH) .depend

depend dep .depend:
	@echo '  DEP       .depend'
	$(Q)$(CC) -I$(INCLUDE) -M $(CFILES) > .depend

install: mpflow
	$(INSTALL) -m 0755 -s mpflow $(BINDIR)

# EOB
