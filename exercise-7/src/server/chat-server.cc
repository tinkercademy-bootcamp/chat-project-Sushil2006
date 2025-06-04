#include <unistd.h>

#include "spdlog/spdlog.h"

#include "../net/chat-sockets.h"
#include "../utils.h"
#include "chat-server.h"

struct tt::chat::server::ClientData{
  int fd = -1; // client's fd
  std::string username = ""; // client's username
  std::string send_buffer = ""; // buffer of the message that needs to be sent to the client
  std::string channel = "";
  ClientData(int fd_){
    fd = fd_;
    channel = "#general";
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

  err_code = listen(socket_, SOMAXCONN);
  check_error(err_code < 0, "listen failed\n");

  err_code = make_socket_non_blocking(socket_);
  check_error(err_code < 0, "socket non blocking failed\n");

  epoll_init();

  std::cout << "Server listening on port " << port << "\n";
}

tt::chat::server::Server::~Server() { close(socket_); }

void tt::chat::server::Server::send_message(ClientData* client_ptr, std::string message){
  client_ptr->send_buffer += message;

  // update client's epoll event to write
  epoll_event client_ev{};
  client_ev.events = EPOLLIN | EPOLLOUT;
  client_ev.data.ptr = static_cast<void*>(client_ptr);

  // modify status in epoll_fd list to read and write
  epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_ptr->fd, &client_ev);
}

void tt::chat::server::Server::send_message_to_all_in_channel(ClientData* client_ptr, std::string &message){
  for(auto &[mp_client_fd, mp_client_ptr] : fd_to_client_map){
    // send message to all of them by updating client's buffer
    if(mp_client_ptr->channel == client_ptr->channel){
      send_message(mp_client_ptr, message);
    }
  }
}

void tt::chat::server::Server::accept_all_incoming_client_connections(){
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
    int err_code = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
    check_error(err_code < 0, "EPOLL_CTL_ADD failed for new client\n");
  } 
}

void tt::chat::server::Server::handle_connections() {
  using namespace tt::chat;
  socklen_t address_size = sizeof(address_);

  while (true) {
    int ready_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    check_error(ready_count < 0, "epoll wait failed\n");

    // process the ready_count events that are ready to be handled
    for(int i = 0; i < ready_count; ++i){
      int fd = static_cast<ClientData*>(events[i].data.ptr)->fd; // get fd of event
      if(fd == socket_){
        // there is at least one incoming connection to server, accept all of them
        accept_all_incoming_client_connections();
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
            std::string message = "[SERVER]: User " + del_client_ptr->username + " has left " + del_client_ptr->channel + "\n";
            send_message_to_all_in_channel(del_client_ptr, message);
            
            // properly clean up the client
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr); // remove client from epoll_fd
            delete del_client_ptr; // delete client's ptr
            fd_to_client_map.erase(fd); // remove him from the map
            close(fd); // close his socket
          }
          else{
            std::string curr_client_message = std::string(buffer, buffer+count);
            std::string command = "", actual_message = "";
            std::string message = "";
            ClientData* curr_client_ptr = fd_to_client_map[fd];
            
            if(curr_client_message[0] == '/'){
              // some command is being sent
              bool space_came = false;

              for(auto& ch : curr_client_message){
                if(space_came){
                  actual_message.push_back(ch);
                }
                else if(ch == ' '){
                  space_came = true;
                }
                else{
                  command.push_back(ch);
                }
              }

              if(command == "/username"){
                if(curr_client_ptr->username.empty()){
                  curr_client_ptr->username = "[" + actual_message + "]";
                  message = "[SERVER]: User " + curr_client_ptr->username + " has joined " + curr_client_ptr->channel + "\n";
                  send_message_to_all_in_channel(curr_client_ptr, message);
                }
                else{
                  message = "[SERVER]: Can't change username\n";
                  send_message(curr_client_ptr, message);
                }
              }
              else if(command == "/create"){
                // channel creation
                if(actual_message.empty()){
                  send_message(curr_client_ptr, "[SERVER]: Channel name can't be empty\n");
                }
                else if(actual_message[0] != '#'){ 
                  send_message(curr_client_ptr, "[SERVER]: Channel name must begin with '#'\n");
                }
                else if(channel_map.count(actual_message)){
                  message = "[SERVER]: Channel " + actual_message + " already exists\n";
                  send_message(curr_client_ptr, message);
                }
                else{
                  message = "[SERVER]: Channel " + actual_message + " created\n";
                  send_message(curr_client_ptr, message);
                  channel_map[actual_message]++;
                }
              }
              else if(command == "/switch"){
                // channel switch
                if(!channel_map.count(actual_message)){
                  message = "[SERVER]: Channel " + actual_message + " doesn't exist\n";
                  send_message(curr_client_ptr, message);
                }
                else if(curr_client_ptr->channel == actual_message){
                  message = "[SERVER]: Already in channel " + actual_message + " \n";
                  send_message(curr_client_ptr, message);
                }
                else{
                  message = "[SERVER]: User " + curr_client_ptr->username + " has left " + curr_client_ptr->channel + "\n";
                  send_message_to_all_in_channel(curr_client_ptr, message);

                  channel_map[curr_client_ptr->channel]--;
                  channel_map[actual_message]++;

                  send_message(curr_client_ptr, "/clear_history\n"); // command to clear history, sent by server to client

                  curr_client_ptr->channel = actual_message;
                  message = "[SERVER]: User " + curr_client_ptr->username + " has joined " + curr_client_ptr->channel + "\n";
                  send_message_to_all_in_channel(curr_client_ptr, message);
                }
              }
              else{
                send_message(curr_client_ptr, "[SERVER]: Invalid command\n");
              }
            }
            else{
              message = curr_client_ptr->username + ": " + curr_client_message + "\n";
              send_message_to_all_in_channel(curr_client_ptr, message);
            }

            std::cout << "Server received:\n";
            std::cout << message;
          }
        }

        // write operation
        if(events[i].events & EPOLLOUT){
          ClientData* curr_client = static_cast<ClientData*>(events[i].data.ptr);
          if(!curr_client->send_buffer.empty()){
            ssize_t sent_count = send(fd, curr_client->send_buffer.c_str(), curr_client->send_buffer.size(), 0);
            
            // std::cout << "=====DEBUG=====" << std::endl;
            // std::cout << fd << std::endl;
            // std::cout << curr_client->send_buffer << std::endl;
            // std::cout << sent_count << std::endl;
            // std::cout << errno << std::endl;
            // std::cout << "===============" << std::endl;

            check_error(sent_count < 0, "server to client send error\n");
            curr_client->send_buffer.erase(0,sent_count);
          }

          if(curr_client->send_buffer.empty()){
            // no more data to send, so disable EPOLLOUT by setting to read only mode
            epoll_event client_ev;
            client_ev.events = EPOLLIN;
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

  // NEED TO CLEAN THIS UP
  ClientData* curr_ptr = new ClientData(socket_);
  ev.data.ptr = curr_ptr;

  // add the event to the list maintained by epoll_fd
  int err_code = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_, &ev);
  check_error(err_code < 0, "add server_fd to epoll_fd failed\n");
}