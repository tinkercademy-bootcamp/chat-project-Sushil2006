#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <string>
#include <map>

namespace tt::chat::server {

struct ClientData;

class Server {
public:
  Server(int port);
  ~Server();
  void handle_connections();

private:
  int socket_;
  sockaddr_in address_;
  int epoll_fd;

  // map from client fd to client object
  std::map<int, ClientData*> fd_to_client_map;

  // map from each channel name to #of users in channel
  std::map<std::string, int> channel_map;
  
  static constexpr int MAX_EVENTS = 10;
  static constexpr int kBufferSize = 256;

  epoll_event events[MAX_EVENTS];

  void handle_accept(int sock);
  static void set_socket_options(int sock, int opt);
  int make_socket_non_blocking(int fd);
  void epoll_init();
  void accept_all_incoming_client_connections();
  void modify_client_status(ClientData* client_ptr, uint32_t events);
  void send_message(ClientData* client_ptr, std::string message);
  void send_message_to_all_in_channel(ClientData* client_ptr, std::string &message);
  void disconnect_client(int fd);
  std::pair<std::string, std::string> split_command_and_message(std::string &curr_client_message);
  void handle_client_command(ClientData* curr_client_ptr, std::string &curr_client_message);
  void handle_username_command(ClientData* curr_client_ptr, std::string &username);
  void handle_create_channel_command(ClientData* curr_client_ptr, std::string &channel_name);
  void handle_channel_switch_command(ClientData* curr_client_ptr, std::string &channel_name);
};
} // namespace tt::chat::server

#endif // CHAT_SERVER_H
