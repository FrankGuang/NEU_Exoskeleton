/* * @Author: Beal.MS
   * @Date: 2021-03-20 12:01:47
 * @Last Modified by: Beal.MS
 * @Last Modified time: 2021-04-26 22:32:51
   * @Description: Admittance Control
   * @input: Expected_Angle, Expected_Velocity_DecareSpace, Force
   * @output: Position_Angle
*/
#ifndef ADMITTANCE_CONTROL_HARDWARE_H
#define ADMITTANCE_CONTROL_HARDWARE_H

#include "exoskeleton_kinematics/exoskeleton_kinematics.hpp"
#include "exoskeleton_dynamics/exoskeleton_dynamics.hpp"
#include <cmath>

class Admittance_control: public Exoskeleton_kinetic, public Exoskeleton_dynamic
{
private:

    // Force Data
    Eigen::Matrix<float,4,1> Force_ext;
    Eigen::Matrix<float,6,1> Force_Base;
    Eigen::Matrix<float,4,1> T_ext;

    // Expected Data
    Eigen::Matrix<float,1,4> Expected_Angle;
    Eigen::Matrix<float,1,4> Expected_Velocity;
    Eigen::Matrix<float,1,4> Expected_Acceleration;

    Eigen::Matrix<float,4,4> Expected_T_end;
    Eigen::Matrix<float,1,6> Expected_KPS6;

    Eigen::Matrix<float,6,6> Expected_RR;

    // Feedback Data
    Eigen::Matrix<float,1,4> Feedback_Angle;
    Eigen::Matrix<float,1,4> Feedback_Velocity;
    Eigen::Matrix<float,1,4> Feedback_Acceleration;

    Eigen::Matrix<float,4,4> Feedback_T_end;
    Eigen::Matrix<float,1,6> Feedback_KPS6;

    Eigen::Matrix<float,6,6> Feedback_RR;

    // Jaccobi
    Eigen::Matrix<float,6,4> Jn_Base;
    Eigen::Matrix<float,6,4> Jn_Tool;

    //阻抗/导纳控制系数
    Eigen::Matrix<float,4,1> m_M_JointSpace;
    Eigen::Matrix<float,4,1> m_D_JointSpace;
    Eigen::Matrix<float,4,1> m_K_JointSpace;

    // flag标志。
    // true: Dicare Space;
    // false: Joint Space;
    bool flag;
    // 导纳系数矩阵
    Eigen::Matrix<float,4,4> M,D,K;
public:

    Admittance_control();

    ~Admittance_control(){};

    /*
     * @Name: main
     * @Description: Thr main function of Admittance Control Algorithm
     * @Input: --Eigen::Matrix<float,1,4> Feedback_Angle_, Expected_Angle_, Feedback_Velocity_, Expected_Velocity_, Feedback_Acceleration_,Expected_Acceleration_
     *         --Eigen::Matrix<float,6,1> Force_
     * @Output: Eigen::Matrix<float,4,1> Angle
    */
    Eigen::Matrix<float,4,1> main(
        const Eigen::Matrix<float,1,4> Feedback_Angle_,
        const Eigen::Matrix<float,1,4> Feedback_Velocity_,
        const Eigen::Matrix<float,1,4> Feedback_Acceleration_,
        const Eigen::Matrix<float,1,4> Expected_Angle_,
        const Eigen::Matrix<float,1,4> Expected_Velocity_,
        const Eigen::Matrix<float,1,4> Expected_Acceleration_,
        const Eigen::Matrix<float,4,1> Force_);

    //-----------------Control-Algorithm-Method-----------------//
    //-------------------------Function-------------------------//
    /*
     * @Name: Admittance Control Algorithm JointSpace
     * @Description: Using Admittance Control Algorithms to Calculate the Velocity and return it
     *               which will be used Integrated and added to Feedback Angle to make each joint
     *               get the expected position.
     * @Input: --Eigen::Matrix<float,4,1> T_ext_,Feedback_Angle_,Expected_Angle_,Feedback_Angle_,
     *                                    Expected_Acceleration_,Expected_Velocity_JointSpace
    */
    Eigen::Matrix<float,4,1> Admittance_Control_Algorithm_JointSpace ();

};

// Admittance_control Algorithms Declareation
Admittance_control::Admittance_control()
{
    //系数设定

    m_M_JointSpace  <<  1.00,
                        1.00,
                        1.00,
                        1.00;

    m_D_JointSpace  <<  1.00,
                        0.0001,
                        0.0001,
                        1.00;

    m_K_JointSpace  <<  1.00,
                        0.001,
                        0.001,
                        1.00;
}


/* @Name: main
 * @Description: The main function in for controlling by Admittance Control Algorithm
 * @Input: Hip_Angle,Thigh_Angle,Calf_Angle
 * @Output: Angle
*/
Eigen::Matrix<float,4,1> Admittance_control::main(
    const Eigen::Matrix<float,1,4> Feedback_Angle_,
    const Eigen::Matrix<float,1,4> Feedback_Velocity_,
    const Eigen::Matrix<float,1,4> Feedback_Acceleration_,
    const Eigen::Matrix<float,1,4> Expected_Angle_,
    const Eigen::Matrix<float,1,4> Expected_Velocity_,
    const Eigen::Matrix<float,1,4> Expected_Acceleration_,
    const Eigen::Matrix<float,4,1> Force_
    )
{
    //--------------------------关节空间(Joint Space)--------------------------//
    // Get the Feedback Angle of each Joint
    Feedback_Angle          = Feedback_Angle_;
    Feedback_Velocity       = Feedback_Velocity_;
    Feedback_Acceleration   = Feedback_Acceleration_;
    Expected_Angle          = Expected_Angle_;
    Expected_Velocity       = Expected_Velocity_;
    Expected_Acceleration   = Expected_Acceleration_;
    Force_ext               = Force_;

    Expected_T_end = Exoskeleton_kinetic::forward_kinematics(Expected_Angle(0,0),Expected_Angle(0,1),Expected_Angle(0,2),Expected_Angle(0,3));
    Feedback_T_end = Exoskeleton_kinetic::forward_kinematics(Feedback_Angle(0,0),Feedback_Angle(0,1),Feedback_Angle(0,2),Feedback_Angle(0,3));

    // Get the Transform Matrix RR.
    //TODO:不确定是否需要对 Force_ext 变换至基坐标系下 进行描述。
    Expected_RR  << Expected_T_end(0,0), Expected_T_end(0,1), Expected_T_end(0,2), 0, 0, 0,
                    Expected_T_end(1,0), Expected_T_end(1,1), Expected_T_end(1,2), 0, 0, 0,
                    Expected_T_end(2,0), Expected_T_end(2,1), Expected_T_end(2,2), 0, 0, 0,
                    0, 0, 0, Expected_T_end(0,0), Expected_T_end(0,1), Expected_T_end(0,2),
                    0, 0, 0, Expected_T_end(1,0), Expected_T_end(1,1), Expected_T_end(1,2),
                    0, 0, 0, Expected_T_end(2,0), Expected_T_end(2,1), Expected_T_end(2,2);
    Feedback_RR <<  Feedback_T_end(0,0), Feedback_T_end(0,1), Feedback_T_end(0,2), 0, 0, 0,
                    Feedback_T_end(1,0), Feedback_T_end(1,1), Feedback_T_end(1,2), 0, 0, 0,
                    Feedback_T_end(2,0), Feedback_T_end(2,1), Feedback_T_end(2,2), 0, 0, 0,
                    0, 0, 0, Feedback_T_end(0,0), Feedback_T_end(0,1), Feedback_T_end(0,2),
                    0, 0, 0, Feedback_T_end(1,0), Feedback_T_end(1,1), Feedback_T_end(1,2),
                    0, 0, 0, Feedback_T_end(2,0), Feedback_T_end(2,1), Feedback_T_end(2,2);

    //TODO:
    //工具坐标系下描述
    Jn_Tool = Exoskeleton_kinetic::Jaccobi(
        Feedback_Angle(0,0),
        Feedback_Angle(0,1),
        Feedback_Angle(0,2),
        Feedback_Angle(0,3),
        Feedback_RR,
        false);

    //TODO:转换末端力至关节力
    // T_ext = Jn_Tool.transpose()*Force_ext;
    T_ext = Force_ext;
    // std::cout<<"T_ext: "<<T_ext[0]<<","
    //     <<T_ext[1]<<","
    //     <<T_ext[2]<<","
    //     <<T_ext[3]<<std::endl;
    //TODO:髋关节及膝关节外部力矩识别
    // T_ext(0,0) = 0.0;  // Hip Joint
    // T_ext(1,0) = 0.0000;  // Thigh Joint
    // T_ext(2,0) = 0.0000;  // Calf Joint
    // T_ext(3,0) = 0.0;  // Ankle Joint


    Eigen::Matrix<float,4,1> Exp_;
    Eigen::Matrix<float,4,1> Angle;
    Exp_ = Admittance_Control_Algorithm_JointSpace ();
    // std::cout<<"Angle: "<<Angle.transpose()<<std::endl;
    // std::cout<<"Feedback_Angle: "<<Feedback_Angle<<std::endl;
    Angle = Exp_ + Expected_Angle.transpose();
    // Exp_Acc = Feedback_Acceleration.transpose() - Exp_Acc;
    // float dt = 0.001;
    // Angle   = ((-Exp_Acc*dt+Feedback_Velocity.transpose())*dt+Feedback_Angle.transpose());
    // std::cout<<"Angle: "<<Angle<<std::endl;
    return Angle;

}

//-----------------Control-Algorithm-Method-----------------//
//-------------------------Function-------------------------//

/* @Name: Admittance_Control_Algorithm_JointSpace
 * @Description: Using the Admittance Control Algorithms in Joint Space.
 * @Input: T_ext, Feedback_Angle, Expected_Angle, Feedback_Velocity, Expected_Acceleration, Expected_Velocity_JointSpace
 * @Output: Angle
*/

Eigen::Matrix<float,4,1> Admittance_control::Admittance_Control_Algorithm_JointSpace ()
{

    M  <<   m_M_JointSpace(0,0),0.0,0.0,0.0,
            0.0,m_M_JointSpace(1,0),0.0,0.0,
            0.0,0.0,m_M_JointSpace(2,0),0.0,
            0.0,0.0,0.0,m_M_JointSpace(3,0);

    D  <<   m_D_JointSpace(0,0),0.0,0.0,0.0,
            0.0,m_D_JointSpace(1,0),0.0,0.0,
            0.0,0.0,m_D_JointSpace(2,0),0.0,
            0.0,0.0,0.0,m_D_JointSpace(3,0);

    K  <<   m_K_JointSpace(0,0),0.0,0.0,0.0,
            0.0,m_K_JointSpace(1,0),0.0,0.0,
            0.0,0.0,m_K_JointSpace(2,0),0.0,
            0.0,0.0,0.0,m_K_JointSpace(3,0);

    Eigen::Matrix<float,4,1> tmp;

    /*  @Description: 为了求出导纳控制算法的输出
        @Input: T_ext, Expected_Angle, Expected_Velocity, Expected_Acceleration, Feedback_Angle,Feedback_Velocity
    */
    // tmp = T_ext_ -
    // K*(Feedback_Angle_-Expected_Angle_).transpose() +
    // M*(Feedback_Velocity_/dt + Expected_Acceleration_).transpose() +
    // D*Expected_Velocity_JointSpace_.transpose();

    tmp = T_ext -
    D*(Feedback_Velocity - Expected_Velocity).transpose() -
    K*(Feedback_Acceleration - Expected_Acceleration).transpose();

    // tmp = T_ext -
    // M*(Feedback_Angle - Expected_Angle).transpose() -
    // D*(Feedback_Velocity - Expected_Velocity).transpose();

    Eigen::Matrix<float,4,1> dx;

    if (tmp.norm()>1e-10)
    {
        dx = M.inverse()*tmp;
    }
    else
    {
        dx << 0.0,0.0,0.0,0.0;
    }
    return dx;
}

#endif