/*
 *  navigation_handler.hpp
 *  LICENSE : BSD - https://raw.github.com/yujinrobot/yujin_ocs/license/LICENSE
 */

#include "yocs_navigator/semantic_navigator.hpp"

namespace yocs {
bool SemanticNavigator::getGoalLocationTable(const std::string location,yocs_msgs::Table& table)
{
  unsigned int i;
  for(i = 0; i < tablelist_.tables.size(); i ++)
  {
    yocs_msgs::Table t = tablelist_.tables[i];
    if(!location.compare(t.name)) // if matches
    {
      table = t;
      return true;
    }
  }
  return false;
}

void SemanticNavigator::terminateNavigation(bool success, const std::string message) 
{
  yocs_msgs::NavigateToResult result;

  result.success = success;
  result.message = message;
  result.distance = distance_to_goal_;

  navigation_in_progress_ = false;
  as_navi_.setSucceeded(result);

  return;
}

void SemanticNavigator::feedbackNavigation(const int status, const double distance, const double remain_time, const std::string message)
{
  yocs_msgs::NavigateToFeedback feedback;
  feedback.status = status;
  feedback.distance = distance;
  feedback.remain_time = remain_time;
  feedback.message = message;
  loginfo(message);
  
  as_navi_.publishFeedback(feedback);
}

bool SemanticNavigator::cancelMoveBaseGoal()
{
  double timeout = 2.0;
  ac_move_base_.cancelAllGoals();
  if (ac_move_base_.waitForResult(ros::Duration(timeout)) == false)
  {
    logwarn("Failed to cancel move_base goal...");
    return false;
  }

  loginfo("move_base goal has cancelled");
  return true;
}

bool SemanticNavigator::clearCostmaps()
{
  ros::Time t0 = ros::Time::now();

  ros::NodeHandle nh;
  ros::ServiceClient client = nh.serviceClient<std_srvs::Empty>(SemanticNavigatorDefaultParam::CLEAR_COSTMAP);
  std_srvs::Empty srv;

  if (client.call(srv) == true)
  {
    ROS_INFO("Successfully cleared costmaps (%f seconds)", (ros::Time::now() - t0).toSec());
    return true;
  }
  else
  {
    ROS_ERROR("Failed to clear costmaps (%f seconds)", (ros::Time::now() - t0).toSec());
    return false;
  }
}



void SemanticNavigator::loginfo(const std::string& msg)
{
  ROS_INFO_STREAM_NAMED(ros::this_node::getName(), msg);
}

void SemanticNavigator::logwarn(const std::string& msg)
{
  ROS_WARN_STREAM_NAMED(ros::this_node::getName(), msg);
}

}
