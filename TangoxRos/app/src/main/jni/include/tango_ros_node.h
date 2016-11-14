// Copyright 2016 Intermodalics All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef TANGO_ROS_NODE_H_
#define TANGO_ROS_NODE_H_
#include <jni.h>
#include <string>

#include <tango_client_api.h>
#include <tango_support_api.h>

#include <opencv2/core/core.hpp>

#include <geometry_msgs/TransformStamped.h>
#include <ros/ros.h>
#include <ros/node_handle.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/PointCloud2.h>
#include <tf/transform_broadcaster.h>

namespace tango_ros_node {
static int kNumberOfFieldsInPointCloud = 4;
static int kImageCompressingQuality = 50;

enum CameraType {
  NONE,
  FISHEYE,
  COLOR
};

struct PublisherConfiguration {
  bool publish_device_pose = false;
  bool publish_point_cloud = false;
  CameraType publish_camera = NONE;

  std::string point_cloud_topic = "tango/point_cloud";
  std::string camera_topic = "tango/image_raw/compressed";
};

class TangoRosNode {
 public:
  TangoRosNode(PublisherConfiguration publisher_config);
  ~TangoRosNode();
  void CheckTangoVersion(JNIEnv* env, jobject activity);
  void OnTangoServiceConnected(JNIEnv* env, jobject binder);
  void TangoDisconnect();
  void Publish();

  void OnPoseAvailable(const TangoPoseData* pose);
  void OnPointCloudAvailable(const TangoPointCloud* point_cloud);
  void OnFrameAvailable(const TangoImageBuffer* buffer);

 private:
  bool TangoSetupConfig();
  void TangoConnect();

  TangoConfig tango_config_;
  ros::NodeHandle node_handle_;
  PublisherConfiguration publisher_config_;
  bool pose_lock_ = false;
  bool point_cloud_lock_ = false;
  bool image_lock_ = false;
  bool new_pose_available_ = false;
  bool new_point_cloud_available_ = false;
  bool new_image_available_ = false;
  bool is_device_T_camera_depth_set_ = false;
  bool is_device_T_camera_set_ = false;
  double time_offset_ = 0.; // Offset between tango time and ros time in ms.

  tf::TransformBroadcaster tf_broadcaster_;
  geometry_msgs::TransformStamped start_of_service_T_device_;
  geometry_msgs::TransformStamped start_of_service_T_camera_depth_;
  geometry_msgs::TransformStamped device_T_camera_depth_;
  geometry_msgs::TransformStamped start_of_service_T_camera_;
  geometry_msgs::TransformStamped device_T_camera_;

  ros::Publisher point_cloud_publisher_;
  sensor_msgs::PointCloud2 point_cloud_;

  ros::Publisher image_publisher_;
  sensor_msgs::CompressedImage compressed_image_;
  cv::Mat image_;
};
}  // namespace tango_ros_node
#endif  // TANGO_ROS_NODE_H_
