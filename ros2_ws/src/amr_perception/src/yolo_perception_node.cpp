#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "builtin_interfaces/msg/time.hpp"
#include "geometry_msgs/msg/pose_array.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

class YoloPerceptionNode : public rclcpp::Node
{
public:
  YoloPerceptionNode() : Node("yolo_perception_node")
  {
    model_path_ = this->declare_parameter<std::string>("model_path", "models/yolov11_warehouse.pt");
    confidence_threshold_ = this->declare_parameter<double>("confidence_threshold", 0.45);
    publish_rate_hz_ = this->declare_parameter<double>("publish_rate_hz", 5.0);
    target_classes_ = this->declare_parameter<std::vector<std::string>>(
      "target_classes", std::vector<std::string>{"person", "box", "pallet"});

    image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
      "/camera/image_raw", rclcpp::SensorDataQoS(),
      std::bind(&YoloPerceptionNode::imageCallback, this, std::placeholders::_1));

    detected_objects_pub_ = this->create_publisher<geometry_msgs::msg::PoseArray>(
      "/detected_objects", 10);

    auto period = std::chrono::duration<double>(1.0 / std::max(1.0, publish_rate_hz_));
    timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::milliseconds>(period),
      std::bind(&YoloPerceptionNode::publishDetections, this));

    RCLCPP_INFO(
      this->get_logger(),
      "YOLO perception interface started (model=%s, conf=%.2f, rate=%.2fHz)",
      model_path_.c_str(), confidence_threshold_, publish_rate_hz_);
  }

private:
  void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
  {
    last_image_stamp_ = msg->header.stamp;
    has_image_stamp_ = true;
  }

  void publishDetections()
  {
    geometry_msgs::msg::PoseArray detections;
    detections.header.stamp = has_image_stamp_ ? last_image_stamp_ : this->now();
    detections.header.frame_id = "map";

    detected_objects_pub_->publish(detections);
  }

  std::string model_path_;
  double confidence_threshold_;
  double publish_rate_hz_;
  std::vector<std::string> target_classes_;

  builtin_interfaces::msg::Time last_image_stamp_{};
  bool has_image_stamp_{false};
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
  rclcpp::Publisher<geometry_msgs::msg::PoseArray>::SharedPtr detected_objects_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<YoloPerceptionNode>());
  rclcpp::shutdown();
  return 0;
}
