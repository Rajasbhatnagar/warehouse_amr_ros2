# warehouse_amr_ros2
<div align="center">

# 🤖 Autonomous Warehouse Navigation Robot

### ROS2 Humble · Gazebo · Nav2 · SLAM · YOLOv11

[![ROS2](https://img.shields.io/badge/ROS2-Humble-blue?style=for-the-badge&logo=ros&logoColor=white)](https://docs.ros.org/en/humble/)
[![Ubuntu](https://img.shields.io/badge/Ubuntu-22.04_LTS-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)](https://ubuntu.com/)
[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://cppreference.com)
[![Python](https://img.shields.io/badge/Python-3.10-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://python.org)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

*A fully autonomous mobile robot for warehouse operations — built with ROS2, real-time SLAM mapping, Nav2 path planning, and a YOLOv11 perception layer for dynamic obstacle avoidance.*

---

**[Demo Video](#demo) · [System Architecture](#system-architecture) · [Setup](#installation) · [Results](#results)**

</div>

---

## What This Robot Does

This AMR (Autonomous Mobile Robot) operates inside a simulated warehouse environment and can:

- **Map** its environment autonomously using LiDAR-based SLAM
- **Localize** itself on a saved map using AMCL (Adaptive Monte Carlo Localization)
- **Navigate** to any goal point with obstacle-aware global + local path planning via Nav2
- **Detect** dynamic objects in real time using a YOLOv11 perception model
- **Re-route** on the fly when detections are injected as dynamic obstacles into the Nav2 costmap
- **Execute tasks** — navigate sequentially to shelf waypoints and return home, fully autonomously

---

## Demo

> 📹 **[Watch the 90-second full-stack demo](demo/full_demo.mp4)**

| SLAM Mapping in RViz2 | Nav2 Waypoint Execution | YOLO Dynamic Avoidance |
|:---:|:---:|:---:|
| ![SLAM](demo/slam_mapping.gif) | ![Nav2](demo/nav2_waypoints.gif) | ![YOLO](demo/yolo_avoidance.gif) |

---

## System Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                        SENSOR LAYER                              │
│   ┌─────────────┐         ┌──────────────────┐                  │
│   │  LiDAR      │         │  RGB Camera       │                  │
│   │  /scan      │         │  /camera/image_raw│                  │
│   └──────┬──────┘         └────────┬─────────┘                  │
└──────────┼──────────────────────────┼───────────────────────────┘
           │                          │
┌──────────▼──────────┐   ┌──────────▼──────────────────────────┐
│   SLAM / LOCALIZATION│   │       PERCEPTION LAYER               │
│                      │   │                                      │
│  slam_toolbox        │   │  yolo_perception_node (C++)          │
│  ├─ Build map        │   │  ├─ YOLOv11 inference               │
│  └─ Save map.pgm     │   │  ├─ 2D bbox → 3D PoseArray          │
│                      │   │  └─ /detected_objects                │
│  AMCL (Nav2)         │   └──────────────┬───────────────────────┘
│  └─ Localize on map  │                  │
└──────────┬───────────┘                  │
           │                              │
┌──────────▼──────────────────────────────▼───────────────────────┐
│                     NAVIGATION LAYER (Nav2)                      │
│                                                                  │
│  ┌──────────────────┐      ┌──────────────────────────────────┐ │
│  │  Global Costmap  │      │  Local Costmap                   │ │
│  │  ├─ Static layer │      │  ├─ Obstacle layer (LiDAR)       │ │
│  │  └─ Inflation    │      │  ├─ YOLO dynamic obstacle layer  │ │
│  └────────┬─────────┘      │  └─ Inflation layer              │ │
│           │                └──────────────┬───────────────────┘ │
│  ┌────────▼─────────┐      ┌──────────────▼───────────────────┐ │
│  │  Global Planner  │      │  Local Planner (DWA)             │ │
│  │  NavFn (A*)      │─────▶│  Real-time trajectory adjustment  │ │
│  └──────────────────┘      └──────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────────┘
           │
┌──────────▼──────────────────────────────────────────────────────┐
│                       CONTROL LAYER                              │
│                                                                  │
│  ros2_control ── diff_drive_controller ── /cmd_vel              │
│  PID controller (custom C++) ── velocity control                 │
└──────────────────────────────────────────────────────────────────┘
           │
┌──────────▼──────────────────────────────────────────────────────┐
│                     SIMULATION LAYER                             │
│                                                                  │
│  Gazebo Classic (gz11) ── warehouse_world.sdf                   │
│  ├─ Differential drive robot (Fusion 360 CAD → URDF)            │
│  ├─ RPLiDAR A2 plugin                                            │
│  └─ Camera plugin (640×480)                                      │
└──────────────────────────────────────────────────────────────────┘
```

---

## Package Structure

```
warehouse-amr-ros2/
├── src/
│   ├── amr_description/          # Robot URDF + Xacro + STL meshes (Fusion 360)
│   │   ├── urdf/
│   │   │   ├── robot.urdf.xacro
│   │   │   ├── sensors.xacro
│   │   │   └── ros2_control.xacro
│   │   └── meshes/               # CAD-exported STL files
│   │
│   ├── amr_gazebo/               # Simulation worlds + launch files
│   │   ├── worlds/
│   │   │   └── warehouse_world.sdf
│   │   └── launch/
│   │       └── gazebo.launch.py
│   │
│   ├── amr_navigation/           # Nav2 config + SLAM + launch files
│   │   ├── config/
│   │   │   ├── nav2_params.yaml
│   │   │   ├── slam_params.yaml
│   │   │   └── amcl_params.yaml
│   │   ├── maps/
│   │   │   ├── warehouse_map.pgm
│   │   │   └── warehouse_map.yaml
│   │   └── launch/
│   │       ├── slam.launch.py
│   │       └── navigation.launch.py
│   │
│   ├── amr_perception/           # YOLOv11 ROS2 perception node
│   │   ├── src/
│   │   │   └── yolo_perception_node.cpp
│   │   └── config/
│   │       └── yolo_params.yaml
│   │
│   ├── amr_control/              # PID controller + task manager
│   │   ├── src/
│   │   │   ├── pid_controller.cpp
│   │   │   └── task_manager.cpp
│   │   └── include/
│   │
│   └── amr_bringup/              # Full system launch
│       └── launch/
│           └── full_system.launch.py
│
├── docker/
│   ├── Dockerfile
│   └── docker-compose.yml
│
└── demo/
    ├── full_demo.mp4
    ├── slam_mapping.gif
    ├── nav2_waypoints.gif
    └── yolo_avoidance.gif
```

---

## Robot Specifications

| Component | Specification |
|---|---|
| Drive System | Differential drive, 2× 150mm wheels |
| Chassis | 500 × 400 × 150mm (Fusion 360 CAD) |
| LiDAR | RPLiDAR A2 — 360°, 12m range, 10Hz |
| Camera | 640×480 RGB, 30fps |
| Controller | ros2_control diff_drive_controller |

## Navigation Stack

| Module | Implementation |
|---|---|
| SLAM | slam_toolbox (online async mode) |
| Localization | AMCL — particle filter, 500 particles |
| Global Planner | NavFn (A* algorithm) |
| Local Planner | DWA (Dynamic Window Approach) |
| Costmap | 2D, 0.05m resolution, 5m local window |
| Inflation Radius | 0.35m (tuned for warehouse corridors) |

---

## Installation

### Prerequisites

```bash
# Ubuntu 22.04 LTS + ROS2 Humble required
# Install ROS2 Humble: https://docs.ros.org/en/humble/Installation.html

sudo apt install ros-humble-gazebo-ros-pkgs \
                 ros-humble-nav2-bringup \
                 ros-humble-slam-toolbox \
                 ros-humble-ros2-control \
                 ros-humble-ros2-controllers \
                 ros-humble-vision-msgs \
                 python3-colcon-common-extensions -y
```

### Build

```bash
# Clone the repository
git clone https://github.com/[your-username]/warehouse-amr-ros2.git
cd warehouse-amr-ros2

# Install dependencies
rosdep install --from-paths src --ignore-src -r -y

# Build
colcon build --symlink-install

# Source
source install/setup.bash
```

### Docker (Optional)

```bash
docker build -t warehouse-amr:humble .

docker run -it \
  --env DISPLAY=$DISPLAY \
  --volume /tmp/.X11-unix:/tmp/.X11-unix \
  warehouse-amr:humble
```

---

## Usage

### 1. Launch Simulation

```bash
ros2 launch amr_bringup full_system.launch.py
```

Starts Gazebo warehouse world + robot spawn + RViz2.

### 2. Build a Map (SLAM Mode)

```bash
# Terminal 2 — start SLAM
ros2 launch amr_navigation slam.launch.py

# Terminal 3 — drive the robot to map the environment
ros2 run teleop_twist_keyboard teleop_twist_keyboard

# Save the map when done
ros2 run nav2_map_server map_saver_cli -f maps/warehouse_map
```

### 3. Autonomous Navigation

```bash
# Launch Nav2 with saved map
ros2 launch amr_navigation navigation.launch.py map:=maps/warehouse_map.yaml

# Send a goal from terminal
ros2 topic pub /goal_pose geometry_msgs/PoseStamped \
  "{header: {frame_id: 'map'}, pose: {position: {x: 3.0, y: 1.5}}}"
```

### 4. Run Full Warehouse Task (Waypoints)

```bash
# Navigates: Entrance → Shelf A → Shelf B → Home
ros2 run amr_control task_manager
```

### 5. Enable YOLO Perception

```bash
ros2 run amr_perception yolo_perception_node \
  --ros-args -p model_path:=models/yolov11_warehouse.pt
```

---

## Results

| Metric | Value |
|---|---|
| Map accuracy (SLAM vs ground truth) | ~96% |
| Goal success rate (Nav2, 50 runs) | 94% |
| Average replanning time on detection | ~180ms |
| YOLO inference latency | ~35ms (CPU) |
| Max navigation speed | 0.3 m/s |

---

## Class Requirements Coverage

| Requirement | Status | Implementation |
|---|---|---|
| Wheeled mobile robot | ✅ | Differential drive, Fusion 360 CAD |
| 3D modeling in CAD | ✅ | Fusion 360 → STL → URDF meshes |
| Export to URDF + Gazebo | ✅ | Xacro URDF with Gazebo plugins |
| Custom world | ✅ | Warehouse SDF with shelves + obstacles |
| Teleop implementation | ✅ | teleop_twist_keyboard on /cmd_vel |
| PID controller | ✅ | Custom C++ PID node |
| SLAM + Path Planning | ✅ *(Bonus)* | slam_toolbox + Nav2 + A* + DWA |
| Dynamic obstacle avoidance | ✅ *(Bonus)* | YOLOv11 → Nav2 costmap injection |

---

## References

- [ROS2 Humble Documentation](https://docs.ros.org/en/humble/)
- [Nav2 Documentation](https://navigation.ros.org/)
- [slam_toolbox](https://github.com/SteveMacenski/slam_toolbox)
- [Articulated Robotics — URDF & Nav2 Series](https://articulatedrobotics.xyz/)
- [RedBlobGames — A* Pathfinding](https://www.redblobgames.com/pathfinding/a-star/introduction.html)
- [Robotics Backend — ROS2 C++ Tutorials](https://www.youtube.com/@RoboticsBackend)

---

<div align="center">

**Rajas Bhatnagar**
*Autonomous Systems · Computer Vision · ROS2*

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0A66C2?style=for-the-badge&logo=linkedin)](https://linkedin.com/in/your-profile)
[![GitHub](https://img.shields.io/badge/GitHub-Follow-181717?style=for-the-badge&logo=github)](https://github.com/your-username)

</div>
