#include "pointer/pointer.h"
#include <cmath>
#include <algorithm>

using namespace rm;
using namespace std;

static cv::Point2f getBarycenter(const cv::Mat& gray, const cv::Point center, int radius) {
    // 计算圆形区域的遍历范围
    
    int x_min = max(center.x - radius, 0);
    int x_max = min(center.x + radius, gray.cols - 1);
    int y_min = max(center.y - radius, 0);
    int y_max = min(center.y + radius, gray.rows - 1);
    
    // 计算圆形区域的灰度和像素坐标
    int gray_sum = 0;
    cv::Point2f pixel_sum = cv::Point2f(0, 0);
    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            if (cv::norm(cv::Point(x, y) - center) <= radius) {
                gray_sum += static_cast<int>(gray.at<uchar>(y, x));
                pixel_sum += cv::Point2f(x, y) * static_cast<int>(gray.at<uchar>(y, x));
            }
        }
    }
    return pixel_sum / gray_sum;
}

PointPair rm::findPointPairBarycenter(Lightbar lightbar, const cv::Mat& gray, double extend_dist, double radius_ratio) {

    // 找到灯条的中心点
    cv::Point2f center = lightbar.rect.center;

    // 拟合直线
    cv::Vec4f line;
    cv::fitLine(lightbar.contour, line, cv::DIST_L2, 0, 1e-2, 1e-2);

    // 计算直线的斜率和截距
    double k = line[1] / line[0];
    double b = line[3] - k * line[2];

    float first_x, first_y, second_x, second_y;

    // 对端点进行延展
    float dx = cos(atan(k)) * extend_dist;
    float dy = sin(atan(k)) * extend_dist;

    first_x = center.x + dx * ((k > 0) ? 1 : -1);
    first_y = center.y - fabs(dy);
    second_x = center.x + dx * ((k > 0) ? -1 : 1);
    second_y = center.y + fabs(dy);

    cv::Point far_point0 = cv::Point(first_x, first_y);
    cv::Point far_point1 = cv::Point(second_x, second_y);

    // 找到轮廓上的两个端点
    cv::Point end_point0, end_point1;
    double min_dist0 = 1e5, min_dist1 = 1e5;

    for(auto point : lightbar.contour) {
        double dist0 = cv::norm(far_point0 - point);
        double dist1 = cv::norm(far_point1 - point);
        if (dist0 < min_dist0) {
            min_dist0 = dist0;
            end_point0 = point;
        }
        if (dist1 < min_dist1) {
            min_dist1 = dist1;
            end_point1 = point;
        }
    }

    int radius = (int)(radius_ratio * cv::norm(end_point0 - end_point1));
    
    // 计算两个端点的质心
    cv::Point2f first_barycenter = getBarycenter(gray, end_point0, radius);
    cv::Point2f second_barycenter = getBarycenter(gray, end_point1, radius);

    // 保证第一个点在上面，第二个点在下面
    if(first_barycenter.y > second_barycenter.y) {
        swap(first_barycenter, second_barycenter);
    }
    return PointPair(first_barycenter, second_barycenter);
}



void rm::findCircleCenterFromContours(
    const std::vector<std::vector<cv::Point>>& contours,
    std::vector<cv::Point2f>& circles,
    double area_threshold, double circularity_threshold
) {
    circles.clear();
    for (size_t i = 0; i < contours.size(); i++) {

        // 面积筛选
        double area = cv::contourArea(contours[i]);
        if (area > area_threshold) continue;

        // 计算最小外接圆
        float radius;
        cv::Point2f center;
        cv::minEnclosingCircle(contours[i], center, radius);

        // 圆度筛选
        double circularity = area / (M_PI * pow((double)radius, 2));
        if (circularity < circularity_threshold) continue;

        circles.push_back(center);
    }
}
