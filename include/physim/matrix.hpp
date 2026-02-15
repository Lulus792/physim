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

#include <array>
#include <cstddef>
#include <utility>
#include <stdexcept>

template<class U, std::size_t _row, std::size_t _col>
struct matrix_t {
  using type = U;
  static constexpr size_t row = _row;
  static constexpr size_t col = _col;

  std::array<U, _row*_col> mat{};

  constexpr U& operator()(std::size_t r, std::size_t c) noexcept { return mat[r * col + c]; }
  constexpr const U& operator()(std::size_t r, std::size_t c) const noexcept { 
    return mat[r * col + c]; 
  }
  
};

// ---- type utilities ----

template<class U1, class U2>
using mul_elem_result_t = decltype(std::declval<U1>() * std::declval<U2>());

template<class U1, class U2>
using add_elem_result_t = decltype(std::declval<U1>() + std::declval<U2>());

template<class U1, class U2, std::size_t M, std::size_t N>
using matmul_result_t = matrix_t<mul_elem_result_t<U1, U2>, M, N>;

// ---- Concepts ----
template<class U>
concept Accumulatable = requires(U x, U y) {
  U{};
  { x += y };
};

template<class U1, class U2>
concept ScalarMul = requires(U1 u1, U2 u2) {
  u1 * u2;
};

template<class U1, class U2>
concept ScalarDiv = requires(U1 u1, U2 u2) {
  u1 / u2;
};

template<class U1, class U2>
concept MatmulElementOK =
  requires(U1 a, U2 b) { a * b; } &&
  Accumulatable<mul_elem_result_t<U1, U2>>;

// --- Operator ---
template<class U, std::size_t row, std::size_t col>
constexpr bool operator==(matrix_t<U, row, col> &A, matrix_t<U, row, col> &B) noexcept {
  for (std::size_t i = 0; i < row * col; ++i) {
    if (A.mat[i] != B.mat[i]) {
      return false;
    }
  }
  return true;
}


// --- Matrix multiplication ---
template<class U1, class U2, std::size_t M, std::size_t K, std::size_t N>
requires MatmulElementOK<U1, U2>
constexpr matmul_result_t<U1, U2, M, N>
operator*(const matrix_t<U1, M, K> &A, const matrix_t<U2, K, N> &B) noexcept {
  using U = mul_elem_result_t<U1, U2>; 
  matrix_t<U, M, N> C{};
  
  for (std::size_t i = 0; i < M; ++i) {
    U *cRow = &C.mat(i * N);
    const U1 *aRow = &A.mat(i * K);
    for (std::size_t k = 0; k < K; ++k) {
      const U1 aik = aRow(k);
      const U2 *bRow = &B.mat(k * N);
      for (std::size_t j = 0; j < N; ++j) {
        cRow(j) += aik * bRow(j);
      }
    }
  }
  return C;
}

// --- Scalar multiplication ---
template<class U, std::size_t row, std::size_t col, class S>
requires ScalarMul<U, S>
constexpr matmul_result_t<U, S, row, col>
operator*(const matrix_t<U, row, col> &A, const S &scalar) {
  matmul_result_t<U, S, row, col> B{};
  for (std::size_t i = 0; i < row * col; ++i) {
    B.mat[i] = A.mat[i] * scalar;
  }
  return B;
}

template<class U, std::size_t row, std::size_t col, class S>
requires ScalarMul<S, U>
constexpr matmul_result_t<U, S, row, col>
operator*(const S &scalar, const matrix_t<U, row, col> &A) {
  matmul_result_t<U, S, row, col> B{};
  for (std::size_t i = 0; i < row * col; ++i) {
    B.mat[i] = scalar * A.mat[i];
  }
  return B;
}

// --- Scalar division ---
template<class U, std::size_t row, std::size_t col, class S>
requires ScalarDiv<U, S>
constexpr matmul_result_t<U, S, row, col>
operator/(const matrix_t<U, row, col> &A, const S &scalar) {
  if (scalar == 0) {
    throw std::runtime_error("Division by zero");
  }
  matmul_result_t<U, S, row, col> B{};
  for (std::size_t i = 0; i < row * col; ++i) {
    B.mat[i] = A.mat[i] / scalar;
  }
  return B;
}

// --- Optimized Matrix multiplication ---
// only for 2x2, 3x3 aswell as vector addition 2x2 + 1x2 and 3x3 + 1x3

// --- Matrix 2x2 ---
template<class U1, class U2>
constexpr matrix_t<mul_elem_result_t<U1, U2>, 2, 2>
matmul_2x2(const matrix_t<U1, 2, 2> &A, const matrix_t<U2, 2, 2> &B) noexcept {
  using U = mul_elem_result_t<U1, U2>;
  matrix_t<U, 2, 2> C{};

  const U1 a00 = A(0, 0), a01 = A(0, 1);
  const U1 a10 = A(1, 0), a11 = A(1, 1);

  const U2 b00 = B(0, 0), b01 = B(0, 1);
  const U2 b10 = B(1, 0), b11 = B(1, 1);

  C(0, 0) = a00 * b00 + a01 * b10;
  C(0, 1) = a00 * b01 + a01 * b11;
  C(1, 0) = a10 * b00 + a11 * b10;
  C(1, 1) = a10 * b01 + a11 * b11;

  return C;
}

// --- Matrix 3x3 ---
template<class U1, class U2>
constexpr matrix_t<mul_elem_result_t<U1, U2>, 3, 3>
matmul_3x3(const matrix_t<U1, 3, 3> &A, const matrix_t<U2, 3, 3> &B) noexcept {
  using U = mul_elem_result_t<U1, U2>;
  matrix_t<U, 3, 3> C{};

  const U1 a00 = A(0, 0), a01 = A(0, 1), a02 = A(0, 2);
  const U1 a10 = A(1, 0), a11 = A(1, 1), a12 = A(1, 2);
  const U1 a20 = A(2, 0), a21 = A(2, 1), a22 = A(2, 2);

  const U2 b00 = B(0, 0), b01 = B(0, 1), b02 = B(0, 2);
  const U2 b10 = B(1, 0), b11 = B(1, 1), b12 = B(1, 2);
  const U2 b20 = B(2, 0), b21 = B(2, 1), b22 = B(2, 2);

  C(0, 0) = a00 * b00 + a01 * b10 + a02 * b20;
  C(0, 1) = a00 * b01 + a01 * b11 + a02 * b21;
  C(0, 2) = a00 * b02 + a01 * b12 + a02 * b22;

  C(1, 0) = a10 * b00 + a11 * b10 + a12 * b20;
  C(1, 1) = a10 * b01 + a11 * b11 + a12 * b21;
  C(1, 2) = a10 * b02 + a11 * b12 + a12 * b22;

  C(2, 0) = a20 * b00 + a21 * b10 + a22 * b20;
  C(2, 1) = a20 * b01 + a21 * b11 + a22 * b21;
  C(2, 2) = a20 * b02 + a21 * b12 + a22 * b22;

  return C;
}

// --- Add 1x2 vector to 2x2 matrix ---
template<class U>
constexpr matrix_t<U, 2, 2>
matadd_vector(const matrix_t<U, 2, 2> &A, const matrix_t<U, 1, 2> &v) noexcept {
  matrix_t<U, 2, 2> B{};
  const U v0 = v[0, 0], v1 = v[0, 1];

  B(0, 0) = A(0, 0) + v0;
  B(0, 1) = A(0, 1) + v1;
  B(1, 0) = A(1, 0) + v0;
  B(1, 1) = A(1, 1) + v1;

  return B;
}

// --- Add 1x2 vector to 2x2 matrix inplace ---
template<class U>
constexpr matrix_t<U, 2, 2> &matadd_vector(
    matrix_t<U, 2, 2> &A, 
    const matrix_t<U, 1, 2> &v, bool _) 
  noexcept {
  const U v0 = v(0, 0), v1 = v(0, 1);

  A(0, 0) =+ v0;
  A(0, 1) =+ v1;
  A(1, 0) =+ v0;
  A(1, 1) =+ v1;

  return A;
}

// --- Add 1x3 vector to 3x3 matrix ---
template<class U>
constexpr matrix_t<U, 3, 3>
matadd_vector(const matrix_t<U, 3, 3> &A, const matrix_t<U, 1, 3> &v) noexcept {
  matrix_t<U, 3, 3> B{};
  const U v0 = v(0, 0), v1 = v(0, 1), v2 = v(0, 2);

  B(0, 0) = A(0, 0) + v0;
  B(0, 1) = A(0, 1) + v1;
  B(0, 2) = A(0, 2) + v2;

  B(1, 0) = A(1, 0) + v0;
  B(1, 1) = A(1, 1) + v1;
  B(1, 2) = A(1, 2) + v2;

  B(2, 0) = A(2, 0) + v0;
  B(2, 1) = A(2, 1) + v1;
  B(2, 2) = A(2, 2) + v2;

  return B;
}

// --- Add 1x3 vector to 3x3 matrix ---
template<class U>
constexpr matrix_t<U, 3, 3> &matadd_vector(
    const matrix_t<U, 3, 3> &A, 
    const matrix_t<U, 1, 3> &v, bool _) 
  noexcept {
  matrix_t<U, 3, 3> B{};
  const U v0 = v(0, 0), v1 = v(0, 1), v2 = v(0, 2);

  A(0, 0) =+ v0;
  A(0, 1) =+ v1;
  A(0, 2) =+ v2;

  A(1, 0) =+ v0;
  A(1, 1) =+ v1;
  A(1, 2) =+ v2;

  A(2, 0) =+ v0;
  A(2, 1) =+ v1;
  A(2, 2) =+ v2;

}

