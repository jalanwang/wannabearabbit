cmake_minimum_required(VERSION 3.8)
project(turtlebot)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# 실행 파일 이름을 qr_navigation_node로 통일
add_executable(qr_navigation_node main.cpp)

target_include_directories(qr_navigation_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(qr_navigation_node
  ${OpenCV_LIBRARIES}
)

ament_target_dependencies(qr_navigation_node
  rclcpp
  sensor_msgs
  cv_bridge
)

# 중요: lib 뒤에 반드시 / 가 있어야 함
install(TARGETS qr_navigation_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()