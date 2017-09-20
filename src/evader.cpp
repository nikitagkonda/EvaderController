#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "tf/transform_broadcaster.h"

ros::Publisher vel_pub;
void sensorCallback(sensor_msgs::LaserScan msg)
{
	int i,flag=0,j=0;
	for(i=85;i<=95;i++)
	{
  		if(msg.ranges[i]<=2.0)
		{
		flag=1;
		break;
		}		
    		
	}
	if(flag==1) {
	ROS_INFO("I hit the obstacle");
        //turn and check
        geometry_msgs::Twist msg1;
	msg1.linear.x=-0.5;
	msg1.angular.z=2.0;
        vel_pub.publish(msg1);
	return;
	
       }
	
	else{
 	geometry_msgs::Twist msg2;
	msg2.linear.x=2.0;
        vel_pub.publish(msg2);
	}
    
}

void odomCallback(nav_msgs::Odometry msg)
{
	static tf::TransformBroadcaster br;
	tf::Transform transform;
	transform.setOrigin( tf::Vector3(msg.pose.pose.position.x, msg.pose.pose.position.y, 0.0) );
	tf::Quaternion q;
	q.setRPY(0,0,msg.pose.pose.orientation.z);
	transform.setRotation(q);
	br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "/robot_0"));
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "evader");
	ros::NodeHandle n;
	vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 10);
	ros::Subscriber sub1 = n.subscribe<sensor_msgs::LaserScan>("base_scan", 10, sensorCallback);
	ros::Subscriber sub2 = n.subscribe<nav_msgs::Odometry>("odom",10,odomCallback);
	ros::spin();
	return 0;
}
