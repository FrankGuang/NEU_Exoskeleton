import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch_ros.actions import Node


def generate_launch_description():
    # Get the launch directory
    bringup_dir = get_package_share_directory('sensor_module')
    launch_dir = os.path.join(bringup_dir, 'launch')

    Piezoelectric_cmd = Node(
        package="sensor_module",
        executable="Piezoelectric",
        name="Piezoelectric"
    )

    Human_Left_Thigh_cmd = Node(
        package="sensor_module",
        executable="Human_Left_Thigh",
        name="Human_Left_Thigh"
    )

    Human_Left_Calf_cmd = Node(
        package="sensor_module",
        executable="Human_Left_Calf",
        name="Human_Left_Calf"
    )

    # Exoskeleton_Left_Thigh_cmd = Node(
    #     package="sensor_module",
    #     executable="Exoskeleton_Thigh",
    #     name="Exoskeleton_Thigh"
    # )

    # Exoskeleton_Left_Calf_cmd = Node(
    #     package="sensor_module",
    #     executable="Exoskeleton_Calf",
    #     name="Exoskeleton_Calf"
    # )

    Sensor_Human_cmd = Node(
        package="sensor_module",
        executable="Sensor_Human",
        name="Sensor_Human"
    )
    # Create the launch description and populate
    ld = LaunchDescription()
    # Declare the launch options
    ld.add_action(Piezoelectric_cmd)
    ld.add_action(Human_Left_Thigh_cmd)
    ld.add_action(Human_Left_Calf_cmd)
    # ld.add_action(Exoskeleton_Left_Thigh_cmd)
    # ld.add_action(Exoskeleton_Left_Calf_cmd)
    ld.add_action(Sensor_Human_cmd)
    return ld