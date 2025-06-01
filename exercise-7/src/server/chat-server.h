#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>

namespace tt::chat::server {

class Server {
public:
  Server(int port);
  ~Server();
  void handle_connections();

private:
  int socket_;
  sockaddr_in address_;
  int epoll_fd;

  static constexpr int MAX_EVENTS = 10;
  static constexpr int kBufferSize = 1024;

  epoll_event events[MAX_EVENTS];

  void handle_accept(int sock);
  static void set_socket_options(int sock, int opt);
  int make_socket_non_blocking(int fd);
  void epoll_init();
};
} // namespace tt::chat::server

#endif // CHAT_SERVER_H
