#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "eci/EciDemo113.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std::chrono_literals;

extern WORD Get_position[12][64];

BYTE Rec_pos_lower_position[12][8]      = {
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},/* read_position */
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},

    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},/* read_position */
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},

    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},/* read_position */
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x40,0x64,0x60,0x00,0x00,0x00,0x00,0x00}
};
BYTE TX_Active_Move[12][8] = {
    {0x00,0X01,0x00,0x00,0x00,0x00,0x00,0x00},/* ACTIVE */
    {0x00,0X01,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0X01,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0X01,0x00,0x00,0x00,0x00,0x00,0x00},

    {0x22,0x60,0x60,0x00,0x01,0x00,0x00,0x00},/* PPM */
    {0x22,0x60,0x60,0x00,0x01,0x00,0x00,0x00},
    {0x22,0x60,0x60,0x00,0x01,0x00,0x00,0x00},
    {0x22,0x60,0x60,0x00,0x01,0x00,0x00,0x00},

    {0x22,0x40,0x60,0x00,0x06,0x00,0x00,0x00},/* DIASBALE */
    {0x22,0x40,0x60,0x00,0x06,0x00,0x00,0x00},
    {0x22,0x40,0x60,0x00,0x06,0x00,0x00,0x00},
    {0x22,0x40,0x60,0x00,0x06,0x00,0x00,0x00}
};

BYTE TX_pos_upper_follow_[12][8] = {
    {0x22,0x40,0x60,0x00,0x0F,0x00,0x00,0x00},/* ENABLE */
    {0x22,0x40,0x60,0x00,0x0F,0x00,0x00,0x00},
    {0x22,0x40,0x60,0x00,0x0F,0x00,0x00,0x00},
    {0x22,0x40,0x60,0x00,0x0F,0x00,0x00,0x00},

    {0x22,0x7A,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x22,0x7A,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x22,0x7A,0x60,0x00,0x00,0x00,0x00,0x00},
    {0x22,0x7A,0x60,0x00,0x00,0x00,0x00,0x00},

    {0x22,0x40,0x60,0x00,0x3F,0x00,0x00,0x00},/* MOVE */
    {0x22,0x40,0x60,0x00,0x3F,0x00,0x00,0x00},
    {0x22,0x40,0x60,0x00,0x3F,0x00,0x00,0x00},
    {0x22,0x40,0x60,0x00,0x3F,0x00,0x00,0x00}
};
DWORD Move_lower_motorID[12]   = {0x605,0x607,0x606,0x608,0x605,0x607,0x606,0x608,0x605,0x607,0x606,0x608};

class Angle_Accept : public rclcpp::Node
{
public:
    Angle_Accept()
    : Node("Angle_Accept"), count_(0)
    {
        Joint_State_Publisher = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "Joint_State_Accept", 10
        );
        Joint_State_Subscriber = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            "Joint_State_Send", 10,
            std::bind(&Angle_Accept::topic_callback, this, std::placeholders::_1));
        timer_ = this->create_wall_timer(
            10ms, std::bind(&Angle_Accept::timer_callback, this)
        );
        hResult = ECI_OK;
        hResult = EciDemo113();
        Can_Tx_Data( hResult, TX_Active_Move, Move_lower_motorID);
    }

private:
    void timer_callback()
    {
        auto message = std_msgs::msg::Float64MultiArray();

        Can_Rx_Position( hResult, Rec_pos_lower_position, Move_lower_motorID);
        OS_Sleep(10);
        if(Get_position[0][0] == 0 && Get_position[1][0] == 0 && Get_position[2][0] == 0 && Get_position[3][0] == 0)
        {
            OS_Sleep(1);
            Can_Rx_Position( hResult, Rec_pos_lower_position, Move_lower_motorID);
        }
        for(i = 0;i < 4;++i)
        {
            for(j = 0;j < 4;++j)
            {
                v|=((unsigned int)Get_position[i][j]&0xFFu)<<(j*8);
            }
            angle[i] = 360 * v /1638400;
            v = 0;
        }
        for(i = 0;i < 4;++i)
            printf("%.2f\n",angle[i]);
        OS_Sleep(50);
        message.data = {angle[0],angle[1],angle[2],angle[3]};
        Joint_State_Publisher->publish(message);
    }
    void topic_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
    {
        // ECI_RESULT hResult = ECI_OK;
        // hResult = EciDemo113();
        printf("Recieved!\n");
        auto angle = msg->data;
        for (int i = 0; i < 4; i++)
        {
            motor_angle[i] = angle[i];
            motor_qc[i] = 1638400/360*motor_angle[i];
            TX_pos_upper_follow_[4][i+4] = (-motor_qc[i]>>(8*i)&0xff);
            TX_pos_upper_follow_[5][i+4] = ( motor_qc[i]>>(8*i)&0xff);
            TX_pos_upper_follow_[6][i+4] = (-motor_qc[i]>>(8*i)&0xff);
            TX_pos_upper_follow_[7][i+4] = ( motor_qc[i]>>(8*i)&0xff);
        }
        //Motive
        Can_Tx_Data( hResult, TX_pos_upper_follow_, Move_lower_motorID);
        sleep(1);
        Can_Tx_Data( hResult, TX_pos_upper_follow_, Move_lower_motorID);
        sleep(1);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr Joint_State_Publisher;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr Joint_State_Subscriber;
    size_t count_;

    ECI_RESULT hResult;
    int i;
    int j;
    int v;

    float motor_angle[4];
    float angle[4];
    int motor_qc[4];
    int motor_sixteen[4];
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Angle_Accept>());
    rclcpp::shutdown();
    return 0;
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::executors::MultiThreadedExecutor executor;
    auto piezoelectric_sensor = std::make_shared<SingleThreadedNode>();
    auto mpu6050_sensors = std::make_shared<DualThreadedNode>();
    auto pubnode = std::make_shared<PublisherNode>();

    executor.add_node(piezoelectric_sensor);
    executor.add_node(mpu6050_sensors);
    executor.add_node(pubnode);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}