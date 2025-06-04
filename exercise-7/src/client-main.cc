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
std::atomic<bool> running(true);

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

const int input_height = 3;
int chat_height;

// initialize chat screen
void initialize_screen(){
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(1);
}

// draw chat window
void draw_chat_window(WINDOW* chat_win){
  werase(chat_win);
  box(chat_win, 0, 0);
  {
    std::lock_guard<std::mutex> lock(msg_mutex);
    int start_line = 1;
    int max_lines = chat_height - 2;
    int total_msgs = messages.size();
    int first_msg = std::max(0, total_msgs - max_lines);
    for (int i = first_msg; i < total_msgs; ++i) {
        mvwprintw(chat_win, start_line++, 2, "%s", messages[i].c_str());
    }
  }
  wrefresh(chat_win);
}

std::string input_line = "";

// draw input window
void draw_input_window(WINDOW* input_win){
  werase(input_win);
  box(input_win, 0, 0);
  mvwprintw(input_win, 1, 2, "You: %s", input_line.c_str());
  wrefresh(input_win);
}

void handle_user_input(WINDOW* input_win, int sock_fd){
  int ch = wgetch(input_win);
  if (ch != ERR) {
    if (ch == '\n') {
      if (input_line == "/quit") {
        running = false;
      }
      else{
        send(sock_fd, input_line.c_str(), input_line.size(), 0);
        input_line.clear();
      }
    } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
      if (!input_line.empty()) input_line.pop_back();
    } else if (isprint(ch)) {
      input_line.push_back(ch);
    }
  }
}

void ui_loop(int sock_fd) {
  initialize_screen();

  int height, width;
  getmaxyx(stdscr, height, width);
  chat_height = height - input_height;

  WINDOW* chat_win = newwin(chat_height, width, 0, 0);
  WINDOW* input_win = newwin(input_height, width, chat_height, 0);

  scrollok(chat_win, TRUE);
  nodelay(input_win, TRUE);  // make input window non-blocking

  while (running) {
    draw_chat_window(chat_win);
    draw_input_window(input_win);
    handle_user_input(input_win, sock_fd);

    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }

  delwin(chat_win);
  delwin(input_win);
  endwin();
}

int main(int argc, char *argv[]) {
  const int kPort = 8080;
  const std::string kServerAddress = "127.0.0.1";

  std::string username = read_args(argc, argv);
  tt::chat::client::Client client{kPort, kServerAddress};
  client.set_username(username);

  std::thread recv_thread(receiver_thread, client.get_socket_fd());
  std::thread ui_thread(ui_loop, client.get_socket_fd());

  recv_thread.join();
  ui_thread.join();

  return 0;
}
