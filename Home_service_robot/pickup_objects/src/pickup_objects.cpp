#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

double pickup_x = 7.155 ;
double pickup_y = 10.211;
double drop_x = -4;
double drop_y = 5;

int main(int argc, char** argv){
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "simple_navigation_goals");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  // Wait 5 sec for move_base action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  // set up the frame parameters
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();

  // Define a position and orientation for the robot to reach
  goal.target_pose.pose.position.x = pickup_x;
  goal.target_pose.pose.position.y = pickup_y;
  goal.target_pose.pose.orientation.w = 1.0;

   // Send the goal position and orientation for the robot to reach
  ROS_INFO("Robot is on the way to pickup location");
  ac.sendGoal(goal);

  // Wait an infinite time for the results
  ac.waitForResult();

  // Check if the robot reached its goal
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    {ROS_INFO("Hooray, the Robot reached pickup location");

    ros::Duration(5).sleep();
    goal.target_pose.pose.position.x = drop_x;
    goal.target_pose.pose.position.y = drop_y;
    goal.target_pose.pose.orientation.w = 1.0;
    ROS_INFO("Robot is on the way to drop location");
    ac.sendGoal(goal);
    ac.waitForResult();

    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
     {ROS_INFO("Hooray, the Robot reached drop location");}
    else
    {ROS_INFO("The Robot failed to reach drop location");} }
 
  else
    {ROS_INFO("The Robot failed to reach pickup location");}

  return 0;
}
