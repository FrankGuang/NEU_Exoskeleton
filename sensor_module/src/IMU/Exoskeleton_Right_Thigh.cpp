/* * @Author: Beal.MS
   * @Date: 2021-05-23 22:24:53
   * @Last Modified by:   Beal.MS
   * @Last Modified time: 2021-05-23 22:24:53
   * @Description: 
*/
#include "sensor_module/MPU6050.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include <cinttypes>
#include <cstdio>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/int32.hpp"

#define     USB_DEVICE  "/dev/ttyUSB7"
#define     topic       "Exoskeleton_Right_Thigh"
#define     offset      8.4
using namespace std::chrono_literals;
/*
 * @Name: MPU6050
 * @Description: Using the MPU6050 Class to let Serial Port be initialized.
*/
MPU6050 mpu6050 = MPU6050 (USB_DEVICE,B115200,"Right_Thigh");


class Exoskeleton_Right_Thigh : public rclcpp::Node
{
public:
    Exoskeleton_Right_Thigh()
    : Node("Exoskeleton_Right_Thigh")
    {
        publisher_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(topic, 10);
        timer_ = this->create_wall_timer(
            0.000000868s,
            std::bind(
                &Exoskeleton_Right_Thigh::timer_callback,
                this)
        );
    }

private:
    void timer_callback()
    {
        auto message = std_msgs::msg::Float64MultiArray();
        float * Angle;
        Angle = mpu6050.Read_Data();
        message.data = {mpu6050.Ang[0],mpu6050.Vel[0],mpu6050.Acc[0]};
        publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr publisher_;
    size_t count_;
};

int  main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Exoskeleton_Right_Thigh>());
    rclcpp::shutdown();
}
