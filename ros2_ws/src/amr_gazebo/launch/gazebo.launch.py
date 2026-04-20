from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution, Command
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time')
    world = LaunchConfiguration('world')
    spawn_robot = LaunchConfiguration('spawn_robot')

    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([FindPackageShare('gazebo_ros'), 'launch', 'gazebo.launch.py'])
        ]),
        launch_arguments={'world': world}.items(),
    )

    robot_description = ParameterValue(
        Command([
            'xacro ',
            PathJoinSubstitution([FindPackageShare('amr_description'), 'urdf', 'robot.urdf.xacro'])
        ]),
        value_type=str,
    )

    rsp = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}, {'robot_description': robot_description}],
    )

    spawn = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        output='screen',
        arguments=['-entity', 'warehouse_amr', '-topic', 'robot_description', '-x', '0.0', '-y', '0.0', '-z', '0.2'],
        condition=IfCondition(spawn_robot),
    )

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='true'),
        DeclareLaunchArgument(
            'world',
            default_value=PathJoinSubstitution([FindPackageShare('amr_gazebo'), 'worlds', 'warehouse_world.sdf'])
        ),
        DeclareLaunchArgument('spawn_robot', default_value='true'),
        gazebo_launch,
        rsp,
        spawn,
    ])
