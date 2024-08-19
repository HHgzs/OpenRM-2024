#include "uniterm/uniterm.h"
#include <map>
#include <iostream>
#include <vector>
#include <structure/shm.hpp>

std::string rm::NumMsgShmKey = "KeyNum_";
std::string rm::StrMsgShmKey = "KeyStr_";
std::string rm::ImgMsgShmKey = "KeyImg_";

size_t rm::NumShmLen = 64ull;
size_t rm::StrShmLen = 32ull;
size_t rm::ImgShmLen = 16ull;

static rm::MsgNum* shm_num;
static rm::MsgStr* shm_str;
static rm::MsgImg* shm_img;

static std::map<std::string, rm::MsgNum> MsgMap;
static std::vector<rm::MsgStr> StrVec(rm::StrShmLen);
static std::vector<rm::MsgImg> ImgVec(rm::ImgShmLen);

static int StrIndex = 0;
static int ImgIndex = 0;

void rm::message_init(const std::string& unique_name) {
    NumMsgShmKey += unique_name;
    StrMsgShmKey += unique_name;
    ImgMsgShmKey += unique_name;
    shm_num = SharedMemory<MsgNum>(NumMsgShmKey, NumShmLen);
    shm_str = SharedMemory<MsgStr>(StrMsgShmKey, StrShmLen);
    shm_img = SharedMemory<MsgImg>(ImgMsgShmKey, ImgShmLen);
    memset(shm_num, 0, sizeof(MsgNum) * NumShmLen);
    memset(shm_str, 0, sizeof(MsgStr) * StrShmLen);
    memset(shm_img, 0, sizeof(MsgImg) * ImgShmLen);
}
void rm::message(const std::string& name, int mi) {
    MsgNum msg;
    msg.num.i = mi;
    msg.type = 'i';
    message(name, msg);
}
void rm::message(const std::string& name, float mf) {
    MsgNum msg;
    msg.num.f = mf;
    msg.type = 'f';
    message(name, msg);  
}
void rm::message(const std::string& name, double md) {
    MsgNum msg;
    msg.num.d = md;
    msg.type = 'd';
    message(name, msg);
}
void rm::message(const std::string& name, char mc) {
    MsgNum msg;
    msg.num.c = mc;
    msg.type = 'c';
    message(name, msg);
}
void rm::message(const std::string& name, MsgNum msg) {
    size_t length = name.length();
    size_t copy_length = std::min(length, static_cast<size_t>(14));

    std::copy(name.begin(), name.begin() + copy_length, msg.name);
    msg.name[copy_length] = '\0';

    MsgMap[name] = msg;
}

void rm::message(const std::string& mstr, MSG type) {
    MsgStr msg;
    msg.type = static_cast<char>(type);
    if (type == rm::MSG_ERROR) std::cerr << mstr << std::endl;
    else std::cout << mstr << std::endl;
    
    size_t length = mstr.length();
    size_t copy_length = std::min(length, static_cast<size_t>(62));

    std::copy(mstr.begin(), mstr.begin() + copy_length, msg.str);
    msg.str[copy_length] = '\0';

    StrIndex %= StrShmLen;
    StrVec[StrIndex++] = msg;
    StrVec[StrIndex] = MsgStr();
}

void rm::message(const std::string& info, int img_width, int img_height, cv::Rect rect) {
    MsgImg msg;
    size_t length = info.length();
    size_t copy_length = std::min(length, static_cast<size_t>(30));

    std::copy(info.begin(), info.begin() + copy_length, msg.str);
    msg.str[copy_length] = '\0';

    msg.type[0] = 'r';

    msg.rect[0] = (float)rect.y / (float)img_height;
    msg.rect[1] = (float)(rect.y + rect.height) / (float)img_height;
    msg.rect[2] = (float)rect.x / (float)img_width;
    msg.rect[3] = (float)(rect.x + rect.width) / (float)img_width;

    ImgVec[ImgIndex++] = msg;
}

void rm::message(const std::string& info, int img_width, int img_height, std::vector<cv::Point2f> four_points) {
    MsgImg msg;
    size_t length = info.length();
    size_t copy_length = std::min(length, static_cast<size_t>(30));

    std::copy(info.begin(), info.begin() + copy_length, msg.str);
    msg.str[copy_length] = '\0';

    msg.type[0] = 'p';
    if (four_points.size() < 4) return;

    for (size_t i = 0; i < 4; i++) {
        msg.rect[i * 2] = four_points[i].x / img_width;
        msg.rect[i * 2 + 1] = four_points[i].y / img_height;
    }

    ImgVec[ImgIndex++] = msg;
}

void rm::message(const std::string& info, int img_width, int img_height, cv::Point2f pointX) {
    MsgImg msg;
    size_t length = info.length();
    size_t copy_length = std::min(length, static_cast<size_t>(30));

    std::copy(info.begin(), info.begin() + copy_length, msg.str);
    msg.str[copy_length] = '\0';

    msg.type[0] = 'x';
    msg.rect[0] = pointX.x / img_width;
    msg.rect[1] = pointX.y / img_height;

    ImgVec[ImgIndex++] = msg;
}

void rm::message_send() {
    size_t index = 0ull;
    for (const auto& msgpair : MsgMap) {
        if(index >= NumShmLen) break;
        shm_num[index++] = msgpair.second;
    }

    for (size_t i = 0; i < StrShmLen; i++) {
        index = (StrShmLen + StrIndex - i - 1) % StrShmLen;
        shm_str[i] = StrVec[index];
    }

    memset(shm_img, 0, sizeof(MsgImg) * ImgShmLen);
    for (int i = 0; i < ImgIndex; i++) {
        shm_img[i] = ImgVec[i];
    }
    ImgIndex = 0;
}
