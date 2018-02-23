CC = gcc
CFLAGS = -I$(IDIR) -g -Wall
IDIR =./include

EXEC1 = oss
OBJS1 = oss.o

EXEC2 = user
OBJS2 = user.o

SHARE = helpers.o

DEPS = global_constants.h helpers.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: oss user

oss: $(OBJS1) $(SHARE)
	gcc -o $(EXEC1) $^ $(CFLAGS)
	
user: $(OBJS2) $(SHARE)
	gcc -o $(EXEC2) $^ $(CFLAGS)

clean:
	rm $(EXEC1) $(OBJS1) $(EXEC2) $(OBJS2) $(SHARE)
