out: obj/main.o obj/client.o obj/memory.o obj/memory-private.o obj/process.o obj/proxy.o obj/server.o obj/synchronization.o obj/configuration.o obj/sotime.o obj/log.o
	gcc obj/main.o obj/client.o obj/memory.o obj/memory-private.o obj/process.o obj/proxy.o obj/server.o obj/synchronization.o obj/configuration.o obj/sotime.o obj/log.o -o sovac -lrt -lpthread

obj/memory-private.o: src/memory-private.c
	gcc -c src/memory-private.c -I include

obj/memory.o: src/memory.c
	gcc -c src/memory.c -I include

obj/synchronization.o: src/synchronization.c
	gcc -c src/synchronization.c -I include

obj/process.o: src/process.c
	gcc -c src/process.c -I include

obj/client.o: src/client.c
	gcc -c src/client.c -I include

obj/proxy.o:src/proxy.c
	gcc -c src/proxy.c -I include

obj/server.o: src/server.c
	gcc -c src/server.c -I include

obj/configuration.o: src/configuration.c
	gcc -c src/configuration.c -I include

obj/sotime.o: src/sotime.c
	gcc -c src/sotime.c -I include

obj/log.o: src/log.c
	gcc -c src/log.c -I include

obj/main.o: src/main.c
	gcc -c src/main.c -I include -Wimplicit-function-declaration
