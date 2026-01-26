#ifndef RED_BALL_GAME_HPP
#define RED_BALL_GAME_HPP

#include "GameStrategy.hpp"
#include "WebcamManager.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <string>

// 설정값들을 상수로 정의하여 가독성 및 유지보수성 향상
namespace RedBallConfig {
    const int GAUSSIAN_BLUR_KERNEL_SIZE = 15;
    const double THRESHOLD_VALUE = 25.0;
    const double MOVEMENT_THRESHOLD_RATIO = 0.1;
    const int BALL_RADIUS = 20;
}

struct Ball {
    cv::Point position;
    int radius;
};

class RedBallGame : public GameStrategy {
private:
    WebcamManager& webcam;
    Ball redBall;
    cv::Mat prev_gray;
    int score;

    cv::Point getRandomPosition(int width, int height, int radius) {
        int x = rand() % (width - 2 * radius) + radius;
        int y = rand() % (height - 2 * radius) + radius;
        return cv::Point(x, y);
    }

public:
    RedBallGame(WebcamManager& wm) : webcam(wm), score(0) {
        srand(static_cast<unsigned int>(time(0)));
        redBall.radius = RedBallConfig::BALL_RADIUS;
    }

    virtual GameState run() override {
        std::cout << "Starting Red Ball Game..." << std::endl;
        
        int width = webcam.getWidth();
        int height = webcam.getHeight();
        redBall.position = getRandomPosition(width, height, redBall.radius);

        cv::namedWindow("GAME");

        while (true) {
            cv::Mat frame, gray_frame, diff, thresh;
            if (!webcam.getFrame(frame)) return GameState::EXIT;
            cv::flip(frame, frame, 1);

            cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur(gray_frame, gray_frame, cv::Size(RedBallConfig::GAUSSIAN_BLUR_KERNEL_SIZE, RedBallConfig::GAUSSIAN_BLUR_KERNEL_SIZE), 0);

            if (prev_gray.empty()) {
                gray_frame.copyTo(prev_gray);
                continue;
            }

            cv::absdiff(prev_gray, gray_frame, diff);
            cv::threshold(diff, thresh, RedBallConfig::THRESHOLD_VALUE, 255.0, cv::THRESH_BINARY);

            cv::Rect ballRect(
                std::max(0, redBall.position.x - redBall.radius),
                std::max(0, redBall.position.y - redBall.radius),
                redBall.radius * 2,
                redBall.radius * 2
            );
            ballRect &= cv::Rect(0, 0, width, height); // Ensure rect is within frame bounds

            cv::Mat roi = thresh(ballRect);
            int movementPixels = cv::countNonZero(roi);
            if (movementPixels > ballRect.area() * RedBallConfig::MOVEMENT_THRESHOLD_RATIO) {
                score++;
                std::cout << "Touch! Score: " << score << std::endl;
                redBall.position = getRandomPosition(width, height, redBall.radius);
            }

            cv::circle(frame, redBall.position, redBall.radius, cv::Scalar(0, 0, 255), -1);
            cv::putText(frame, "Score: " + std::to_string(score), cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
            cv::imshow("GAME", frame);
            gray_frame.copyTo(prev_gray);

            if (cv::waitKey(10) == 27) return GameState::EXIT;
        }
        return GameState::EXIT;
    }
};

#endif // RED_BALL_GAME_HPP