
#include "../include/particles.hpp"

template<class U>
U& si::Particle::operator()() {
  if constexpr (std::is_same_v<U, energie_t>) {
    return this->e;
  }
  else if constexpr (std::is_same_v<U, mass_t>) {
    return this->m; 
  }
  else if constexpr (std::is_same_v<U, velocity_t>) {
    return this->v; 
  }
  else if constexpr (std::is_same_v<U, momentum_t>) {
    return this->p; 
  }
  else {
    return nullptr;
  }
}

template<class U>
U& si::Particle::get() {
  if constexpr (std::is_same_v<U, energie_t>) {
    return this->e;
  }
  else if constexpr (std::is_same_v<U, mass_t>) {
    return this->m; 
  }
  else if constexpr (std::is_same_v<U, velocity_t>) {
    return this->v; 
  }
  else if constexpr (std::is_same_v<U, momentum_t>) {
    return this->p; 
  }
  else {
    return nullptr;
  }
}

si::Energie& si::Particle::energie() {
  return this->e;
}

double si::Particle::mass() {
  return *this->m;
}

double si::Particle::velocity() {
  return *this->v;
}

double si::Particle::momentum() {
  return *this->p;
}

