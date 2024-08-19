#include "pointer/pointer.h"
#include <cmath>
#include <algorithm>
#include "uniterm/uniterm.h"
using namespace rm;
using namespace std;


void rm::setLightbarRotatedRect(Lightbar& lightbar) {
    lightbar.rect = cv::minAreaRect(lightbar.contour);
}

void rm::setLigntbarAngleRectCV45(Lightbar& lightbar) {
    lightbar.angle = ((lightbar.rect.size.width > lightbar.rect.size.height) ? 
                      (lightbar.rect.angle - 90) : (lightbar.rect.angle));
}

void rm::setLigntbarAngleRectCV41(Lightbar& lightbar) {
    lightbar.angle = ((lightbar.rect.size.width > lightbar.rect.size.height) ? 
                      (lightbar.rect.angle + 90) : (lightbar.rect.angle));
}

void rm::setLightbarRectBasedLength(Lightbar& lightbar) {
    lightbar.length = ((lightbar.rect.size.width > lightbar.rect.size.height) ? 
                       (lightbar.rect.size.width) : (lightbar.rect.size.height));
}

void rm::setLightbar(Lightbar& lightbar, const std::vector<cv::Point>& contour) {
    lightbar.contour = contour;
    setLightbarRotatedRect(lightbar);
    setLigntbarAngleRectCV45(lightbar);
    setLightbarRectBasedLength(lightbar);   
}

void rm::setArmorRectCenter(Armor& armor) {
    armor.center.x = armor.rect.x + armor.rect.width / 2;
    armor.center.y = armor.rect.y + armor.rect.height / 2;
}

void rm::setArmorExtendRectIOU(Armor& armor, const cv::Rect& rect, int src_width, int src_height, double ratio_x, double ratio_y) {
    int new_width = rect.width * ratio_x;
    int new_height = rect.height * ratio_y;

    armor.rect.x = max((rect.x - (new_width - rect.width) / 2), 0);
    armor.rect.y = max((rect.y - (new_height - rect.height) / 2), 0);
    armor.rect.width = min(new_width, src_width - armor.rect.x - 2);
    armor.rect.height = min(new_height, src_height - armor.rect.y - 2);
}

void rm::setArmorBaseClass36(Armor& armor, 
                  const cv::Rect& rect, 
                  ArmorClass armor_class,
                  int src_width, 
                  int src_height,
                  double ratio_x, 
                  double ratio_y,
                  ArmorSize size
) {
    armor.id = getArmorIDfromClass36(armor_class);
    armor.color = getArmorColorFromClass36(armor_class);
    setArmorExtendRectIOU(armor, rect,src_width, src_height, ratio_x, ratio_y);
    armor.size = size;
    setArmorRectCenter(armor);
}

void rm::setArmorBaseClass7(Armor& armor, 
                            const cv::Rect& rect, 
                            ArmorID armor_id,
                            int src_width, 
                            int src_height,
                            double ratio_x, 
                            double ratio_y,
                            ArmorSize size
) {
    armor.id = armor_id;
    armor.color = rm::ARMOR_COLOR_NONE;
    setArmorExtendRectIOU(armor, rect,src_width, src_height, ratio_x, ratio_y);
    armor.size = size;
    setArmorRectCenter(armor);
}


void rm::setRelativeToAbsoluteTrans(Armor& armor) {
    if (armor.four_points.size() < 4) {
        return;
    }
    for (int i = 0; i < 4; i++) {
        armor.four_points[i].x += armor.rect.x;
        armor.four_points[i].y += armor.rect.y;
    }
}

void rm::setArmorFourPointsRelative(Armor& armor, const PointPair& pp0, const PointPair& pp1) {
    armor.four_points.clear();
    if((pp0.point_up.x + pp0.point_down.x) > (pp1.point_up.x + pp1.point_down.x)) {
        armor.four_points.push_back(pp1.point_up);
        armor.four_points.push_back(pp0.point_up);
        armor.four_points.push_back(pp1.point_down);
        armor.four_points.push_back(pp0.point_down);

    } else {
        armor.four_points.push_back(pp0.point_up);
        armor.four_points.push_back(pp1.point_up);
        armor.four_points.push_back(pp0.point_down);
        armor.four_points.push_back(pp1.point_down);
    }
}

void rm::setArmorFourPoints(Armor& armor, const PointPair& pp0, const PointPair& pp1) {
    setArmorFourPointsRelative(armor, pp0, pp1);
    setRelativeToAbsoluteTrans(armor);
}

void rm::resetArmorFourPoints(const cv::Mat& src, Armor& armor, double radius) {
    if (armor.four_points.size() != 4) return;

    double dist = 0.5 * cv::norm(armor.four_points[0] - armor.four_points[2]) +
                  0.5 * cv::norm(armor.four_points[1] - armor.four_points[3]);
    int r = radius * dist;

    for(int i = 0; i < 4; i++) {
        
        cv::Point center = static_cast<cv::Point>(armor.four_points[i]);
        int x_min = max(center.x - r, 0);
        int x_max = min(center.x + r, src.cols - 1);
        int y_min = max(center.y - r, 0);
        int y_max = min(center.y + r, src.rows - 1);

        int gray_sum = 0;
        cv::Point2f pixel_sum = cv::Point2f(0, 0);
        for (int y = y_min; y <= y_max; ++y) {
            for (int x = x_min; x <= x_max; ++x) {
                if (cv::norm(cv::Point(x, y) - center) > r) continue;
                
                int gray_num = 0;
                switch(armor.color) {
                    case ARMOR_COLOR_BLUE:
                        gray_num = src.at<cv::Vec3b>(y, x)[0];
                        break;
                    case ARMOR_COLOR_RED:
                        gray_num = src.at<cv::Vec3b>(y, x)[2];
                        break;
                    case ARMOR_COLOR_PURPLE:
                        gray_num = src.at<cv::Vec3b>(y, x)[0] + src.at<cv::Vec3b>(y, x)[2];
                        break;
                    case ARMOR_COLOR_NONE:
                        return;
                }
                pixel_sum += cv::Point2f(x, y) * gray_num;
                gray_sum += gray_num;
            }
        }
        armor.four_points[i] = pixel_sum / gray_sum;
    }
}

void rm::setArmorSizeByPoints(Armor& armor, double ratio) {
    if(ratio < 1.0) ratio = 1.0 / ratio;
    double width = cv::norm(armor.four_points[0] - armor.four_points[1]) + cv::norm(armor.four_points[2] - armor.four_points[3]);
    double height = cv::norm(armor.four_points[0] - armor.four_points[2]) + cv::norm(armor.four_points[1] - armor.four_points[3]);
    double armor_ratio = width / height;
    rm::message("armor ratio", armor_ratio);
    if (armor_ratio > ratio) {
        armor.size = ARMOR_SIZE_BIG_ARMOR;
    } else {
        armor.size = ARMOR_SIZE_SMALL_ARMOR;
    }
}

void rm::setLighterLUT(cv::Mat& src){
    uchar lutData[256 * 3];
	for (int i = 0; i < 256; i++)
	{
        for(int j = 0; j < 3; j++){
            switch(i / 40) {
                case 0: lutData[i * 3 + j] = 40; break;
                case 1: lutData[i * 3 + j] = 80; break;
                case 2: lutData[i * 3 + j] = 120; break;
                case 3: lutData[i * 3 + j] = 160; break;
                case 4: lutData[i * 3 + j] = 200; break;
                case 5: lutData[i * 3 + j] = 240; break;
                case 6: lutData[i * 3 + j] = 255; break;
                default: break;
            }
		}
	}
    cv::Mat lut(1, 256, CV_8UC3, lutData);
    cv::LUT(src, lut, src);
}

void setLighterHSV(cv::Mat& src){
    cv::cvtColor(src, src, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> channels;
    cv::split(src, channels);

    float brightness_factor = 1.5; 
    channels[2] *= brightness_factor;

    cv::merge(channels, src);

    cvtColor(src, src, cv::COLOR_HSV2BGR);
}