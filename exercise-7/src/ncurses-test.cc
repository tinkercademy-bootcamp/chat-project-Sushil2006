#include <ncurses.h>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

// Global message buffer and synchronization
std::vector<std::string> messages;
std::mutex msg_mutex;
std::atomic<bool> running(true);

// Constants
const int input_height = 3;

void dummy_receiver() {
    int count = 0;
    while (running) {
        {
            std::lock_guard<std::mutex> lock(msg_mutex);
            messages.push_back("Server: message " + std::to_string(count++));
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void ui_loop() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

    int height, width;
    getmaxyx(stdscr, height, width);

    int chat_height = height - input_height;

    WINDOW* chat_win = newwin(chat_height, width, 0, 0);
    WINDOW* input_win = newwin(input_height, width, chat_height, 0);

    scrollok(chat_win, TRUE);
    nodelay(input_win, TRUE);  // Make input window non-blocking

    std::string input_line;

    while (running) {
        // --- Draw chat window ---
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

        // --- Draw input window ---
        werase(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 2, "You: %s", input_line.c_str());
        wrefresh(input_win);

        // --- Handle user input ---
        int ch = wgetch(input_win);
        if (ch != ERR) {
            if (ch == '\n') {
                if (input_line == "/quit") {
                    running = false;
                    break;
                }
                std::lock_guard<std::mutex> lock(msg_mutex);
                messages.push_back("You: " + input_line);
                input_line.clear();
            } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (!input_line.empty()) input_line.pop_back();
            } else if (isprint(ch)) {
                input_line.push_back(ch);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    delwin(chat_win);
    delwin(input_win);
    endwin();
}

int main() {
    std::thread recv_thread(dummy_receiver);
    std::thread ui_thread(ui_loop);

    ui_thread.join();
    running = false;
    recv_thread.join();

    return 0;
}
