#ifndef __OPENRM_UNITERM_UNITERM_H__
#define __OPENRM_UNITERM_UNITERM_H__

#include <string>
#include <vector>
#include <cstdint>
#include <opencv2/opencv.hpp>

namespace rm {

enum MSG {
    MSG_NOTE,
    MSG_OK,
    MSG_WARNING,
    MSG_ERROR,
};

union msg_type {
    int    i;
    float  f;
    double d;
    char   c;
    msg_type() {}
};

struct MsgNum {
    char name[15];
    char type;
    msg_type num;
};

struct MsgStr {
    char type;
    char str[63];
};

struct MsgImg {
    char   str[31];
    char   type[1];
    float  rect[8];
};

extern std::string NumMsgShmKey;
extern std::string StrMsgShmKey;
extern std::string ImgMsgShmKey;

extern size_t NumShmLen;
extern size_t StrShmLen;
extern size_t ImgShmLen;

void message(const std::string& name, int msg);
void message(const std::string& name, float msg);
void message(const std::string& name, double msg);
void message(const std::string& name, char msg);
void message(const std::string& name, MsgNum msg);
void message(const std::string& msg, MSG type = MSG_NOTE);
void message(const std::string& info, int img_width, int img_height, cv::Rect rect);
void message(const std::string& info, int img_width, int img_height, std::vector<cv::Point2f> four_points);
void message(const std::string& info, int img_width, int img_height, cv::Point2f pointX);

void message_init(const std::string& unique_name);
void message_send();

int term_hash(const char*);
void term_init();

void dashboard(std::vector<std::string>& key_name);
void oscilloscope(const std::vector<std::string>& key_name, const std::vector<std::string>& msg_name);
void monitor(std::vector<std::string>& key_name);

}


#endif