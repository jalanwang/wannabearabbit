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
        cap.open(0, cv::CAP_V4L2);
        if (!cap.isOpened()) {
            std::cerr << "캠을 열 수 없습니다." << std::endl;
            release();
            return false;
        }

        // wsl 환경 설정
        cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 800);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
        cap.set(cv::CAP_PROP_FPS, 30);

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