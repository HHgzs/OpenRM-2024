/*
 * AzureBot Camera Test
 * 
 * Simple camera verification program that works on both macOS and Linux.
 * Tests if your camera is working properly before running full detection.
 * 
 * Usage:
 *   ./camera_test           # Use default camera (0)
 *   ./camera_test 1         # Use camera ID 1
 */

#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    // Parse camera ID from command line (default = 0)
    int camera_id = 0;
    if (argc > 1) {
        camera_id = std::atoi(argv[1]);
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "  AzureBot Camera Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Camera ID: " << camera_id << std::endl;
    std::cout << "Press 'q' to quit" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // Open camera
    cv::VideoCapture cap(camera_id);
    if (!cap.isOpened()) {
        std::cerr << "ERROR: Cannot open camera " << camera_id << std::endl;
        std::cerr << "Try a different camera ID or check your camera connection." << std::endl;
        return -1;
    }

    // Get camera properties
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    
    std::cout << "Camera opened successfully!" << std::endl;
    std::cout << "Resolution: " << width << "x" << height << std::endl;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "\n";

    cv::Mat frame;
    auto last_time = std::chrono::high_resolution_clock::now();
    int frame_count = 0;
    
    while (true) {
        // Capture frame
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "ERROR: Empty frame captured" << std::endl;
            break;
        }

        // Calculate FPS
        frame_count++;
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - last_time).count();
        
        if (elapsed >= 1000) {  // Update every second
            double real_fps = frame_count * 1000.0 / elapsed;
            cv::putText(frame, 
                       cv::format("FPS: %.1f", real_fps),
                       cv::Point(10, 30),
                       cv::FONT_HERSHEY_SIMPLEX,
                       1.0,
                       cv::Scalar(0, 255, 0),
                       2);
            
            std::cout << "Real-time FPS: " << real_fps << std::endl;
            frame_count = 0;
            last_time = current_time;
        }

        // Add status text
        cv::putText(frame, 
                   "Camera Test - Press 'q' to quit",
                   cv::Point(10, frame.rows - 20),
                   cv::FONT_HERSHEY_SIMPLEX,
                   0.6,
                   cv::Scalar(255, 255, 255),
                   2);

        // Display frame
        cv::imshow("AzureBot Camera Test", frame);

        // Check for quit key
        char key = cv::waitKey(1);
        if (key == 'q' || key == 'Q' || key == 27) {  // 'q' or ESC
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    
    std::cout << "\nCamera test completed successfully!" << std::endl;
    return 0;
}

