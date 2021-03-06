#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "tf/transform_broadcaster.h"

void odomCallback(nav_msgs::Odometry msg)
{
	static tf::TransformBroadcaster br;
	tf::Transform transform;
	transform.setOrigin( tf::Vector3(msg.pose.pose.position.x, msg.pose.pose.position.y, 0.0) );
	tf::Quaternion q;
	q.setRPY(msg.pose.pose.orientation.z, 0, 0);
	transform.setRotation(q);
	br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "/robot_1"));
	
}

int main(int argc, char** argv){
	ros::init(argc, argv, "pursuer");
	ros::NodeHandle n;
	ros::Subscriber odom_sub = n.subscribe<nav_msgs::Odometry>("odom",10,odomCallback);
	ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel",10);
	tf::TransformListener listener;
	ros::Rate rate(10.0);
	while (n.ok()){
	ros::spinOnce();
	    tf::StampedTransform transform;
	    try{
		ros::Time now=ros::Time::now();
		ros::Time past=now - ros::Duration(1.0);
		listener.waitForTransform("/robot_1",past,"/robot_0",now,"/world",ros::Duration(1.0));
	        listener.lookupTransform("/robot_1",past,"/robot_0",now,"/world", transform);
	    }
	    catch (tf::TransformException &ex) {
	      ROS_ERROR("%s",ex.what());
	      ros::Duration(1.0).sleep();
	      continue;
	    }

	    geometry_msgs::Twist vel_msg;
	
	    vel_msg.angular.z = 0.3* atan2(transform.getOrigin().y(),
		                            transform.getOrigin().x());
	    vel_msg.linear.x = 4*sqrt(pow(transform.getOrigin().x(), 2) +
		                          pow(transform.getOrigin().y(), 2));
	    vel_pub.publish(vel_msg);

	    rate.sleep();
  	}
  return 0;
}

