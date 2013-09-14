#
# Makefile for SPI OLED Sample
#
# Coded by Yasuhiro ISHII,4/15/2013
#

CROSS		 = 
CC		 = $(CROSS)gcc
CFLAGS		 = -O0 -I. -g
OBJS		 = oled.o fb.o gpiolib.o
TARGET		 = oled

.PHONY: clean purge all

all:	$(TARGET)

.c.o:
	@echo "========================================================"
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	@echo "Clean project =========================================="
	rm -f *.bak $(TARGET) $(OBJS)

purge:
	rm -f *~

depend: $(OBJS:.o=.c)
	-@ makedepend -- $(CFLAGS) -- -Y $^ -f Makefile 2> /dev/null

# DO NOT DELETE

oled.o: fb.h gpiolib.h
fb.o: fb.h
gpiolib.o: gpiolib.h
