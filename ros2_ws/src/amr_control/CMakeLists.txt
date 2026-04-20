cmake_minimum_required(VERSION 3.8)
project(amr_control)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(pid_controller src/pid_controller.cpp)
ament_target_dependencies(pid_controller rclcpp geometry_msgs)

add_executable(task_manager src/task_manager.cpp)
ament_target_dependencies(task_manager rclcpp geometry_msgs)

install(TARGETS pid_controller task_manager DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
