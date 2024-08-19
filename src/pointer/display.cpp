#include "pointer/pointer.h"
#include <cmath>
using namespace std;
using namespace rm;

void rm::displaySingleArmorRect(const cv::Mat& src, const Armor& armor, int thickness) {
    if(thickness < 1) thickness = 1;
    if (isRectValidInImage(src, armor.rect)) {
        cv::rectangle(src, armor.rect, cv::Scalar(255, 255, 255), thickness);
    }
}

void rm::displaySingleArmorLine(const cv::Mat& src, const Armor& armor, int thickness) {
    if (armor.four_points.size() < 4) {
        return;
    }
    for (int i = 0; i < 4; i++) {
        if (!isPointValidInImage(src, armor.four_points[i])) {
            return;
        }
    }
    if(thickness < 1) thickness = 1;
    cv::line(src, armor.four_points[0], armor.four_points[1], cv::Scalar(0, 255, 0), thickness);
    cv::line(src, armor.four_points[2], armor.four_points[3], cv::Scalar(0, 255, 0), thickness);
    cv::line(src, armor.four_points[0], armor.four_points[2], cv::Scalar(0, 255, 0), thickness);
    cv::line(src, armor.four_points[1], armor.four_points[3], cv::Scalar(0, 255, 0), thickness);
}

void rm::displaySingleArmorClass(const cv::Mat& src, const Armor& armor) {
    std::string word;
    std::string color_str, size_str;
    switch (armor.color) {
    case ArmorColor::ARMOR_COLOR_BLUE:
        color_str = "B";
        break;
    case ArmorColor::ARMOR_COLOR_RED:
        color_str = "R";
        break;
    case ArmorColor::ARMOR_COLOR_PURPLE:
        color_str = "P";
        break;
    default:
        color_str = "N";
        break;
    }
    switch (armor.size) {
    case ArmorSize::ARMOR_SIZE_SMALL_ARMOR:
        size_str = "S";
        break;
    case ArmorSize::ARMOR_SIZE_BIG_ARMOR:
        size_str = "B";
        break;
    default:
        size_str = "N";
        break;
    }
    word = "Color: " + color_str + " ID: " + to_string(armor.id) + " Size: " + size_str;
    cv::putText(
        src,
        word,
        cv::Point(armor.rect.x, armor.rect.y - 10),
        cv::FONT_HERSHEY_TRIPLEX,
        0.5,
        cv::Scalar(0, 255, 0),
        1
    );
}


void rm::displaySingleArmorID(const cv::Mat& src, const Armor& armor) {
    std::string word;
    word = "ID:" + to_string(armor.id);
    cv::putText(
        src,
        word,
        cv::Point(armor.rect.x, armor.rect.y + 200),
        cv::FONT_HERSHEY_TRIPLEX,
        2,
        cv::Scalar(0, 255, 255),
        2
    );
}


void rm::displaySingleArmorWord(const cv::Mat& src, const Armor& armor, const std::string& word) {
    if (!isRectValidInImage(src, armor.rect)) {
        return;
    }
    cv::putText(
        src,
        word,
        cv::Point(armor.rect.x, armor.rect.y + 10),
        cv::FONT_HERSHEY_TRIPLEX,
        0.5,
        cv::Scalar(0, 255, 0),
        1
    );
}

void rm::displayPredictTargetX(const cv::Mat& src, const cv::Point2f point, ArmorID id) {
    float diagonal = std::sqrt(src.cols * src.cols + src.rows * src.rows);
    float cx = point.x;
    float cy = point.y;
    float dmin = diagonal / 300.0f;
    float dmax = diagonal / 20.0f;
    std::string word;
    word = " ID: " + to_string(id);
    cv::circle(src, point, dmin / 2, cv::Scalar(0, 255, 255), 1);
    cv::line(src, cv::Point2f(cx - dmin, cy - dmin), cv::Point2f(cx - dmax, cy - dmax), cv::Scalar(0, 255, 255), 1);
    cv::line(src, cv::Point2f(cx + dmin, cy - dmin), cv::Point2f(cx + dmax, cy - dmax), cv::Scalar(0, 255, 255), 1);
    cv::line(src, cv::Point2f(cx - dmin, cy + dmin), cv::Point2f(cx - dmax, cy + dmax), cv::Scalar(0, 255, 255), 1);
    cv::line(src, cv::Point2f(cx + dmin, cy + dmin), cv::Point2f(cx + dmax, cy + dmax), cv::Scalar(0, 255, 255), 1);
    cv::putText(src, word, cv::Point2f(cx + dmax + dmin, cy - dmax - dmin), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar(0, 255, 255), 1);
} 

void rm::displayStrVecOnImage(const cv::Mat& src, const std::vector<std::string>& lines) {
    int fontSize = 16;
    int lineHeight = fontSize + 4;
    cv::Point currentPosition = cv::Point(10, 30);
    cv::Scalar textColor(255, 255, 255);

    for (const auto& line : lines) {
        cv::putText(src, line, currentPosition, cv::FONT_HERSHEY_SIMPLEX, fontSize / 25.0, textColor, 1);
        currentPosition.y += lineHeight;
    }
}