#ifndef __OPENRM_POINTER_POINTER_H__
#define __OPENRM_POINTER_POINTER_H__

#include "structure/stamp.hpp"
#include "structure/enums.hpp"

namespace rm {

void getGrayScaleRGB(const cv::Mat& input, cv::Mat& gray, ArmorColor color);            // 通过RGB通道拆分获取灰度图
void getGrayScaleHSV(const cv::Mat& input, cv::Mat& gray, ArmorColor color);            // 通过HSV限制色相获取灰度图
void getGrayScaleCVT(const cv::Mat& input, cv::Mat& gray);                              // 将图像直接转换为灰度图
void getGrayScaleMix(const cv::Mat& input, cv::Mat& gray, ArmorColor color);            // 通过RGB和HSV混合方法获取灰度图
void getGrayScaleSub(const cv::Mat& input, cv::Mat& gray, ArmorColor color);            // 通过RGB通道差值方法获取灰度图
void getGrayScale(const cv::Mat& input, cv::Mat& gray, ArmorColor color = ARMOR_COLOR_BLUE, 
                  GrayScaleMethod method = GRAY_SCALE_METHOD_CVT);                      // 获取灰度图统一接口

void getBinaryAverageThreshold(const cv::Mat& input, cv::Mat& binary, int threshold);   // 通过平均阈值获取二值图
void getBinaryMaxMinRatio(const cv::Mat& input, cv::Mat& binary, double ratio);         // 通过最大最小值比率获取二值图
void getBinary(const cv::Mat& input, cv::Mat& binary, double threshold, 
               BinaryMethod method = BINARY_METHOD_MAX_MIN_RATIO);                      // 获取二值图统一接口

ArmorID getArmorIDfromClass36(ArmorClass armor_class);                                    // 通过装甲板类别获取装甲板id
ArmorColor getArmorColorFromClass36(ArmorClass armor_class);                              // 通过装甲板类别获取装甲板颜色
ArmorColor getArmorColorFromHSV(const cv::Mat& src, const rm::LightbarPair &rect);      // 通过HSV获取装甲板颜色
ArmorColor getArmorColorFromHSV(const cv::Mat& src, const rm::YoloRect& rect);          // 通过HSV获取装甲板颜色
ArmorColor getArmorColorFromRGB(const cv::Mat& src, const rm::LightbarPair &rect);      // 通过RBG获取装甲板颜色
ArmorColor getArmorColorFromRGB(const cv::Mat& src, const rm::YoloRect& rect);          // 通过RBG获取装甲板颜色

double getLightHighHLS(const cv::Mat& src);

cv::Rect getExtendRect(const cv::Rect& rect, int src_width, int src_height,
                       double ratio_x = 1.4, double ratio_y = 1.8);                     // 按比例扩展矩形框，具有边界检查

std::string getStringArmorColor(ArmorColor armor_color);                                // 获取装甲板颜色字符串
std::string getStringArmorSize(ArmorSize armor_size);                                   // 获取装甲板大小字符串
std::string getStringArmorID(ArmorID armor_id);                                         // 获取装甲板id字符串

double getRatioOverLap(const cv::Rect &rect1, const cv::Rect &rect2);                   // 获取两个矩形框的重叠率

ArmorColor getRegionColor(const cv::Mat &src, const cv::RotatedRect &rotate_region);    // 获取旋转矩形框内颜色
ArmorColor getRegionColor(const cv::Mat &src, const cv::Rect &rect_region);             // 获取矩形框内颜色

double getRatioRectSide(const cv::Rect &rect);                                          // 获取矩形框长宽比，大比小
double getRatioRectSide(const cv::RotatedRect &rect);                                   // 获取旋转矩形框长宽比，大比小
double getRatioAreaContourToRect(const std::vector<cv::Point> &contour, 
                                 const cv::RotatedRect &rect);                          // 获取轮廓到矩形框的面积比
double getLightbarAngleRectCV45(const cv::RotatedRect &rect);                           // 获取OpenCV4.5灯条角度，-90 ~ 90
double getLightbarAngleRectCV41(const cv::RotatedRect &rect);                           // 获取OpenCV4.1灯条角度，-45 ~ 45

void getLightbarsFromContours(const std::vector<std::vector<cv::Point>> &contours, 
                                 std::vector<Lightbar> &lightbars,
                                 double min_rect_side, 
                                 double max_rect_side,
                                 double min_value_area,
                                 double min_ratio_area,
                                 double max_angle);                                     // 从轮廓列表获取灯条列表

double getValueLengthLightbarPair(const Lightbar &lb1, const Lightbar &lb2);            // 获取两个灯条的长度平均值
double getRatioLengthLightbarPair(const Lightbar &lb1, const Lightbar &lb2);            // 获取两个灯条的长度比, 大比小
double getRatioAreaLightbarPair(const Lightbar &lb1, const Lightbar &lb2);              // 获取两个灯条的面积比, 大比小
double getRatioArmorSide(const Lightbar &lb1, const Lightbar &lb2);                     // 获取两灯条组成的可能装甲板的长宽比
double getAngleDiffLightbarPair(const Lightbar &lb1, const Lightbar &lb2);              // 获取两个灯条的角度差
double getAngleAvgLightbarPair(const Lightbar &lb1, const Lightbar &lb2);               // 获取两个灯条的角度平均值
double getCenterOffsetLightbarPair(const Lightbar &lb1, const Lightbar &lb2);           // 获取两个灯条中心点的在平均方向上偏移量

cv::Point2f getLightbarPairCenter(const Lightbar &lb1, const Lightbar &lb2);            // 获取两个灯条中心点
cv::Point2f getArmorRectCenter(const Armor& armor);                                     // 获取装甲板中心点
cv::Point2f getRelativeToAbsoluteTrans(const cv::Rect& rect, const cv::Point2f& point); // 将点坐标从iou矩形框的相对位置转换到原图的绝对位置


LightbarPair getBestMatchedLightbarPair(const std::vector<LightbarPair>& pairs, 
                                        const Armor& armor);                            // 获取最佳匹配的灯条对

bool getBestMatchedLightbarPair(const std::vector<rm::Lightbar>& lightbars,
                                const Armor& armor,
                                LightbarPair& best_pair,
                                double max_ratio_length, 
                                double max_ratio_area,
                                double min_ratio_side,
                                double max_ratio_side,
                                double max_angle_diff,
                                double max_angle_avg,
                                double max_offset);                                     // 将灯条匹配成对，并获取最佳匹配的灯条对


void getClampRect(const cv::Mat& src, cv::Rect& rect);                                  // 获取矩形框在图像内的边界检查


void showHistogram(cv::Mat& histogram, cv::Mat& ShowImage, int wid = 0, int len = 0);   // 返回直方图的图像版，wid和len为直方图的宽和高
void setLine_Histogram(cv::Mat& input, cv::Mat& output, cv::Mat& histogram, 
                       int set_line = 0, int flag = 0);                                 // 在直方图的图像上画线, flag为0为水平线, 1为垂直线
void getHistogram(const cv::Mat& src, cv::Mat& histogram, int color = 0);               // 获取直方图，color默认为0灰色, 1为蓝色, 2为绿色, 3为红色
std::pair<int, int> getHistDoublePeak(const cv::Mat& histogram);                        // 获取直方图双峰值 
void getHistIncludePeak(const cv::Mat& src, cv::Mat& ShowImage);                        // 获取带有峰位置的直方图图像，竖线标识峰值位置
int getThresholdFromHist(const cv::Mat& src, int Cut_thresold, int bios = 0);           // 通过直方图获取阈值
int getThresholdFromHist(const cv::Mat& src, cv::Mat& ShowImage, 
                         int Cut_thresold, int bios = 0);                               // 通过自定义水平切割直方图 获取阈值，bios为向右偏移量
int getThresholdFromHistPeak(const cv::Mat& src, cv::Mat& ShowImage, int bios = 0);     // 通过直方图峰值获取阈值

void getHistogramEqualization(const cv::Mat& src, cv::Mat& dst);                        // 直方图均衡化


PointPair findPointPairBarycenter(Lightbar lightbar, const cv::Mat& gray,
                                  double extend_dist = 32.0,
                                  double radius_ratio = 0.1);                           // 重心法获取灯条轮廓
void findCircleCenterFromContours(
    const std::vector<std::vector<cv::Point>>& contours,
    std::vector<cv::Point2f>& circles,
    double area_threshold, double circularity_threshold);                               // 从轮廓中获取圆心


void setLightbarRotatedRect(Lightbar& lightbar);                                        // 设置灯条最小外接旋转矩形
void setLigntbarAngleRectCV45(Lightbar& lightbar);                                      // 设置OpenCV4.5灯条角度，-90 ~ 90
void setLigntbarAngleRectCV41(Lightbar& lightbar);                                      // 设置OpenCV4.1灯条角度，-45 ~ 45
void setLightbarRectBasedLength(Lightbar& lightbar);                                    // 设置基于最小外接矩形的灯条长度

void setLightbar(Lightbar& lightbar, const std::vector<cv::Point>& contour);            // 设置灯条轮廓从而设置灯条各参数

void setArmorRectCenter(Armor& armor);                                                  // 设置装甲板中心点

void setArmorExtendRectIOU(Armor& armor, const cv::Rect& rect, int src_width, int src_height,
                           double ratio_x = 1.4, double ratio_y = 1.8);                 // 从yolo扩展为装甲板IOU矩形框，具有边界检查

void setArmorBaseClass36(Armor& armor, 
                        const cv::Rect& rect, 
                        ArmorClass armor_class,
                        int src_width, 
                        int src_height,
                        double ratio_x = 1.4, 
                        double ratio_y = 1.8,
                        ArmorSize size = ARMOR_SIZE_SMALL_ARMOR);                       // 设置装甲板基本信息

void setArmorBaseClass7(Armor& armor, 
                        const cv::Rect& rect, 
                        ArmorID armor_id,
                        int src_width, 
                        int src_height,
                        double ratio_x = 1.4, 
                        double ratio_y = 1.8,
                        ArmorSize size = ARMOR_SIZE_SMALL_ARMOR);                       // 设置装甲板基本信息

void setRelativeToAbsoluteTrans(Armor& armor);                                          // 装甲板点坐标绝对位置转换
void setArmorFourPointsRelative(Armor& armor, const PointPair& pp0, const PointPair& pp1); // 设置装甲板四个顶点相对位置
void setArmorFourPoints(Armor& armor, const PointPair& pp0, const PointPair& pp1);      // 设置装甲板四个顶点
void setArmorSizeByPoints(Armor& armor, double ratio);                                  // 根据装甲板长宽比确定大小
void resetArmorFourPoints(const cv::Mat& src, Armor& armor, double radius);             // 重置装甲板四个顶点

void setLighterLUT(cv::Mat& src);                                                       // 使用 LUT 设置亮度略微提高
void setLighterHSV(cv::Mat& src);                                                       // 使用 HSV 设置亮度略微提高

void displaySingleArmorRect(const cv::Mat& src, const Armor& armor, int thickness = 1); // 显示单个装甲板矩形框
void displaySingleArmorLine(const cv::Mat& src, const Armor& armor, int thickness = 1); // 显示单个装甲板四个顶点
void displaySingleArmorClass(const cv::Mat& src, const Armor& armor);                   // 显示单个装甲板类别文字
void displaySingleArmorID(const cv::Mat& src, const Armor& armor);                      // 显示单个装甲板id文字
void displaySingleArmorWord(const cv::Mat& src, const Armor& armor, const std::string& word);
void displayPredictTargetX(const cv::Mat& src, const cv::Point2f point, ArmorID id);    // 显示预测目标的中心
void displayStrVecOnImage(const cv::Mat& src, const std::vector<std::string>& str_vec); // 显示字符串列表

bool isRatioRectSideValid(const Lightbar& lightbar, double min, double max);            // 判断矩形长宽比是否合法
bool isValueRectAreaValid(const Lightbar& lightbar, double min);                        // 判断矩形框面积是否合法
bool isRatioRectAreaValid(const Lightbar& lightbar, double min);                        // 判断轮廓在矩形框内面积占比是否合法
bool isValueAngleValid(const Lightbar& lightbar, double max);                           // 判断灯条角度是否合法

bool isLightBarValid(const Lightbar& lightbar,
                     double min_rect_side, 
                     double max_rect_side,
                     double min_value_area,
                     double min_ratio_area,
                     double max_angle);                                                 // 判断灯条是否合法

bool isRatioLengthLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, 
                                    double max);                                        // 判断两个灯条长度比是否合法
bool isRatioAreaLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, 
                                    double max);                                        // 判断两个灯条面积比是否合法，比较有用
bool isRatioArmorSideValid(const Lightbar& lb1, const Lightbar& lb2, 
                                    double min, double max);                            // 判断两个灯条组成的可能装甲板的长宽比是否合法
bool isAngleDiffLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, 
                                    double max);                                        // 判断两个灯条角度差是否合法
bool isAngleAvgLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, 
                                    double max);                                        // 判断两个灯条角度平均值是否合法
bool isCenterOffsetLightbarPairValid(const Lightbar& lb1, const Lightbar& lb2, 
                                    double max);                                        // 判断归一化的偏移量是否合法，比较有用

bool isLightBarMatched(const Lightbar& lb1, const Lightbar& lb2, 
                       double max_ratio_length, 
                       double max_ratio_area,
                       double min_ratio_side,
                       double max_ratio_side,
                       double max_angle_diff,
                       double max_angle_avg,
                       double max_offset);                                              // 判断两个灯条是否匹配

bool isLightBarAreaPercentValid(Armor armor, double min_area_percent);                  // 判断两个灯条围成的面积占装甲板面积是否过小

bool isRectValidInImage(const cv::Mat& image, const cv::Rect& rect);                    // 判断矩形框是否在图像内
bool isPointValidInImage(const cv::Mat& image, const cv::Point2f& point);               // 判断点是否在图像内

bool isArmorColorEnemy(const cv::Mat& input,
                      const rm::LightbarPair &lbp,
                      rm::ArmorColor enemy_color,
                      double threshold);

void paramReprojection(double real_point_sw, double real_point_sh,
                       double real_point_bw, double real_point_bh);
void initReprojection(double small_width, double small_height,
                      double big_width, double big_height,
                      std::string small_path,
                      std::string big_path = "");                                      // 初始化重投影参数
void setReprojection(const cv::Mat& src, cv::Mat& dst,
                     std::vector<cv::Point2f> four_points,
                     rm::ArmorSize size);                                                // 设置重投影


}
#endif