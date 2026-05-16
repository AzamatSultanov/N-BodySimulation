CC = g++
CFLAGS = -Wall -std=c++14 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

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
	rm *.o