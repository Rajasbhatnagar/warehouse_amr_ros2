#include <chrono>
#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_broadcaster.h"

using namespace std::chrono_literals;

class DynamicBroadcaster : public rclcpp::Node
{
public:
    DynamicBroadcaster() : Node("dynamic_broadcaster")
    {
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        timer_ = this->create_wall_timer(100ms, std::bind(&DynamicBroadcaster::broadcast_tf, this));
    }

private:
    void broadcast_tf()
    {
        static double t = 0.0;
        t += 0.1;

        geometry_msgs::msg::TransformStamped transform;

        transform.header.stamp = this->get_clock()->now();
        transform.header.frame_id = "world";
        transform.child_frame_id = "base_link";

        transform.transform.translation.x = cos(t);
        transform.transform.translation.y = sin(t);
        transform.transform.translation.z = 0.0;

        transform.transform.rotation.w = 1.0;

        tf_broadcaster_->sendTransform(transform);
    }

    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DynamicBroadcaster>());
    rclcpp::shutdown();
    return 0;
}