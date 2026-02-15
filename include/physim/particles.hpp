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

#include <physim/matrix.hpp>
#include <physim/energie.hpp>

#include <functional>

namespace si {

template<size_t M, size_t N>
struct particle_t {
  matrix_t<ekin_t, M, N> ekin{};
  matrix_t<epot_t, M, N> epot{};
  mass_t m{};
  matrix_t<velocity_t, M, N> v{};
  matrix_t<momentum_t, M, N> p{};

  particle_t() = default;
  explicit particle_t(
      matrix_t<ekin_t, M, N> _ekin, 
      matrix_t<epot_t, M, N> _epot, 
      mass_t _m, 
      matrix_t<velocity_t, M, N> _v, 
      matrix_t<momentum_t, M, N> _p
  ) : ekin(_ekin), epot(_epot), m(_m), v(_v), p(_p) {}

  inline void momentum() { this->p = this->m * this->v; }
  inline void ekin_CM() { this->ekin.classical_mechanic(this->m, this->v); }

  template<class F, class ...Args>
  requires  std::invocable<F, Args...> &&
            std::assignable_from<double&, std::invoke_result_t<F&, Args...>>
  void set_v(F&& f, Args&&... args) {
    this->v = si::velocity_t{ std::invoke(std::forward<F>(f), std::forward<Args>(args)...) };
  }

  template<class F, class ...Args>
  requires  std::invocable<F, Args...> &&
            std::assignable_from<si::velocity_t&, std::invoke_result_t<F&, Args...>>
  void set_v(F&& f, Args&&... args) {
    this->v = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
  }
  
};

} // namespace si

