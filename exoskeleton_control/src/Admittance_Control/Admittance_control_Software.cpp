/* * @Author: Beal.MS
   * @Date: 2021-03-20 12:01:47
 * @Last Modified by: Beal.MS
 * @Last Modified time: 2021-03-20 23:32:53
   * @Description: Admittance Control
   * @input: Excepted_Angle, Excepted_Velosity, Force
   * @output: Position_Angle
*/
#include <iostream>
#include "exoskeleton_control/Admittance_control_Software.hpp"
#include <unistd.h>
#include <math.h>
#include <memory>
#include <chrono>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

Eigen::Matrix<float,1,4> Feedback_Angle_;
Eigen::Matrix<float,1,4> Expected_Angle_;
Eigen::Matrix<float,1,4> Expected_Left_Ang;
Eigen::Matrix<float,1,4> Expected_Right_Ang;
Eigen::Matrix<float,1,4> Expected_Velocity_;
Eigen::Matrix<float,1,4> Expected_Acceleration_;

bool flag_;

#define Joint_Subscription_Topic                "joint_states"
#define Sensor_Subscription_Topic               "Sensor_Human"
#define Interaction_Force_Subscription_Topic    "Interaction_Force"

#define Joint_Thigh_Offset          -0.7046042369967704
#define Joint_Calf_Offset           -0.7046042369967704+0.4744191163880517

class Admittance_Control_Subscription :
    public rclcpp::Node,
    public Admittance_control
{
public:
    Admittance_Control_Subscription()
    : Node("minimal_subscriber"),Admittance_control()
    {
        /*Subscription Node Initialized*/
        Joint_Subscription = this->create_subscription<sensor_msgs::msg::JointState>(
            Joint_Subscription_Topic,
            10,
            std::bind(&Admittance_Control_Subscription::Joint_Callback, this, _1)
        );

        Sensor_Subscription = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            Sensor_Subscription_Topic,
            10,
            std::bind(&Admittance_Control_Subscription::Sensor_Callback, this, _1)
        );

        Interaction_Force_Subscription = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            Interaction_Force_Subscription_Topic,
            10,
            std::bind(&Admittance_Control_Subscription::Interaction_Force_Callback, this, _1)
        );
        Joint_Publisher = this->create_publisher<std_msgs::msg::Float64MultiArray>("topic", 1);
    }

private:

    void Joint_Callback(const sensor_msgs::msg::JointState::SharedPtr msg)
    {
        auto position = msg->position;
        //Feedback Joint Message
        RCLCPP_INFO(this->get_logger(),
        "Left: '%f','%f','%f','%f' Right: '%f','%f','%f','%f'",
        -position[0],-position[1],-position[2],-position[3],
        -position[4],-position[5],-position[6],-position[7]);

        Feedback_Angle_ << -position[0], -position[1], -position[2], -position[3];
        // Expected_Angle_(0,1) = ((Angle_Thigh)/180)*PI-Joint_Thigh_Offset;
        // Expected_Angle_(0,2) = ((Angle_Calf - Angle_Thigh )/180)*PI-Joint_Calf_Offset;
        Expected_Left_Ang(0,1) = ((Left_Thigh_Angle)/180)*PI+Joint_Thigh_Offset;
        Expected_Left_Ang(0,2) = ((Left_Calf_Angle - Left_Thigh_Angle )/180)*PI+Joint_Calf_Offset-1.1;

        Expected_Right_Ang(0,1) = ((Right_Thigh_Angle)/180)*PI+Joint_Thigh_Offset+Joint_Thigh_Offset+0.2;
        Expected_Right_Ang(0,2) = ((Right_Calf_Angle - Right_Thigh_Angle )/180)*PI+Joint_Calf_Offset-0.2;
        Force_ << 0.0, 0.0, 0.0, 0.0;

        // Left_Angle = main(
        //     Feedback_Angle_,
        //     Expected_Left_Ang,
        //     Expected_Velocity_,
        //     Expected_Acceleration_,
        //     Force_
        //     );
        // Right_Angle = main(
        //     Feedback_Angle_,
        //     Expected_Right_Ang,
        //     Expected_Velocity_,
        //     Expected_Acceleration_,
        //     Force_
        //     );

        auto message = std_msgs::msg::Float64MultiArray();
        message.data = {  -Expected_Left_Ang(0,0),  Expected_Left_Ang(0,1),  Expected_Left_Ang(0,2),  -Expected_Left_Ang(0,3),
                        -Expected_Right_Ang(0,0), Expected_Right_Ang(0,1), Expected_Right_Ang(0,2), -Expected_Right_Ang(0,3) };

        Joint_Publisher->publish(message);

      //????????????
      // sleep(1);

        std::cout<<"Left: " << -Left_Angle(0,0)  << -Left_Angle(1,0)  << -Left_Angle(2,0)  << -Left_Angle(3,0)  <<" ,Published!!!"<<std::endl;
        std::cout<<"Right: "<< -Right_Angle(0,0) << -Right_Angle(1,0) << -Right_Angle(2,0) << -Right_Angle(3,0) <<" ,Published!!!"<<std::endl;
    }

    void Sensor_Callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        auto Sensor_Data = msg->data;
        // Angle_Thigh = Sensor_Data[3];
        // Angle_Calf  = Sensor_Data[6];

        Left_Thigh_Angle = Sensor_Data[0];
        Left_Calf_Angle  = Sensor_Data[1];
        Right_Thigh_Angle = Sensor_Data[2];
        Right_Calf_Angle = Sensor_Data[3];
    }

    void Interaction_Force_Callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        auto Force_Data = msg->data;
        Force_(0,0)     = 0.0;
        Force_(1,0)     = 100*(Force_Data[0]-Force_Data[1])/180*PI;
        Force_(2,0)     = 100*(Force_Data[2]-Force_Data[3])/180*PI;
        Force_(3,0)     = 0.0;
    }
private:
    /*Subscription Node*/
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr       Joint_Subscription;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr   Sensor_Subscription;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr   Interaction_Force_Subscription;

    /*Publisher Node*/
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr      Joint_Publisher;

    /*Data*/
    Eigen::Matrix<float,4,1> Left_Angle, Right_Angle;
    Eigen::Matrix<float,4,1> Force_;

    float Angle_Thigh;
    float Angle_Calf;

    float Left_Thigh_Angle;
    float Left_Calf_Angle;
    float Right_Thigh_Angle;
    float Right_Calf_Angle;
};

int main(int argc, char * argv[])
{

    Expected_Angle_ << 0.0,0.0,0.0,0.0;

    Expected_Left_Ang << 0.0,0.0,0.0,0.0;
    Expected_Right_Ang << 0.0,0.0,0.0,0.0;

    Expected_Velocity_ << 0.0,0.0,0.0,0.0;
    Expected_Acceleration_ << 0.0,0.0,0.0,0.0;

    /*For Test*/

  // Force_ << 0.0, -10000.0, 0.0, 0.0, 0.0, 0.0;
  // Admittance_control admittance_control();
  // float a,b,c,d;
  // for (int i = 0; i < 5; i++)
  // {
  //   std::cin>>a>>b>>c>>d;
  //   admittance_control.main(PI*a,PI*b,PI*c,PI*d, Force_);
  // }

    /*ROS Node*/

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Admittance_Control_Subscription>());
    rclcpp::shutdown();
    return 0;
}