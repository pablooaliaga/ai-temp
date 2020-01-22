// Include for ROS framework
#include <ros/node_handle.h>
#include <ros/publisher.h>
#include <ros/subscriber.h>

// Include for the messages used in this script
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/JointState.h>

/**
 * logic
 */
class SimpleController {
public: SimpleController(const ros::NodeHandle& _nodeHandle);
public:
  void odometryCallback(const nav_msgs::OdometryPtr& _odometry);
  void desiredVelocityCallback(const geometry_msgs::TwistPtr& _twist);
private:
  void publishControlledVelocity();
  geometry_msgs::TwistPtr m_lastDesiredVelocityMsg;
  nav_msgs::OdometryPtr m_lastOdometryMsg;

  ros::NodeHandle m_nodeHandle;
  ros::Subscriber m_odometrySub, m_desiredVelocitySub;
  ros::Publisher m_wheelCmdPub;

  float m_left = 0;
  float m_right = 0;
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "simple_controller");
  ros::NodeHandle n;

  SimpleController sc(n);

  printf("Starting :)\n");

  ros::spin();
  return 0;
}

SimpleController::SimpleController(const ros::NodeHandle& _nodeHandle) : m_nodeHandle(_nodeHandle) {
  m_odometrySub = m_nodeHandle.subscribe(
    "odometry",
    1,
    &SimpleController::odometryCallback,
    this);
  m_desiredVelocitySub = m_nodeHandle.subscribe(
    "desired_velocity",
    1,
    &SimpleController::desiredVelocityCallback,
    this);

    m_wheelCmdPub = m_nodeHandle.advertise<sensor_msgs::JointState>("wheel_cmd", 1);
}

void SimpleController::odometryCallback(const nav_msgs::OdometryPtr& _odometry) {
  // Keep a copy of the message and publish an update of the controlled velocity
  m_lastOdometryMsg = _odometry;
  publishControlledVelocity();
}

void SimpleController::desiredVelocityCallback(const geometry_msgs::TwistPtr& _twist) {
  m_lastDesiredVelocityMsg = _twist;
  publishControlledVelocity();
}

void SimpleController::publishControlledVelocity() {
  if(m_lastDesiredVelocityMsg and m_lastOdometryMsg) {
    sensor_msgs::JointState cmd;

    float lin_diff = m_lastDesiredVelocityMsg -> linear.x - m_lastOdometryMsg->twist.twist.linear.x;
    if(lin_diff > 0.1) {
      m_right += 0.01;
      m_left += 0.01;
    } else if(lin_diff < -0.1) {
      m_right -= 0.01;
      m_left -= 0.01;
    }

    float ang_diff = m_lastDesiredVelocityMsg -> angular.z - m_lastOdometryMsg->twist.twist.angular.z;
    if(ang_diff > 0.05) {
      m_right += 0.01;
      m_left -= 0.01;
    } else if(ang_diff < -0.05) {
      m_right -= 0.01;
      m_left += 0.01;
    }

    cmd.name = {"left", "right"};
    cmd.effort = {m_left, m_right};
    m_wheelCmdPub.publish(cmd);
  }
}
