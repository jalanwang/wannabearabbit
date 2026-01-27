#ifndef QR_GAME_HPP
#define QR_GAME_HPP

#include "GameStrategy.hpp"
#include "WebcamManager.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

class QRGame : public GameStrategy {
private:
    WebcamManager& webcam;
    cv::QRCodeDetector qr_detector;
    std::vector<cv::Point> points;

public:
    QRGame(WebcamManager& wm) : webcam(wm) {}

    virtual GameState run() override {
        std::cout << "Starting QR Game..." << std::endl;
        cv::namedWindow("GAME");
        cv::moveWindow("GAME", 0, 0);
        
        while (true) {
            cv::Mat frame;
            if (!webcam.getFrame(frame)) return GameState::EXIT;

            // 제공해주신 로직을 적용하여 프레임 처리
            std::string data = processFrame(frame);
            
            if (!data.empty()) {
                // 인식된 QR 데이터를 기반으로 목표 좌표 획득
                TargetPos target = getTargetLocation(data);
                std::cout << "Target Location -> x: " << target.x << ", y: " << target.y << std::endl;
                
                // [ROS 2 Navigation 연동 계획]
                // 현재는 좌표 계산 로직만 존재하지만, 실제 이동을 위해서는 ROS 2(Nav2)가 필요합니다.
                // 추후 이 부분을 ROS 2 Action Client 호출 코드로 대체하여
                // 'NavigateToPose' 액션을 통해 로봇을 해당 좌표로 이동시켜야 합니다.
            }

            cv::imshow("GAME", frame);

            int key = cv::waitKey(10);
            if (key == 27) return GameState::EXIT; // ESC to exit
            if (key == 32) return GameState::FLAG_GAME; // Space to Flag Game
        }
        return GameState::EXIT;
    }

    // [신규 기능] QR 데이터(숫자)를 좌표 정보로 매핑함
    struct TargetPos { double x; double y; };
    TargetPos getTargetLocation(const std::string& qr_data) {
        // 간단한 매핑 예시 (추후 parking.db 연동 가능)
        if (qr_data == "101") return {1.2, 0.5};
        if (qr_data == "102") return {2.5, -1.0};
        return {0.0, 0.0};
    }

    // [신규 기능] 프레임 내에서 QR 코드를 찾고 내용을 반환함
    std::string processFrame(cv::Mat &frame) {
        points.clear();
        std::string data = qr_detector.detectAndDecode(frame, points);

        if (!data.empty()) {
            drawResult(frame, data);
        } else {
            cv::putText(frame, "Show QR Code", cv::Point(50, 50),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
        }
        return data;
    }

    // [신규 기능] QR 코드의 위치를 이미지 상에 시각화함
    void drawResult(cv::Mat &frame, const std::string &data) {
        if (points.empty()) return;

        for (size_t i = 0; i < points.size(); i++) {
            cv::line(frame, points[i], points[(i + 1) % points.size()], cv::Scalar(0, 255, 0), 3);
        }

        cv::putText(frame, "ID: " + data, points[0], 
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0), 2);
    }
};

#endif // QR_GAME_HPP