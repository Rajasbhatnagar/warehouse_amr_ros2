from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution, PythonExpression
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time')
    mode = LaunchConfiguration('mode')
    run_perception = LaunchConfiguration('run_perception')
    run_control = LaunchConfiguration('run_control')

    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([FindPackageShare('amr_gazebo'), 'launch', 'gazebo.launch.py'])
        ]),
        launch_arguments={'use_sim_time': use_sim_time}.items(),
    )

    slam = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([FindPackageShare('amr_navigation'), 'launch', 'slam.launch.py'])
        ]),
        condition=IfCondition(PythonExpression(["'", mode, "' == 'slam'"])),
        launch_arguments={'use_sim_time': use_sim_time}.items(),
    )

    navigation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([FindPackageShare('amr_navigation'), 'launch', 'navigation.launch.py'])
        ]),
        condition=IfCondition(PythonExpression(["'", mode, "' != 'slam'"])),
        launch_arguments={'use_sim_time': use_sim_time}.items(),
    )

    perception = Node(
        package='amr_perception',
        executable='yolo_perception_node',
        output='screen',
        condition=IfCondition(run_perception),
        parameters=[{'use_sim_time': use_sim_time}],
    )

    pid = Node(
        package='amr_control',
        executable='pid_controller',
        output='screen',
        condition=IfCondition(run_control),
        parameters=[{'use_sim_time': use_sim_time}],
    )

    task_manager = Node(
        package='amr_control',
        executable='task_manager',
        output='screen',
        condition=IfCondition(run_control),
        parameters=[
            PathJoinSubstitution([FindPackageShare('amr_control'), 'config', 'task_manager.yaml']),
            {'use_sim_time': use_sim_time},
        ],
    )

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='true'),
        DeclareLaunchArgument('mode', default_value='mission'),
        DeclareLaunchArgument('run_perception', default_value='true'),
        DeclareLaunchArgument('run_control', default_value='true'),
        gazebo,
        slam,
        navigation,
        perception,
        pid,
        task_manager,
    ])
