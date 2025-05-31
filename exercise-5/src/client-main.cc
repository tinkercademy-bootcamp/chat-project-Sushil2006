#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "net/chat-client.h"

std::string read_args(int argc, char *argv[]) {
  std::string message = "Hello from client";
  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " <message>\n";
    exit(EXIT_FAILURE);
  }
  if (argc > 1) {
    message = argv[1];
  }
  return message;
}

int main(int argc, char *argv[]) {
  const int kPort = 8080;
  const std::string kServerAddress = "127.0.0.1";

  std::string message = read_args(argc, argv);

  Client client;
  int my_socket = client.create_socket();
  sockaddr_in server_address = client.create_server_address(kServerAddress, kPort);

  client.connect_to_server(my_socket, server_address);
  client.send_and_receive_message(my_socket, message);
  close(my_socket);

  return 0;
}
