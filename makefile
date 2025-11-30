CC = gcc
CFLAGS = -Wall -g

TARGET = library

OBJS = library.o data.o index.o deletedlist.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

library.o: library.c data.h index.h deletedlist.h
	$(CC) $(CFLAGS) -c library.c

data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c

index.o: index.c index.h
	$(CC) $(CFLAGS) -c index.c

deletedlist.o: deletedlist.c deletedlist.h
	$(CC) $(CFLAGS) -c deletedlist.c

clean:
	rm -f $(TARGET) *.o

mrproper: clean
	rm -f *.db *.ind *.lst
