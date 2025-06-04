#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

int main(int argc, char *argv[]) {
  const int kPort = 8080;
  const std::string kServerAddress = "127.0.0.1";

  std::string username = read_args(argc, argv);
  tt::chat::client::Client client{kPort, kServerAddress};
  client.set_username(username);

  tt::chat::gui::Gui gui{client.get_socket_fd()};

  std::vector<std::string> messages;
  std::mutex msg_mutex;

  std::thread recv_thread(&tt::chat::client::Client::receiver_thread, &client, std::ref(messages), std::ref(msg_mutex));
  std::thread ui_thread(&tt::chat::gui::Gui::ui_loop, &gui, std::ref(messages), std::ref(msg_mutex));

  recv_thread.join();
  ui_thread.join();

  return 0;
}
