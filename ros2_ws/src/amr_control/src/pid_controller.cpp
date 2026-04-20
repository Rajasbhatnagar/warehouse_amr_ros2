#include <algorithm>
#include <chrono>
#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

namespace
{
constexpr double kControlLoopPeriodSeconds = 0.05;
}

class PIDControllerNode : public rclcpp::Node
{
public:
  PIDControllerNode() : Node("pid_controller")
  {
    kp_ = this->declare_parameter<double>("kp", 1.0);
    ki_ = this->declare_parameter<double>("ki", 0.0);
    kd_ = this->declare_parameter<double>("kd", 0.1);
    max_linear_output_ = this->declare_parameter<double>("max_linear_output", 0.3);
    max_angular_output_ = this->declare_parameter<double>("max_angular_output", 0.8);

    target_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "/target_velocity", 10,
      std::bind(&PIDControllerNode::targetCallback, this, std::placeholders::_1));

    feedback_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "/current_velocity", 10,
      std::bind(&PIDControllerNode::feedbackCallback, this, std::placeholders::_1));

    cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<double>(kControlLoopPeriodSeconds)),
      std::bind(&PIDControllerNode::controlLoop, this));
  }

private:
  void targetCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    target_ = *msg;
    has_target_ = true;
  }

  void feedbackCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    feedback_ = *msg;
    has_feedback_ = true;
  }

  void controlLoop()
  {
    if (!has_target_ || !has_feedback_) {
      return;
    }

    const double dt = kControlLoopPeriodSeconds;

    const double linear_error = target_.linear.x - feedback_.linear.x;
    linear_integral_ += linear_error * dt;
    const double linear_derivative = (linear_error - previous_linear_error_) / dt;

    const double angular_error = target_.angular.z - feedback_.angular.z;
    angular_integral_ += angular_error * dt;
    const double angular_derivative = (angular_error - previous_angular_error_) / dt;

    geometry_msgs::msg::Twist cmd;
    cmd.linear.x = clamp(kp_ * linear_error + ki_ * linear_integral_ + kd_ * linear_derivative, max_linear_output_);
    cmd.angular.z = clamp(kp_ * angular_error + ki_ * angular_integral_ + kd_ * angular_derivative, max_angular_output_);

    previous_linear_error_ = linear_error;
    previous_angular_error_ = angular_error;

    cmd_pub_->publish(cmd);
  }

  static double clamp(double value, double limit)
  {
    return std::clamp(value, -limit, limit);
  }

  double kp_;
  double ki_;
  double kd_;
  double max_linear_output_;
  double max_angular_output_;

  double linear_integral_{0.0};
  double angular_integral_{0.0};
  double previous_linear_error_{0.0};
  double previous_angular_error_{0.0};

  geometry_msgs::msg::Twist target_;
  geometry_msgs::msg::Twist feedback_;
  bool has_target_{false};
  bool has_feedback_{false};

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr target_sub_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr feedback_sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PIDControllerNode>());
  rclcpp::shutdown();
  return 0;
}
