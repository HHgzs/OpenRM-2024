#include "tensorrt/tensorrt.h"
#include "uniterm/uniterm.h"
using namespace rm;

// yolo输出的四点顺序：左上-左下-右下-右上
struct alignas(float) yolofpRaw {
    float pose[8];
    float confidence;
};

struct alignas(float) yolov5Raw {
    float bbox[4];
    float confidence;
};

static int classes_num;
static int yolo_size;

static float* output_buffer;
static int output_bboxes_num;

static float confidence_threshold;
static float nms_threshold;

static int input_width;
static int input_height;
static int infer_width;
static int infer_height;

static float infer_to_input_ratio;
static float top_move_from_input;
static float left_move_from_input;


static void nms_set_ratio() {
    float width_ratio = (float)input_width / (float)infer_width;
    float height_ratio = (float)input_height / (float)infer_height;

    top_move_from_input = ((float)infer_height * width_ratio - (float)input_height) / 2.f;
    left_move_from_input = ((float)infer_width * height_ratio - (float)input_width) / 2.f;

	// 根据缩放比最大的边设置缩放比例
    if (width_ratio > height_ratio) {
        infer_to_input_ratio = width_ratio;
        left_move_from_input = 0;
    } else {
        infer_to_input_ratio = height_ratio;
        top_move_from_input = 0;
    }
}

// 从yolo推理的四点，转化为外接矩形
static cv::Rect nms_get_rect(yolofpRaw* yolo_raw) {
    float min_x = std::min(std::min(yolo_raw->pose[0], yolo_raw->pose[2]), std::min(yolo_raw->pose[4], yolo_raw->pose[6]));
    float max_x = std::max(std::max(yolo_raw->pose[0], yolo_raw->pose[2]), std::max(yolo_raw->pose[4], yolo_raw->pose[6]));
    float min_y = std::min(std::min(yolo_raw->pose[1], yolo_raw->pose[3]), std::min(yolo_raw->pose[5], yolo_raw->pose[7]));
    float max_y = std::max(std::max(yolo_raw->pose[1], yolo_raw->pose[3]), std::max(yolo_raw->pose[5], yolo_raw->pose[7]));

    float left = min_x * infer_to_input_ratio - left_move_from_input;
    float top = min_y * infer_to_input_ratio - top_move_from_input;
    float width = (max_x - min_x) * infer_to_input_ratio;
    float height = (max_y - min_y) * infer_to_input_ratio;

    return cv::Rect(round(left), round(top), round(width), round(height));
}

static std::vector<cv::Point2f> nms_get_fp(yolofpRaw* yolo_raw) {
    int x_index[4] = {0, 6, 2, 4};
    int y_index[4] = {1, 7, 3, 5};
    std::vector<cv::Point2f> four_points;

    for(int i = 0; i < 4; i++) {
        double x = yolo_raw->pose[x_index[i]] * infer_to_input_ratio - left_move_from_input;
        double y = yolo_raw->pose[y_index[i]] * infer_to_input_ratio - top_move_from_input;
        if(x < 0 || x >= input_width || y < 0 || y >= input_height) {
            return std::vector<cv::Point2f>();
        }
        four_points.push_back(cv::Point2f(x, y));
    }
    return four_points;
}

// 从yolo推理的框，转化为opencv的Rect
static cv::Rect nms_get_rect(yolov5Raw* yolo_raw) {
    float x = yolo_raw->bbox[0];
    float y = yolo_raw->bbox[1];
    float w = yolo_raw->bbox[2];
    float h = yolo_raw->bbox[3];

    float half_w = w / 2.f;
    float half_h = h / 2.f;

    float left = (x - half_w) * infer_to_input_ratio - left_move_from_input;
    float top = (y - half_h) * infer_to_input_ratio - top_move_from_input;
    float right = (x + half_w) * infer_to_input_ratio - left_move_from_input;
    float bottom = (y + half_h) * infer_to_input_ratio - top_move_from_input;

    return cv::Rect(round(left), round(top), round(right - left), round(bottom - top));
}

// 上古传承代码，计算iou交并比的函数
static float nms_calcu_iou(cv::Rect box1, cv::Rect box2) {

    // 计算重叠区域左上角坐标
    int x1 = std::max(box1.x, box2.x);
    int y1 = std::max(box1.y, box2.y);
    // 计算重叠区域右下角坐标
    int x2 = std::min(box1.x + box1.width, box2.x + box2.width);
    int y2 = std::min(box1.y + box1.height, box2.y + box2.height);
    // 计算重叠区域宽高
    int w = std::max(0, x2 - x1 + 1);
    int h = std::max(0, y2 - y1 + 1);

    // 计算交并集面积, 1e-5防止除以0
    float over_area = w * h;
    float union_area = box1.width * box1.height + box2.width * box2.height - over_area + 1e-5;

    return over_area / union_area;
}

static void nms_select_confidence_fp(std::vector<YoloRect> &list) {
    // 遍历yolov5推理结果
    for (int i = 0; i < output_bboxes_num; i++) {
		
        // 使用结构体截断Raw数据，其float长度为：8 + 1 + 类别数
        yolofpRaw* yolo_raw = (yolofpRaw*)(output_buffer + i * yolo_size);
        float iou_confidence = yolo_raw->confidence;
        if (iou_confidence < confidence_threshold) continue;


        // 对所有类别的置信度进行筛选，找到最高的
        int class_index = -1;
        float class_confidence = 0;
        for (int i = 0; i < classes_num; i++) {
            float* yolo_float = (float*)yolo_raw;
            float confidence = yolo_float[9 + i] * iou_confidence;
            if (confidence > class_confidence && confidence > confidence_threshold) {
                class_index = i;
                class_confidence = confidence;
            }
        }
        if(class_index == -1) continue;

        // 对贴近边缘的框进行过滤
        bool flag_pose = true;
        for(int i = 0; i < 4; i++) {
            if(yolo_raw->pose[2 * i] < 1e-3 || yolo_raw->pose[2 * i] > (infer_width - 1.001) ||
               yolo_raw->pose[2 * i + 1] < 1e-3 || yolo_raw->pose[2 * i + 1] > (infer_height - 1.001)) {
                flag_pose = false;
                break;
            }
        }
        if(!flag_pose) continue;


        // 创建推理框结构体并赋值
        YoloRect detection_rect;
        detection_rect.confidence = class_confidence;
        detection_rect.class_id = class_index;
        detection_rect.box = nms_get_rect(yolo_raw);
        detection_rect.four_points = nms_get_fp(yolo_raw);

        // 将推理框推入vector中 
        if(detection_rect.four_points.size() != 4) continue;
        list.push_back(detection_rect);
    }
}

// 筛选置信度并创建推理框对象
static void nms_select_confidence_v5(std::vector<YoloRect> &list) {
    for (int i = 0; i < output_bboxes_num; i++) {
		
        // 使用结构体截断Raw数据，其float长度为：4 + 1 + 类别数
        yolov5Raw* yolo_raw = (yolov5Raw*)(output_buffer + i * yolo_size);
        float iou_confidence = yolo_raw->confidence;
        if(iou_confidence < confidence_threshold) continue;

        // 对所有类别的置信度进行筛选，找到最高的
        int class_index = -1;
        float class_confidence = 0;
        for (int i = 0; i < classes_num; i++) {
            float* yolo_float = (float*)yolo_raw;
            float confidence = yolo_float[5 + i] * iou_confidence;
            if (confidence > class_confidence && confidence > confidence_threshold) {
                class_index = i;
                class_confidence = confidence;
            }
        }
        if(class_index == -1) continue;

        // 创建推理框结构体并赋值
        YoloRect detection_rect;
        detection_rect.confidence = class_confidence;
        detection_rect.class_id = class_index;
        detection_rect.box = nms_get_rect(yolo_raw);

        // 将推理框推入vector中 
        list.push_back(detection_rect);
    }
}

static void nms_select_confidence_fpx(std::vector<YoloRect> &list) {
    // 遍历推理结果
    for (int i = 0; i < output_bboxes_num; i++) {
		
        // 使用结构体截断Raw数据，其float长度为：8 + 1 + 颜色数 + 类别数
        yolofpRaw* yolo_raw = (yolofpRaw*)(output_buffer + i * yolo_size);
        float iou_confidence = yolo_raw->confidence;
        if (iou_confidence < confidence_threshold) continue;


        // 对颜色置信度进行筛选
        int color_index = -1;
        float color_confidence = 0;
        for (int i = 0; i < 3; i++) {
            float* yolo_float = (float*)yolo_raw;
            float confidence = yolo_float[9 + i] * iou_confidence;
            if (confidence > color_confidence && confidence > confidence_threshold) {
                color_index = i;
                color_confidence = confidence;
            }
        }
        if(color_index == -1) continue;


        // 对所有类别的置信度进行筛选
        int class_index = -1;
        float class_confidence = 0;
        for (int i = 0; i < classes_num; i++) {
            float* yolo_float = (float*)yolo_raw;
            float confidence = yolo_float[13 + i] * iou_confidence;
            if (confidence > class_confidence && confidence > confidence_threshold) {
                class_index = i;
                class_confidence = confidence;
            }
        }
        if(class_index == -1) continue;


        // 对贴近边缘的框进行过滤
        bool flag_pose = true;
        for(int i = 0; i < 4; i++) {
            if(yolo_raw->pose[2 * i] < 1e-3 || yolo_raw->pose[2 * i] > (infer_width - 1.001) ||
               yolo_raw->pose[2 * i + 1] < 1e-3 || yolo_raw->pose[2 * i + 1] > (infer_height - 1.001)) {
                flag_pose = false;
                break;
            }
        }
        if(!flag_pose) continue;


        // 创建推理框结构体并赋值
        YoloRect detection_rect;
        detection_rect.confidence = class_confidence;
        detection_rect.color_id = color_index;
        detection_rect.class_id = class_index;
        detection_rect.box = nms_get_rect(yolo_raw);
        detection_rect.four_points = nms_get_fp(yolo_raw);

        // 将推理框推入vector中 
        if(detection_rect.four_points.size() != 4) continue;
        list.push_back(detection_rect);
    }
}

// 按置信度排序
static void nms_sort_confidence(std::vector<YoloRect> &list) {
    if (list.size() <= 1) return;
    std::sort(
        list.begin(),
        list.end(),
        [](const YoloRect& a, const YoloRect& b) {
            return a.confidence > b.confidence;
        }
    );
}

static void nms_select_iou(std::vector<YoloRect> &list) {
    // 创建保留的推理框vector，用于最后替换原推理框vector
    std::vector<YoloRect> retained_list;

    // 对所有类别进行遍历
    if (list.size() <= 1) return;
        
    // 要被决定是否保留的关注框，从当前类别的第一个开始
    YoloRect focus_rect = list[0];

    // 为保留推理框vector创建新的对象，将第一个框推进去
    retained_list = std::vector<YoloRect>();
    retained_list.push_back(focus_rect);

    // 对类内所有推理框进行遍历
    for (size_t focus_index = 1; focus_index < list.size(); focus_index++) {

        // 对所有保留框进行遍历
        bool avaliable_rect = true;
        for (size_t retained_index = 0; retained_index < retained_list.size(); retained_index++) {

            // 计算当前遍历到的保留框和关注框的iou
            float iou = nms_calcu_iou(
                list[focus_index].box,
                retained_list[retained_index].box
            );

            // 已排序说明关注框置信度小于保留框，iou过大则舍弃关注框
            if (iou > nms_threshold) {
                avaliable_rect = false;
                break;
            }
        }

        // 遍历结束后，标签仍为true，则该框可被保留
        if(avaliable_rect) {
            retained_list.push_back(list[focus_index]);
        }
    }

    // 替换当前类的推理框vector为保留框vector
    list = retained_list;
}


std::vector<YoloRect> rm::yoloArmorNMS_FP(
    float* _output_host_buffer,
    int _output_bboxes_num,
    int _classes_num,
    float _confidence_threshold,
    float _nms_threshold,
    int _input_width,
    int _input_height,
    int _infer_width,
    int _infer_height
) {
    output_buffer = _output_host_buffer;
    output_bboxes_num = _output_bboxes_num;
    classes_num = _classes_num;
    confidence_threshold = _confidence_threshold;
    nms_threshold = _nms_threshold;
    input_width = _input_width;
    input_height = _input_height;
    infer_width = _infer_width;
    infer_height = _infer_height;
    yolo_size = 9 + classes_num;
    nms_set_ratio();

    std::vector<YoloRect> detection_list;
    nms_select_confidence_fp(detection_list);
    nms_sort_confidence(detection_list);
    nms_select_iou(detection_list);
    return detection_list;
}

std::vector<YoloRect> rm::yoloArmorNMS_V5(
    float* _output_host_buffer,
    int _output_bboxes_num,
    int _classes_num,
    float _confidence_threshold,
    float _nms_threshold,
    int _input_width,
    int _input_height,
    int _infer_width,
    int _infer_height
) {
    output_buffer = _output_host_buffer;
    output_bboxes_num = _output_bboxes_num;
    classes_num = _classes_num;
    confidence_threshold = _confidence_threshold;
    nms_threshold = _nms_threshold;
    input_width = _input_width;
    input_height = _input_height;
    infer_width = _infer_width;
    infer_height = _infer_height;
    yolo_size = 5 + classes_num;
    nms_set_ratio();

    std::vector<YoloRect> detection_list;
    nms_select_confidence_v5(detection_list);
    nms_sort_confidence(detection_list);
    nms_select_iou(detection_list);
    return detection_list;
}

std::vector<YoloRect> rm::yoloArmorNMS_FPX(
    float* _output_host_buffer,
    int _output_bboxes_num,
    int _classes_num,
    float _confidence_threshold,
    float _nms_threshold,
    int _input_width,
    int _input_height,
    int _infer_width,
    int _infer_height
) {
    output_buffer = _output_host_buffer;
    output_bboxes_num = _output_bboxes_num;
    classes_num = _classes_num;
    confidence_threshold = _confidence_threshold;
    nms_threshold = _nms_threshold;
    input_width = _input_width;
    input_height = _input_height;
    infer_width = _infer_width;
    infer_height = _infer_height;
    yolo_size = 9 + 4 + classes_num;
    nms_set_ratio();

    std::vector<YoloRect> detection_list;
    nms_select_confidence_fpx(detection_list);
    nms_sort_confidence(detection_list);
    nms_select_iou(detection_list);
    return detection_list;
}
