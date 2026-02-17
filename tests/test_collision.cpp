/* 
 * -*- coding: utf-8 -*- 
 * Copyright 2026 physim devlopers
 *
 * This file is part of physim.
 *
 * physim is free software: you can redistribute it and/or modify
 * it under the term of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the license, or
 * (at your option) any later version.
 *
 * physim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with physim. If not, see <https:://www.gnu.org/license/#GPL>
 */

#include <physim/collision.hpp>
#include <test_support.h>

class TestCollision {
private:
  si::ekin_t _ekin;
  si::epot_t _epot;
  si::mass_t _m;
  matrix_t<si::velocity_t, 1, 1> _v;
  matrix_t<si::momentum_t, 1, 1> _p;
  si::particle_t<1, 1> p1{};
  si::particle_t<1, 1> p2{};

public:
  TestCollision() {
    _ekin = si::ekin_t{500.0};
    _epot = si::epot_t{};
    _m = si::mass_t{10.0};
    _v.flat(true, 10.0 );
    _p.flat(true, 100.0 );
    p1.set(
      _ekin,
      _epot,
      _m,
      _v,
      _p
    );
    p2 = p1;
  }

  void test_1D_collision() {
    si::collision::_1D_collision_particle(&p1, &p2);
    // p1
    EXPECT_NEAR(p1.v(1, 1).raw(), 10.0, 1e-9);
    EXPECT_NEAR(p1.p(1, 1).raw(), 100.0, 1e-9);
    EXPECT_NEAR(p1.ekin.raw(), 500.0, 1e-9);

    // p2
    EXPECT_NEAR(p2.v(1, 1).raw(), 10.0, 1e-9);
    EXPECT_NEAR(p2.p(1, 1).raw(), 100.0, 1e-9);
    EXPECT_NEAR(p2.ekin.raw(), 500.0, 1e-9);
  }
};

TEST_METHOD(TestCollision, test_1D_collision)
