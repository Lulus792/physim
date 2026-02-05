
#include "../include/energie.hpp"

si::Energie::Energie(si::ekin_t _kinetic) {
  this->kinetic = _kinetic;
}

si::ekin_t si::classical_mechanic(si::mass_t m, si::velocity_t v) {
  return 0.5 * m * v * v;
}

