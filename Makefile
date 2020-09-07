CFLAGS= -Wfatal-errors
LDFLAGS=
OBJS= main.o mod.o wav.o

all: readmod
readmod: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $<
%.o: %.c
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $<
clean:
	$(RM) *.o $(OBJS) readmod
