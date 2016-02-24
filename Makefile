
# Project: pfind
# Author:  Beth Boose
#
# Makefile to build pfind
# CFLAGS:
#   -Wall  turns on a lot of useful warning messages
#   -Wextra turns on more warning messages missed by -Wall
#   -std=gnu99 use C99 for compiling
#
# VARIABLES:
#   TGT - name of the executable
#   SRC - class files files
#   DEPS - header files
#   OBJS -.o files generated from class files
#

CFLAGS = -Wall -Wextra -std=gnu99
TGT = pfind
SRC = $(wildcard *.c)
DEPS := $(wildcard *.h)
OBJS := $(SRC: .c=.o)
	
$(TGT): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm -f *.o core $(TGT)
	