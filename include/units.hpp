#pragma once

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
  constexpr explicit unit_t(double _u = 0.0) noexcept : u(_u) {}
  constexpr double raw() const noexcept { return u; }
};

using mass_t            = unit_t<Dim< 1, 0, 0>>;
using length_t          = unit_t<Dim< 0, 1, 0>>;
using time_t            = unit_t<Dim< 0, 0, 1>>;

using velocity_t        = unit_t<Dim< 0, 1,-1>>;
using acceleration_t    = unit_t<Dim< 0, 1,-2>>;

using force_t           = unit_t<Dim< 1, 1,-2>>;
using energie_t         = unit_t<Dim< 1, 2,-2>>;
using power_t           = unit_t<Dim< 1, 2,-3>>;

using area_t            = unit_t<Dim< 0, 2, 0>>;
using volume_t          = unit_t<Dim< 0, 3, 0>>;
using pressure_t        = unit_t<Dim< 1,-1,-2>>;

using momentum_t        = unit_t<Dim< 1, 1,-1>>;
using density_t         = unit_t<Dim< 1,-3, 0>>;

template<class D> struct Canon { using type = unit_t<D>; };

template<> struct Canon<Dim< 1, 0, 0>> { using type = mass_t;         };
template<> struct Canon<Dim< 0, 1, 0>> { using type = length_t;       };
template<> struct Canon<Dim< 0, 0, 1>> { using type = time_t;         };

template<> struct Canon<Dim< 0, 1,-1>> { using type = velocity_t;     };
template<> struct Canon<Dim< 0, 1,-2>> { using type = acceleration_t; };

template<> struct Canon<Dim< 1, 1,-2>> { using tpye = force_t;        };
template<> struct Canon<Dim< 1, 2,-2>> { using type = energie_t;      };
template<> struct Canon<Dim< 1, 2,-3>> { using type = power_t;        };

template<> struct Canon<Dim< 0, 2, 0>> { using type = area_t;         };
template<> struct Canon<Dim< 0, 3, 0>> { using type = volume_t;       };
template<> struct Canon<Dim< 1,-1,-2>> { using type = pressure_t;     };

template<> struct Canon<Dim< 1, 1,-1>> { using type = momentum_t;     };
template<> struct Canon<Dim< 1,-3, 0>> { using type = density_t;      };

// --- Multiplication ---
template<class D1, class D2>
constexpr typename Canon<DimMul<D1, D2>>::type
operator*(unit_t<D1> rhs, unit_t<D2> lhs) noexcept {
  using R = typename Canon<DimMul<D1, D2>>::type;
  return R(rhs.raw() * lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator*(double rhs, unit_t<D> lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs * lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator*(unit_t<D> rhs, double lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs.raw() * lhs);
}

// --- Division ---
template<class D1, class D2>
constexpr typename Canon<DimDiv<D1, D2>>::type
operator/(unit_t<D1> rhs, unit_t<D2> lhs) noexcept {
  using R = typename Canon<DimDiv<D1, D2>>::type;
  return R(rhs.raw() / lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator/(double rhs, unit_t<D> lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs / lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator/(unit_t<D> rhs, double lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs.raw() / lhs);
}

// --- Addition ---
template<class D>
constexpr typename Canon<D>::type
operator+(unit_t<D> rhs, unit_t<D> lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs.raw() + lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator+(double rhs, unit_t<D> lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs + lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator+(unit_t<D> rhs, double lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs.raw() + lhs);
}

// --- Substraction ---
template<class D>
constexpr typename Canon<D>::type
operator-(unit_t<D> rhs, unit_t<D> lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs.raw() - lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator-(double rhs, unit_t<D> lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs - lhs.raw());
}

template<class D>
constexpr typename Canon<D>::type
operator-(unit_t<D> rhs, double lhs) noexcept {
  using R = typename Canon<D>::type;
  return R(rhs.raw() - lhs);
}

}

