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
// Class for purely geometric (position + velocity) state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef FASTRACK_SPACE_POSITION_VELOCITY_H
#define FASTRACK_SPACE_POSITION_VELOCITY_H

#include <fastrack/space/state.h>

namespace fastrack {
namespace space {

class PositionVelocity : public State {
public:
  ~PositionVelocity() {}
  explicit PositionVelocity(double x, double y, double z,
                            double vx, double vy, double vz)
    : State(),
      position_(Vector3d(x, y, z)),
      velocity_(Vector3d(vx, vy, vz)) {}
  explicit PositionVelocity(const Vector3d& position,
                            const Vector3d& velocity)
    : State(),
      position_(position),
      velocity_(velocity) {}

  // Accessors.
  inline double X() const { return position_(0); }
  inline double Y() const { return position_(1); }
  inline double Z() const { return position_(2); }

  inline Vector3d Position() const { return position_; }
  inline Vector3d Velocity() const { return velocity_; }

  // Compound assignment operators.
  PositionVelocity& operator+=(const PositionVelocity& rhs) {
    position_ += rhs.position_;
    velocity_ += rhs.velocity_;
    return *this;
  }

  PositionVelocity& operator-=(const PositionVelocity& rhs) {
    position_ -= rhs.position_;
    velocity_ -= rhs.velocity_;
    return *this;
  }

  PositionVelocity& operator*=(double s) {
    position_ *= s;
    velocity_ *= s;
    return *this;
  }

  PositionVelocity& operator/=(double s) {
    position_ /= s;
    velocity_ /= s;
    return *this;
  }

  // Binary operators.
  friend PositionVelocity operator+(PositionVelocity lhs,
                                    const PositionVelocity& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend PositionVelocity operator-(PositionVelocity lhs,
                                    const PositionVelocity& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend PositionVelocity operator*(PositionVelocity lhs,
                                    const PositionVelocity& rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend PositionVelocity operator/(PositionVelocity lhs,
                                    const PositionVelocity& rhs) {
    lhs /= rhs;
    return lhs;
  }

private:
  Vector3d position_;
  Vector3d velocity_;
}; //\class PositionVelocity

} //\namespace space
} //\namespace fastrack

#endif
