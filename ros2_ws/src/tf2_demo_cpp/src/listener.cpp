#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"

class TFListener : public rclcpp::Node
{
public:
    TFListener() : Node("tf_listener")
    {
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&TFListener::lookup_transform, this));
    }

private:
    void lookup_transform()
    {
        try
        {
            auto transform = tf_buffer_->lookupTransform("world", "base_link", tf2::TimePointZero);
            RCLCPP_INFO(this->get_logger(), "X: %.2f Y: %.2f",
                        transform.transform.translation.x,
                        transform.transform.translation.y);
        }
        catch (tf2::TransformException &ex)
        {
            RCLCPP_WARN(this->get_logger(), "%s", ex.what());
        }
    }

    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TFListener>());
    rclcpp::shutdown();
    return 0;
}