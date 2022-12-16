CC=gcc
CFLAGS=-g -Wall
TARGET=knumetheus
OBJS=src/str_helper.o src/knumetheus.o
 
$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/str_helper.o : src/str_helper.c
	$(CC) $(CFLAGS) -c -o src/str_helper.o src/str_helper.c

src/knumetheus.o : src/knumetheus.c
	$(CC) $(CFLAGS) -c -o src/knumetheus.o src/knumetheus.c

clean:
	rm $(OBJS) $(TARGET)
