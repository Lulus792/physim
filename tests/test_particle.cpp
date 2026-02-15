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

#include <physim/particles.hpp>
#include <test_support.h>

static double new_v(double new_value) {
  return new_value;
}

class TestParticle {
private:
  si::particle_t<1, 1> p{};

public:
  void default_construct_is_zero() {
    matrix_t<si::ekin_t, 1, 1> compare{};
    ASSERT_EQ(p.ekin.mat, compare);
    ASSERT_EQ(p.epot.raw(), 0.0);
    ASSERT_EQ(p.m.raw(), 0.0);
    ASSERT_EQ(p.v.raw(), 0.0);
    ASSERT_EQ(p.p.raw(), 0.0);
  }

  void construct_from_type_values() {
    si::particle_t p_type(
        si::ekin_t{ 1 },
        si::epot_t{ 2 },
        si::mass_t{ 3 },
        si::velocity_t{ 4 },
        si::momentum_t{ 5 }
    );
    ASSERT_EQ(p_type.ekin.raw(), 1.0);
    ASSERT_EQ(p_type.epot.raw(), 2.0);
    ASSERT_EQ(p_type.m.raw(), 3.0);
    ASSERT_EQ(p_type.v.raw(), 4.0);
    ASSERT_EQ(p_type.p.raw(), 5.0);
  }

  void test_set_v() {
    p.set_v(new_v, 10.0);
    ASSERT_EQ(p.v.raw(), 10.0);
  }

  void update_momentum() {
    p.v = 10.0;
    p.m = 10.0;
    p.momentum();
    EXPECT_NEAR(p.p.raw(), 100.0, 1e-12);
  }

  void update_ekin_CM() {
    p.v = 10.0;
    p.m = 10.0;
    p.ekin_CM();
    EXPECT_NEAR(p.ekin.raw(), 500.0, 1e-12);
  }
};

TEST_METHOD(TestParticle, default_construct_is_zero);
TEST_METHOD(TestParticle, construct_from_type_values);
TEST_METHOD(TestParticle, test_set_v);
TEST_METHOD(TestParticle, update_momentum);
TEST_METHOD(TestParticle, update_ekin_CM);
