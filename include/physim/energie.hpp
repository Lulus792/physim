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

#include <physim/unit.hpp> 
#include <physim/matrix.hpp>

namespace si {

typedef struct ekin_t : energy_t {
  using energy_t::unit_t;

  template<std::size_t M, std::size_t N>
  ekin_t *classical_mechanic(si::mass_t m, matrix_t<si::velocity_t, M, N> v) {
    this->u = (0.5 * m * cx::powc<2>(v.frobenius_norm())).raw();
    return this;
  }
} ekin_t;

typedef struct epot_t : energy_t {
  using energy_t::unit_t;
} epot_t;


} // namespace si
