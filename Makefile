# If your application is in a file named myapp.cpp or myapp.c
# this is the line you will need to build the binary.
all: client

client: client.cpp
	$(CC) -o $@ $< `pkg-config --cflags --libs libpjproject`

clean:
	rm -f client.o client
