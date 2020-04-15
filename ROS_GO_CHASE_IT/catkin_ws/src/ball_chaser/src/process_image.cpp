#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <image_transport/image_transport.h>
#include <list>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{

// Requesting the service and passing the velocities
  ROS_INFO_STREAM("Robot chasing the ball");
  float linear_x, angular_z;
  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;

  if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot ");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::ImageConstPtr& msg)
{
  cv_bridge::CvImageConstPtr cv_ptr;
  try
    {
      cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::BGR8);
      cv::Mat bgr_image;
      cv::medianBlur(cv_ptr->image,bgr_image,3);

      // finding the center of the image
      int image_center = bgr_image.cols/2;
  
     // convert input image to HSV
      cv::Mat hsv_image;
      cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

     // threshold HSV image, keep only the white pixels
      cv::Mat lower_white_hue_range;
      cv::Mat upper_white_hue_range;
      cv::inRange(hsv_image, cv::Scalar(0,0,212), cv::Scalar(131,255,255),lower_white_hue_range);
      cv::inRange(hsv_image, cv::Scalar(0,0,212), cv::Scalar(131,255,255),upper_white_hue_range);

     // combine the above images
      cv::Mat white_hue_image;
      cv::addWeighted(lower_white_hue_range, 1.0, upper_white_hue_range, 1.0, 0, white_hue_image);
      
      cv::GaussianBlur(white_hue_image, white_hue_image, cv::Size(9,9), 2, 2);

     // use the hough transform to detect the circles in the white hue image
      std::vector<cv::Vec3f> circles;
      cv::HoughCircles(white_hue_image, circles, CV_HOUGH_GRADIENT, 1, white_hue_image.rows/8, 100, 30, 0,0);
      
      ROS_INFO_STREAM(circles.size() << "circles detected");   
      ROS_INFO_STREAM(image_center << "center of the image");
      
    // if the robot doesnot find any white circular ball, then it rotates 360 deg in search of the ball. Whenever it finds the ball, it starts tracking it.    
      if(circles.size()==0){
        drive_robot(0,0.5);
        
}        
      else if(circles.size()==1){
          int radius = cvRound(circles[0][2]);
          ROS_INFO_STREAM(radius << "radius");
           if(radius<150){
          int ball_center = cvRound(circles[0][0]);
          ROS_INFO_STREAM(ball_center << "center of the ball");
          
          if(ball_center<image_center-30){
             drive_robot(0.1,0.3);}
          else if(ball_center>image_center+30){
             drive_robot(0.1,-0.3);}
          else{
             drive_robot(0.3,0);
} }
          // if it closer to the object, it stops
          else{
             drive_robot(0,0);
}
}
 
      // showing the image
      //cv::namedWindow( "white ball detection", CV_WINDOW_AUTOSIZE );
      //cv::imshow( "white ball detection", white_hue_image ); 
      //cv::waitKey(0);
    }

    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
