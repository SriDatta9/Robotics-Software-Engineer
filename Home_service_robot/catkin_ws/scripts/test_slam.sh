#!/bin/sh

xterm  -e "roslaunch my_robot world.launch" &
sleep 5

xterm  -e "roslaunch my_robot mapping.launch" &
sleep 5

xterm  -e "roslaunch turtlebot_rviz_launchers view_navigation.launch" &
sleep 5

xterm  -e "rosrun teleop_twist_keyboard teleop_twist_keyboard.py"
