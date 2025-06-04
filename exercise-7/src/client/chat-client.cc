#include "chat-client.h"
#include "../net/chat-sockets.h"
#include "../utils.h"

tt::chat::client::Client::Client(int port,
                                         const std::string &server_address)
    : socket_{tt::chat::net::create_socket()} {
  sockaddr_in address = create_server_address(server_address, port);
  connect_to_server(socket_, address);
}

int tt::chat::client::Client::get_socket_fd(){
  return socket_;
}

std::string tt::chat::client::Client::send_and_receive_message(
    const std::string &message) {
  using namespace tt::chat;
  char recv_buffer[kBufferSize] = {0};

  // Send the message to the server
  send(socket_, message.c_str(), message.size(), 0);
  std::cout << "Sent: " << message << "\n";

  // Receive response from the server
  ssize_t read_size = read(socket_, recv_buffer, kBufferSize);
  if (read_size > 0) {
    return std::string(recv_buffer);
  } else if (read_size == 0) {
    return "Server closed connection.\n";
  } else {
    return "Read error.\n";
  }
}

void tt::chat::client::Client::set_username(const std::string &username) {
  // Send the message to the server
  std::string message = "/username " + username;
  send(socket_, message.c_str(), message.size(), 0);
}

tt::chat::client::Client::~Client() { close(socket_); }

sockaddr_in tt::chat::client::Client::create_server_address(
    const std::string &server_ip, int port) {
  using namespace tt::chat;
  sockaddr_in address = net::create_address(port);
  // Convert the server IP address to a binary format
  auto err_code = inet_pton(AF_INET, server_ip.c_str(), &address.sin_addr);
  check_error(err_code <= 0, "Invalid address/ Address not supported\n");
  return address;
}

void tt::chat::client::Client::connect_to_server(
    int sock, sockaddr_in &server_address) {
  using namespace tt::chat;
  auto err_code =
      connect(sock, (sockaddr *)&server_address, sizeof(server_address));
  check_error(err_code < 0, "Connection Failed.\n");
}

void tt::chat::client::Client::receiver_thread(std::vector<std::string> &messages, std::mutex &msg_mutex){
  while(true){
    ssize_t bytes_count = recv(socket_, message_buffer, sizeof(message_buffer), 0);
    if(bytes_count <= 0) break;
    update_messages(messages, msg_mutex, bytes_count);
  }
}

void tt::chat::client::Client::update_messages(std::vector<std::string> &messages, std::mutex &msg_mutex, int bytes_count){
  std::lock_guard<std::mutex> lock(msg_mutex);

  for(int i = 0; i < bytes_count; ++i){
    char ch = message_buffer[i];
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