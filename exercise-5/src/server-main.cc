#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "net/chat-server.h"

int main() {
  const int kPort = 8080;

  Server server;
  int my_socket = server.create_server_socket();
  sockaddr_in address = server.create_server_address(kPort);

  // start listening on the socket
  server.bind_address_to_socket(my_socket, address);
  server.listen_on_socket(my_socket);

  std::cout << "Server listening on port " << kPort << "\n";
  server.handle_connections(my_socket, address);
  close(my_socket);

  return 0;
}
