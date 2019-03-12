CC=cc -g

STATIC_LIBRARIES=/usr/local/lib/libuv.a -pthread


all: hello-static idle_basic fs uvtee server client

hello-static: hello.c
	$(CC) -o hello-static hello.c $(STATIC_LIBRARIES)

idle_basic: idle_basic.c
	$(CC) -o idle_basic idle_basic.c $(STATIC_LIBRARIES)

fs: fs.c
	$(CC) -o fs fs.c $(STATIC_LIBRARIES)


uvtee: uvtee.c
	$(CC) -o uvtee uvtee.c $(STATIC_LIBRARIES)

server: server.c
	$(CC) -o server server.c $(STATIC_LIBRARIES)

client:	client.c
	$(CC) -o client client.c $(STATIC_LIBRARIES)
