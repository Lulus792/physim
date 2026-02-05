
#pragma once

#include "units.hpp"

namespace si {

using ekin_t = energie_t;
using epot_t = energie_t;

class Energie {
private:
  ekin_t kinetic{};
  epot_t potential{};

public:
  Energie() = default;
  Energie(ekin_t _kinetic);
  inline ekin_t& ekin() noexcept { return kinetic; }
  inline epot_t epot() noexcept { return potential; }
};

ekin_t classical_mechanic(si::mass_t m, si::velocity_t v);

}
