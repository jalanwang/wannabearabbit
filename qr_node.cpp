#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp" // Nav2 목표 지점 메시지 타입
#include "cv_bridge/cv_bridge.h"
#include <opencv2/opencv.hpp>

using std::placeholders::_1;

class QRNavigationNode : public rclcpp::Node {
public:
    QRNavigationNode() : Node("qr_navigation_node") {
        // [개선] 토픽 이름을 파라미터로 설정 (기본값: /image_raw)
        // 실행 시 변경 가능: ros2 run turtlebot_qr qr_node --ros-args -p image_topic:=/camera/image
        this->declare_parameter<std::string>("image_topic", "/image_raw");
        std::string image_topic = this->get_parameter("image_topic").as_string();

        // [구독] 카메라 데이터 수신 (기존 WebcamManager 대체)
        image_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            image_topic, 
            rclcpp::SensorDataQoS(), 
            std::bind(&QRNavigationNode::image_callback, this, _1));

        // [발행] Nav2에게 목표 지점 전달 (이동 명령)
        goal_publisher_ = this->create_publisher<geometry_msgs::msg::PoseStamped>(
            "/goal_pose", 
            10);

        // QR 감지기 초기화
        qr_detector_ = cv::QRCodeDetector();

        RCLCPP_INFO(this->get_logger(), "QR Navigation Node 시작. 대기 중인 토픽: %s", image_topic.c_str());
    }

private:
    // 멤버 변수 선언
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscription_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr goal_publisher_;
    
    cv::QRCodeDetector qr_detector_;
    std::vector<cv::Point> points_;

    // [콜백] 카메라 이미지가 들어올 때마다 실행됨
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        try {
            // ROS 이미지를 OpenCV Mat으로 변환 (복사 모드)
            cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;

            // QR 처리 로직 실행
            std::string qr_data = processFrame(frame);

            if (!qr_data.empty()) {
                // 좌표 변환
                TargetPos target = getTargetLocation(qr_data);
                RCLCPP_INFO(this->get_logger(), "QR 발견: %s -> 목표 좌표: x=%.2f, y=%.2f", 
                            qr_data.c_str(), target.x, target.y);

                // ROS 2 Nav2로 이동 명령 발행
                publishGoal(target.x, target.y);
            }

            // 디버깅용 화면 출력 (GUI 환경인 경우)
            cv::imshow("Robot Camera View", frame);
            cv::waitKey(1);

        } catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge 예외: %s", e.what());
        }
    }

    // --- 기존 QRGame.hpp 로직 이식 ---

    struct TargetPos { double x; double y; };

    // QR 데이터 -> 좌표 매핑
    TargetPos getTargetLocation(const std::string& qr_data) {
        // 예시 좌표 (실제 맵 좌표계에 맞춰 수정 필요)
        if (qr_data == "101") return {1.2, 0.5};
        if (qr_data == "102") return {2.5, -1.0};
        return {0.0, 0.0};
    }

    // 프레임 처리 및 시각화
    std::string processFrame(cv::Mat &frame) {
        points_.clear();
        std::string data = qr_detector_.detectAndDecode(frame, points_);

        if (!data.empty()) {
            drawResult(frame, data);
        } else {
            cv::putText(frame, "Waiting for QR...", cv::Point(50, 50),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
        }
        return data;
    }

    void drawResult(cv::Mat &frame, const std::string &data) {
        if (points_.empty()) return;
        for (size_t i = 0; i < points_.size(); i++) {
            cv::line(frame, points_[i], points_[(i + 1) % points_.size()], cv::Scalar(0, 255, 0), 3);
        }
        cv::putText(frame, "ID: " + data, points_[0], 
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0), 2);
    }

    // --- ROS 2 이동 명령 함수 ---

    void publishGoal(double x, double y) {
        geometry_msgs::msg::PoseStamped goal_msg;

        // 헤더 설정 (현재 시간, 기준 좌표계)
        goal_msg.header.stamp = this->now();
        goal_msg.header.frame_id = "map"; // 보통 'map' 좌표계를 기준으로 이동함

        // 목표 위치 설정
        goal_msg.pose.position.x = x;
        goal_msg.pose.position.y = y;
        goal_msg.pose.position.z = 0.0;

        // 방향(Orientation)은 일단 정면(0도)으로 설정 (Quaternion)
        goal_msg.pose.orientation.x = 0.0;
        goal_msg.pose.orientation.y = 0.0;
        goal_msg.pose.orientation.z = 0.0;
        goal_msg.pose.orientation.w = 1.0;

        // 토픽 발행 -> Nav2가 수신하여 이동 시작
        goal_publisher_->publish(goal_msg);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<QRNavigationNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}