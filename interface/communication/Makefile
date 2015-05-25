OUT = communication
CC = gcc
ODIR = build
SDIR = ../../
CFLAGS=-Wall -I/home/sidor/projects/microgrid/ -Ilibserialport

OBJECTS=build/shared/algorithm/circular_buffer.o \
		build/shared/algorithm/map.o \
		build/shared/algorithm/set.o \
		build/shared/algorithm/vector.o \
		build/shared/utils.o \
		build/shared/communication/ethermini.o \
		build/shared/communication/message.o \
		build/shared/communication/utils/message_builder.o \
		build/interface/communication/serial.o


all: $(OUT)

$(OUT): libserialport/libserialport.la $(OBJECTS)
	libtool --mode=link $(CC) $(CFLAGS) main.c $(OBJECTS)  libserialport/libserialport.la -o $(OUT)

libserialport/libserialport.la: | libserialport_build
	touch $@

libserialport_build:
	echo updating libserialport git repo; \
	git submodule init; \
	git submodule update; \
	echo entering libserialport dir; \
	cd libserialport; \
	echo autogen; \
	./autogen.sh; \
	./configure; \
	make; \
	cd ..


$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf $(ODIR)/* communication libserialport/libserialport.la
