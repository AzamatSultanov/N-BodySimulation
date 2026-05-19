CC = g++
CFLAGS = -Wall -O3 -std=c++17 -I./external/raylib/include

LDFLAGS = ./external/raylib/lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

main: main.o bhtree.o body.o quad.o
	$(CC) -o main main.o bhtree.o body.o quad.o $(LDFLAGS)

main.o: src/main.cpp src/bhtree.hpp src/body.hpp src/quad.hpp
	$(CC) $(CFLAGS) -c src/main.cpp

bhtree.o: src/bhtree.cpp src/bhtree.hpp src/body.hpp src/quad.hpp
	$(CC) $(CFLAGS) -c src/bhtree.cpp

body.o: src/body.cpp src/body.hpp src/quad.hpp
	$(CC) $(CFLAGS) -c src/body.cpp

quad.o: src/quad.cpp src/quad.hpp
	$(CC) $(CFLAGS) -c src/quad.cpp

clean:
	rm -f *.o