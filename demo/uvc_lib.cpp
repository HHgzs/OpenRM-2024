#include <openrm/openrm.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main() {
    std::vector<std::string> device_list;
    // rm::listUVC(device_list, "usb_cam_");
    rm::listUVC(device_list, "video");
    std::vector<rm::Camera*> cameras;
    std::vector<TimePoint> tp;

    for(int i = 0; i < device_list.size(); i++) {
        rm::Camera* camera = new rm::Camera();
        rm::openUVC(camera, 1920, 1080, 30, 24, device_list[i]);
        rm::runUVC(camera, nullptr, 30);
        cameras.push_back(camera);
        TimePoint tp0 = getTime();
        tp.push_back(tp0);
    }
    
    while(1) {
        for (int i = 0; i < cameras.size(); i++) {

            std::shared_ptr<rm::Frame> frame;
            frame = nullptr;

            frame = cameras[i]->buffer->pop();
            while (frame == nullptr) {
                frame = cameras[i]->buffer->pop();
            }

            TimePoint tp1 = getTime();

            double dt = getDoubleOfS(tp[i], tp1);
            tp[i] = tp1;

            rm::print3d(i, dt, 1 / dt, "id", "dt", "fps");
        }
    }
}

