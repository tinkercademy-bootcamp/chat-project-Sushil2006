#include <unistd.h>

#include "spdlog/spdlog.h"

#include "../net/chat-sockets.h"
#include "../utils.h"
#include "chat-server.h"

tt::chat::server::Server::Server(int port)
    : socket_(tt::chat::net::create_socket()),
      address_(tt::chat::net::create_address(port)) {
  using namespace tt::chat;
  set_socket_options(socket_, 1);

  address_.sin_addr.s_addr = INADDR_ANY;

  auto err_code = bind(socket_, (sockaddr *)&address_, sizeof(address_));
  check_error(err_code < 0, "bind failed\n");

  err_code = listen(socket_, 3);
  check_error(err_code < 0, "listen failed\n");

  err_code = make_socket_non_blocking(socket_);
  check_error(err_code < 0, "socket non blocking failed\n");

  epoll_init();

  std::cout << "Server listening on port " << port << "\n";
}

tt::chat::server::Server::~Server() { close(socket_); }

void tt::chat::server::Server::handle_connections() {
  socklen_t address_size = sizeof(address_);

  while (true) {
    int accepted_socket = accept(socket_, (sockaddr *)&address_, &address_size);
    tt::chat::check_error(accepted_socket < 0, "Accept error n ");
    handle_accept(accepted_socket);
  }
}

void tt::chat::server::Server::set_socket_options(int sock, int opt) {
  using namespace tt::chat;
  auto err_code = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                             &opt, sizeof(opt));
  check_error(err_code < 0, "setsockopt() error\n");
}

void tt::chat::server::Server::handle_accept(int sock) {
  using namespace tt::chat;

  char buffer[kBufferSize] = {0};
  ssize_t read_size = read(sock, buffer, kBufferSize);

  if (read_size > 0) {
    SPDLOG_INFO("Received: {}", buffer);
    send(sock, buffer, read_size, 0);
    SPDLOG_INFO("Echo message sent");
  } else if (read_size == 0) {
    SPDLOG_INFO("Client disconnected.");
  } else {
    SPDLOG_ERROR("Read error on client socket {}", socket_);
  }
  close(sock);
}

// epoll functionality implemented below

// makes socket non-blocking
int tt::chat::server::Server::make_socket_non_blocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// initialize epoll instance
void tt::chat::server::Server::epoll_init(){
  // create epoll instance
  epoll_fd = epoll_create1(0);
  check_error(epoll_fd < 0, "epoll init failed\n");

  // event to check for incoming connection requests to server socket
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.fd = socket_;

  // add the event to the list maintained by epoll_fd
  int err_code = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_, &ev);
  check_error(err_code < 0, "add server_fd to epoll_fd failed\n");
}