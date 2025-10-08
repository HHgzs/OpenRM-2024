#include "uniterm/uniterm.h"
#include "structure/shm.hpp"
#include <thread>
#include <chrono>
#include <curses.h>
#include <string>
#include <vector>
#include <map>

static rm::MsgNum find_msg_num(std::vector<rm::MsgNum*>& msg_num_vec, int msg_name_hash) {
    for(size_t i = 0; i < msg_num_vec.size(); i++) {
        for(size_t j = 0; j < rm::NumShmLen; j++) {
            if (msg_num_vec[i][j].name[0] == '\0') break;
            int hash = rm::term_hash(msg_num_vec[i][j].name);
            if (hash == msg_name_hash) return msg_num_vec[i][j];
        }
    }
    return rm::MsgNum();
}

static double cast_to_double(rm::MsgNum& msg_num) {
    double msg_num_val;
    switch(msg_num.type) {
        case 'i':
            msg_num_val = static_cast<double>(msg_num.num.i);
            break;
        case 'f':
            msg_num_val = static_cast<double>(msg_num.num.f);
            break;
        case 'd':
            msg_num_val = msg_num.num.d;
            break;
        case 'c':
            msg_num_val = static_cast<double>(msg_num.num.c);
            break;
    }
    return msg_num_val;
}

void rm::oscilloscope(const std::vector<std::string>& key_name, const std::vector<std::string>& msg_name) {

    std::vector<MsgNum*> msg_num_vec(key_name.size(), nullptr);
    for(size_t i = 0; i < key_name.size(); i++) {
        std::string num_shm_name = NumMsgShmKey + key_name[i];
        msg_num_vec[i] = SharedMemory<MsgNum>(num_shm_name, NumShmLen);
    }

    std::vector<int> msg_hash_vec(msg_name.size(), 0);
    for(size_t i = 0; i < msg_name.size(); i++) {
        msg_hash_vec[i] = term_hash(msg_name[i].c_str());
    }

    int cursor = 0;
    std::vector<std::vector<double>> vec(msg_name.size(), std::vector<double>(100, 0.0));

    while(true) {

        int w, h, x, y;
        int canvas_w, canvas_h;
        double min = 0.0, max = 0.0;
        
        erase();
        getmaxyx(stdscr, h, w);
        canvas_w = w - 6;
        canvas_h = h - 3;

        // 更新游标
        cursor = (cursor + 1) % canvas_w;

        // 更新数字存储窗口
        min = vec[0][0];
        max = vec[0][0];
        for (size_t i = 0; i < msg_name.size(); i++) {

            // 扩充数字存储窗口大小
            if (vec[i].size() < (size_t)canvas_w) vec[i].resize(canvas_w, 0.0);
            
            // 获取消息参数
            int msg_name_hash = msg_hash_vec[i];
            MsgNum msg_num = find_msg_num(msg_num_vec, msg_name_hash);
            
            // 更新数字存储窗口
            vec[i][cursor] = cast_to_double(msg_num);
            
            // 更新最大最小值
            for(int j = 0; j < canvas_w; j++) {
                if (vec[i][j] < min) min = vec[i][j];
                if (vec[i][j] > max) max = vec[i][j];
            }
        }

        // 绘制坐标轴
        for (int i = 0; i < h - 1; i++) mvprintw(i, 5, "|");
        for (int i = 6; i < w; i++)     mvprintw(h - 2, i, "_");
        for (int i = 6; i < w; i += 10) mvprintw(h - 1, i, "|");
        for (int i = 0; i < h - 2; i++) mvprintw(i, cursor + 7, "|");
        for (int i = 0; i < canvas_h; i += 5) {
            const char* str = std::to_string(max - (max - min) / canvas_h * i).substr(0, 5).c_str();
            mvprintw(i, 0, str);
        }

        // 绘制波形图
        for (size_t i = 0; i < msg_name.size(); i++) {
            for (int j = 0; j < canvas_w; j++) {
                double val = vec[i][j];
                int x = j + 6;
                int y = canvas_h - (val - min) / (max - min) * canvas_h;
                int c = (i % 4) + 5;
                if (j == cursor + 1) continue;

                attron(COLOR_PAIR(c));
                mvprintw(y, x, "*");
                attroff(COLOR_PAIR(c));
            }
        }
        refresh();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}