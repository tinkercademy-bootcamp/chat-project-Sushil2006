#include "chat-sockets.h"

class SocketClass {
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
