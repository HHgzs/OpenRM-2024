#include "pointer/pointer.h"
#include "uniterm/uniterm.h"    
#include <cmath>
#include <algorithm>
#include <cstdlib>

using namespace rm;
using namespace std;


void rm::getHistogram(const cv::Mat& src, cv::Mat& histogram, int color){
    cv::Mat Gray_image, my_src = src.clone();
    std::vector<cv::Mat> channels;

    //设置直方图参数
    cv::Mat R_hist, G_hist , B_hist, Gray_hist;
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    bool uniform = true;
    bool accumulate = false;

    if(color == 0){
        cv::cvtColor(my_src, Gray_image, cv::COLOR_BGR2GRAY);
        //计算直方图
        cv::calcHist(&Gray_image, 1, 0, cv::Mat(), Gray_hist, 1, &histSize, &histRange, uniform, accumulate);
    }
    else{
        cv::split(my_src, channels);
        //计算直方图
        cv::calcHist(&channels[0], 1, 0, cv::Mat(), B_hist, 1, &histSize, &histRange, uniform, accumulate);
        cv::calcHist(&channels[1], 1, 0, cv::Mat(), G_hist, 1, &histSize, &histRange, uniform, accumulate);
        cv::calcHist(&channels[2], 1, 0, cv::Mat(), R_hist, 1, &histSize, &histRange, uniform, accumulate);
    }
    
    switch (color)
    {
    case 0: histogram = Gray_hist.clone(); break;
       
    case 1: histogram = B_hist.clone(); break;

    case 2: histogram = G_hist.clone(); break;

    case 3: histogram = R_hist.clone(); break;
    
    default: histogram = Gray_hist.clone(); break;
    }

    //归一化
    cv::normalize(histogram, histogram, 0, 512, cv::NORM_MINMAX, -1, cv::Mat());
    
    return;

}

void rm::setLine_Histogram(cv::Mat& input, cv::Mat& output, cv::Mat& histogram, int set_line, int flag){
    //显示直方图参数
    int histSize = histogram.total();
    int hist_w = input.cols, hist_h = input.rows;
    int bin_w = cvRound((double)hist_w / histSize);
    int bin_h = cvRound((double)hist_h / histSize);
    output = input.clone();
    
    //画出定位线
    if (flag == 0){
        int line_y = set_line * bin_h;
        cv::Point point_start(0, hist_h - line_y);
        cv::Point point_end(output.cols - 1, hist_h - line_y);
        cv::line(output, point_start, point_end, cv::Scalar(0, 255, 255), 2);
    }
    else{
        int line_x = set_line * bin_w;
        cv::Point point_start(line_x, 0);
        cv::Point point_end(line_x, output.rows - 1);
        cv::line(output, point_start, point_end, cv::Scalar(0, 255, 255), 2);
    }
}

void rm::showHistogram(cv::Mat& histogram, cv::Mat& ShowImage, int wid, int len) {
    //显示直方图参数
    int histSize = histogram.total();
    int hist_w = wid, hist_h = len;
    int bin_w = cvRound((double)hist_w / histSize);
    int bin_h = cvRound((double)hist_h / histSize);
    cv::Mat my_histogram = histogram.clone();
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    
    cv::normalize(my_histogram, my_histogram, 0, 512, cv::NORM_MINMAX, -1, cv::Mat());
    //绘制
    for(int i = 1; i < histSize; i++){
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(my_histogram.at<float>(i - 1))), 
                            cv::Point(bin_w * (i), hist_h - cvRound(my_histogram.at<float>(i))), 
                            cv::Scalar(255, 255, 255), 2, 8, 0);
    }
    //画出坐标
    for(int i = 0; i < 256; i += 10){
        if(i >= 256){
            break;
        }
        cv::putText(histImage, std::to_string(i), cv::Point((bin_w * i), hist_h - 10), 
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    }
    ShowImage = histImage.clone();
}

std::pair<int, int> rm::getHistDoublePeak(const cv::Mat& histogram){
    int firstPeak = 0;
    double maxVal;
    int histSize = histogram.total();
    cv::minMaxLoc(histogram, NULL, &maxVal, NULL, NULL);
    //寻找最大峰
    for(int i = 0; i < histSize; ++i){
        if(histogram.at<float>(i) == maxVal){
            firstPeak = i;
            break;
        }
    }
    //找第一个峰末尾
    int firstPeakEnd = firstPeak;
    for(int i = firstPeak; i < histSize; ++i){
        if(histogram.at<float>(i) <= (maxVal / 5)){
            firstPeakEnd = i;
            break;
        }
    }

    int myend = 0;
    for(int i = firstPeak; i < histSize; ++i){
        if(histogram.at<float>(i) <= 3){
            myend = (i + 30 > 250) ? 250 : (i + 30);
            break;
        }
    }
    
    cv::Mat RestDists = cv::Mat::zeros(256, 1, CV_32F);
    //从第一关峰末尾开始 加权
    for(int k = 0; k < myend && k < histSize; k++){  
        RestDists.at<float>(k) = std::pow(k - firstPeak, 2) * histogram.at<float>(k);
    }

    int secondPeak = 0;
    cv::Point my_p;
    //求解第二个峰
    cv::minMaxLoc(RestDists, NULL, &maxVal, NULL, &my_p);
    secondPeak = my_p.y;

    if(secondPeak > 255){
        secondPeak = 255;
    }
    return std::pair<int ,int>(firstPeak, secondPeak);
}

void rm::getHistIncludePeak(const cv::Mat& src, cv::Mat& ShowImage){
    cv::Mat histogram;
    rm::getHistogram(src, histogram, 0);
    std::pair<int, int> HistPeakNum = rm::getHistDoublePeak(histogram);
    rm::showHistogram(histogram, ShowImage, 1000, 512);
    rm::setLine_Histogram(ShowImage, ShowImage, histogram, HistPeakNum.first, 1);
    rm::setLine_Histogram(ShowImage, ShowImage, histogram, HistPeakNum.second, 1);
}

int rm::getThresholdFromHist(const cv::Mat& src, int Cut_thresold, int bios){
    cv::Mat histogram;
    rm::getHistogram(src, histogram, 0);
    int n = 0, avgNum = 0, my_End = 80, my_Begin = 10;
    int final_thread = my_Begin;

    for(int i = my_End; i > my_Begin; i--){
        if(histogram.at<float>(i) >= Cut_thresold){
            final_thread = i;
            break;
        }
    }
    final_thread = final_thread + bios;
    if(final_thread >= 255){
        final_thread = 254;
    }
    return final_thread;
}

int rm::getThresholdFromHist(const cv::Mat& src, cv::Mat& ShowImage, int Cut_thresold, int bios){
    cv::Mat histogram;
    rm::getHistogram(src, histogram, 0);
    rm::showHistogram(histogram, ShowImage, 1000, 512);
    int n = 0, avgNum = 0, my_End = 130, my_Begin = 10;
    int final_thread = my_Begin;
    
    for(int i = my_End; i > my_Begin; i--){
        if(histogram.at<float>(i) >= Cut_thresold){
            final_thread = i;
            break;
        }
    }
    final_thread = final_thread + bios;
    if(final_thread >= 255){
        final_thread = 254;
    }
    rm::setLine_Histogram(ShowImage, ShowImage, histogram, Cut_thresold, 0);
    rm::setLine_Histogram(ShowImage, ShowImage, histogram, final_thread, 1);
    rm::message("final_thread: ", final_thread);
    return final_thread;
}

int rm::getThresholdFromHistPeak(const cv::Mat& src, cv::Mat& ShowImage, int bios){
    cv::Mat histogram;
    rm::getHistogram(src, histogram, 0);
    rm::getHistIncludePeak(src, ShowImage);
    std::pair<int, int> HistPeakNum = rm::getHistDoublePeak(histogram);
    int n = 0, avgNum = 0, my_End = 130, my_Begin = 10;
    int final_thread = my_Begin;
    
    for(int i = HistPeakNum.second; i < my_Begin; i++){
        if(n >= 10 && avgNum / n <= 6){
            final_thread = i - (n / 2);
            break;
        }
        if(histogram.at<float>(i) <= 10){
            n++;
            avgNum += histogram.at<float>(i);
        }
        else{
            n = 0;
        }
    }
    final_thread = final_thread + bios;
    if(final_thread >= 255){
        final_thread = 254;
    }
    rm::setLine_Histogram(ShowImage, ShowImage, histogram, final_thread, 1);
    return final_thread;
}

void rm::getHistogramEqualization(const cv::Mat& src, cv::Mat& dst) {
    if (src.depth() != CV_8U || src.channels() != 3) {
        dst = src.clone();
        std::string err_str = "Invalid input depth: " + std::to_string(src.depth()) 
                            + " channels: " + std::to_string(src.channels());
        rm::message(err_str, rm::MSG_ERROR);
        return;
    }

    int sum[3] = {0};
    int hist[3][256] = {0};
    int lut[3][256]  = {0};
    int rc_total = src.rows * src.cols;

    for (int i = 0; i < src.rows; i++) {
        const uchar* p = src.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++) {
            for (int k = 0; k < 3; k++) {
                hist[k][p[j * 3 + k]]++;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 256; j++) {
            sum[i] += hist[i][j];
            lut[i][j] = sum[i] * 255 / rc_total;
        }
    }

    dst.create(src.size(), src.type());

    for (int i = 0; i < src.rows; i++) {
        uchar* p_dst = dst.ptr<uchar>(i);
        const uchar* p_src = src.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++) {
            for (int k = 0; k < 3; k++) {
                p_dst[j * 3 + k] = lut[k][p_src[j * 3 + k]];
            }
        }
    }
}