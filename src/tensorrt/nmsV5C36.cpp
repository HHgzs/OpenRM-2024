#include "tensorrt/tensorrt.h"
using namespace rm;

struct alignas(float) yoloArmorRaw_V5C36 {
    float bbox[4];
    float confidence;
    float classes[36];
};

static float* output_buffer;

static int output_bboxes_num;
static int armor_classes_num;

static float confidence_threshold;
static float nms_threshold;

static int input_width;
static int input_height;
static int infer_width;
static int infer_height;

static int yolo_size = sizeof(yoloArmorRaw_V5C36) / sizeof(float);

static float infer_to_input_ratio;
static float top_move_from_input;
static float left_move_from_input;

// 为静态参数变量赋值
static void nms_member_init(
    float* _output_buffer,
    int _output_bboxes_num,
    int _armor_classes_num,
    float _confidence_threshold,
    float _nms_threshold,
    int _input_width,
    int _input_height,
    int _infer_width,
    int _infer_height
) {
    output_buffer = _output_buffer;

    armor_classes_num = _armor_classes_num;
    output_bboxes_num = _output_bboxes_num;

    confidence_threshold = _confidence_threshold;
    nms_threshold = _nms_threshold;

    input_width = _input_width;
    input_height = _input_height;
    infer_width = _infer_width;
    infer_height = _infer_height;

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

// 从yolo推理的框，转化为opencv的Rect
static cv::Rect nmstool_get_rect(yoloArmorRaw_V5C36* yolo_raw) {
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
static float nmstool_calcu_iou(cv::Rect box1, cv::Rect box2) {
    // ----------------------------------> x
    // |    A----------
    // |    |         |
    // |    |    B------------
    // |    |    ||||||      |
    // |    -----|----C      |
    // |         |           |
    // |         ------------D
    // y
    //
    // A坐标：(box1.x, box1.y)
    // B坐标：(box2.x, box2.y)
    // 相交区域左上角坐标：(max(box1.x, box2.x), max(box1.y, box2.y))
    // 右下角坐标同理

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

// 筛选置信度并创建推理框对象
static void nms_select_confidence(std::vector<std::vector<YoloRect>> &detection_class_list) {
    // 遍历25200个yolov5推理结果
    for (int i = 0; i < output_bboxes_num; i++) {
        // 对bbox框内是否存在结果的置信度进行筛选
        if (output_buffer[i * yolo_size + 4] < confidence_threshold) {
            continue;
        }
		
        // 使用结构体截断Raw数据，其float长度为：4+1+类别数
        yoloArmorRaw_V5C36* yolo_raw = (yoloArmorRaw_V5C36*)(output_buffer + i * yolo_size);

        // 对所有类别的置信度进行筛选，找到最高的
        int class_index = -1;
        float class_confidence = 0;
        for (int i = 0; i < armor_classes_num; i++) {
            if (yolo_raw->classes[i] > class_confidence && yolo_raw->classes[i] > confidence_threshold) {
                class_index = i;
                class_confidence = yolo_raw->classes[i];
            }
        }
        if(class_index == -1)
            continue;

        // 创建推理框结构体并赋值
        YoloRect detection_rect;
        detection_rect.confidence = yolo_raw->confidence;
        detection_rect.class_id = class_index;
        detection_rect.box = nmstool_get_rect(yolo_raw);

        // 将推理框按类别推入对应的vector中 
        detection_class_list[class_index].push_back(detection_rect);
    }
}

// 对每个类别的推理框vector，同类中按置信度排序
static void nms_sort_confidence(std::vector<std::vector<YoloRect>> &detection_class_list) {
    // 遍历所有类别
    for (auto& detection_rect_list: detection_class_list) {
        if (detection_rect_list.size() <= 1) {
            continue;
        }

        // 设置类内按置信度排序
        std::sort(
            detection_rect_list.begin(),
            detection_rect_list.end(),
            [](const YoloRect& a, const YoloRect& b) {
                return a.confidence > b.confidence;
            }
        );
    }
}

// 在同一个类别中进行nms
static void nms_select_iou_single(std::vector<std::vector<YoloRect>> &detection_class_list) {
    // 创建保留的推理框vector，用于最后替换原推理框vector
    std::vector<YoloRect> retained_rect_list;

    // 对所有类别进行遍历
    for (auto& detection_rect_list: detection_class_list) {
        if (detection_rect_list.size() <= 1) {
            continue;
        }
        
        // 要被决定是否保留的关注框，从当前类别的第一个开始
        YoloRect focus_rect = detection_rect_list[0];

        // 为保留推理框vector创建新的对象，将第一个框推进去
        retained_rect_list = std::vector<YoloRect>();
        retained_rect_list.push_back(focus_rect);

        // 对类内所有推理框进行遍历
        for (size_t focus_index = 1; focus_index < detection_rect_list.size(); focus_index++) {

            // 对所有保留框进行遍历
            bool avaliable_rect = true;
            for (size_t retained_index = 0; retained_index < retained_rect_list.size(); retained_index++) {
                // 计算当前遍历到的保留框和关注框的iou
                float iou = nmstool_calcu_iou(
                    detection_rect_list[focus_index].box,
                    retained_rect_list[retained_index].box
                );

                // 已排序说明关注框置信度小于保留框，iou过大则舍弃关注框
                if (iou > nms_threshold) {
                    avaliable_rect = false;
                    break;
                }
            }

            // 遍历结束后，标签仍为true，则该框可被保留
            if(avaliable_rect) {
                retained_rect_list.push_back(detection_rect_list[focus_index]);
            }
        }

        // 替换当前类的推理框vector为保留框vector
        detection_rect_list = retained_rect_list;
        retained_rect_list.clear();
    }
}

// 对不同类间的推理框进行nms
static void nms_select_iou_class(std::vector<std::vector<YoloRect>> &detection_class_list, std::vector<YoloRect> &result_rect_list) {
    // 清空用于返回的推理框vector
    result_rect_list.clear();

    // 对类进行遍历
    for (size_t class_index = 0; class_index < detection_class_list.size(); class_index++) {
        if (detection_class_list[class_index].empty()) {
            continue;
        }
        
        // 获取当前类的预选框数量，肯定会小于等于4，大于则说明错了
        size_t rect_list_size = detection_class_list[class_index].size();
        if(rect_list_size > 4ull) {
            continue;
        }

        // 对类内所有框进行遍历
        for (size_t rect_index = 0; rect_index < rect_list_size; rect_index++) {
            YoloRect focus_rect = detection_class_list[class_index][rect_index];

            // 如果返回框vector为空，直接推入
            if (result_rect_list.empty()) {
                result_rect_list.push_back(focus_rect);
                continue;
            }


            // 对返回框进行遍历
            bool avaliable_rect = true;
            for (size_t result_index = 0; result_index < result_rect_list.size(); result_index++) {
                YoloRect result_rect = result_rect_list[result_index];

                // 由于上一个函数已经筛过同类的框了，所以如果同类则跳过
                if (result_rect.class_id == focus_rect.class_id) {
                    continue;
                }

                // 对不同类的返回框和关注框计算iou
                float iou = nmstool_calcu_iou(focus_rect.box, result_rect.box);
                
                // 如果iou超过阈值则比较两者的置信度，保留大的
                if (iou > nms_threshold) {
                    if (focus_rect.confidence > result_rect.confidence) {
                        result_rect_list[result_index] = focus_rect;
                    }

                    // 此时关注框已被换进去了，所以改完标签直接结束
                    avaliable_rect = false;
                    break;
                }
            }

            // 根据标签决定是否推入
            if (avaliable_rect) {
                result_rect_list.push_back(focus_rect);
            }
        }
    }
}

std::vector<YoloRect> rm::yoloArmorNMS_V5C36(
    float* _output_host_buffer,
    int _output_bboxes_num,
    int _armor_classes_num,
    float _confidence_threshold,
    float _nms_threshold,
    int _input_width,
    int _input_height,
    int _infer_width,
    int _infer_height
) {
    nms_member_init(
        _output_host_buffer,
        _output_bboxes_num,
        _armor_classes_num,
        _confidence_threshold,
        _nms_threshold,
        _input_width,
        _input_height,
        _infer_width,
        _infer_height
    );

    // 初始化二维vector，第一层记录不同类，第二层是同类推理框
    std::vector<std::vector<YoloRect>> detection_class_list;
    detection_class_list.resize(armor_classes_num);
    
    // 初始化用于返回的框
    std::vector<YoloRect> result_rect_list;

    nms_select_confidence(detection_class_list);
    nms_sort_confidence(detection_class_list);
    nms_select_iou_single(detection_class_list);
    nms_select_iou_class(detection_class_list, result_rect_list);

    return result_rect_list;
}