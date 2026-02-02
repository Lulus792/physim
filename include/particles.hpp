
#pragma once

#include "units.hpp"
#include "energie.hpp"

namespace si {

class Particle {
private:
  si::Energie e{};
  si::Mass m{};
  si::Velocity v{};
  si::Momentum p{};

public:
  Particle() = default;
  explicit Particle(si::Energie _e, si::Mass _m, si::Velocity _v, si::Momentum _p)
    : e(_e), m(_m), v(_v), p(_p) {}
  template<class U>
  U& operator()(); 
  template<class U>
  U& get();
  si::Energie& energie();
  double mass();
  double velocity();
  double momentum();
};

}
