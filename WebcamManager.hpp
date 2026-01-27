#ifndef WEBCAM_MANAGER_HPP
#define WEBCAM_MANAGER_HPP

#include "opencv2/opencv.hpp"
#include <iostream>

class WebcamManager {
private:
    cv::VideoCapture cap;
    int width;
    int height;

public:
    WebcamManager() : width(0), height(0) {}

    ~WebcamManager() {
        release();
    }

    bool initialize() {
        cap.open(0);
        if (!cap.isOpened()) {
            std::cerr << "캠을 열 수 없습니다." << std::endl;
            return false;
        }

        // 로봇 하드웨어(Raspberry Pi 등) 성능 최적화를 위해 해상도를 640x480으로 조정
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(cv::CAP_PROP_FPS, 30);
        cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

        width = cvRound(cap.get(cv::CAP_PROP_FRAME_WIDTH));
        height = cvRound(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

        return true;
    }

    bool getFrame(cv::Mat& frame) {
        if (!cap.isOpened()) return false;
        cap >> frame;
        return !frame.empty();
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    void release() {
        if (cap.isOpened()) {
            cap.release();
        }
    }
};

#endif // WEBCAM_MANAGER_HPP