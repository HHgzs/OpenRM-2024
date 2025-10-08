#ifndef __OPENRM_UTILS_TIMER_H__
#define __OPENRM_UTILS_TIMER_H__

#include <chrono>
#include <string>


// 时间点 time_point
typedef std::chrono::high_resolution_clock::time_point TimePoint;

// 时间间隔 duration
typedef std::chrono::duration<double> Duration_s;
typedef std::chrono::duration<unsigned long long, std::ratio<1, 1000>> Duration_ms;
typedef std::chrono::duration<unsigned long long, std::ratio<1, 1000000>> Duration_us;

// 获取当前时间
TimePoint getTime();

// 计算时间间隔，单位为秒
Duration_s getDuration_s(const TimePoint& start, const TimePoint& end);

// 计算时间间隔，单位为毫秒
Duration_ms getDuration_ms(const TimePoint& start, const TimePoint& end);

// 计算时间间隔，单位为微秒
Duration_us getDuration_us(const TimePoint& start, const TimePoint& end);

// 提取时间间隔的数字部分，单位秒
double getDoubleOfS(Duration_s duration_s);

// 提取时间间隔的数字部分，单位毫秒
unsigned long long getNumOfMs(Duration_ms duration_ms);

// 提取时间间隔的数字部分，单位微秒
unsigned long long getNumOfUs(Duration_us duration_us);

// 直接计算时间间隔的数字部分，单位秒
double getDoubleOfS(const TimePoint& start, const TimePoint& end);

// 直接计算时间间隔的数字部分，单位毫秒
unsigned long long getNumOfMs(const TimePoint& start, const TimePoint& end);

// 直接计算时间间隔的数字部分，单位微秒
unsigned long long getNumOfUs(const TimePoint& start, const TimePoint& end);

// 将时间转换为unsigned long long
unsigned long long transTimeToUll(const TimePoint& time);

// 将unsigned long long转换为时间
TimePoint transUlltoTime(unsigned long long time);

// 通过时间获取字符串，可用于命名
std::string getTimeStr();

// 通过时间获取字符串，可用于命名
std::string getMsStr();



#endif