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

namespace si {

template<size_t M, size_t N>
struct particle_t {
  ekin_t ekin{};
  epot_t epot{};
  mass_t m{};
  matrix_t<velocity_t, M, N> v{};
  matrix_t<momentum_t, M, N> p{};

  particle_t() = default;
  explicit particle_t(
      ekin_t _ekin, 
      epot_t _epot, 
      mass_t _m, 
      matrix_t<velocity_t, M, N> _v, 
      matrix_t<momentum_t, M, N> _p
  ) : ekin(_ekin), epot(_epot), m(_m), v(_v), p(_p) {}

  particle_t &set(
      ekin_t _ekin,
      epot_t _epot,
      mass_t _m,
      matrix_t<velocity_t, M, N> _v,
      matrix_t<momentum_t, M, N> _p
  ) noexcept {
    this->ekin = _ekin;
    this->epot = _epot;
    this->m = _m;
    this->v = _v;
    this->p = _p;
  }

  inline void momentum() { this->p = this->m * this->v; }
  inline void ekin_CM() { this->ekin.classical_mechanic(this->m, this->v); }
  
};

} // namespace si

