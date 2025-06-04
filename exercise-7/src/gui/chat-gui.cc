#include "chat-gui.h"
#include "../utils.h"

tt::chat::gui::Gui::Gui(int port){
  socket_ = port;
  initialize_screen();
  set_chat_height();
  initialize_windows();
  ui_loop();
}

void tt::chat::gui::Gui::initialize_screen(){
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(1);
}

void tt::chat::gui::Gui::set_chat_height(){
  int height, width;
  getmaxyx(stdscr, height, width);
  chat_height = height - input_height;
}

void tt::chat::gui::Gui::initialize_windows(){
  chat_win = newwin(chat_height, width, 0, 0);
  input_win = newwin(input_height, width, chat_height, 0);

  scrollok(chat_win, TRUE);
  nodelay(input_win, TRUE);  // make input window non-blocking
}

void tt::chat::gui::Gui::ui_loop(){
  while (true) {
    draw_chat_window();
    draw_input_window();
    handle_user_input();

    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }
}

void tt::chat::gui::Gui::draw_chat_window(std::vector<std::string>> &messages){
  werase(chat_win);
  box(chat_win, 0, 0);
  int start_line = 1;
  int max_lines = chat_height - 2;
  int total_msgs = messages.size();
  int first_msg = std::max(0, total_msgs - max_lines);
  for (int i = first_msg; i < total_msgs; ++i) {
      mvwprintw(chat_win, start_line++, 2, "%s", messages[i].c_str());
  }
  wrefresh(chat_win);
}

void tt::chat::gui::Gui::draw_input_window(){
  werase(input_win);
  box(input_win, 0, 0);
  mvwprintw(input_win, 1, 2, "You: %s", input_line.c_str());
  wrefresh(input_win);
}

void tt::chat::gui::Gui::handle_user_input(){
  int ch = wgetch(input_win);
  if (ch != ERR) {
    if (ch == '\n') {
      send(socket_, input_line.c_str(), input_line.size(), 0);
      input_line.clear();
    } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
      if (!input_line.empty()) input_line.pop_back();
    } else if (isprint(ch)) {
      input_line.push_back(ch);
    }
  }
}