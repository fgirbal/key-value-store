CC = gcc
CFLAGS = -g -Wall
OBJECTS = list.o psiskv_lib.o front-aux.o data-aux.o
LINKS = -lpthread

all: front-server data-server client

front-server : $(OBJECTS)
	$(CC) $(CFLAGS) front-server.c $(OBJECTS) -o front-server $(LINKS)

data-server : $(OBJECTS)
	$(CC) $(CFLAGS) data-server.c $(OBJECTS) -o data-server $(LINKS)

list.o : list.c
	$(CC) $(CFLAGS) -c list.c

psiskv_lib.o : psiskv_lib.c
	$(CC) $(CFLAGS) -c psiskv_lib.c

front-aux.o: front-aux.c
	$(CC) $(CFLAGS) -c front-aux.c $(LINKS)

data-aux.o: data-aux.c
	$(CC) $(CFLAGS) -c data-aux.c $(LINKS)

clean:
	rm -f $(OBJECTS) front-server data-server