#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <ncurses.h>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

#include "client/chat-client.h"
#include "gui/chat-gui.h"

namespace {
std::string read_args(int argc, char *argv[]) {
  using namespace tt::chat;
  std::string message = "Hello from client";
  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " <username>\n";
    exit(EXIT_FAILURE);
  }
  if (argc > 1) {
    message = argv[1];
  }
  return message;
}
} // namespace

// Global message buffer and synchronization
std::vector<std::string> messages;
std::mutex msg_mutex;

void update_messages(char buffer[], int bytes_count){
  std::lock_guard<std::mutex> lock(msg_mutex);

  for(int i = 0; i < bytes_count; ++i){
    char ch = buffer[i];
    if(messages.empty()) messages.push_back("");
    if(ch == '\n'){
      if(messages.back() == "/clear_history"){
        messages.clear();
      }
      messages.push_back("");
    }
    else{
      messages.back().push_back(ch);
    }
  }
}

void receiver_thread(int sock_fd){
  char buffer[256];
  while(true){
    ssize_t bytes_count = recv(sock_fd, buffer, sizeof(buffer), 0);
    if(bytes_count <= 0) break;
    update_messages(buffer, bytes_count);
  }
}

int main(int argc, char *argv[]) {
  const int kPort = 8080;
  const std::string kServerAddress = "127.0.0.1";

  std::string username = read_args(argc, argv);
  tt::chat::client::Client client{kPort, kServerAddress};
  client.set_username(username);

  tt::chat::gui::Gui gui{client.get_socket_fd()};

  std::thread recv_thread(receiver_thread, client.get_socket_fd());
  std::thread ui_thread(&tt::chat::gui::Gui::ui_loop, &gui, std::ref(messages), std::ref(msg_mutex));

  recv_thread.join();
  ui_thread.join();

  return 0;
}
