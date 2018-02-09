/*
 * Copyright (c) 2018, The Regents of the University of California (Regents).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Please contact the author(s) of this library if you have any questions.
 * Authors: David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// Templated class to hold timestamped sequences of states and rapidly
// interpolate between states linearly.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef FASTRACK_PLANNING_TRAJECTORY_H
#define FASTRACK_PLANNING_TRAJECTORY_H

#include <fastrack/space/state.h>
#include <fastrack/utils/types.h>

#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <geometry_msgs/Vector3.h>
#include <std_msgs/ColorRGBA.h>

namespace fastrack {
namespace planning {

template<typename S>
class Trajectory {
public:
  ~Trajectory() {}
  explicit Trajectory(const std::vector<S> states,
                      const std::vector<double> times);

  // Interpolate at a particular time.
  S Interpolate(double t) const;

  // Visualize this trajectory.
  void Visualize(const ros::Publisher& pub, const std::string& frame) const;

private:
  // Custom colormap for the given time.
  std_msgs::ColorRGBA Colormap(double t) const;

  // Lists of states and times.
  const std::vector<S> states_;
  const std::vector<double> times_;
}; //\class Trajectory

// ------------------------------ IMPLEMENTATION ----------------------------- //

template<typename S>
Trajectory<S>::Trajectory(const std::vector<S> states,
                          const std::vector<double> times)
  : states_(states),
    times_(times) {
  // Warn if state/time lists are not the same length and truncate
  // the longer one to match the smaller.
  if (states_.size() != times_.size()) {
    ROS_ERROR("Trajectory: states/times are not the same length.");

    // Resize the shorter one.
    if (states_.size() > times_.size())
      states_.resize(times_.size());
    else
      times_.resize(states_.size());
  }

  // Make sure times are sorted. Overwrite any inversions with the larger
  // time as we move left to right in the list.
  for (size_t ii = 1; ii < times_.size(); ii++) {
    if (times_[ii - 1] > times_[ii]) {
      ROS_ERROR("Trajectory: fixing an inversion in the list of times.");
      times_[ii] = times_[ii + 1];
    }
  }
}

// Interpolate at a particular time.
template<typename S>
S Trajectory<S>::Interpolate(double t) const {
  // Get an iterator pointing to the first element in times_ that does
  // not compare less than t.
  const std::vector<S>::const_iterator iter =
    std::lower_bound(times_.begin(), times_.end(), t);

  // Catch case where iter points to the beginning of the list.
  // This will happen if t occurs before the first time in the list.
  if (iter == times_.begin()) {
    ROS_WARN_THROTTLE(1.0, "Trajectory: interpolating before first time.");
    return states_.front();
  }

  // Catch case where iter points to the end of the list.
  // This will happen if t occurs after the last time in the list.
  if (iter == times_.end()) {
    ROS_WARN_THROTTLE(1.0, "Trajectory: interpolating after the last time.");
    return states_.back();
  }

  // Iterator definitely points to somewhere in the middle of the list.
  // Get indices sandwiching t.
  const size_t hi = (iter - times_.begin());
  const size_t lo = hi - 1;

  // Linearly interpolate states.
  const double frac = (t - times_[lo]) / (times_[hi] - times_[lo]);
  return (1.0 - frac) * states_[lo] + frac * states_[hi];
}

// Visualize this trajectory.
template<typename S>
void Trajectory<S>::Visualize(const ros::Publisher& pub,
                              const std::string& frame) const {
  if (pub.getNumSubscribers() == 0)
    return;

  // Set up spheres marker.
  visualization_msgs::Marker spheres;
  spheres.ns = "spheres";
  spheres.header.frame_id = frame;
  spheres.header.stamp = ros::Time::now();
  spheres.id = 0;
  spheres.type = visualization_msgs::Marker::SPHERE_LIST;
  spheres.action = visualization_msgs::Marker::ADD;
  spheres.scale.x = 0.1;
  spheres.scale.y = 0.1;
  spheres.scale.z = 0.1;

  // Set up line strip marker.
  visualization_msgs::Marker lines;
  lines.ns = "lines";
  lines.header.frame_id = frame_id;
  lines.header.stamp = ros::Time::now();
  lines.id = 0;
  lines.type = visualization_msgs::Marker::LINE_STRIP;
  lines.action = visualization_msgs::Marker::ADD;
  lines.scale.x = 0.05;

  // Populate markers.
  for (size_t ii = 0; ii < states_.size(); ii++) {
    geometry_msgs::Point p;
    p.x = states_[ii].X();
    p.y = states_[ii].Y();
    p.z = states_[ii].Z();

    const std_msgs::ColorRGBA c = Colormap(times_[ii]);

    spheres.points.push_back(p);
    spheres.colors.push_back(c);
    lines.points.push_back(p);
    lines.colors.push_back(c);
  }

  // Publish.
  pub.publish(spheres);
  if (states_.size() > 1)
    pub.publish(lines);
}

// Custom colormap for the given time.
template<typename S>
std_msgs::ColorRGBA Trajectory<S>::Colormap(double t) const {
  std_msgs::ColorRGBA c;
  c.r = std::clamp((t - times_.front()) /
                   (times_.back() - times_.front()), 0.0, 1.0);
  c.g = 0.0;
  c.b = 1.0 - c.r;
  c.a = 0.9;
}

} //\namespace planning
} //\namespace fastrack

#endif
