#include <unistd.h>

#include "spdlog/spdlog.h"

#include "../net/chat-sockets.h"
#include "../utils.h"
#include "chat-server.h"
#include <map>

struct ClientData{
  int fd = -1; // client's fd
  std::string username = ""; // client's username
  std::string send_buffer = ""; // buffer of the message that needs to be sent to the client
  ClientData(int fd_){
    fd = fd_;
    username = "Guy " + std::to_string(fd);
  }
};

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
  using namespace tt::chat;
  socklen_t address_size = sizeof(address_);

  // map from client fd to client object
  std::map<int, ClientData*> fd_to_client_map;
  
  while (true) {
    int ready_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    check_error(ready_count < 0, "epoll wait failed\n");

    // process the ready_count events that are ready to be handled
    for(int i = 0; i < ready_count; ++i){
      int fd = events[i].data.fd; // get fd of event
      if(fd == socket_){
        // there is at least one incoming connection to server
        // accept all of them, till possible
        while(true){
          // accepting a client connection
          sockaddr_in client_addr{};
          socklen_t client_len = sizeof(client_addr);
          int client_fd = accept(socket_, (sockaddr*)&client_addr, &client_len);

          if(client_fd == -1){
            // no more pending clients to be accepted
            break;
          }
          
          // make client socket non-blocking
          make_socket_non_blocking(client_fd);

          // create new client object
          ClientData* client_ptr = new ClientData(client_fd);
          fd_to_client_map[client_fd] = client_ptr;

          // add "read from client" event to epoll_fd
          epoll_event client_ev{};
          client_ev.events = EPOLLIN; // event type: read
          client_ev.data.ptr = static_cast<void*>(client_ptr); // store pointer to client object, so that more data about client can be stored
          client_ev.data.fd = client_fd; // store fd of client
          int err_code = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
          
          // acknowledge new client connected
          std::cout << "New client connected\n" << std::endl;
        } 
      }
      else{
        // read operation
        if(events[i].events & EPOLLIN){
          char buffer[1024];
          ssize_t count = recv(fd, buffer, sizeof(buffer), 0);
          
          if(count <= 0){
            // client disconnected

            // extract pointer to client
            ClientData* del_client_ptr = fd_to_client_map[fd];
            std::string message = "User " + del_client_ptr->username + " has left the channel\n";

            // send acknowledgement to everyone
            for(auto &[client_fd, client_ptr] : fd_to_client_map){
              if(client_fd == fd) continue;

              // send acknowledgement that guy is leaving by updating client's buffer
              client_ptr->send_buffer += message;

              // update client's epoll event to write
              epoll_event client_ev{};
              client_ev.events = EPOLLIN | EPOLLOUT;
              client_ev.data.fd = client_fd;
              client_ev.data.ptr = static_cast<void*>(client_ptr);

              // modify status in epoll_fd list to read and write
              epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &client_ev);
            }

            // properly clean up the client
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr); // remove client from epoll_fd
            delete del_client_ptr; // delete client's ptr
            fd_to_client_map.erase(fd); // remove him from the map
            close(fd); // close his socket
          }
          else{
            // send client's message to everybody
            ClientData* curr_client_ptr = fd_to_client_map[fd];
            std::string message = "[" + curr_client_ptr->username + "]: " + (std::string) buffer + "\n";
            std::cout << "Server received:\n";
            std::cout << message;

            // send message to everyone
            for(auto &[client_fd, client_ptr] : fd_to_client_map){
              if(client_fd == fd) continue;

              // send acknowledgement that guy is leaving by updating client's buffer
              client_ptr->send_buffer += message;

              // update client's epoll event to write
              epoll_event client_ev{};
              client_ev.events = EPOLLIN | EPOLLOUT;
              client_ev.data.fd = client_fd;
              client_ev.data.ptr = static_cast<void*>(client_ptr);

              // modify status in epoll_fd list to read and write
              epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &client_ev);
            }
          }
        }

        // write operation
        if(events[i].events & EPOLLOUT){
          ClientData* curr_client = static_cast<ClientData*>(events[i].data.ptr);
          if(!curr_client->send_buffer.empty()){
            ssize_t sent_count = send(fd, curr_client->send_buffer.c_str(), curr_client->send_buffer.size(), 0);
            check_error(sent_count < 0, "server to client send error\n");
            curr_client->send_buffer.erase(0,sent_count);
          }

          if(curr_client->send_buffer.empty()){
            // no more data to send, so disable EPOLLOUT by setting to read only mode
            epoll_event client_ev;
            client_ev.events = EPOLLIN;
            client_ev.data.fd = fd;
            client_ev.data.ptr = static_cast<void*>(curr_client);
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &client_ev);
          }
        }
      }
    }
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
  using namespace tt::chat;

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