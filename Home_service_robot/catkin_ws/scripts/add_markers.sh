#!/bin/sh

export TURTLEBOT_GAZEBO_WORLD_FILE=$(rospack find my_robot)/worlds/home_service2.world
export TTURTLEBOT_GAZEBO_MAP_FILE=$(rospack find my_robot)/maps/home_map.yaml

xterm  -e "roslaunch turtlebot_gazebo turtlebot_world.launch" &
sleep 5

xterm  -e "roslaunch turtlebot_gazebo amcl_demo.launch" &
sleep 5

xterm  -e "roslaunch turtlebot_rviz_launchers view_navigation.launch" &
sleep 5

xterm  -e "rosrun add_markers add_markers"
