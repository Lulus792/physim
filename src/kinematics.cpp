
#include "../include/kinematics.hpp"
#include "units.hpp"

void kinematics::Collision::_1D_collision(si::Particle *p1, si::Particle *p2) {
  auto new_velocity = [](si::Particle *_p1, si::Particle *_p2) {
    return 2*((_p1->mass() * _p1->velocity()) + (_p2->mass() * _p2->velocity()))/
      (_p1->mass() + _p2->mass()); 
  };

  auto new_ekin = [](si::Particle *p) {
    return p->mass() * p->velocity();
  };
  
  p1->set<si::velocity_t>() = new_velocity(p1, p2);
  p2->set<si::velocity_t>() = new_velocity(p2, p1);

  p1->set<si::momentum_t>() = new_ekin(p1);
  p2->set<si::momentum_t>() = new_ekin(p2);
}
