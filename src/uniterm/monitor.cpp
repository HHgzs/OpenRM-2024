#include "uniterm/uniterm.h"
#include "structure/shm.hpp"
#include <thread>
#include <chrono>
#include <curses.h>
#include <string>
#include <vector>

void rm::monitor(std::vector<std::string>& key_name) {
    std::vector<MsgImg*> msg_img(key_name.size(), nullptr);
    for (size_t i = 0; i < key_name.size(); i++) {
        std::string img_shm_name = ImgMsgShmKey + key_name[i];
        msg_img[i] = SharedMemory<MsgImg>(img_shm_name, ImgShmLen);
    }
    
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        erase();
        int rows, cols;
        getmaxyx(stdscr, rows, cols);

        for (size_t i = 0; i < key_name.size(); i++) {
            for (size_t j = 0; j < ImgShmLen; j++) {
                MsgImg msg = msg_img[i][j];
                std::string str = msg.str;

                if (msg.type[0] == 'r') {
                    int up = std::round(msg.rect[0] * rows) - 1;
                    int down = std::round(msg.rect[1] * rows) + 1;
                    int left = std::round(msg.rect[2] * cols) - 3;
                    int right = std::round(msg.rect[3] * cols) + 3;

                    for (int y = up; y <= down; y++) {
                        mvprintw(y, left, "|");
                        mvprintw(y, right, "|");
                    }
                    for (int x = left; x <= right; x++) {
                        mvprintw(up, x, "-");
                        mvprintw(down, x, "-");
                    }
                    
                    mvprintw(up - 1, left, str.c_str());

                } else if (msg.type[0] == 'p') {
                    int str_x = cols, str_y = 0;
                    attron(COLOR_PAIR(5));
                    for (int i = 0; i < 4; i++) {
                        int x = std::round(msg.rect[i * 2] * cols);
                        int y = std::round(msg.rect[i * 2 + 1] * rows);
                        str_x = std::min(str_x, x);
                        str_y = std::max(str_y, y);
                        mvprintw(y, x, "O");
                    }
                    mvprintw(str_y + 3, str_x, str.c_str());
                    attroff(COLOR_PAIR(5));

                } else if (msg.type[0] == 'x') {
                    attron(COLOR_PAIR(6));
                    int x = std::round(msg.rect[0] * cols);
                    int y = std::round(msg.rect[1] * rows);
                    mvprintw(y, x, "X");
                    mvprintw(2, 2, str.c_str());
                    attroff(COLOR_PAIR(6));
                } else {
                    break;
                }
            }
        }
        refresh();
    }
}