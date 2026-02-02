
#include "../include/units.hpp"

si::Ekin& si::Ekin::classical_mechanic(mass_t m, velocity_t v) {
  this->unit = 0.5 * m.u * v.u * v.u;
  return *this;
}
