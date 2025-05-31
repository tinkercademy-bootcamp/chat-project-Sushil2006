#include "chat-sockets.h"

int SocketClass::create_socket(){
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  check_error(sock < 0, "Socket creation error\n");
  return sock;
}

sockaddr_in SocketClass::create_address(int port) {
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  return address;
}
