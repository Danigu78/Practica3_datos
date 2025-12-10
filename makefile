CC = gcc
CFLAGS = -Wall -g

TARGET = library

OBJS = library.o data.o index.o deletedlist.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

library.o: library.c data.h index.h deletedlist.h
	$(CC) $(CFLAGS) -c library.c

data.o: data.c data.h index.h
	$(CC) $(CFLAGS) -c data.c

index.o: index.c index.h
	$(CC) $(CFLAGS) -c index.c

deletedlist.o: deletedlist.c deletedlist.h
	$(CC) $(CFLAGS) -c deletedlist.c

firstfit: $(TARGET)
	./$(TARGET) first_fit test

bestfit: $(TARGET)
	./$(TARGET) best_fit test

worstfit: $(TARGET)
	./$(TARGET) worst_fit test




valgrind-firstfit: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET) first_fit test

valgrind-bestfit: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET) best_fit test

valgrind-worstfit: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET) worst_fit test


clean:
	rm -f $(TARGET) *.o test.db test.ind test.lst midemo.db midemo.ind midemo.lst
