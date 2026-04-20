# AMR Integration Contracts

## TF Frames
- `map`: global planning frame
- `odom`: local odometry frame
- `base_link`: robot base frame
- `lidar_link`: laser sensor frame
- `camera_link`: camera sensor frame

## Topics
- `/scan` (`sensor_msgs/msg/LaserScan`): LiDAR from simulation
- `/camera/image_raw` (`sensor_msgs/msg/Image`): camera stream
- `/detected_objects` (`geometry_msgs/msg/PoseArray`): perception outputs in map frame
- `/goal_pose` (`geometry_msgs/msg/PoseStamped`): high-level navigation goal input
- `/target_velocity` (`geometry_msgs/msg/Twist`): desired velocity for PID controller
- `/current_velocity` (`geometry_msgs/msg/Twist`): feedback velocity for PID controller
- `/cmd_vel` (`geometry_msgs/msg/Twist`): motion command output

## Services/Actions (Nav2 stack)
- Nav2 action APIs are expected to be available when `amr_navigation` is launched.
- Mission control currently publishes to `/goal_pose`; integration to NavigateToPose action can be added in a next phase.

## QoS Guidance
- Sensor streams should use `SensorDataQoS` where appropriate.
- Command/control topics use reliable QoS with depth 10 by default.
