OUT = communication
CC = gcc
ODIR = build
SDIR = ../../
CFLAGS=-Wall -I../../

OBJECTS=build/shared/algorithm/circular_buffer.o \
		build/shared/algorithm/map.o \
		build/shared/algorithm/set.o \
		build/shared/algorithm/vector.o \
		build/shared/utils.o \
		build/shared/communication/ethermini.o \
		build/shared/communication/message.o \
		build/shared/communication/utils/message_builder.o \
		build/interface/communication/serial.o \
		build/interface/communication/rs232.o

.PHONY: clean

all: $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o $(OUT)

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(ODIR)/* communication
