#ifndef CHAT_GUI_H
#define CHAT_GUI_H

#include <sys/socket.h>
#include <ncurses.h>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>

namespace tt::chat::gui {
class Gui {
public:
  Gui(int port);
  void initialize_screen();
  void set_chat_height();
  void initialize_windows();
  void ui_loop(std::vector<std::string> &messages, std::mutex &msg_mutex);
  void draw_chat_window(std::vector<std::string> &messages, std::mutex &msg_mutex);
  void draw_input_window();
  void handle_user_input();
  ~Gui(); // TODO: NEED TO HANDLE DESTRUCTOR AND PROPER CLEANING UP OF VARIABLES

private:
  int socket_;
  const int input_height = 3;
  int chat_height;
  int height, width;
  WINDOW* chat_win = nullptr;
  WINDOW* input_win = nullptr;
  std::string input_line = "";
};
} // namespace tt::chat::gui

#endif // CHAT_GUI_H
