#include "WebcamManager.hpp"
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "cv_bridge/cv_bridge.h"
#include <opencv2/opencv.hpp>

#include "GameStrategy.hpp"
#include "QRgame.hpp"
#include "FlagGame.hpp"
#include <iostream>
#include <memory>

int main() {
    WebcamManager webcam;
    if (!webcam.initialize()) {
        return -1;
class GameNode : public rclcpp::Node {
public:
    GameNode() : Node("game_node"), current_state_(GameState::QR_GAME) {
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 
            rclcpp::SensorDataQoS(), 
            std::bind(&GameNode::image_callback, this, std::placeholders::_1)
        );

        // Initialize the first game
        current_game_ = std::make_unique<QRGame>();
        
        // Setup window
        cv::namedWindow("GAME");
        cv::moveWindow("GAME", 0, 0);

        RCLCPP_INFO(this->get_logger(), "Game Node Started. State: QR_GAME");
    }

    std::unique_ptr<GameStrategy> currentGame;
    GameState currentState = GameState::QR_GAME; // 초기 게임: QR 게임
    ~GameNode() {
        cv::destroyAllWindows();
    }

    while (currentState != GameState::EXIT) {
        switch (currentState) {
private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        try {
            // Use toCvCopy because the strategy might modify the frame (e.g., flip, draw)
            cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;
            
            // Handle key press (needed for OpenCV window update and game logic)
            int key = cv::waitKey(1);

            if (current_game_) {
                GameState next_state = current_game_->process(frame, key);
                
                if (next_state != current_state_) {
                    change_state(next_state);
                }
            }
            
            cv::imshow("GAME", frame);

            if (current_state_ == GameState::EXIT) {
                rclcpp::shutdown();
            }

        } catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge error: %s", e.what());
        }
    }

    void change_state(GameState new_state) {
        current_state_ = new_state;
        switch (current_state_) {
            case GameState::QR_GAME:
                currentGame = std::make_unique<QRGame>(webcam);
                current_game_ = std::make_unique<QRGame>();
                RCLCPP_INFO(this->get_logger(), "Switched to QR_GAME");
                break;
            case GameState::FLAG_GAME:
                currentGame = std::make_unique<FlagGame>(webcam);
                current_game_ = std::make_unique<FlagGame>();
                RCLCPP_INFO(this->get_logger(), "Switched to FLAG_GAME");
                break;
            default:
                currentState = GameState::EXIT;
                continue;
            case GameState::EXIT:
                RCLCPP_INFO(this->get_logger(), "Exiting Game...");
                current_game_.reset();
                break;
        }

        if (currentGame) {
            // 선택된 게임 실행 (게임이 종료되면 다음 상태 반환)
            currentState = currentGame->run();
        }
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    std::unique_ptr<GameStrategy> current_game_;
    GameState current_state_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<GameNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}