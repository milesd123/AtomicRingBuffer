# MAC 
CC=clang++ -Werror -O3 -std=c++14 -Isrc/external/asio-1.36.0/include/

# Linux

main: src/build/program
test: src/build/test

src/build/test: src/build/test.o src/build/ProxyUtils.o src/build/AsioSocket.o src/build/TestSocket.o src/build/AtomicSPSCQueue.o
	$(CC) $^ -o $@

src/build/program: src/build/main.o src/build/ProxyUtils.o src/build/AsioSocket.o src/build/TestSocket.o src/build/AtomicSPSCQueue.o
	$(CC) $^ -o $@

src/build/ProxyUtils.o: src/code/sources/ProxyUtils.cpp
	$(CC) -c $^ -o $@

src/build/AsioSocket.o: src/code/sources/AsioSocket.cpp
	$(CC) -c $^ -o $@

src/build/TestSocket.o: src/code/sources/TestSocket.cpp
	$(CC) -c $^ -o $@

src/build/AtomicSPSCQueue.o: src/code/sources/AtomicSPSCQueue.cpp
	$(CC) -c $^ -o $@


src/build/main.o: src/examples/main.cpp
	$(CC) -c $^ -o $@

src/build/test.o: src/examples/test.cpp
	$(CC) -c $^ -o $@

clean:
	rm -rf src/build/*