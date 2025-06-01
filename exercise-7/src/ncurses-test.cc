#include <ncurses.h>
#include <string>
#include <sstream>

void print_multiline(WINDOW* win, const std::string& text, int start_y, int start_x) {
    std::istringstream stream(text);
    std::string line;
    int y = start_y;

    while (std::getline(stream, line)) {
        mvwprintw(win, y++, start_x, "%s", line.c_str());
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height = 10;
    int width = COLS;
    int input_height = 3;

    // Create display window (top)
    WINDOW* display_win = newwin(height, width, 0, 0);
    box(display_win, 0, 0);
    mvwprintw(display_win, 1, 1, "Display Window:");
    std::string message = "line1\nline2\nline3\nline4";
    print_multiline(display_win, message, 2, 2);  // start printing at (2, 2)
    wrefresh(display_win);

    // Create input window (bottom)
    WINDOW* input_win = newwin(input_height, width, height, 0);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "Input: ");
    wrefresh(input_win);

    // Wait for user input
    char input_buf[256];
    wgetnstr(input_win, input_buf, sizeof(input_buf) - 1);

    delwin(display_win);
    delwin(input_win);
    endwin();

    return 0;
}
