cmake_minimum_required(VERSION 3.8)
project(turtlebot_qr)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# ROS 2 패키지 및 OpenCV 찾기
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# 실행 파일 생성 (qr_node.cpp 사용)
add_executable(qr_node qr_node.cpp)

# 의존성 링크
ament_target_dependencies(qr_node
  rclcpp
  sensor_msgs
  geometry_msgs
  cv_bridge
  OpenCV
)

# 설치 규칙 (colcon build 후 install 폴더로 복사됨)
install(TARGETS qr_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
