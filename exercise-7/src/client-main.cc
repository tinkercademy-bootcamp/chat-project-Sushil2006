#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <mutex>

#include "client/chat-client.h"

namespace {
std::string read_args(int argc, char *argv[]) {
  using namespace tt::chat;
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
} // namespace



std::vector<std::string> messages;
std::mutex mtx;

void receiver_thread(int sock_fd){
  char buffer[1024];
  while(true){
    ssize_t bytes_count = recv(sock_fd, buffer, sizeof(buffer), 0);
    if(bytes_count <= 0) break;
    std::lock_guard<std::mutex> lock(mtx);

    for(int i = 0; i < bytes_count; ++i){
      char ch = buffer[i];
      if(messages.empty()) messages.push_back("");
      if(ch == '\n'){
        messages.push_back("");
      }
      else{
        messages.back().push_back(ch);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  const int kPort = 8080;
  const std::string kServerAddress = "127.0.0.1";

  std::string username = read_args(argc, argv);
  tt::chat::client::Client client{kPort, kServerAddress};

  while(true){
    std::cout << "Enter message:";
    std::string message;
    std::cin >> message;

    std::string response = client.send_and_receive_message(message);
  }

  return 0;
}
