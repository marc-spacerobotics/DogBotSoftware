/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2015, University of Colorado, Boulder
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Univ of CO, Boulder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Dave Coleman
   Desc:   Example control loop for reading, updating, and writing commands to a hardware interface
   using MONOTOIC system time
*/

#include "dogbot_control/dogbot_hw_control_loop.h"

// ROS parameter loading
#include <rosparam_shortcuts/rosparam_shortcuts.h>

namespace dogbot_control
{

DogbotHWControlLoop::DogbotHWControlLoop(
    ros::NodeHandle& nh,
    std::shared_ptr<DogBotHWInterface> hardware_interface
    )
  : nh_(nh),
    hardware_interface_(hardware_interface)
{
  // Create the controller manager
  controller_manager_.reset(new controller_manager::ControllerManager(hardware_interface_.get(), nh_));

  // Load rosparams
  ros::NodeHandle rpsnh(nh, name_);
  std::size_t error = 0;
  error += !rosparam_shortcuts::get(name_, rpsnh, "loop_hz", loop_hz_);
  error += !rosparam_shortcuts::get(name_, rpsnh, "cycle_time_error_threshold", cycle_time_error_threshold_);
  rosparam_shortcuts::shutdownIfError(name_, error);

  sleep(1);

  // Get current time for use with first update
  m_lastTime = DogBotN::TimePointT::clock::now();

  double updatePeriod = 1.0 / loop_hz_;

  // Start timer
  ros::Duration desired_update_freq = ros::Duration(updatePeriod);

  hardware_interface_->setWritePeriod(updatePeriod);

  non_realtime_loop_ = nh_.createTimer(desired_update_freq, &DogbotHWControlLoop::update, this);
}

void DogbotHWControlLoop::update(const ros::TimerEvent& e)
{
  m_currentTime = DogBotN::TimePointT::clock::now();

  // Get change in time
  elapsed_time_ = ros::Duration(std::chrono::duration<double>(m_currentTime - m_lastTime).count());
  m_lastTime = m_currentTime;

  // Error check cycle time
  const double cycle_time_error = (elapsed_time_ - desired_update_freq_).toSec();
  if (cycle_time_error > cycle_time_error_threshold_)
  {
    ROS_WARN_STREAM_NAMED(name_, "Cycle time exceeded error threshold by: "
                                     << cycle_time_error << ", cycle time: " << elapsed_time_
                                     << ", threshold: " << cycle_time_error_threshold_);
  }

  // Input
  hardware_interface_->read(m_currentTime,elapsed_time_);

  // Control
  controller_manager_->update(ros::Time::now(), elapsed_time_);

  // Output
  hardware_interface_->write(m_currentTime,elapsed_time_);
}

}  // namespace
