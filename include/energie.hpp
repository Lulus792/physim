
#pragma once

#include "units.hpp"

namespace si {

class Energie {
private:
  si::Ekin ekin{};

public:
  Energie() = default;
  explicit Energie(energie_t _ekin) : ekin(_ekin) {}
  si::Ekin& kinetic();
};

}
