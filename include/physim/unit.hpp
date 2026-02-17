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

#pragma once

#include <stdexcept>

namespace si {

template<int M, int L, int T> 
struct Dim {
  static constexpr int m = M, l = L, t = T;
};

template<class A, class B> 
using DimMul = Dim<A::m + B::m, A::l + B::l, A::t + B::t>;

template<class A, class B> 
using DimDiv = Dim<A::m - B::m, A::l - B::l, A::t - B::t>;

template<class D> 
struct unit_t {
  double u;
  unit_t() = default;
  constexpr explicit unit_t(double _u) noexcept : u(_u) {}
  constexpr double raw() const noexcept { return u; }
  constexpr unit_t &operator=(double value) {
    u = value;
    return *this;
  }
};

using mass_t          = unit_t<Dim< 1, 0, 0>>;
using length_t        = unit_t<Dim< 0, 1, 0>>;
using time_t          = unit_t<Dim< 0, 0, 1>>;

using velocity_t      = unit_t<Dim< 0, 1,-1>>;
using acceleration_t  = unit_t<Dim< 0, 1,-2>>;

using force_t         = unit_t<Dim< 1, 1,-2>>;
using energy_t        = unit_t<Dim< 1, 2,-2>>;
using power_t         = unit_t<Dim< 1, 2,-3>>;

using area_t          = unit_t<Dim< 0, 2, 0>>;
using volume_t        = unit_t<Dim< 0, 3, 0>>;
using pressure_t      = unit_t<Dim< 1,-1,-2>>;

using momentum_t      = unit_t<Dim< 1, 1,-1>>;
using density_t       = unit_t<Dim< 1,-3, 0>>;

template<class D> struct Canon { using type = unit_t<D>; };

template<> struct Canon<Dim< 1, 0, 0>> { using type = mass_t; };
template<> struct Canon<Dim< 0, 1, 0>> { using type = length_t; };
template<> struct Canon<Dim< 0, 0, 1>> { using type = time_t; };

template<> struct Canon<Dim< 0, 1,-1>> { using type = velocity_t; };
template<> struct Canon<Dim< 0, 1,-2>> { using type = acceleration_t; };

template<> struct Canon<Dim< 1, 1,-2>> { using type = force_t; };
template<> struct Canon<Dim< 1, 2,-2>> { using type = energy_t; };
template<> struct Canon<Dim< 1, 2,-3>> { using type = power_t; };

template<> struct Canon<Dim< 0, 2, 0>> { using type = area_t; };
template<> struct Canon<Dim< 0, 3, 0>> { using type = volume_t; };
template<> struct Canon<Dim< 1,-1,-2>> { using type = pressure_t; };

template<> struct Canon<Dim< 1, 1,-1>> { using type = momentum_t; };
template<> struct Canon<Dim< 1,-3, 0>> { using type = density_t; };

// --- Multiplication ---
template<class D1, class D2>
constexpr typename Canon<DimMul<D1, D2>>::type 
operator*(unit_t<D1> lhs, unit_t<D2> rhs) noexcept {
  using R = typename Canon<DimMul<D1, D2>>::type;
  return R(lhs.raw() * rhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator*(double lhs, unit_t<D> rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs * rhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator*(unit_t<D> lhs, double rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs.raw() * rhs);
}

// --- Division ---
template<class D1, class D2>
constexpr typename Canon<DimDiv<D1, D2>>::type
operator/(unit_t<D1> lhs, unit_t<D2> rhs) {
  if (rhs == 0.0) {
    throw std::runtime_error("Division by zero");
  }
  using R = typename Canon<DimDiv<D1, D2>>::type;
  return R(lhs.raw() / rhs.raw());
}

template<class D>
constexpr typename Canon<DimDiv<Dim<0, 0, 0>, D>>::type
operator/(double lhs, unit_t<D> rhs) {
  if (rhs == 0.0) {
    throw std::runtime_error("Division by zero");
  }
  using R = typename Canon<D>::type;
  return R(lhs / rhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator/(unit_t<D> lhs, double rhs) {
  if (rhs == 0.0) {
    throw std::runtime_error("Division by zero");
  }
  using R = typename Canon<D>::type;
  return R(lhs.raw() / rhs);
}

// --- Addition ---
template<class D>
constexpr typename Canon<D>::type
operator+(unit_t<D> lhs, unit_t<D> rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs.raw() + rhs.raw());
}

// --- Subtraction ---
template<class D>
constexpr typename Canon<D>::type
operator-(unit_t<D> lhs, unit_t<D> rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs.raw() - rhs.raw());
}

// --- Comparison ---
template<class D>
constexpr bool operator==(unit_t<D> lhs, unit_t<D> rhs) {
  if (lhs.raw() == rhs.raw()) {
    return true;
  }
  return false;
}

template<class D>
constexpr bool operator==(unit_t<D> lhs, double rhs) noexcept {
  if (lhs.raw() == rhs) {
    return true;
  }
  return false;
}

template<class D>
constexpr bool operator==(double lhs, unit_t<D> rhs) noexcept {
  if (lhs == rhs.raw()) {
    return true;
  }
  return false;
}

#ifdef _IMPLICIT_CONVERSION_

// --- Addition ---
template<class D>
constexpr typename Canon<D>::type
operator+(double lhs, unit_t<D> rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs + rhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator+(unit_t<D> lhs, double rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs.raw() + rhs);
}

// --- Subtraction ---
template<class D>
constexpr typename Canon<D>::type
operator-(double lhs, unit_t<D> rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs - rhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator-(unit_t<D> lhs, double rhs) noexcept {
  using R = typename Canon<D>::type;
  return R(lhs.raw() - rhs);
}

#endif

} // namespace si

