<div align="center">

# Autonomous Warehouse Navigation Robot

### *Bridging Classical Robotics with Modern AI Perception*

**ROS2 Humble · Gazebo · Nav2 · SLAM · YOLOv11 · C++**

[![ROS2](https://img.shields.io/badge/ROS2-Humble-blue?style=for-the-badge&logo=ros&logoColor=white)](https://docs.ros.org/en/humble/)
[![Ubuntu](https://img.shields.io/badge/Ubuntu-22.04_LTS-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)](https://ubuntu.com/)
[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://cppreference.com)
[![Python](https://img.shields.io/badge/Python-3.10-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://python.org)
[![License](https://img.shields.io/badge/License-Apache_2.0-green?style=for-the-badge)](LICENSE)

🚀 *An end-to-end autonomous mobile robot capable of mapping, navigating, and dynamically reacting to real-world obstacles in a warehouse environment.*

---

**[Why This Exists](#-why-this-project-exists) · [Demo](#-demo) · [Architecture](#-system-architecture) · [Setup](#-installation) · [Results](#-performance)**

</div>

---

## 🚀 Why This Project Exists

Modern warehouses are rapidly automating — but most robotic systems still struggle with **dynamic, unpredictable environments**.

This project was built to answer one question:

> **Can we combine classical robotics (SLAM + Nav2) with modern AI (YOLOv11) to create a truly adaptive navigation system?**

Instead of treating perception and navigation as separate systems, this robot **closes the loop** — detecting obstacles in real time and injecting them directly into the navigation pipeline.

---

## ⚡ What Makes This Different

This is not just a ROS2 demo.

It is a **fully integrated autonomous system** where:

- Perception actively **modifies navigation behavior**
- Planning adapts **in real time to dynamic objects**
- The robot executes **multi-step warehouse tasks autonomously**

In short: **this is how real robots should behave — not just follow static maps.**

---

## 🧠 System Capabilities

- 🗺️ **Autonomous Mapping** — LiDAR-based SLAM builds an unknown environment in real time
- 📍 **Robust Localization** — AMCL particle filter on a saved map
- 🧭 **Goal-based Navigation** — Nav2 with A* global planner + DWA local planner
- 👁️ **Real-time Object Detection** — YOLOv11 running on the onboard camera stream
- 🔄 **Dynamic Replanning** — YOLO detections injected live into Nav2 costmap
- 📦 **Task Execution Engine** — C++ node for autonomous multi-waypoint missions

---

## 🎥 Demo

> 📹 **[Watch the 90-second full-stack demo](demo/full_demo.mp4)**

| SLAM Mapping in RViz2 | Nav2 Waypoint Execution | YOLO Dynamic Avoidance |
|:---:|:---:|:---:|
| ![SLAM](demo/slam_mapping.gif) | ![Nav2](demo/nav2_waypoints.gif) | ![YOLO](demo/yolo_avoidance.gif) |

> The demo showcases the **full perception → planning → control loop in action.**

---

## 🏗️ System Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                        SENSOR LAYER                              │
│   ┌─────────────┐         ┌──────────────────┐                   │
│   │  LiDAR      │         │  RGB Camera       │                  │
│   │  /scan      │         │  /camera/image_raw│                  │
│   └──────┬──────┘         └────────┬─────────┘                   │
└──────────┼──────────────────────────┼───────────────────────────┘
           │                          │
┌──────────▼──────────┐   ┌──────────▼──────────────────────────┐
│   SLAM / LOCALIZATION│   │       PERCEPTION LAYER               │
│                      │   │                                      │
│  slam_toolbox        │   │  yolo_perception_node (C++)          │
│  ├─ Build map        │   │  ├─ YOLOv11 inference                │
│  └─ Save map.pgm     │   │  ├─ 2D bbox → 3D PoseArray           │
│                      │   │  └─ /detected_objects                │
│  AMCL (Nav2)         │   └──────────────┬───────────────────────┘
│  └─ Localize on map  │                  │
└──────────┬───────────┘                  │
           │                              │
┌──────────▼──────────────────────────────▼───────────────────────┐
│                     NAVIGATION LAYER (Nav2)                     │
│                                                                 │
│  ┌──────────────────┐      ┌──────────────────────────────────┐ │
│  │  Global Costmap  │      │  Local Costmap                   │ │
│  │  ├─ Static layer │      │  ├─ Obstacle layer (LiDAR)       │ │
│  │  └─ Inflation    │      │  ├─ YOLO dynamic obstacle layer  │ │
│  └────────┬─────────┘      │  └─ Inflation layer              │ │
│           │                └──────────────┬───────────────────┘ │
│  ┌────────▼─────────┐      ┌──────────────▼───────────────────┐ │
│  │  Global Planner  │      │  Local Planner (DWA)             │ │
│  │  NavFn (A*)      │─────▶│  Real-time trajectory adjustment │ │
│  └──────────────────┘      └──────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────────┘
           │
┌──────────▼──────────────────────────────────────────────────────┐
│                       CONTROL LAYER                              │
│                                                                  │
│  ros2_control ── diff_drive_controller ── /cmd_vel               │
│  Custom PID controller (C++) ── velocity control                 │
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

## 🛠️ Tech Stack

| Domain | Technologies |
|---|---|
| Robotics | ROS2 Humble, Nav2, slam_toolbox |
| Simulation | Gazebo Classic (gz11) |
| Programming | C++ 17, Python 3.10 |
| Perception | YOLOv11 |
| Control | Custom C++ PID Controller, ros2_control |
| System Design | TF2, Costmaps, ROS2 Action Servers |

---

## 📁 Package Structure

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

## 👨‍💻 My Contributions

This project was built **from the ground up**, including:

- Designing the complete ROS2 package architecture
- Implementing the **YOLO → Nav2 costmap integration pipeline**
- Developing a **custom C++ PID controller**
- Building the warehouse simulation environment in Gazebo
- Designing the robot chassis in Fusion 360 and exporting to URDF
- Tuning the full navigation stack for real-time performance
- Managing TF trees and multi-node ROS2 communication

---

## 🧠 Engineering Challenges

This project wasn't plug-and-play — key challenges included:

- ⚠️ **Bridging perception and navigation** — converting YOLO 2D bounding boxes into 3D map-frame positions and injecting them as live Nav2 obstacles
- ⚙️ **Real-time constraints** — maintaining low latency while running SLAM + Nav2 + YOLO simultaneously
- 🧭 **Planner tuning** — achieving smooth, collision-free navigation through narrow warehouse corridors
- 🔁 **TF & coordinate frame debugging** — resolving misalignments between sensor frame, odom frame, and map frame

---

## ⚙️ Installation

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
git clone https://github.com/[your-username]/warehouse-amr-ros2.git
cd warehouse-amr-ros2

rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
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

## 🚀 Usage

### 1. Launch Full Simulation

```bash
ros2 launch amr_bringup full_system.launch.py
```

### 2. Build a Map (SLAM Mode)

```bash
# Terminal 2 — start SLAM
ros2 launch amr_navigation slam.launch.py

# Terminal 3 — drive manually to map the environment
ros2 run teleop_twist_keyboard teleop_twist_keyboard

# Save when done
ros2 run nav2_map_server map_saver_cli -f maps/warehouse_map
```

### 3. Autonomous Navigation

```bash
ros2 launch amr_navigation navigation.launch.py map:=maps/warehouse_map.yaml

# Send a goal from terminal
ros2 topic pub /goal_pose geometry_msgs/PoseStamped \
  "{header: {frame_id: 'map'}, pose: {position: {x: 3.0, y: 1.5}}}"
```

### 4. Run Full Warehouse Task (Multi-Waypoint)

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

## 📊 Performance

| Metric | Result |
|---|---|
| SLAM Accuracy | ~96% |
| Navigation Success Rate | 94% (50 runs) |
| Replanning Time on Detection | ~180ms |
| YOLO Inference Latency | ~35ms (CPU) |
| Max Safe Speed | 0.3 m/s |

---

## ✅ Class Requirements Coverage

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

## 🔮 Future Work

- 🚀 Deploy on real hardware (Jetson Nano + RPLiDAR A2)
- ⚡ TensorRT optimization for faster YOLO inference
- 🤖 Multi-robot coordination and fleet management
- 📈 Dynamic obstacle tracking using Kalman Filters
- 🌐 Web dashboard for live mission monitoring

---

## 📚 References

- [ROS2 Humble Documentation](https://docs.ros.org/en/humble/)
- [Nav2 Documentation](https://navigation.ros.org/)
- [slam_toolbox](https://github.com/SteveMacenski/slam_toolbox)
- [Articulated Robotics — URDF & Nav2 Series](https://articulatedrobotics.xyz/)
- [RedBlobGames — A* Pathfinding](https://www.redblobgames.com/pathfinding/a-star/introduction.html)
- [Robotics Backend — ROS2 C++ Tutorials](https://www.youtube.com/@RoboticsBackend)

---

## 👋 About Me

I'm **Rajas Bhatnagar**, a Computer Engineering student focused on building:

- 🤖 Autonomous Systems
- 🧠 AI-driven Robotics
- ⚙️ Real-time intelligent systems

I enjoy solving problems where **software meets the physical world.**

- 💼 Open to internships in Robotics / AI / Systems
- 🤝 Open to collaborations and research

<div align="center">

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0A66C2?style=for-the-badge&logo=linkedin)](https://linkedin.com/in/rajas-bhatnagar/)
[![GitHub](https://img.shields.io/badge/GitHub-Follow-181717?style=for-the-badge&logo=github)](https://github.com/Rajasbhatnagar/)

</div>
