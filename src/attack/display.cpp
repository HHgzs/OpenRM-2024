#include "attack/attack.h"
using namespace rm;

void AttackDisplayer::push(int id, int color, double x, double y) {
    int center_xy = canvas_side_ / 2;
    int r = canvas_side_ / 40;

    int x_show = static_cast<int>(center_xy + x * center_xy / max_dist_);
    int y_show = static_cast<int>(center_xy - y * center_xy / max_dist_);
    x_show = std::clamp(x_show, 0, canvas_side_ - 1);
    y_show = std::clamp(y_show, 0, canvas_side_ - 1);

    std::string text = std::to_string(id);
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 2;
    cv::Scalar text_color(255, 255, 255);
    cv::Scalar circle_color(255, 255, 255);
    cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, nullptr);
    cv::Point text_org(x_show - textSize.width / 2, y_show + textSize.height / 2);

    switch (color) {
        case 0:
            circle_color = cv::Scalar(255, 150, 50);
            break;
        case 1:
            circle_color = cv::Scalar(100, 100, 255);
            break;
    }

    cv::circle(canvas_, cv::Point(x_show, y_show), r, circle_color, 2);
    cv::putText(canvas_, text, text_org, fontFace, fontScale, text_color, thickness);
}

void AttackDisplayer::push(int id, double pose_x, double pose_y) {
    int center_xy = canvas_side_ / 2;
    int min_r = canvas_side_ / 50;
    int max_r = canvas_side_ / 30;

    int x = static_cast<int>(center_xy + pose_x * center_xy / max_dist_);
    int y = static_cast<int>(center_xy - pose_y * center_xy / max_dist_);

    cv::Scalar color(0, 255, 255);
    cv::line(canvas_, cv::Point(x - min_r, y - min_r), cv::Point(x - max_r, y - max_r), color, 2);
    cv::line(canvas_, cv::Point(x - min_r, y + min_r), cv::Point(x - max_r, y + max_r), color, 2);
    cv::line(canvas_, cv::Point(x + min_r, y - min_r), cv::Point(x + max_r, y - max_r), color, 2);
    cv::line(canvas_, cv::Point(x + min_r, y + min_r), cv::Point(x + max_r, y + max_r), color, 2);
}

void AttackDisplayer::push(double body_angle, double head_angle) {
    int center_xy = canvas_side_ / 2;
    int body_r = canvas_side_ / 15;
    int head_r = canvas_side_ / 20;
    int extend = canvas_side_ / 40;

    cv::circle(canvas_, cv::Point(center_xy, center_xy), body_r, cv::Scalar(0, 255, 255), 2);
    cv::circle(canvas_, cv::Point(center_xy, center_xy), head_r, cv::Scalar(0, 255, 0), 2);

    cv::Point body_end((body_r + extend) * cos(body_angle) + center_xy, center_xy - (body_r + extend) * sin(body_angle));
    cv::Point body_start_0(body_r * cos(body_angle + 0.5) + center_xy, center_xy - body_r * sin(body_angle + 0.5));
    cv::Point body_start_1(body_r * cos(body_angle - 0.5) + center_xy, center_xy - body_r * sin(body_angle - 0.5));
    cv::line(canvas_, body_start_0, body_end, cv::Scalar(0, 255, 255), 2);
    cv::line(canvas_, body_start_1, body_end, cv::Scalar(0, 255, 255), 2);

    cv::Point head_end((head_r + extend) * cos(head_angle) + center_xy, center_xy - (head_r + extend) * sin(head_angle));
    cv::Point head_start_0(head_r * cos(head_angle + 0.5) + center_xy, center_xy - head_r * sin(head_angle + 0.5));
    cv::Point head_start_1(head_r * cos(head_angle - 0.5) + center_xy, center_xy - head_r * sin(head_angle - 0.5));
    cv::line(canvas_, head_start_0, head_end, cv::Scalar(0, 255, 0), 2);
    cv::line(canvas_, head_start_1, head_end, cv::Scalar(0, 255, 0), 2);
}

cv::Mat AttackDisplayer::refresh() {
    cv::Mat ret_mat = canvas_.clone();
    canvas_ = cv::Mat(canvas_side_, canvas_side_, CV_8UC3, cv::Scalar(50, 50, 50));
    return ret_mat;
}