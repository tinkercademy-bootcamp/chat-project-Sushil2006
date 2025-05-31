#ifndef CHAT_SOCKETS_H
#define CHAT_SOCKETS_H

#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class SocketClass {
public:
  template <typename T, typename S> void check_error(T test, S error_message);
  int create_socket();
  sockaddr_in create_address(int port);
};

template <typename T, typename S> 
void SocketClass::check_error(T test, S error_message) {
  if (test) {
    std::cerr << error_message << "\n";
    throw std::runtime_error(error_message);
  }
}

#endif