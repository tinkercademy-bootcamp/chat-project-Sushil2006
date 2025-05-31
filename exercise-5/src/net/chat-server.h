#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "chat-sockets.h"

class Server : SocketClass {
public:

  void set_socket_options(int sock, int opt);
  int create_server_socket();
  void bind_address_to_socket(int sock, sockaddr_in &address);
  void listen_on_socket(int sock);
  void handle_accept(int sock);
  sockaddr_in create_server_address(int port);
  void handle_connections(int sock, sockaddr_in &address);
};

#endif