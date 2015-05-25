OUT = communication
CC = gcc
ODIR = build
SDIR = ..
CFLAGS=-Wall -I/home/sidor/projects/microgrid/ -Ilibserialport

OBJECTS=build/shared/algorithm/circular_buffer.o \
		build/shared/algorithm/map.o \
		build/shared/algorithm/set.o \
		build/shared/algorithm/vector.o \
		build/shared/utils.o \
		build/shared/communication/ethermini.o \
		build/shared/communication/message.o \
		build/shared/communication/utils/message_builder.o \
		build/interface2/serial.o


#OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(OUT): $(OBJECTS)
	libtool --mode=link $(CC) $(CFLAGS) main.c $(OBJECTS)  libserialport/libserialport.la -o $(OUT)

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf $(ODIR)/*
