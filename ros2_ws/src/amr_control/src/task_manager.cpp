#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "geometry_msgs/msg/pose_stamped.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

namespace
{
constexpr double kDefaultWaypointPublishPeriodSeconds = 6.0;
}

class TaskManagerNode : public rclcpp::Node
{
public:
  TaskManagerNode() : Node("task_manager")
  {
    waypoints_ = this->declare_parameter<std::vector<double>>(
      "waypoints",
      std::vector<double>{0.0, 0.0, 3.0, 1.0, 3.0, -1.0, 0.0, 0.0});
    frame_id_ = this->declare_parameter<std::string>("goal_frame", "map");
    publish_period_seconds_ = this->declare_parameter<double>(
      "publish_period_seconds", kDefaultWaypointPublishPeriodSeconds);

    goal_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/goal_pose", 10);
    timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double>(publish_period_seconds_)),
      std::bind(&TaskManagerNode::publishNextGoal, this));

    if (waypoints_.size() % 2 != 0) {
      RCLCPP_WARN(this->get_logger(), "Waypoint list has odd length; last value will be ignored.");
      waypoints_.pop_back();
    }
  }

private:
  void publishNextGoal()
  {
    if (waypoints_.empty()) {
      RCLCPP_WARN(this->get_logger(), "No waypoints configured for task manager.");
      return;
    }

    const size_t point_count = waypoints_.size() / 2;
    const size_t index = current_waypoint_index_ % point_count;

    geometry_msgs::msg::PoseStamped goal;
    goal.header.stamp = this->now();
    goal.header.frame_id = frame_id_;
    goal.pose.position.x = waypoints_[2 * index];
    goal.pose.position.y = waypoints_[2 * index + 1];
    goal.pose.orientation.w = 1.0;

    goal_pub_->publish(goal);

    RCLCPP_INFO(
      this->get_logger(), "Published waypoint %zu/%zu: (%.2f, %.2f)",
      index + 1, point_count, goal.pose.position.x, goal.pose.position.y);

    ++current_waypoint_index_;
  }

  std::vector<double> waypoints_;
  std::string frame_id_;
  double publish_period_seconds_;
  size_t current_waypoint_index_{0};

  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr goal_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TaskManagerNode>());
  rclcpp::shutdown();
  return 0;
}
