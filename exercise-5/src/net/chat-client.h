#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "chat-sockets.h"

class Client : public SocketClass {
public:

  sockaddr_in create_server_address(const std::string &server_ip, int port);
  void connect_to_server(int sock, sockaddr_in &server_address);
  void send_and_receive_message(int sock, const std::string &message);
};


#endif