#pragma once

#include <functional>
#include <concepts>
#include <utility>
#include <type_traits>

struct unit_t {
  double u;
  unit_t& operator=(double _u) {
    this->u = _u;
    return *this;
  }
  double operator*() const {
    return this->u;
  }
};

typedef struct energie_t : unit_t {
  using unit_t::operator*;
  using unit_t::operator=;
} energie_t;

typedef struct momentum_t : unit_t {
  using unit_t::operator*;
  using unit_t::operator=;
} momentum_t;

typedef struct velocity_t : unit_t {
  using unit_t::operator*; 
  using unit_t::operator=;
} velocity_t;

typedef struct mass_t : unit_t {
  using unit_t::operator*;
  using unit_t::operator=;
} mass_t;

namespace si {

template<class U>
class Unit {
protected:
  U unit{};

public:
  Unit() = default;
  explicit Unit(const U value) : unit(value) {}

  U operator*() const noexcept {
    return unit;
  }
  Unit& operator=(U _unit) noexcept { 
    this->unit = std::move<U>(_unit);
    return *this;
  }

  template<class F, class... Args>
  requires  std::invocable<F, Args...> &&
            std::assignable_from<U&, std::invoke_result_t<F&, Args...>>
  Unit& operator()(F&& f, Args&&... args) {
    this->unit = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    return *this;
  }
};

class Ekin : public Unit<energie_t> { 
  public:
    using Unit<energie_t>::Unit;
    using Unit<energie_t>::operator=;
    //using Unit<energie_t>::operator();
    template<class F, class... Args>
    requires  std::invocable<F, Args...> &&
              std::assignable_from<energie_t&, std::invoke_result_t<F&, Args...>>
    Ekin& operator()(F&& f, Args&&... args) {
      this->unit = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
      return *this;
    }
    double operator*() const {
      return this->unit.u;
    }
    Ekin& classical_mechanic(mass_t m, velocity_t v);
};

class Momentum : public Unit<momentum_t> {
public:
  using Unit<momentum_t>::Unit;
  using Unit<momentum_t>::operator=;
  using Unit<momentum_t>::operator();
  double operator*() const {
    return this->unit.u;
  }
};

class Mass : public Unit<mass_t> {
public:
  using Unit<mass_t>::Unit;
  using Unit<mass_t>::operator=;
  using Unit<mass_t>::operator();
  double operator*() const {
    //return this->unit.u;
    return this->unit.u;
  }
};

class Velocity : public Unit<velocity_t> {
public:
  using Unit<velocity_t>::Unit;
  using Unit<velocity_t>::operator=;
  using Unit<velocity_t>::operator();
  double operator*() const {
    return this->unit.u;
  }
};

}

