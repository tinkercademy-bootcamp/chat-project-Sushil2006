#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <netinet/in.h>
#include <string>
#include <vector>
#include <mutex>

namespace tt::chat::client {
class Client {
public:
  Client(int port, const std::string &server_address);
  std::string send_and_receive_message(const std::string &message);
  void set_username(const std::string &username);
  ~Client();
  int get_socket_fd();
  void receiver_thread(std::vector<std::string> &messages, std::mutex &msg_mutex);
  void update_messages(std::vector<std::string> &messages, std::mutex &msg_mutex, int bytes_count);

  private:
  int socket_;
  sockaddr_in create_server_address(const std::string &server_ip, int port);
  void connect_to_server(int sock, sockaddr_in &server_address);

  static constexpr int kBufferSize = 256;
  char message_buffer[kBufferSize];
};
} // namespace tt::chat::client

#endif // CHAT_CLIENT_H
