#include "utils/timer.h"
#include <chrono>    // 用于处理时间相关操作
#include <ctime>     // 用于处理时间和日期的函数
#include <iomanip>   // 用于格式化输出
#include <sstream>   // 用于字符串流操作
#include <string>    // 用于字符串操作

TimePoint getTime() {
    return std::chrono::high_resolution_clock::now();
}

Duration_s getDuration_s(const TimePoint& start, const TimePoint& end) {
    return std::chrono::duration_cast<Duration_s>(end - start);
}

Duration_ms getDuration_ms(const TimePoint& start, const TimePoint& end) {
    return std::chrono::duration_cast<Duration_ms>(end - start);
}

Duration_us getDuration_us(const TimePoint& start, const TimePoint& end) {
    return std::chrono::duration_cast<Duration_us>(end - start);
}

double getDoubleOfS(Duration_s duration_s) {
    return duration_s.count();
}

unsigned long long getNumOfMs(Duration_ms duration_ms) {
    return duration_ms.count();
}

unsigned long long getNumOfUs(Duration_us duration_us) {
    return duration_us.count();
}

double getDoubleOfS(const TimePoint& start, const TimePoint& end) {
    return getDoubleOfS(getDuration_s(start, end));
}

unsigned long long getNumOfMs(const TimePoint& start, const TimePoint& end) {
    return getNumOfMs(getDuration_ms(start, end));
}

unsigned long long getNumOfUs(const TimePoint& start, const TimePoint& end) {
    return getNumOfUs(getDuration_us(start, end));
}

unsigned long long transTimeToUll(const TimePoint& time) {
    // 将时间点转换为持续时间类型
    auto duration = time.time_since_epoch();

    // 将持续时间转换为纳秒
    auto durationInNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

    // 获取纳秒数的整数值
    unsigned long long timeValue = durationInNanoseconds.count();

    return timeValue;
}

TimePoint transUlltoTime(unsigned long long time) {
    // 将 unsigned long long 转换为持续时间类型
    std::chrono::nanoseconds duration(time);

    // 创建时间点
    TimePoint timePoint(duration);

    return timePoint;
}

std::string getTimeStr() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 将时间点转换为本地时间
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // 使用本地时间创建tm结构体
    std::tm* localTime = std::localtime(&currentTime);

    // 使用std::put_time格式化为字符串
    std::ostringstream oss;
    // oss << std::put_time(localTime, "%Y%m%d%H%M%S");
    oss << std::put_time(localTime, "%d%H%M%S");

    return oss.str();
}

std::string getMsStr() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 将时间点转换为本地时间
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // 使用本地时间创建tm结构体
    std::tm* localTime = std::localtime(&currentTime);

    // 获取毫秒部分
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % 1000;

    // 使用std::put_time格式化为字符串
    std::ostringstream oss;
    oss << std::put_time(localTime, "%d%H%M%S") << std::setfill('0') << std::setw(3) << milliseconds.count();

    return oss.str();
}
