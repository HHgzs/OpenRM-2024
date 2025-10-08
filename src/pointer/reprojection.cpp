#include "pointer/pointer.h"
#include "uniterm/uniterm.h"
#include <vector>

using namespace rm;

static cv::Mat small_decal;
static cv::Mat big_decal;

static std::vector<cv::Point2f> small_decal_points;
static std::vector<cv::Point2f> big_decal_points;

const static int real_armor_sw = 135;
const static int real_armor_sh = 125;

const static int real_armor_bw = 230;
const static int real_armor_bh = 127; 

void rm::initReprojection(
    double real_point_sw, double real_point_sh,
    double real_point_bw, double real_point_bh,
    std::string small_path,
    std::string big_path
) {
    small_decal = cv::imread(small_path, cv::IMREAD_COLOR);
    if(big_path.size() == 0) {
        big_decal = small_decal;
    } else {
        big_decal = cv::imread(big_path, cv::IMREAD_COLOR);
    }
    if (small_decal.empty()) {
        message("Could not open or find the image!\n", rm::MSG_ERROR);
        return;
    }
    paramReprojection(real_point_sw, real_point_sh, real_point_bw, real_point_bh);
}

void rm::paramReprojection(
    double real_point_sw, double real_point_sh,
    double real_point_bw, double real_point_bh
) {

    int pixel_armor_sw = small_decal.cols;
    int pixel_armor_sh = small_decal.rows;

    int pixel_armor_bw = big_decal.cols;
    int pixel_armor_bh = big_decal.rows;

    int pixel_point_sw = (real_point_sw / real_armor_sw) * pixel_armor_sw;
    int pixel_point_sh = (real_point_sh / real_armor_sh) * pixel_armor_sh;

    int pixel_point_bw = (real_point_bw / real_armor_bw) * pixel_armor_bw;
    int pixel_point_bh = (real_point_bh / real_armor_bh) * pixel_armor_bh;

    small_decal_points.clear();
    small_decal_points.emplace_back(pixel_armor_sw/2 - pixel_point_sw/2, pixel_armor_sh/2 - pixel_point_sh/2);
    small_decal_points.emplace_back(pixel_armor_sw/2 + pixel_point_sw/2, pixel_armor_sh/2 - pixel_point_sh/2);
    small_decal_points.emplace_back(pixel_armor_sw/2 - pixel_point_sw/2, pixel_armor_sh/2 + pixel_point_sh/2);
    small_decal_points.emplace_back(pixel_armor_sw/2 + pixel_point_sw/2, pixel_armor_sh/2 + pixel_point_sh/2);

    big_decal_points.clear();
    big_decal_points.emplace_back(pixel_armor_bw/2 - pixel_point_bw/2, pixel_armor_bh/2 - pixel_point_bh/2);
    big_decal_points.emplace_back(pixel_armor_bw/2 + pixel_point_bw/2, pixel_armor_bh/2 - pixel_point_bh/2);
    big_decal_points.emplace_back(pixel_armor_bw/2 - pixel_point_bw/2, pixel_armor_bh/2 + pixel_point_bh/2);
    big_decal_points.emplace_back(pixel_armor_bw/2 + pixel_point_bw/2, pixel_armor_bh/2 + pixel_point_bh/2);

}

void rm::setReprojection(const cv::Mat& src, cv::Mat& dst, std::vector<cv::Point2f> four_points, rm::ArmorSize size) {
    if(four_points.size() != 4) return;
    cv::Mat copy = src.clone();

    if(size == rm::ARMOR_SIZE_SMALL_ARMOR) {
        cv::Mat small_trans_matrix = cv::getPerspectiveTransform(small_decal_points, four_points);
        cv::warpPerspective(small_decal, copy, small_trans_matrix, copy.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
    } else if (size == rm::ARMOR_SIZE_BIG_ARMOR) {
        cv::Mat big_trans_matrix = cv::getPerspectiveTransform(big_decal_points, four_points);
        cv::warpPerspective(big_decal, copy, big_trans_matrix, copy.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
    }

    cv::Mat mask, mask_inv;
    cv::inRange(copy, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 0), mask);
    cv::bitwise_not(mask, mask_inv);

    cv::Mat background, foreground;
    cv::bitwise_and(src, src, background, mask_inv);
    cv::bitwise_and(copy, copy, foreground, mask);
    
    cv::add(background, foreground, dst);
}
