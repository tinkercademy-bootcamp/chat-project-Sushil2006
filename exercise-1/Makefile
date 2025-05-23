all: build/client build/server

build/client: src/tcp_echo_client.cc | build
	g++ -std=c++20 src/tcp_echo_client.cc -o build/client

build/server: src/tcp_echo_server.cc | build
	g++ -std=c++20 src/tcp_echo_server.cc -o build/server

build:
	mkdir -p build

clean:
	rm -rf build 
