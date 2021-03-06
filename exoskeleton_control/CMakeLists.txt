cmake_minimum_required(VERSION 3.5)
project(exoskeleton_control)

# Default to C99
if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    # the following line skips the linter which checks for copyrights
    # uncomment the line when a copyright and license is not present in all source files
    #set(ament_cmake_copyright_FOUND TRUE)
    # the following line skips cpplint (only works in a git repo)
    # uncomment the line when this package is not in a git repo
    #set(ament_cmake_cpplint_FOUND TRUE)
    ament_lint_auto_find_test_dependencies()
endif()
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
add_library(EciDemo113
    src/Eci/EciDemo113.cpp
)
add_library(EciDemoCommon
    src/Eci/EciDemoCommon.c
)
# target_link_libraries(EciDemo113 ${catkin_LIBRARIES})

ament_target_dependencies(EciDemoCommon ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
ament_target_dependencies(EciDemo113 ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(EciDemo src/Eci/EciDemo113.cpp src/Eci/LinuxEciDemo.cpp)
target_link_libraries(EciDemo  EciDemoCommon ${catkin_LIBRARIES} "/usr/lib/libeci113DriverLinux.so")

add_executable(Eci_keyboard src/Eci/EciDemo113.cpp src/Eci/Eci_keyboard.cpp)
target_link_libraries(Eci_keyboard EciDemoCommon ${catkin_LIBRARIES} "/usr/lib/libeci113DriverLinux.so")

add_executable(Angle_Accept  src/Eci/Angle_Accept.cpp)
ament_target_dependencies(Angle_Accept  rclcpp std_msgs)
target_link_libraries(Angle_Accept EciDemoCommon EciDemo113 "/usr/lib/libeci113DriverLinux.so")
add_executable(Angle_Send  src/Eci/Angle_Send.cpp)
ament_target_dependencies(Angle_Send  rclcpp std_msgs)
target_link_libraries(Angle_Send EciDemoCommon EciDemo113 "/usr/lib/libeci113DriverLinux.so")
add_executable(Angle_Driver  src/Eci/EciDemo113.cpp src/Eci/Angle_Driver.cpp)
ament_target_dependencies(Angle_Driver  rclcpp std_msgs)
target_link_libraries(Angle_Driver EciDemoCommon EciDemo113 "/usr/lib/libeci113DriverLinux.so")


install(TARGETS
EciDemo
Eci_keyboard
Angle_Accept
Angle_Send
Angle_Driver
DESTINATION lib/${PROJECT_NAME}
)

add_executable(Exoskeleton_kinematics src/Kinematic_Dynamic/exoskeleton_kinematics.cpp)
ament_target_dependencies(Exoskeleton_kinematics ${catkin_LIBRARIES} rclcpp std_msgs)
add_executable(Exoskeleton_dynamics src/Kinematic_Dynamic/exoskeleton_dynamics.cpp)
ament_target_dependencies(Exoskeleton_dynamics ${catkin_LIBRARIES} rclcpp std_msgs)

add_executable(Admittance_control_Software src/Admittance_Control/Admittance_control_Software.cpp)
ament_target_dependencies(Admittance_control_Software ${catkin_LIBRARIES} rclcpp std_msgs sensor_msgs)
add_executable(Admittance_control_Hardware src/Admittance_Control/Admittance_control_Hardware.cpp)
ament_target_dependencies(Admittance_control_Hardware ${catkin_LIBRARIES} rclcpp std_msgs sensor_msgs)

add_executable(Test src/Eci/Test.cpp)
ament_target_dependencies(Test ${catkin_LIBRARIES} rclcpp std_msgs sensor_msgs)


install(TARGETS
Exoskeleton_kinematics
Exoskeleton_dynamics
Admittance_control_Software
Admittance_control_Hardware
Test
DESTINATION lib/${PROJECT_NAME}
)

ament_package()
