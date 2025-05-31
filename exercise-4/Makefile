all: build/client build/server

build/client: src/tcp_echo_client.cpp | build
	g++ -std=c++20 src/tcp_echo_client.cpp src/functions.cpp -o build/client

build/server: src/tcp_echo_server.cpp | build
	g++ -std=c++20 src/tcp_echo_server.cpp src/functions.cpp -o build/server

debug: build/client_debug build/server_debug

build/client_debug: src/tcp_echo_client.cpp | build
	g++ -std=c++20 -g src/tcp_echo_client.cpp src/functions.cpp -o build/client_debug

build/server_debug: src/tcp_echo_server.cpp | build
	g++ -std=c++20 -g src/tcp_echo_server.cpp src/functions.cpp -o build/server_debug

build:
	mkdir -p build

clean:
	rm -rf build
