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

#include <limits>

namespace cx {

template<class U>
constexpr U abs(U x) { return x < U{ 0 } ? -x : x; }

template<int exp, class U>
constexpr U powc(U base) {
  if constexpr (exp == 0) return U{ 1 };
  if constexpr (exp == 1) return base;
  if constexpr (exp % 2 == 0) return powc<exp / 2>(base * base);
  if constexpr (exp < 0) return U{ 1 } / powc<-exp>(base);
  else return base * powc<exp - 1>(base);
}

namespace detail {

template< unsigned AbsN, class U>
constexpr U nth_root_pos_n(U x, unsigned iters = 60) {
  static_assert(AbsN >= 1, "nth_root_pos_n: AbsN must be >= 1");

  if (x == U{ 0 }) return U{ 0 };
  if constexpr (AbsN == 1) return x;

  U r = (x >= U{ 1 }) ? x : U{ 1 };
  
  for (unsigned i = 0; i < iters; ++i) {
    if (r == U{ 0 }) r = U{ 1 };

    const U r_pow = powc<AbsN - 1>(r);
    const U next = (U{ AbsN - 1 } * r + x / r_pow) / U{ AbsN };

    const U tol = std::numeric_limits<U>::epsilon() * (abs(r) + U{ 1 });
    if (abs(next - r) <= tol) {
      r = next;
      break;
    }
    r = next;
  }
  return r;
}

} // namespace detail

template<class U>
constexpr U sqrt(U x, unsigned iters = 60) {
  if (x <= U{ 0 }) return U{ 0 };
  U r = x;
  for (unsigned i = 0; i < iters; ++i) {
    r = (r + x / r) / U { 2 };
  }
  return r;
}

template<int N, class U>
constexpr U nth_root(U x, unsigned iters = 60) {
  static_assert(N != 0, "nth_root<N>: N must not be 0");
  
  constexpr unsigned AbsN = (N < 0) ? unsigned(-N) : unsigned(N);
  
  const bool neg = (x < U{ 0 });
  if (neg) {
    static_assert((AbsN % 2u) == 1u, "nth_root: even root of negative x is not real");
    x = -x;
  }

  U r = detail::nth_root_pos_n<AbsN>(x, iters);
  if (neg) r = -r;

  if constexpr (N < 0) {
    return U { 1 } / r;
  } else {
    return r;
  }
}

} // namespace cx

