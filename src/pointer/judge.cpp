#include "pointer/pointer.h"
#include "uniterm/uniterm.h"
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace rm;
using namespace std;


bool rm::isRatioRectSideValid(const Lightbar& lightbar, double min, double max) {
    double ratio = getRatioRectSide(lightbar.rect);
    if (ratio < min || ratio > max) {
        #ifdef POINTER_IO
        rm::message("lb rect side ratio: " + to_string(ratio), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;  
}

bool rm::isValueRectAreaValid(const Lightbar& lightbar, double min) {
    double area = lightbar.rect.size.area();
    if (area < min) {
        #ifdef POINTER_IO
        rm::message("lb rect area: " + to_string(area), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isRatioRectAreaValid(const Lightbar& lightbar, double min) {
    double ratio = getRatioAreaContourToRect(lightbar.contour, lightbar.rect);
    if (ratio < min) {
        #ifdef POINTER_IO
        rm::message("lb rect area ratio: " + to_string(ratio), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isValueAngleValid(const Lightbar& lightbar, double max) {
    double angle = getLightbarAngleRectCV45(lightbar.rect);
    if (abs(angle) > max) {
        #ifdef POINTER_IO
        // rm::message("lb rect: " + to_string(lightbar.rect.angle) + " " + to_string(lightbar.rect.size.width) +  " " + to_string(lightbar.rect.size.height), rm::MSG_NOTE);
        rm::message("lb angle: " + to_string(angle), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isLightBarValid(const Lightbar& lightbar,
                     double min_rect_side, 
                     double max_rect_side,
                     double min_value_area,
                     double min_ratio_area,
                     double max_angle
) {
    bool flag_rect_side = isRatioRectSideValid(lightbar, min_rect_side, max_rect_side);
    bool flag_value_area = isValueRectAreaValid(lightbar, min_value_area);
    bool flag_ratio_area = isRatioRectAreaValid(lightbar, min_ratio_area);
    bool flag_angle = isValueAngleValid(lightbar, max_angle);
    if (flag_rect_side && flag_value_area && flag_ratio_area && flag_angle) {
        return true;
    }
    return false;
}

bool rm::isRatioLengthLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, double max) {
    double ratio = getRatioLengthLightbarPair(lb1, lb2);
    if (ratio > max) {
        #ifdef POINTER_IO
        rm::message("lb pair ratio length: " + to_string(ratio), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isRatioAreaLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, double max) {
    double ratio = getRatioAreaLightbarPair(lb1, lb2);
    if (ratio > max) {
        #ifdef POINTER_IO
        rm::message("lb pair ratio area: " + to_string(ratio), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isRatioArmorSideValid(const Lightbar& lb1, const Lightbar& lb2, double min, double max) {
    double ratio = getRatioArmorSide(lb1, lb2);
    if (ratio < min || ratio > max) {
        #ifdef POINTER_IO
        rm::message("lb pair ratio side: " + to_string(ratio), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isAngleDiffLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, double max) {
    double angle = getAngleDiffLightbarPair(lb1, lb2);
    if (angle > max) {
        #ifdef POINTER_IO
        rm::message("lb pair angle diff: " + to_string(angle), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isAngleAvgLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, double max) {
    double angle = getAngleAvgLightbarPair(lb1, lb2);
    if(abs(angle) > max) {
        #ifdef POINTER_IO
        rm::message("lb pair angle avg: " + to_string(angle), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isCenterOffsetLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, double max) {
    double offset = getCenterOffsetLightbarPair(lb1, lb2);
    double avg_len = getValueLengthLightbarPair(lb1, lb2);
    double ratio = offset / avg_len;
    if (ratio > max) {
        #ifdef POINTER_IO
        rm::message("lb pair center offset: " + to_string(ratio), rm::MSG_NOTE);
        #endif
        return false;
    }
    return true;
}

bool rm::isLightBarMatched(const Lightbar& lb1, const Lightbar& lb2, 
                       double max_ratio_length, 
                       double max_ratio_area,
                       double min_ratio_side,
                       double max_ratio_side,
                       double max_angle_diff,
                       double max_angle_avg,
                       double max_offset
) { 
    if (!isRatioAreaLightbarPairValid(lb1, lb2, max_ratio_area)) {
        return false;
    }
    if (!isCenterOffsetLightbarPairValid(lb1, lb2, max_offset)) {
        return false;
    }
    
    bool flag_ratio_length = isRatioLengthLightbarPairValid(lb1, lb2, max_ratio_length);
    bool flag_ratio_side = isRatioArmorSideValid(lb1, lb2, min_ratio_side, max_ratio_side);
    bool flag_angle_diff = isAngleDiffLightbarPairValid(lb1, lb2, max_angle_diff);
    bool flag_angle_avg = isAngleAvgLightbarPairValid(lb1, lb2, max_angle_avg);
    if (flag_ratio_length && flag_ratio_side && flag_angle_diff && flag_angle_avg) {
        return true;
    }
    return false;
} 

bool rm::isLightBarAreaPercentValid(rm::Armor armor, double min_area_percent) {
    double armor_area = armor.rect.area();
    double light_bar_area = cv::norm(armor.four_points[0] - armor.four_points[1]) * cv::norm(armor.four_points[0] - armor.four_points[2]);
    return light_bar_area / armor_area > min_area_percent;
}

bool rm::isRectValidInImage(const cv::Mat& image, const cv::Rect& rect) {
    int cols = image.cols;
    int rows = image.rows;

    if (rect.width <= 0 || rect.height <= 0) {
        return false;
    }
    if (rect.x < 0 || rect.x >= cols || rect.x + rect.width > cols) {
        return false;
    }
    if (rect.y < 0 || rect.y >= rows || rect.y + rect.height > rows) {
        return false;
    }
    return true;
}


bool rm::isPointValidInImage(const cv::Mat& image, const cv::Point2f& point) {
    int cols = image.cols;
    int rows = image.rows;

    if (point.x < 0 || point.x >= cols) {
        return false;
    }
    if (point.y < 0 || point.y >= rows) {
        return false;
    }
    return true;

}

bool rm::isArmorColorEnemy(const cv::Mat& input,  const rm::LightbarPair &lbp, rm::ArmorColor enemy_color, double threshold) {
    cv::Mat gray;
    cv::Mat channels[3];
    cv::split(input, channels);
    
    if(enemy_color == ARMOR_COLOR_BLUE) {
        gray = channels[0] - channels[2];
    } else if (enemy_color == ARMOR_COLOR_RED) {
        gray = channels[2] - channels[0];
    } else {
        return false;
    }
    cv::Rect rect_left = lbp.first.rect.boundingRect();
    cv::Rect rect_right = lbp.second.rect.boundingRect();
    rm::getClampRect(gray, rect_left);
    rm::getClampRect(gray, rect_right);
    cv::Scalar mean_left = cv::mean(gray(rect_left));
    cv::Scalar mean_right = cv::mean(gray(rect_right));
    int threshold_total = 0.5 * (int)mean_left.val[0] + 0.5 * (int)mean_right.val[0];

    rm::message("split avg", threshold_total);

    if(threshold_total > threshold) return true;
    return false;
}
