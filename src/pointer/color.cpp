#include "pointer/pointer.h"
#include "uniterm/uniterm.h"    
#include <cmath>
#include <algorithm>
#include <cstdlib>

using namespace rm;
using namespace std;


ArmorColor rm::getArmorColorFromHSV(const cv::Mat& src, const rm::LightbarPair &rect_pair) {
    cv::Mat hsv_M;
    cvtColor(src, hsv_M, cv::COLOR_BGR2HSV);

    cv::Rect rect_region_1 = rect_pair.first.rect.boundingRect();
    cv::Rect rect_region_2 = rect_pair.second.rect.boundingRect();

    rect_region_1.x -= fmax(3, rect_region_1.width * 0.5);
    rect_region_1.y -= fmax(3, rect_region_1.height * 0.25);
    rect_region_1.width += 2 * fmax(3, rect_region_1.width * 0.5);
    rect_region_1.height += 2 * fmax(3, rect_region_1.height * 0.25);

    rect_region_2.x -= fmax(3, rect_region_2.width * 0.5);
    rect_region_2.y -= fmax(3, rect_region_2.height * 0.25);
    rect_region_2.width += 2 * fmax(3, rect_region_2.width * 0.5);
    rect_region_2.height += 2 * fmax(3, rect_region_2.height * 0.25);

    rect_region_1 &= cv::Rect(0, 0, src.cols, src.rows);
    rect_region_2 &= cv::Rect(0, 0, src.cols, src.rows);

    cv::Mat roi_1 = hsv_M(rect_region_1);
    cv::Mat roi_2 = hsv_M(rect_region_2);
    
    double h = 0, s = 0, v = 0, j = 0;
    int R = 0, G = 0, B = 0, P = 0;
    for (int row = 0; row < roi_1.rows; row++) {
        for (int col = 0; col < roi_1.cols; col++) {
            h = roi_1.at<cv::Vec3b>(row, col)[0];
            s = roi_1.at<cv::Vec3b>(row, col)[1];
            v = roi_1.at<cv::Vec3b>(row, col)[2];
            if(s < 43 || v < 46 || v > 240){
                continue;
            }
            if(h <= 155 && h >= 125){
                P++;
            }
            else if((h <= 180 && h >= 156) || (h <= 10 && h >= 0)){
                R++;
            }
            else if(h <= 124 && h >= 100){
                B++;
            }
            else{
                continue;
            }
        }
    }
    for (int row = 0; row < roi_2.rows; row++) {
        for (int col = 0; col < roi_2.cols; col++) {
            h = roi_2.at<cv::Vec3b>(row, col)[0];
            s = roi_2.at<cv::Vec3b>(row, col)[1];
            v = roi_2.at<cv::Vec3b>(row, col)[2];
            if(s < 43 || v < 46 || v > 240) {
                continue;
            }
            if(h <= 155 && h >= 125) {
                P++;
            } else if ((h <= 180 && h >= 156) || (h <= 10 && h >= 0)) {
                R++;
            } else if (h <= 124 && h >= 100) {
                B++;
            } else {
                continue;
            }
        }
    }

    if(2 * P > R && 2 * P > B){
        return ARMOR_COLOR_PURPLE;
    } else if (R >= B + P && R != 0){
        return ARMOR_COLOR_RED;
    } else if (B > R + P){
        return ARMOR_COLOR_BLUE;
    } else {
        return ARMOR_COLOR_NONE;
    }
    
}

ArmorColor rm::getArmorColorFromHSV(const cv::Mat& src, const rm::YoloRect& rect) {
    cv::Mat hsv_M;
    cvtColor(src, hsv_M, cv::COLOR_BGR2HSV);

    cv::Rect rect_region_1 = rect.box;
    
    rect_region_1.x -= fmax(3, rect_region_1.width * 0.5);
    rect_region_1.y -= fmax(3, rect_region_1.height * 0.25);
    rect_region_1.width += 2 * fmax(3, rect_region_1.width * 0.5);
    rect_region_1.height += 2 * fmax(3, rect_region_1.height * 0.25);

    rect_region_1 &= cv::Rect(0, 0, src.cols, src.rows);
    
    cv::Mat roi_1 = hsv_M(rect_region_1);
    
    double h = 0, s = 0, v = 0, j = 0;
    int R = 0, G = 0, B = 0, P = 0, None = 0;
    for (int row = 0; row < roi_1.rows; row++) {
        for (int col = 0; col < roi_1.cols; col++) {
            h = roi_1.at<cv::Vec3b>(row, col)[0];
            s = roi_1.at<cv::Vec3b>(row, col)[1];
            v = roi_1.at<cv::Vec3b>(row, col)[2];
            if(s < 43 || v < 46 || v > 240){
                None++;
            }
            if(h <= 155 && h >= 125){
                P++;
            }
            else if((h <= 180 && h >= 156) || (h <= 10 && h >= 0)){
                R++;
            }
            else if(h <= 124 && h >= 100){
                B++;
            }
            else{
                continue;
            }
        }
    }
    
    // std::cout<<R<<" "<<B<<" "<<P<<" "<<None<<std::endl;

    if((R + B + P) <= None){
        return ARMOR_COLOR_NONE;
    } else if(1.5 * P > R && 1.5 * P > B){
        return ARMOR_COLOR_PURPLE;
    } else if (R >= B + P && R != 0){
        return ARMOR_COLOR_RED;
    } else if (B > R + P){
        return ARMOR_COLOR_BLUE;
    } else {
        return ARMOR_COLOR_NONE;
    }
}

ArmorColor rm::getArmorColorFromRGB(const cv::Mat& src, const rm::LightbarPair &rect_pair){
    cv::Mat _M = src.clone();
    cv::Rect rect_region_1 = rect_pair.first.rect.boundingRect();
    cv::Rect rect_region_2 = rect_pair.second.rect.boundingRect();
    rect_region_1 &= cv::Rect(0, 0, src.cols, src.rows);
    rect_region_2 &= cv::Rect(0, 0, src.cols, src.rows);
    cv::Mat roi_1 = _M(rect_region_1);
    cv::Mat roi_2 = _M(rect_region_2);

    long long R = 0, G = 0, B = 0;
    long long n = 0;
    for (int i = 0; i < roi_1.rows; i++){
        for (int j = 0; j < roi_1.cols; j++){
            if (roi_1.at<cv::Vec3b>(i, j)[2] > 250 &&
                roi_1.at<cv::Vec3b>(i, j)[1] > 250 &&
                roi_1.at<cv::Vec3b>(i, j)[0] > 250) {
                continue;
            }

            R += roi_1.at<cv::Vec3b>(i, j)[2];
            G += roi_1.at<cv::Vec3b>(i, j)[1];
            B += roi_1.at<cv::Vec3b>(i, j)[0];
            n++;
        }
    }
    for(int i = 0; i < roi_2.rows; i++){
        for(int j = 0; j < roi_2.cols; j++){
            if (roi_2.at<cv::Vec3b>(i, j)[2] > 250 && 
                roi_1.at<cv::Vec3b>(i, j)[1] > 250 && 
                roi_1.at<cv::Vec3b>(i, j)[0] > 250) {
                continue;
            }
                

            R += roi_2.at<cv::Vec3b>(i, j)[2];
            G += roi_2.at<cv::Vec3b>(i, j)[1];
            B += roi_2.at<cv::Vec3b>(i, j)[0];
            n++;
        }
    }

    R = R / n;
    G = G / n;
    B = B / n;

    double B_R = B - R;
    double B_G = B - G;

    if(B_R > 90){
        return ARMOR_COLOR_BLUE;
    } else if (B_R < -90){
        return ARMOR_COLOR_RED;
    } else if (B_R = 0 || (R < 10 && G < 10 && B < 10)){
        return ARMOR_COLOR_NONE;
    } else {
        return ARMOR_COLOR_PURPLE;
    }

}

ArmorColor rm::getArmorColorFromRGB(const cv::Mat& src, const rm::YoloRect& rect) {
    return ARMOR_COLOR_NONE;
}


double rm::getLightHighHLS(const cv::Mat& src){
    cv::Mat roi = src.clone();
    double Allnum = 0, n = 0;
    cv::cvtColor(roi, roi, cv::COLOR_BGR2HLS);
    for (int i = 0; i < roi.rows; i++){
        for(int j = 0; j < roi.cols; j++){
            Allnum += roi.at<cv::Vec3b>(i, j)[1];
            n++;
        }
    }
    return Allnum / n;
}