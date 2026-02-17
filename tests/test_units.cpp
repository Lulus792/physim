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

#include <physim/unit.hpp>
#include <test_support.h>

using t = si::unit_t<si::Dim<3, 3, 3>>;
using _t = si::unit_t<si::Dim<-3, -3, -3>>;
using res = si::unit_t<si::Dim<0, 0, 0>>;
template<> struct si::Canon<si::Dim<3, 3, 3>> { using type = t; };
template<> struct si::Canon<si::Dim<-3, -3, -3>> { using type = _t; };
template<> struct si::Canon<si::Dim<0, 0, 0>> { using type = res; };

class TestUnit {
private:
  si::unit_t<si::Dim< 0, 0, 0>> u;

public:
  void default_construct_is_zero() {
    EXPECT_NEAR(u.raw(), 0.0, 1e-12);
  }

  void construct_from_value() {
    si::unit_t<si::Dim< 0, 0, 0>> tmp(10.0);
    EXPECT_NEAR(tmp.raw(), 10.0, 1e-12);
  }

  void compile_time_properties() {
    EXPECT_TRUE((std::is_default_constructible_v<si::unit_t<si::Dim< 0, 0, 0>>>));
    EXPECT_TRUE((std::is_copy_constructible_v<si::unit_t<si::Dim< 0, 0, 0>>>));
    EXPECT_TRUE((std::is_move_constructible_v<si::unit_t<si::Dim< 0, 0, 0>>>));
  }
};

TEST_METHOD(TestUnit, default_construct_is_zero)
TEST_METHOD(TestUnit, construct_from_value)
TEST_METHOD(TestUnit, compile_time_properties)

class TestUnitComplete {
private:
  si::mass_t m1{ 10.0 };
  si::mass_t m2{ 10.0 };
  si::velocity_t v1{ 10.0 };
  si::velocity_t v2{ 10.0 };
  si::momentum_t p{ 100.0 };

public:

  void type_match() {
    ASSERT_SAME_TYPE(m1, m2);
    ASSERT_SAME_TYPE(v1, v2);
  }

  void add_return_type_match() {
    ASSERT_TYPE(m1 + m2, si::mass_t);
    ASSERT_TYPE(v1 + v2, si::velocity_t);
  }
  
  void sub_return_type_match() {
    ASSERT_TYPE(m1 - m2, si::mass_t);
    ASSERT_TYPE(v1 - v2, si::velocity_t);
  }

  void mul_return_type_match() {
    ASSERT_TYPE(m1 * v1, si::momentum_t);
    ASSERT_TYPE(m2 * v2, si::momentum_t);
  }

  void div_return_type_match() {
    ASSERT_TYPE(p / m1, si::velocity_t);
  }

  void add_type() {
    EXPECT_NEAR((m1 + m2).raw(), 20.0, 1e-12);
    EXPECT_NEAR((v1 + v2).raw(), 20.0, 1e-12);
  }

  void sub_type() {
    EXPECT_NEAR((m1 - m2).raw(), 0.0, 1e-12);
    EXPECT_NEAR((v1 - v2).raw(), 0.0, 1e-12);
  }

  void mul_type() {
    EXPECT_NEAR((m1 * m2).raw(), 100.0, 1e-12);
    EXPECT_NEAR((v1 * v2).raw(), 100.0, 1e-12);
  }

  void div_type() {
    EXPECT_NEAR((m1 / m2).raw(), 1.0, 1e-12);
    EXPECT_NEAR((v1 / v2).raw(), 1.0, 1e-12);
  }

  void add_double() {
    EXPECT_NEAR((m1 + 10.0).raw(), 20.0, 1e-12);
    EXPECT_NEAR((10.0 + m1).raw(), 20.0, 1e-12);
  }

  void sub_double() {
    EXPECT_NEAR((m1 - 10.0).raw(), 0.0, 1e-12);
    EXPECT_NEAR((10.0 - m1).raw(), 0.0, 1e-12);
  }

  void mul_double() {
    EXPECT_NEAR((m1 * 10.0).raw(), 100.0, 1e-12);
    EXPECT_NEAR((10.0 * m1).raw(), 100.0, 1e-12);
  }

  void div_double() {
    EXPECT_NEAR((m1 / 10.0).raw(), 1.0, 1e-12);
    EXPECT_NEAR((10.0 / m1).raw(), 1.0, 1e-12);
  }

  void create_type() {
    t var1{};
    _t var2{};
    ASSERT_TYPE(var1, t);
    EXPECT_NEAR(var1.raw(), 0, 1e-12);
    ASSERT_TYPE((var1 * var2), res);
  }
};

TEST_METHOD(TestUnitComplete, type_match);
TEST_METHOD(TestUnitComplete, add_return_type_match);
TEST_METHOD(TestUnitComplete, sub_return_type_match);
TEST_METHOD(TestUnitComplete, mul_return_type_match);
TEST_METHOD(TestUnitComplete, div_return_type_match);
TEST_METHOD(TestUnitComplete, add_type);
TEST_METHOD(TestUnitComplete, sub_type);
TEST_METHOD(TestUnitComplete, mul_type);
TEST_METHOD(TestUnitComplete, div_type);
TEST_METHOD(TestUnitComplete, add_double);
TEST_METHOD(TestUnitComplete, sub_double);
TEST_METHOD(TestUnitComplete, mul_double);
TEST_METHOD(TestUnitComplete, div_double);
TEST_METHOD(TestUnitComplete, create_type);

