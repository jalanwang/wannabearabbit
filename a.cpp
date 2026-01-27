/*
참조파일
*/

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "cv_bridge/cv_bridge.h"
#include <opencv2/opencv.hpp>

#include "QRgame.hpp"

/**
 * QRNavigationNode 클래스
 * camera_ros 드라이버가 쏘는 /camera/image_raw 토픽을 구독하도록 수정됨.
 */
class QRNavigationNode : public rclcpp::Node {
public:
    QRNavigationNode() : Node("qr_navigation_node") {
        // camera_ros 패키지의 기본 토픽명인 /camera/image_raw로 변경
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 
            rclcpp::SensorDataQoS(), 
            std::bind(&QRNavigationNode::image_callback, this, std::placeholders::_1)
        );

        qr_engine_ = std::make_unique<QRGame>();

        RCLCPP_INFO(this->get_logger(), "QR Navigation Node가 시작되었습니다. (/camera/image_raw 구독 중)");
    }

private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        try {
            // ROS 이미지 메시지를 OpenCV Mat 포맷으로 변환
            cv::Mat frame = cv_bridge::toCvShare(msg, "bgr8")->image;

            // QR 코드 처리
            std::string qr_data = qr_engine_->processFrame(frame);

            if (!qr_data.empty()) {
                RCLCPP_INFO(this->get_logger(), "QR 감지됨: [%s]", qr_data.c_str());
                auto target = qr_engine_->getTargetLocation(qr_data);
                RCLCPP_INFO(this->get_logger(), "이동 목표: x=%.2f, y=%.2f", target.x, target.y);
            }

            cv::imshow("Robot Camera View", frame);
            cv::waitKey(1);

        } catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge 에러: %s", e.what());
        }
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    std::unique_ptr<QRGame> qr_engine_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<QRNavigationNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}