#include "pointer/pointer.h"
#include "uniterm/uniterm.h"    
#include <cmath>
#include <algorithm>
#include <cstdlib>

using namespace rm;
using namespace std;

static int id_map[9] = {0, 1, 2, 3, 4, 5, 6, 6, 6};

static std::pair<cv::Scalar, cv::Scalar> RED_HUE =
    std::make_pair(cv::Scalar(0, 0, 0), cv::Scalar(10, 255, 255));

static std::pair<cv::Scalar, cv::Scalar> BLUE_HUE =
    std::make_pair(cv::Scalar(100, 0, 0), cv::Scalar(124, 255, 255));

static std::pair<cv::Scalar, cv::Scalar> PURPLE_HUE =
    std::make_pair(cv::Scalar(125, 0, 0), cv::Scalar(155, 255, 255));

void rm::getGrayScaleRGB(const cv::Mat& input, cv::Mat& gray, ArmorColor color) {
    cv::Mat channels[3];
    cv::split(input, channels);
    switch (color) {
        case ARMOR_COLOR_BLUE:
            gray = channels[0];
            break;
        case ARMOR_COLOR_RED:
            gray = channels[2];
            break;
        case ARMOR_COLOR_PURPLE:
            gray = channels[0] + channels[2];
            break;
        default:
            gray = channels[1];
            break;
    }
}

void rm::getGrayScaleHSV(const cv::Mat& input, cv::Mat& gray, ArmorColor color) {
    // 色彩空间转换
    cv::Mat hsv;
    cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

    // 根据颜色设置Hue掩码
    cv::Mat mask;
    switch (color) {
        case ARMOR_COLOR_BLUE:
            cv::inRange(hsv, BLUE_HUE.first, BLUE_HUE.second, mask);
            break;
        case ARMOR_COLOR_RED:
            cv::inRange(hsv, RED_HUE.first, RED_HUE.second, mask);
            break;
        case ARMOR_COLOR_PURPLE:
            cv::inRange(hsv, PURPLE_HUE.first, PURPLE_HUE.second, mask);
            break;
        default:
            cv::inRange(hsv, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255), mask);
            break;
    }

    // 使用掩码处理原图像
    cv::bitwise_and(hsv, hsv, gray, mask);

    // 色彩空间转换
    cv::cvtColor(gray, gray, cv::COLOR_HSV2BGR);
    cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
}

void rm::getGrayScaleCVT(const cv::Mat& input, cv::Mat& gray) {
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
}

void rm::getGrayScaleMix(const cv::Mat& input, cv::Mat& gray, ArmorColor color) {
    switch (color) {
        case ArmorColor::ARMOR_COLOR_BLUE:
        case ArmorColor::ARMOR_COLOR_RED:
            getGrayScaleRGB(input, gray, color);
            break;

        case ArmorColor::ARMOR_COLOR_PURPLE:
            getGrayScaleHSV(input, gray, color);
            break;

        default:
            getGrayScaleCVT(input, gray);
            break;
    }
}

void rm::getGrayScaleSub(const cv::Mat& input, cv::Mat& gray, ArmorColor color) {
    cv::Mat channels[3];
    cv::split(input, channels);
    switch (color) {
        case ArmorColor::ARMOR_COLOR_BLUE:
            gray = channels[0] - channels[2];
            break;
        case ArmorColor::ARMOR_COLOR_RED:
            gray = channels[2] - channels[0];
            break;

        case ArmorColor::ARMOR_COLOR_PURPLE:
            gray = channels[0] + channels[2] - channels[1];
            break;
        default:
            gray = channels[1];
            break;
    }
}

void rm::getGrayScale(const cv::Mat& input, cv::Mat& gray, ArmorColor color, GrayScaleMethod method) {
    switch (method) {
        case GrayScaleMethod::GRAY_SCALE_METHOD_RGB:
            getGrayScaleRGB(input, gray, color);
            break;

        case GrayScaleMethod::GRAY_SCALE_METHOD_HSV:
            getGrayScaleHSV(input, gray, color);
            break;

        case GrayScaleMethod::GRAY_SCALE_METHOD_CVT:
            getGrayScaleCVT(input, gray);
            break;

        case GrayScaleMethod::GRAY_SCALE_METHOD_MIX:
            getGrayScaleMix(input, gray, color);
            break;

        case GrayScaleMethod::GRAY_SCALE_METHOD_SUB:
            getGrayScaleSub(input, gray, color);
            break;
            
        default:
            getGrayScaleCVT(input, gray);
            break;
    }
}

void rm::getBinaryAverageThreshold(const cv::Mat& input, cv::Mat& binary, int threshold) {
    int threshold_add = static_cast<int>(threshold);         
    cv::Scalar mean = cv::mean(input);
    int threshold_total = (int)mean.val[0] + threshold_add;
    threshold_total = clamp(threshold_total, 0, 255);
    cv::threshold(input, binary, threshold_total, 255, cv::THRESH_BINARY);
}

void rm::getBinaryMaxMinRatio(const cv::Mat& input, cv::Mat& binary, double ratio) {
    double min_value, max_value;
    cv::minMaxLoc(input, &min_value, &max_value);
    double threshold_value = (min_value + max_value) * ratio;

    cv::threshold(input, binary, threshold_value, 255, cv::THRESH_BINARY);
}

void rm::getBinary(const cv::Mat& input, cv::Mat& binary, double threshold, BinaryMethod method) {
    switch (method) {
        case BinaryMethod::BINARY_METHOD_MAX_MIN_RATIO:
            getBinaryMaxMinRatio(input, binary, threshold);
            break;

        case BinaryMethod::BINARY_METHOD_AVERAGE_THRESHOLD:
            getBinaryAverageThreshold(input, binary, threshold);
            break;

        case BinaryMethod::BINARY_METHOD_DIRECT_THRESHOLD:
            cv::threshold(input, binary, threshold, 255, cv::THRESH_BINARY);
            break;

        default:
            cv::threshold(input, binary, threshold, 255, cv::THRESH_BINARY);
            break;
    }
}

ArmorID rm::getArmorIDfromClass36(ArmorClass armor_class) {
    int armor_id = armor_class % 9;
    armor_id = id_map[armor_id];
    return (ArmorID)armor_id;
}

ArmorColor rm::getArmorColorFromClass36(ArmorClass armor_class) {
    return (ArmorColor)(armor_class / 9);
}

cv::Rect rm::getExtendRect(const cv::Rect& rect, int src_width, int src_height, double ratio_x, double ratio_y) {
    int new_w = rect.width * ratio_x;
    int new_h = rect.height * ratio_y;

    int new_x = max((rect.x - (new_w - rect.width) / 2), 0);
    int new_y = max((rect.y - (new_h - rect.height) / 2), 0);
    new_w = min(new_w, src_width - rect.x - 2);
    new_h = min(new_h, src_height - rect.y - 2);

    return cv::Rect(new_x, new_y, new_w, new_h);
}


string rm::getStringArmorColor(ArmorColor armor_color) {
    string armor_color_str;
    switch (armor_color) {
        case ARMOR_COLOR_BLUE:
            armor_color_str = "blue";
            break;
        case ARMOR_COLOR_RED:
            armor_color_str = "red";
            break;
        case ARMOR_COLOR_NONE:
            armor_color_str = "none";
            break;
        case ARMOR_COLOR_PURPLE:
            armor_color_str = "purple";
            break;
        default:
            armor_color_str = "unknown";
            break;
    }
    return armor_color_str;
}

string rm::getStringArmorSize(ArmorSize armor_size) {
    string armor_size_str;
    switch (armor_size) {
        case ARMOR_SIZE_SMALL_ARMOR:
            armor_size_str = "small";
            break;
        case ARMOR_SIZE_BIG_ARMOR:
            armor_size_str = "big";
            break;
        default:
            armor_size_str = "unknown";
            break;
    }
    return armor_size_str;
}

string rm::getStringArmorID(ArmorID armor_id) {
    string armor_id_str;
    switch (armor_id) {
        case ARMOR_ID_SENTRY:
            armor_id_str = "sentry";
            break;
        case ARMOR_ID_HERO:
            armor_id_str = "hero";
            break;
        case ARMOR_ID_ENGINEER:
            armor_id_str = "engineer";
            break;
        case ARMOR_ID_INFANTRY_3:
            armor_id_str = "infantry3";
            break;
        case ARMOR_ID_INFANTRY_4:
            armor_id_str = "infantry4";
            break;
        case ARMOR_ID_INFANTRY_5:
            armor_id_str = "infantry5";
            break;
        case ARMOR_ID_TOWER:
            armor_id_str = "tower";
            break;
    }
    return armor_id_str;
}

// 获取两个矩形框的重叠率，重叠率定义为重叠面积与两个矩形框所占区域面积和的比值
double rm::getRatioOverLap(const cv::Rect &rect1, const cv::Rect &rect2) {
    int x1 = max(rect1.x, rect2.x);
    int y1 = max(rect1.y, rect2.y);
    int x2 = min(rect1.x + rect1.width, rect2.x + rect2.width);
    int y2 = min(rect1.y + rect1.height, rect2.y + rect2.height);
    int w = max(0, x2 - x1 + 1);
    int h = max(0, y2 - y1 + 1);
    double area_overlap = w * h;
    double area1 = rect1.width * rect1.height;
    double area2 = rect2.width * rect2.height;
    return area_overlap / (area1 + area2 - area_overlap + 1e-5);
}

// 获取矩形框内颜色，矩形框扩大一定比例，只分辨红和蓝
ArmorColor rm::getRegionColor(const cv::Mat &src, const cv::Rect &rect_region) {
    cv::Rect region = rect_region;

    region.x -= fmax(3, region.width * 0.1);
    region.y -= fmax(3, region.height * 0.05);
    region.width += 2 * fmax(3, region.width * 0.1);
    region.height += 2 * fmax(3, region.height * 0.05);

    region &= cv::Rect(0, 0, src.cols, src.rows);

    cv::Mat roi = src(region);

    int red_cnt = 0, blue_cnt = 0;
    for (int row = 0; row < roi.rows; row++) {
        for (int col = 0; col < roi.cols; col++) {
            red_cnt += roi.at<cv::Vec3b>(row, col)[2];
            blue_cnt += roi.at<cv::Vec3b>(row, col)[0];
        }
    }
    return (blue_cnt > red_cnt) ? ARMOR_COLOR_BLUE : ARMOR_COLOR_RED;
}

// 获取旋转矩形框内颜色
ArmorColor rm::getRegionColor(const cv::Mat &src, const cv::RotatedRect &rotate_region) {
    cv::Rect rect_region = rotate_region.boundingRect();
    return getRegionColor(src, rect_region);
}

// 获取矩形框的长宽比，较长的边除以较短的边
double rm::getRatioRectSide(const cv::Rect &rect) {
    return (rect.width > rect.height) ? 
        (double)(rect.width / rect.height) : (double)(rect.height / rect.width);
}

// 获取旋转矩形的长宽比，较长的边除以较短的边
double rm::getRatioRectSide(const cv::RotatedRect &rect) {
    return (rect.size.width > rect.size.height) ? 
        (double)(rect.size.width / rect.size.height) : (double)(rect.size.height / rect.size.width);
}

// 获取轮廓的面积与轮廓最小外接矩形的面积的比值
double rm::getRatioAreaContourToRect(const std::vector<cv::Point> &contour, const cv::RotatedRect &rect) {
    return cv::contourArea(contour) / rect.size.area();
}

// 定义竖直状态的灯条为0度，顺时针为正，逆时针为负
//
// OpenCV4.5 版本定义为:
//      x 轴顺时针旋转最先重合的边为 w
//      angle 为 x 轴顺时针旋转的角度
//      angle 取值为 (0,90]
//
double rm::getLightbarAngleRectCV45(const cv::RotatedRect &rect) {
    return (rect.size.width > rect.size.height) ? (rect.angle - 90) : (rect.angle);
}

//
// OpenCV4.1 版本定义为:
//      x 轴逆时针旋转最先重合的边为 w
//      此时 x 轴逆时针旋转的角度为 -angle
//
double rm::getLightbarAngleRectCV41(const cv::RotatedRect &rect) {
    return (rect.size.width > rect.size.height) ? (rect.angle + 90) : (rect.angle);
}

void rm::getLightbarsFromContours(const std::vector<std::vector<cv::Point>> &contours, 
                                 std::vector<Lightbar> &lightbars,
                                 double min_rect_side, 
                                 double max_rect_side,
                                 double min_value_area,
                                 double min_ratio_area,
                                 double max_angle
) {
    lightbars.clear();
    for(auto contour: contours) {
        rm::Lightbar lightbar;
        rm::setLightbar(lightbar, contour);
        if(isLightBarValid(lightbar, min_rect_side, max_rect_side, min_value_area, min_ratio_area, max_angle)) {
            lightbars.push_back(lightbar);
        }
    }    
}

// 两个灯条长度的平均值
double rm::getValueLengthLightbarPair(const Lightbar &lb1, const Lightbar &lb2) {
    return (lb1.length + lb2.length) / 2;
}

// 两个灯条长度的比值，大比小
double rm::getRatioLengthLightbarPair(const Lightbar &lightbar1, const Lightbar &lightbar2) {
    double ratio = (lightbar1.length > lightbar2.length) ? 
        (lightbar1.length / lightbar2.length) : (lightbar2.length / lightbar1.length);
    return ratio;
}

// 两个灯条最小外接矩形面积的比值，大比小
double rm::getRatioAreaLightbarPair(const Lightbar &lb1, const Lightbar &lb2) {
    double area1 = lb1.rect.size.area();
    double area2 = lb2.rect.size.area();
    return (area1 > area2) ? (area1 / area2) : (area2 / area1);
}


//  准armor的宽高比，宽比高
//  下图为示意图(* 组成的线条 表示 灯条)，宽高比为 10 / 4 = 2.5
//
// 	     * - - - - - - - - - *
// 	     *                   *
// 	     *                   *
// 	     * - - - - - - - - - *
//
//  在比赛中，有大小装甲板两种，所以这里有两个阈值范围
//
double rm::getRatioArmorSide(const Lightbar &lb1, const Lightbar &lb2) {
    // 将两个灯条中心距离视为装甲板的宽度
    cv::Point2f center_vec = lb1.rect.center - lb2.rect.center;
    float center_dis = sqrt(center_vec.ddot(center_vec));

    // 将第一个灯条的长度视为装甲板的高度，然后计算宽高比
    float ratio = center_dis / lb1.length;
    return ratio;
}


// 两个灯条的角度差，返回角度值的绝对值
double rm::getAngleDiffLightbarPair(const Lightbar &lb1, const Lightbar &lb2) {
    return abs(lb1.angle - lb2.angle);
}

// 两个灯条的角度平均值
double rm::getAngleAvgLightbarPair(const Lightbar &lb1, const Lightbar &lb2) {
    return (lb1.angle + lb2.angle) / 2;
}

// 判断两个灯条的相对位置
//
// 1. 先取两个灯条的平均方向，得到一条直线
// 2. 将两个灯条的中心点投影到这条直线上，得到两个投影点
// 3. 计算两个投影点的距离
// 4. 该距离认定为两个灯条的相对位移
//
double rm::getCenterOffsetLightbarPair(const Lightbar &lb1, const Lightbar &lb2) {
    double angle = getAngleAvgLightbarPair(lb1, lb2) * CV_PI / 180.0;
    double dx = lb2.rect.center.x - lb1.rect.center.x;
    double dy = lb2.rect.center.y - lb1.rect.center.y;
    double offset = abs(sin(angle) * dx - cos(angle) * dy);
    return offset;
}

cv::Point2f rm::getLightbarPairCenter(const Lightbar &lb1, const Lightbar &lb2) {
    return cv::Point2f(
        (lb1.rect.center.x + lb2.rect.center.x) / 2,
        (lb1.rect.center.y + lb2.rect.center.y) / 2
    );
}

// 装甲板中心，根据装甲板矩形框计算
cv::Point2f rm::getArmorRectCenter(const Armor& armor) {
    return cv::Point2f(
        armor.rect.x + armor.rect.width / 2,
        armor.rect.y + armor.rect.height / 2
    );
}

// 将装甲板四点坐标从矩形框的相对坐标转换为图像的绝对坐标
cv::Point2f rm::getRelativeToAbsoluteTrans(const cv::Rect& rect, const cv::Point2f& point_relative) {
    return cv::Point2f(
        rect.x + point_relative.x,
        rect.y + point_relative.y
    );
}

// 从匹配成对的灯条中获取最佳的一对
LightbarPair rm::getBestMatchedLightbarPair(const std::vector<LightbarPair>& pairs, const Armor& armor) {
    LightbarPair best_pair;
    double min_dis = 1e10;
    for (const auto& pair : pairs) {
        double dis = cv::norm((getLightbarPairCenter(pair.first, pair.second) - armor.center));
        if (dis < min_dis) {
            min_dis = dis;
            best_pair = pair;
        }
    }
    return best_pair;
}

// 将灯条匹配成对，并筛选最佳配对
bool rm::getBestMatchedLightbarPair(const std::vector<rm::Lightbar>& lightbars,
                                    const Armor& armor,
                                    LightbarPair& best_pair,
                                    double max_ratio_length, 
                                    double max_ratio_area,
                                    double min_ratio_side,
                                    double max_ratio_side,
                                    double max_angle_diff,
                                    double max_angle_avg,
                                    double max_offset
){
    std::vector<rm::LightbarPair> lightbar_pair_list;
    for(size_t i = 0; i < lightbars.size(); i++) {
        for(size_t j = i + 1; j < lightbars.size(); j++) {
            if(isLightBarMatched(lightbars[i], lightbars[j], max_ratio_length, max_ratio_area, min_ratio_side, max_ratio_side, max_angle_diff, max_angle_avg, max_offset)) {
                lightbar_pair_list.push_back(rm::LightbarPair(lightbars[i], lightbars[j]));
            }
        }
    }
    if(lightbar_pair_list.size() <= 0) {
        best_pair = rm::LightbarPair();
        return false;
    } else if(lightbar_pair_list.size() == 1) {
        best_pair = lightbar_pair_list[0];
        return true;
    } else {
        best_pair = getBestMatchedLightbarPair(lightbar_pair_list, armor);
        return true;
    }
}

void rm::getClampRect(const cv::Mat& src, cv::Rect& rect) {
    
    int left = std::clamp((int)(rect.x - rect.width / 2), 0, src.cols - 1);
    int top = std::clamp((int)(rect.y - rect.height / 2), 0, src.rows - 1);
    int right = std::clamp((int)(rect.x + rect.width / 2), 0, src.cols - 1);
    int bottom = std::clamp((int)(rect.y + rect.height / 2), 0, src.rows - 1);

    rect = cv::Rect(cv::Point(left, top), cv::Point(right, bottom));
}

