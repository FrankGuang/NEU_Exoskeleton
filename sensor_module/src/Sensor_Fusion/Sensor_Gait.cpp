/* * @Author: Beal.MS
   * @Date: 2021-04-14 12:16:11
 * @Last Modified by: Beal.MS
 * @Last Modified time: 2021-04-14 12:48:33
   * @Description: Sensor module colaboration
*/
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include <cinttypes>
#include <cstdio>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

#define Piezoelectric_topic                            "Plantar_Pressure"
#define Human_Left_Thigh_topic                         "Human_Left_Thigh"
#define Human_Left_Calf_topic                          "Human_Left_Calf"
#define Human_Right_Thigh_topic                        "Human_Right_Thigh"
#define Human_Right_Calf_topic                         "Human_Right_Calf"
#define Sensor_topic                                   "Sensor_Gait"

float Left_Thigh_Human[3]={0,0,0};
float Left_Calf_Human[3]={0,0,0};
float Right_Thigh_Human[3]={0,0,0};
float Right_Calf_Human[3]={0,0,0};

float Left_Plantar_Pressor[3]={0,0,0};
float Right_Plantar_Pressor[3]={0,0,0};

std::string string_thread_id()
{
    auto hashed = std::hash<std::thread::id>()(std::this_thread::get_id());
    return std::to_string(hashed);
}

double convertFromString(std::string str)
{
    std::istringstream iss(str);
    double x;
    if(iss >> x)
        return x;
    return 0.0;
}

/*
    * @Name: class Force_Node
    * @Description: create a class for Piezoelectric Sensor
*/
class Plantar_Pressure : public rclcpp::Node
{
public:
    Plantar_Pressure()
    : Node("Plantar_Pressure")
    {
        Force_Callback = this->create_callback_group(
            rclcpp::CallbackGroupType::MutuallyExclusive
        );

        auto sub1_opt = rclcpp::SubscriptionOptions();
        sub1_opt.callback_group = Force_Callback;

        Force_Subscription = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            Piezoelectric_topic,
            rclcpp::QoS(10),
            std::bind(
                &Plantar_Pressure::subscriber1_cb,
                this,
                std::placeholders::_1
            ),
            sub1_opt
        );
    }
private:

    void subscriber1_cb(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        Left_Plantar_Pressor[0] = msg->data[0];
        Left_Plantar_Pressor[1] = msg->data[1];
        Left_Plantar_Pressor[2] = msg->data[2];

        Right_Plantar_Pressor[0] = msg->data[3];
        Right_Plantar_Pressor[1] = msg->data[4];
        Right_Plantar_Pressor[2] = msg->data[5];
    }

    rclcpp::CallbackGroup::SharedPtr                                    Force_Callback;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr   Force_Subscription;
};

/*
    * @Name: class IMU_Node
    * @Description: This class for the two MPU6050 sensors which will be used.
*/
class IMU_Node : public rclcpp::Node
{
public:
    IMU_Node()
    : Node("Human_IMU_Node")
    {
        callback_group_subscriber1_ = this->create_callback_group(
            rclcpp::CallbackGroupType::MutuallyExclusive
        );
        callback_group_subscriber2_ = this->create_callback_group(
            rclcpp::CallbackGroupType::MutuallyExclusive
        );
        callback_group_subscriber3_ = this->create_callback_group(
            rclcpp::CallbackGroupType::MutuallyExclusive
        );
        callback_group_subscriber4_ = this->create_callback_group(
            rclcpp::CallbackGroupType::MutuallyExclusive
        );

        auto sub1_opt = rclcpp::SubscriptionOptions();
        sub1_opt.callback_group = callback_group_subscriber1_;
        auto sub2_opt = rclcpp::SubscriptionOptions();
        sub2_opt.callback_group = callback_group_subscriber2_;
        auto sub3_opt = rclcpp::SubscriptionOptions();
        sub3_opt.callback_group = callback_group_subscriber3_;
        auto sub4_opt = rclcpp::SubscriptionOptions();
        sub4_opt.callback_group = callback_group_subscriber4_;

        subscription1_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            Human_Left_Thigh_topic,
            rclcpp::QoS(2),
            std::bind(
                &IMU_Node::subscriber1_cb,
                this,
                std::placeholders::_1
            ),
            sub1_opt
        );

        subscription2_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            Human_Left_Calf_topic,
            rclcpp::QoS(2),
            std::bind(
                &IMU_Node::subscriber2_cb,
                this,
                std::placeholders::_1
            ),
            sub2_opt
        );

        subscription3_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            Human_Right_Thigh_topic,
            rclcpp::QoS(2),
            std::bind(
                &IMU_Node::subscriber3_cb,
                this,
                std::placeholders::_1
            ),
            sub3_opt
        );

        subscription4_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            Human_Right_Calf_topic,
            rclcpp::QoS(2),
            std::bind(
                &IMU_Node::subscriber4_cb,
                this,
                std::placeholders::_1
            ),
            sub4_opt
        );

        start = true;
    }

private:
    std::string timing_string()
    {
        if(start)
        {
            rclcpp::Time time = this->now();
            start_time = convertFromString(std::to_string(time.seconds()));
            start = false;
            return std::to_string(time.seconds());
        }
        else{
            rclcpp::Time time = this->now();
            return std::to_string(time.seconds());
        }
    }

    void subscriber1_cb(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        Left_Thigh_Human[0] = msg->data[0];
        Left_Thigh_Human[1] = msg->data[1];
        Left_Thigh_Human[2] = msg->data[2];
    }

    void subscriber2_cb(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {

        Left_Calf_Human[0] = msg->data[0];
        Left_Calf_Human[1] = msg->data[1];
        Left_Calf_Human[2] = msg->data[2];
    }

    void subscriber3_cb(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {

        Right_Thigh_Human[0] = msg->data[0];
        Right_Thigh_Human[1] = msg->data[1];
        Right_Thigh_Human[2] = msg->data[2];
    }

    void subscriber4_cb(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {

        Right_Calf_Human[0] = msg->data[0];
        Right_Calf_Human[1] = msg->data[1];
        Right_Calf_Human[2] = msg->data[2];
    }

    rclcpp::CallbackGroup::SharedPtr                                    callback_group_subscriber1_;
    rclcpp::CallbackGroup::SharedPtr                                    callback_group_subscriber2_;
    rclcpp::CallbackGroup::SharedPtr                                    callback_group_subscriber3_;
    rclcpp::CallbackGroup::SharedPtr                                    callback_group_subscriber4_;

    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr   subscription1_;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr   subscription2_;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr   subscription3_;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr   subscription4_;

    bool                                                                start;
    double                                                              start_time;
};

class PublisherNode : public rclcpp::Node
{
public:
    PublisherNode()
    : Node("Human_Sensor_Publisher"), count_(0)
    {
        start = true;
        publisher_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(Sensor_topic, 10);
        auto timer_callback =
        [this]()->void {
            auto message_received_at = timing_string();
            double time = convertFromString(message_received_at) - start_time;

            auto message = std_msgs::msg::Float64MultiArray();
            message.data = {
                            /* Angle */
                            Left_Thigh_Human[0],
                            Left_Calf_Human[0],
                            Right_Thigh_Human[0],
                            Right_Calf_Human[0],

                            /* Velocity */
                            Left_Thigh_Human[1],
                            Left_Calf_Human[1],
                            Right_Thigh_Human[1],
                            Right_Calf_Human[1],

                            /* Acceleration */
                            Left_Thigh_Human[2],
                            Left_Calf_Human[2],
                            Right_Thigh_Human[2],
                            Right_Calf_Human[2],

                            /* Plantar Pressure */
                            Left_Plantar_Pressor[0],
                            Left_Plantar_Pressor[1],
                            Left_Plantar_Pressor[2],

                            Right_Plantar_Pressor[0],
                            Right_Plantar_Pressor[1],
                            Right_Plantar_Pressor[2],

                            /* Time */
                            time};
            this->publisher_->publish(message);
        };
        timer_ = this->create_wall_timer(2ms, timer_callback);
    }

private:
    std::string timing_string()
    {
        if(start)
        {
            rclcpp::Time time = this->now();
            start_time = convertFromString(std::to_string(time.seconds()));
            start = false;
            return std::to_string(time.seconds());
        }
        else{
            rclcpp::Time time = this->now();
            return std::to_string(time.seconds());
        }
    }
    rclcpp::TimerBase::SharedPtr                                        timer_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr      publisher_;
    size_t                                                              count_;

    bool                                                                start;
    double                                                              start_time;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::executors::MultiThreadedExecutor executor;
    auto piezoelectric_sensor = std::make_shared<Plantar_Pressure>();
    auto mpu6050_sensors = std::make_shared<IMU_Node>();
    auto pubnode = std::make_shared<PublisherNode>();

    executor.add_node(piezoelectric_sensor);
    executor.add_node(mpu6050_sensors);
    executor.add_node(pubnode);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}