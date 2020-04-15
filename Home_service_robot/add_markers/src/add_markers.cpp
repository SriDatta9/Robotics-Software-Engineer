#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <nav_msgs/Odometry.h>

double robot_x, robot_y ;
double pickup_x = 7.155, pickup_y = 10.211;
double drop_x = -4, drop_y = 5;
double threshold = 0.01;

void odomCallback(const nav_msgs::Odometry::ConstPtr &msg)
{
// updated robot_position
robot_x = msg->pose.pose.position.x;
robot_y = msg->pose.pose.position.y;
}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "add_markers");
  ros::NodeHandle n;
  ros::Rate r(1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);


  // Set our initial shape type to be a cube

  uint32_t shape = visualization_msgs::Marker::CUBE;

  int state =0;
    visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one

    marker.ns = "basic_shapes";
    marker.id = 0;


    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER

    marker.type = shape;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)

    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side

    marker.scale.x = 1.0;
    marker.scale.y = 1.0;
    marker.scale.z = 1.0;


    // Set the color -- be sure to set alpha to something non-zero!

    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

    marker.lifetime = ros::Duration();

   // Subscribing to /odom_pose
   ros::Subscriber sub1 = n.subscribe("/odom", 1000, odomCallback);

 while (ros::ok())
  {
    if (state == 0)
    {
      double pickup_distance = abs(robot_x - pickup_x) + abs(robot_y - pickup_y);

      if (pickup_distance > threshold)
      {
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = pickup_x;
        marker.pose.position.y = pickup_y;
      }
      else
      {
        state = 1;
        marker.action = visualization_msgs::Marker::DELETE;
      }
    }

    else if (state == 1)
    {
      double drop_distance = abs(robot_x - drop_x) + abs(robot_y - drop_y);

      if (drop_distance > threshold)
      {
        marker.action = visualization_msgs::Marker::DELETE;
      }
      else
      {
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = drop_x;
        marker.pose.position.y = drop_y;
        
      }
    }
  marker_pub.publish(marker);
    // Sleep for 1 seconds
  sleep(1);

    // Handle ROS communication events
  ros::spinOnce();

}
}
