#include "chat-sockets.h"

class SocketClass {
public:
  template <typename T, typename S> void check_error(T test, S error_message) {
    if (test) {
      std::cerr << error_message << "\n";
      throw std::runtime_error(error_message);
    }
  }

  int create_socket(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    check_error(sock < 0, "Socket creation error\n");
    return sock;
  }

  sockaddr_in create_address(int port) {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    return address;
  }
};

class Server : SocketClass {
public:

  void set_socket_options(int sock, int opt) {
    auto err_code = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                              &opt, sizeof(opt));
    check_error(err_code < 0, "setsockopt() error\n");
  }

  int create_server_socket() {
    int sock = create_socket();
    set_socket_options(sock, 1);
    return sock;
  }

  void bind_address_to_socket(int sock, sockaddr_in &address) {
    auto err_code = bind(sock, (sockaddr *)&address, sizeof(address));
    check_error(err_code < 0, "bind failed\n");
  }

  void listen_on_socket(int sock) {
    auto err_code = listen(sock, 3);
    check_error(err_code < 0, "listen failed\n");
  }

  void handle_accept(int sock) {
    const int kBufferSize = 1024;
    char buffer[kBufferSize] = {0};
    ssize_t read_size = read(sock, buffer, kBufferSize);

    check_error(read_size < 0,
                    "Read error on client socket " + std::to_string(sock));
    if (read_size > 0) {
      std::cout << "Received:" << buffer << "\n";
      send(sock, buffer, read_size, 0);
      std::cout << "Echo message sent\n";
    } else if (read_size == 0) {
      std::cout << "Client disconnected.\n";
    } else {
      std::cerr << "Read error on client socket " << sock << "\n";
    }
    close(sock);
  }

  sockaddr_in create_server_address(int port) {
    sockaddr_in address = create_address(port);
    address.sin_addr.s_addr = INADDR_ANY;
    return address;
  }

  void handle_connections(int sock, sockaddr_in &address) {
    socklen_t address_size = sizeof(address);

    while (true) {
      int accepted_socket = accept(sock, (sockaddr *)&address, &address_size);
      check_error(accepted_socket < 0, "Accept error n ");
      handle_accept(accepted_socket);
    }
  }
};